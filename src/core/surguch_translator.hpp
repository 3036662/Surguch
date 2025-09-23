#ifndef SURGUCH_TRANSLATOR_HPP
#define SURGUCH_TRANSLATOR_HPP

#include <QObject>

namespace core {

class SurguchTranslator : public QObject {
    Q_OBJECT
   public:
    explicit SurguchTranslator(QObject *parent = nullptr);

    Q_INVOKABLE QString surguchTranslate(const QString &str) const;
};

}  // namespace core

#endif  // SURGUCH_TRANSLATOR_HPP
