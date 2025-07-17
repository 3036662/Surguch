#ifndef CUSTOMEVENTFILTER_HPP
#define CUSTOMEVENTFILTER_HPP

#include <QCoreApplication>
#include <QEvent>
#include <QObject>
#include <QWheelEvent>

namespace core::gui {

class EventFilterInstaller : public QObject {
    Q_OBJECT
   public:
    Q_INVOKABLE void installEventFilter(QObject *target, QObject *filter) {
        target->installEventFilter(filter);
    }
};

class WheelEventFilter : public QObject {
    Q_OBJECT
   public:
    explicit WheelEventFilter(QObject *parent = nullptr) : QObject(parent) {}

    Q_PROPERTY(bool dispactch_to_target MEMBER dispatch_to_target_);

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
