#pragma once

#include <QFontDatabase>
#include <QObject>
#include <QStringList>

namespace core {

class FontHelper : public QObject {
    Q_OBJECT
   public:
    explicit FontHelper(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE QStringList cyrillicFamilies() {
        return QFontDatabase::families(QFontDatabase::Cyrillic);
    }
};

}  // namespace core
