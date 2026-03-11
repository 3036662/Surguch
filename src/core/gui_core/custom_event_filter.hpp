/* File: custom_event_filter.hpp
Copyright (C) Basealt LLC,  2025
Author: Oleg Proskurin, <proskurinov@basealt.ru>

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef CUSTOMEVENTFILTER_HPP
#define CUSTOMEVENTFILTER_HPP

#include <QCoreApplication>
#include <QEvent>
#include <QObject>
#include <QWheelEvent>

namespace core::gui {

/**
 * @brief The EventFilterInstaller class installs the given event filter to the
 * given QObject
 */
class EventFilterInstaller : public QObject {
    Q_OBJECT
   public:
    /// @brief install the filter
    static Q_INVOKABLE void installEventFilter(QObject *target,
                                               QObject *filter) {
        target->installEventFilter(filter);
    }
};

/**
 * @brief The WheelEventFilter class rethrows the will event to the given target
 * @details utilized to fix Qt6.4 bug, when the wheel event is not propagate to
 * the nested popup
 */
class WheelEventFilter : public QObject {
    Q_OBJECT
   public:
    explicit WheelEventFilter(QObject *parent = nullptr) : QObject(parent) {}

    /// @brief the event will be dispatched to target if set to true
    Q_PROPERTY(bool dispatch_to_target MEMBER dispatch_to_target_);

    /// @brief set the target
    Q_INVOKABLE void setTargetForDispatch(QObject *target) {
        if (target == nullptr) {
            dispatch_to_target_ = false;
            return;
        }
        dispatch_target_ = target;
    }

    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::Wheel) {
            // NOLINTNEXTLINE
            auto *wheelEvent = static_cast<QWheelEvent *>(event);
        }
        if (dispatch_to_target_) {
            qDebug() << "Sent event to target:" << dispatch_target_;
            QCoreApplication::sendEvent(dispatch_target_, event);
        }
        return QObject::eventFilter(obj, event);
    }

   private:
    bool dispatch_to_target_ = false;
    QObject *dispatch_target_ = nullptr;
};

}  // namespace core::gui

#endif  // CUSTOMEVENTFILTER_HPP
