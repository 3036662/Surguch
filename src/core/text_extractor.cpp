#include "text_extractor.hpp"

#include <QDebug>
#include <QFuture>
#include <QtConcurrent>
#include <stdexcept>

namespace core {

TextExtractor::TextExtractor(fz_context *fzctx, fz_document *fzdoc,
                             QObject *parent)
    : QObject{parent}, fzctx_(fzctx), fzdoc_(fzdoc) {
  if (fzctx == nullptr || fzdoc == nullptr) {
    throw std::invalid_argument("[TextExtractor] nullptr recieved");
  }
}

void TextExtractor::updateCache() {
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
}

void TextExtractor::waitForCacheReady() {
  if (cache_watcher_ && cache_future_ && cache_future_->isValid()) {
    cache_watcher_->waitForFinished();
  }
}

}  // namespace core
