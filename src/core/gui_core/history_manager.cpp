#include "history_manager.hpp"

#include "gui_utils.hpp"

namespace core::gui {

HistoryManager::HistoryManager(QObject* parent ): QObject{parent} {
    undo_actions_.reserve(100);
    redo_actions_.reserve(100);
}

void HistoryManager::addAction(std::unique_ptr<RubberStamp> action){
    std::unique_lock lock(mutex_);
    if (action->res->data_ != nullptr && action->res->image_ != nullptr) {
        undo_actions_.emplace_back(std::move(action));
    }
}

std::vector<HistoryManager::EditActions> HistoryManager::getActionsOnPage(size_t page_index) const {
    std::shared_lock lock(mutex_);
    if (!undo_actions_.empty()) {
        std::vector<EditActions> actions_on_page;
        for (const auto& action : undo_actions_) {
            if (action->page_index == page_index) {
                actions_on_page.emplace_back(action);
            }
        }
        return actions_on_page;
    } else {
        return {};
    }
}

void HistoryManager::clearRedo() {
    std::unique_lock lock(mutex_);
    redo_actions_.clear();
}

void HistoryManager::clearHistory() {
    std::unique_lock lock(mutex_);
    redo_actions_.clear();
    undo_actions_.clear();
}

void HistoryManager::undoAction() {
    std::unique_lock lock(mutex_);
    if (!undo_actions_.empty()) {
        redo_actions_.emplace_back(undo_actions_.back());
        undo_actions_.pop_back();
    }
}

void HistoryManager::redoAction() {
    std::unique_lock lock(mutex_);
    if (!redo_actions_.empty()) {
        undo_actions_.emplace_back(redo_actions_.back());
        redo_actions_.pop_back();
    }
}

/// @brief get annotations(rubber stamps) params for embedding them into pdf
std::vector<pdfcsp::pdf::CAnnotParams> HistoryManager::getAnnotsParams() {
    std::shared_lock lock(mutex_);
    c_annot_params_ = core::gui::createAnnotParams(undo_actions_);
    return c_annot_params_;
}

int HistoryManager::getUndoCount() const{
    std::shared_lock lock(mutex_);
    return undo_actions_.size();
}

int HistoryManager::getRedoCount() const{
    std::shared_lock lock(mutex_);
    return redo_actions_.size();
}




}  //namespace core::gui
