#pragma once

#include <QFontDatabase>
#include <QObject>
#include <QStringList>

namespace core {

/// @brief utility class to get list of fonts which support Cyrillic
class FontHelper : public QObject {
    Q_OBJECT
   public:
    explicit FontHelper(QObject *parent = nullptr) : QObject(parent) {}

    static Q_INVOKABLE QStringList cyrillicFamilies() {
        return QFontDatabase::families(QFontDatabase::Cyrillic);
    }
};

}  // namespace core
