#include "history_manager.hpp"

namespace core::gui {

HistoryManager::HistoryManager(QObject* parent ): QObject{parent}{}

void HistoryManager::addAction(std::unique_ptr<RubberStamp> action){
    undo_actions_.append(std::move(action));
}

QList<HistoryManager::EditActions> HistoryManager::getActionsOnPage(size_t page_index) const {
    if (!undo_actions_.isEmpty()) {
        return undo_actions_;
    } else {
        return {};
    }
}

void HistoryManager::clearRedo() {
    redo_actions_.clear();
}

void HistoryManager::clearHistory() {
    redo_actions_.clear();
    undo_actions_.clear();
}

void HistoryManager::undoAction() {
    if (!undo_actions_.isEmpty()) {
        redo_actions_.append(undo_actions_.takeLast());
    }
}

void HistoryManager::redoAction() {
    if (!redo_actions_.isEmpty()) {
        undo_actions_.append(redo_actions_.takeLast());
    }
}


}  //namespace core::gui
