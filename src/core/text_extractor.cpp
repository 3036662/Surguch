#include "text_extractor.hpp"

#include <QDebug>
#include <QFuture>
#include <QtConcurrent>
#include <iostream>
#include <mutex>
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
    std::unique_lock<std::mutex> mtx(search_mtx_);
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
        search_context_->cbegin(), search_context_->cend(), size_t(0),
        [](size_t acc,
           const std::pair<const size_t, utils::NeedleRectsOnPage> &pair) {
          return acc + pair.second->size();
        });
  }
  search_mtx_.unlock();
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

/**
 * @brief search all pages for the needle, create
 * @param needle
 * @param case_sensitive
 * @details async under the hood, returns immediately
 */
void TextExtractor::performSearch(const QString &needle, bool case_sensitive) {
  if (needle.isEmpty()) {
    std::unique_lock<std::mutex> mtx(search_mtx_);
    search_context_ = nullptr;
    needles_count_ = 0;
    return;
  }
  if (!cache_) {                      // if no cache exists
    if (!cache_future_->isValid()) {  // if caching is not in progress
      updateCache();
    }
    waitForCacheReady();
  }
  search_mtx_.lock();
  search_watcher_ = std::make_unique<SearchContextWatcher>();
  QObject::connect(search_watcher_.get(), &SearchContextWatcher::finished, this,
                   &TextExtractor::saveSearchContext);
  search_future_ = std::make_unique<SearchFuture>(QtConcurrent::run(
      &TextExtractor::buildSearchContext, this, needle, case_sensitive));
  search_watcher_->setFuture(*search_future_);
}

TextExtractor::SearchContext TextExtractor::buildSearchContext(
    const QString &needle, bool case_sensitive) {
  std::unique_lock<std::mutex> mtx(cach_mtx_);
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
                  if (needle_rects && !needle_rects->empty()) {
                    res->insert_or_assign(page_index, std::move(needle_rects));
                  }
                });
  return res;
};

}  // namespace core
