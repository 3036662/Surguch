#ifndef TEXT_EXTRACTOR_HPP
#define TEXT_EXTRACTOR_HPP
#include <QFuture>
#include <QFutureWatcher>

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

  TextExtractor(fz_context* fzctx, fz_document* fzdoc,
                QObject* parent = nullptr);

  /// @brief update the cache for the current document
  void updateCache();

  /// @brief blocks until the cache is ready
  void waitForCacheReady();

  [[nodiscard]] const TextCache& getCache() const& { return cache_; };

 private slots:
  void saveCache();

 private:
  fz_context* fzctx_ = nullptr;
  fz_document* fzdoc_ = nullptr;
  std::unique_ptr<CacheFutureWatcher> cache_watcher_;
  std::unique_ptr<CacheFuture> cache_future_;
  TextCache cache_;
};

}  // namespace core

#endif  // TEXT_EXTRACTOR_HPP
