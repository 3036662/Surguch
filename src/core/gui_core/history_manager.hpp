#ifndef HISTORY_MANAGER_HPP
#define HISTORY_MANAGER_HPP

#include <QFuture>
#include <QFutureWatcher>
#include <QImage>
#include <QList>
#include <QObject>
#include <memory>
#include <shared_mutex>

#include "pdf_csp_c.hpp"
#include "rubber_structs.hpp"

namespace core::gui {

/**
 * @brief The HistoryManager stores and handles undo and redo actions for rubber
 * stamps.
 * @details This class owns images and data for rubber stamps.
 */

class HistoryManager : public QObject {
    Q_OBJECT

   public:
    HistoryManager(QObject* parent = nullptr);

    using EditActions = std::shared_ptr<RubberStamp>;

    /// @brief add action to list
    void addAction(std::unique_ptr<RubberStamp> action);

    /// @brief get array of actions on page x
    [[nodiscard]] std::vector<EditActions> getActionsOnPage(
        size_t page_index) const;

    /// @brief clears redo actions array if something done after undo
    void clearRedo();

    /// @brief clear all actions if file was saved
    void clearHistory();

    /// @brief undo placed stamp(ctrl+z)
    void undoAction();

    /// @brief redo previously undo stamps(ctrl+shift+z)
    void redoAction();

    /// @brief get annotations(rubber stamps) params for embedding them into pdf
    std::vector<pdfcsp::pdf::CAnnotParams> getAnnotsParams();

    /// @brief get undo action size
    int getUndoCount() const;

    /// @brief get redo action size
    int getRedoCount() const;

   private:
    std::vector<EditActions> undo_actions_;
    std::vector<EditActions> redo_actions_;
    mutable std::shared_mutex mutex_;
    std::vector<pdfcsp::pdf::CAnnotParams> c_annot_params_;
};

}  // namespace core::gui

#endif  // HISTORY_MANAGER_HPP
