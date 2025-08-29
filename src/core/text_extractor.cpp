#include "text_extractor.hpp"

#include <QDebug>
#include <QFuture>
#include <QtConcurrent>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>

namespace core {

TextExtractor::TextExtractor(fz_context *fzctx, fz_document *fzdoc,
                             QObject *parent)
    : QObject{parent}, fzctx_(fzctx), fzdoc_(fzdoc) {
    if (fzctx == nullptr || fzdoc == nullptr) {
        throw std::invalid_argument("[TextExtractor] nullptr recieved");
    }
}

/// @brief update the cache for the current document
/// @return async under the hood, returns immediately
void TextExtractor::updateCache() {
    {
        const std::unique_lock mtx(search_mtx_);
        search_context_ = nullptr;
        needles_count_ = 0;
    }
    cach_mtx_.lock();
    cache_watcher_ = std::make_unique<CacheFutureWatcher>();
    QObject::connect(cache_watcher_.get(), &CacheFutureWatcher::finished, this,
                     &TextExtractor::saveCache);
    cache_future_ = std::make_unique<CacheFuture>(
        QtConcurrent::run(utils::extractTextAllPages, fzctx_, fzdoc_));
    cache_watcher_->setFuture(*cache_future_);
}

void TextExtractor::saveCache() {
    if (cache_future_ && cache_future_->isValid()) {
        cache_ = cache_future_->takeResult();
    }
    cach_mtx_.unlock();
    emit cacheReady();
}

void TextExtractor::saveSearchContext() {
    if (search_future_ && search_future_->isValid()) {
        search_context_ = search_future_->takeResult();
        needles_count_ = std::accumulate(
            search_context_->cbegin(), search_context_->cend(),
            static_cast<size_t>(0),
            [](size_t acc,
               const std::pair<const size_t, utils::NeedleRectsOnPage> &pair) {
                return acc + pair.second->needle_rects.size();
            });
    }
    search_mtx_.unlock();
    // qWarning() << "[TextExtractor] SIGNAL search completed";
    emit searchCompleted();
}

/// @brief blocks until the cache is ready
void TextExtractor::waitForCacheReady() {
    if (cache_watcher_ && cache_future_ && cache_future_->isValid()) {
        cache_watcher_->waitForFinished();
    }
}

/// @brief blocks until the cache is finished
void TextExtractor::waitForSearchReady() {
    if (search_watcher_ && search_future_ && search_future_->isValid()) {
        search_watcher_->waitForFinished();
    }
}

bool TextExtractor::isReady() {
    return search_future_ && search_future_->isFinished() && cache_future_ &&
           cache_future_->isFinished();
}

/**
 * @brief search all pages for the needle, create
 * @param needle
 * @param case_sensitive
 * @details async under the hood, returns immediately
 */
void TextExtractor::performSearch(const QString &needle, bool case_sensitive) {
    if (needle == needle_ && case_sensitive == case_sensitive_) {
        qWarning() << "search the same needle";
        emit searchCompleted();
        return;
    }
    search_mtx_.lock();
    needle_ = needle;
    case_sensitive_ = case_sensitive;
    if (needle.isEmpty()) {
        search_context_ = nullptr;
        needles_count_ = 0;
        saveSearchContext();  // it will unlock the mutex
        return;
    }
    if (!cache_) {                        // if no cache exists
        if (!cache_future_->isValid()) {  // if caching is not in progress
            updateCache();
        }
        waitForCacheReady();
    }
    search_watcher_ = std::make_unique<SearchContextWatcher>();
    QObject::connect(search_watcher_.get(), &SearchContextWatcher::finished,
                     this, &TextExtractor::saveSearchContext);
    search_future_ = std::make_unique<SearchFuture>(QtConcurrent::run(
        &TextExtractor::buildSearchContext, this, needle, case_sensitive));
    search_watcher_->setFuture(*search_future_);
}

TextExtractor::SearchContext TextExtractor::buildSearchContext(
    const QString &needle, bool case_sensitive) {
    std::unique_lock mtx(cach_mtx_);
    auto pages_with_needle =
        core::utils::findPagesWithText(needle, cache_, case_sensitive);
    mtx.unlock();
    auto res = std::make_unique<std::map<size_t, utils::NeedleRectsOnPage>>();
    std::for_each(pages_with_needle.cbegin(), pages_with_needle.cend(),
                  [fzctx = fzctx_, fzdoc = fzdoc_, &needle, &res,
                   case_sensitive](size_t page_index) {
                      core::utils::NeedleRectsOnPage needle_rects =
                          core::utils::findNeedleRectsOnPage(
                              needle, page_index, case_sensitive, fzctx, fzdoc);
                      if (needle_rects && !needle_rects->needle_rects.empty()) {
                          res->insert_or_assign(page_index,
                                                std::move(needle_rects));
                      }
                  });
    return res;
};

size_t TextExtractor::getNeedlesTotal() {
    std::shared_lock lock{search_mtx_, std::defer_lock};
    if (!lock.try_lock()) {
        return 0;
    }
    return needles_count_;
}

std::pair<size_t, std::pair<float, float>> TextExtractor::getNeedlePageAndXY(
    size_t needle_index) {
    std::shared_lock lock{search_mtx_, std::defer_lock};
    if (!lock.try_lock()) {
        return {0, {0, 0}};
    }
    if (!search_context_ || search_context_->empty()) {
        return {0, {0, 0}};
    }
    size_t local_index = needle_index;
    auto it_page = std::find_if(
        search_context_->cbegin(), search_context_->cend(),
        [&local_index](
            const std::pair<size_t, utils::NeedleRectsOnPage> &page_pair) {
            if (!page_pair.second) {
                return false;
            }
            const auto &ptr_vector = page_pair.second->needle_rects;
            if (ptr_vector.empty()) {
                return false;
            }
            if (ptr_vector.size() <= local_index) {
                local_index -= ptr_vector.size();
                return false;
            }
            return true;
        });
    if (it_page == search_context_->cend()) {
        return {0, {0, 0}};
    }
    const auto &p_vec_rect = it_page->second->needle_rects;
    if (local_index >= p_vec_rect.size()) {
        return {it_page->first, {0, 0}};  // return only page index
    }
    const auto &rect = p_vec_rect.at(local_index);
    // save current rect for the additional highlighting
    current_rect_to_gighlight_ = std::make_unique<RectToHiglightCurrent>(
        RectToHiglightCurrent{it_page->first, rect});
    const auto &page_rect = it_page->second->page_rect;
    const float page_height = std::fabs(page_rect.y1 - page_rect.y0);
    float y_relative = page_height > 1 ? rect.y0 / page_height : 0.5F;
    if (y_relative > 1) {
        y_relative = 0.5;
    }
    const float page_width = std::fabs(page_rect.x1 - page_rect.x0);
    float x_relative = page_width > 1 ? rect.x0 / page_width : 0.5F;
    if (x_relative > 1) {
        x_relative = 0.5;
    }
    return {it_page->first, {x_relative, y_relative}};
}

core::utils::NeedleRectsOnPage TextExtractor::getNeedlesForPage(
    size_t page_index) {
    std::shared_lock lock{search_mtx_, std::defer_lock};
    if (!lock.try_lock()) {
        return {};
    }
    if (!search_context_ || search_context_->empty() ||
        search_context_->count(page_index) == 0) {
        return {};
    }
    // copy the rects
    auto result =
        std::make_shared<utils::PageRects>(*search_context_->at(page_index));
    // if we need to highlight the current item on this page - push it to the
    // result
    if (current_rect_to_gighlight_ &&
        current_rect_to_gighlight_->first == page_index) {
        result->highlight_current = true;
        result->current = current_rect_to_gighlight_->second;
    }
    return result;
}

TextExtractor::SearchContext TextExtractor::getSearchContext() {
    std::shared_lock lock{search_mtx_, std::defer_lock};
    if (!lock.try_lock()) {
        return nullptr;
    }
    if (!search_context_ || search_context_->empty()) {
        return {};
    }
    return std::make_unique<std::map<size_t, utils::NeedleRectsOnPage>>(
        *search_context_);
}

/// @brief get a copy of current rect to highlight
std::shared_ptr<TextExtractor::RectToHiglightCurrent>
TextExtractor::getCurrentNeedleRect(size_t page_index) {
    if (!current_rect_to_gighlight_ ||
        current_rect_to_gighlight_->first != page_index) {
        return nullptr;
    }
    return std::make_shared<RectToHiglightCurrent>(*current_rect_to_gighlight_);
}

// @brief retrieve all URIs on the given page using provided mouse cursor
// positions
std::shared_ptr<utils::PageUriList> TextExtractor::getTargetAllUriPage(
    size_t page_index, core::utils::MousePos const &mouse_pos) {
    std::shared_lock lock{cach_mtx_, std::defer_lock};

    if (!lock.try_lock()) {
        return {};
    }

    if (!cache_ || cache_->empty()) {
        return {};
    }

    auto found_uri_data = utils::findAllUriPage(page_index, mouse_pos, cache_);

    return std::make_shared<utils::PageUriList>(std::move(found_uri_data));
}

bool TextExtractor::checkMouseOverUri(size_t page_index,
                                      utils::MousePos const &mouse_pos) {
    std::shared_lock lock{cach_mtx_, std::defer_lock};
    if (!lock.try_lock()) {
        return {};
    }

    if (!cache_ || cache_->empty()) {
        return {};
    }

    auto result = std::make_unique<utils::PageUriList>(
        findAllUriPage(page_index, mouse_pos, cache_));

    return !result->empty();
}

}  // namespace core
