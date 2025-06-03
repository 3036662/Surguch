#ifndef HISTORY_MANAGER_HPP
#define HISTORY_MANAGER_HPP

#include <QFuture>
#include <QFutureWatcher>
#include <QImage>
#include <QList>
#include <QObject>
#include <memory>

#include "pdf_csp_c.hpp"
#include "rubber_structs.hpp"

namespace core::gui {

/**
 * @brief The HistoryManager stores and handles undo and redo actions for rubber stamps.
 * @details This class owns images and data for rubber stamps.
 */

class HistoryManager: public QObject {
Q_OBJECT

    public:
    HistoryManager(QObject* parent = nullptr);

    using EditActions = std::shared_ptr<RubberStamp>;

    /// @brief add action to list
    void addAction(std::unique_ptr<RubberStamp> action);

    /// @brief get array of actions on page x
    [[nodiscard]] QList<EditActions> getActionsOnPage(size_t page_index) const;

    /// @brief clears redo actions array if something done after undo
    void clearRedo();

    /// @brief clear all actions if file was saved
    void clearHistory();

    /// @brief undo placed stamp(ctrl+z)
    void undoAction();

    /// @brief redo previously undo stamps(ctrl+shift+z)
    void redoAction();


private:
    QList<EditActions> undo_actions_;
    QList<EditActions> redo_actions_;
};

}  // namespace core::gui

#endif //HISTORY_MANAGER_HPP
