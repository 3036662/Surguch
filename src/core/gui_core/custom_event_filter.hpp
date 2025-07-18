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
    Q_INVOKABLE void installEventFilter(QObject *target, QObject *filter) {
        target->installEventFilter(filter);
    }
};

/**
 * @brief The WheelEventFilter class rethrows the will event to the given turget
 * @details utilized to fix Qt6.4 bug, when the wheel event is not propagate to
 * the nested popup
 */
class WheelEventFilter : public QObject {
    Q_OBJECT
   public:
    explicit WheelEventFilter(QObject *parent = nullptr) : QObject(parent) {}

    /// @brief the event will be dispatched to target if set to true
    Q_PROPERTY(bool dispactch_to_target MEMBER dispatch_to_target_);

    /// @brief set the tatget
    Q_INVOKABLE void setTargetForDispatch(QObject *target) {
        if (target == nullptr) {
            dispatch_to_target_ = false;
            return;
        }
        dispatch_target_ = target;
    }

   protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::Wheel) {
            auto *wheelEvent = static_cast<QWheelEvent *>(event);
            qDebug() << "Wheel event detected on object:" << obj
                     << "Delta:" << wheelEvent->angleDelta()
                     << "Position:" << wheelEvent->position()
                     << "Global position:" << wheelEvent->globalPosition();
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
