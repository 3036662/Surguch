#ifndef CUSTOMEVENTFILTER_HPP
#define CUSTOMEVENTFILTER_HPP

#include <QEvent>
#include <QObject>
#include <QWheelEvent>

namespace core {

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

   protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::Wheel) {
            QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
            qWarning() << "Wheel event detected on object:" << obj
                       << "Delta:" << wheelEvent->angleDelta()
                       << "Position:" << wheelEvent->position()
                       << "Global position:" << wheelEvent->globalPosition();
        }
        return QObject::eventFilter(obj, event);
    }
};

}  // namespace core

#endif  // CUSTOMEVENTFILTER_HPP
