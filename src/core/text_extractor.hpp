#ifndef TEXT_EXTRACTOR_HPP
#define TEXT_EXTRACTOR_HPP
#include <QFuture>
#include <QFutureWatcher>
#include <shared_mutex>

#include "mupdf/fitz.h"
#include "utils.hpp"

namespace core {

/**
 * @brief The TextExtractor handles the extraction of text and search.
 * @throws on construct if nullptr recieved
 * @details This class doesn't own mupdf and fzdoc contexts.
 */

class TextExtractor : public QObject {
    Q_OBJECT

   public:
    using CacheFuture = QFuture<utils::PagesTextCache>;
    using CacheFutureWatcher = QFutureWatcher<utils::PagesTextCache>;
    using TextCache = utils::PagesTextCache;
    using SearchContext =
        std::unique_ptr<std::map<size_t, utils::NeedleRectsOnPage>>;
    using SearchContextWatcher = QFutureWatcher<SearchContext>;
    using SearchFuture = QFuture<SearchContext>;
    using RectToHiglightCurrent = std::pair<size_t, fz_rect>;

    TextExtractor(fz_context* fzctx, fz_document* fzdoc,
                  QObject* parent = nullptr);

    /// @brief update the cache for the current document
    /// @return async under the hood, returns immediately
    void updateCache();

    /**
     * @brief search all pages for the needle, create
     * @param needle
     * @param case_sensitive
     * @details async under the hood, returns immediately
     */
    void performSearch(const QString& needle, bool case_sensitive);

    /// @brief blocks until the cache is ready (for testing purposes)
    void waitForCacheReady();

    /// @brief blocks until the search is finished (for testing purposes)
    void waitForSearchReady();

    /// @brief returns true if all operations are completed
    [[nodiscard]] bool isReady();

    // [[nodiscard]] const TextCache& getCache() const& { return cache_; };

    [[nodiscard]] size_t getNeedlesTotal();
    [[nodiscard]] std::pair<size_t, std::pair<float, float>> getNeedlePageAndXY(
        size_t needle_index);

    /// @details returns a copy of the search context
    [[nodiscard]] SearchContext getSearchContext();

    /**
     * @brief returns a copy of needls for page
     * @details Creates a copy to make sure the access is thread-safe.
     */
    [[nodiscard]] core::utils::NeedleRectsOnPage getNeedlesForPage(
        size_t page_index);

    /// @brief get a copy of current rect to highlight
    [[nodiscard]] std::shared_ptr<RectToHiglightCurrent> getCurrentNeedleRect(
        size_t page_index);

    // @brief retrieve all URIs on the given page using provided mouse cursor positions
    [[nodiscard]] std::unique_ptr<QStringList> getTargetAllUriPage(size_t page_index,
                                                                   core::utils::MousePos const &mouse_pos) const;
   /**
    * @brief check if there is an URI at the given mouse positions
    * @param mouse_pos mouse cursor position
    * @return true if the URI is found, otherwise false
    */
    [[nodiscard]] bool checkMouseOverUri(size_t page_index, utils::MousePos const& mouse_pos);

   signals:

    void searchCompleted();
    void cacheReady();

   private slots:
    void saveCache();
    void saveSearchContext();

   private:
    SearchContext buildSearchContext(const QString& needle,
                                     bool case_sensitive);

    fz_context* fzctx_ = nullptr;
    fz_document* fzdoc_ = nullptr;
    std::unique_ptr<CacheFutureWatcher> cache_watcher_;
    std::unique_ptr<CacheFuture> cache_future_;
    TextCache cache_;
    std::shared_mutex cach_mtx_;

    SearchContext search_context_;
    size_t needles_count_ = 0;
    std::unique_ptr<SearchFuture> search_future_;
    std::unique_ptr<SearchContextWatcher> search_watcher_;
    std::shared_mutex search_mtx_;
    QString needle_;
    bool case_sensitive_ = false;

    std::unique_ptr<RectToHiglightCurrent> current_rect_to_gighlight_;
};

}  // namespace core

#endif  // TEXT_EXTRACTOR_HPP
