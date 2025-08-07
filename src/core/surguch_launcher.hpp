#ifndef SURGUCH_LAUNCHER_HPP
#define SURGUCH_LAUNCHER_HPP

#include <QObject>

namespace core {

/// @brief helper class to open multiply windows when more than 1 file selected
/// to open
class SurguchLauncher : public QObject {
    Q_OBJECT

   public:
    explicit SurguchLauncher(QObject* parent = nullptr);

    /*!
     * \brief Launch new surguch with selected file
     * \param file - pdf file to open
     */
    Q_INVOKABLE void launchSurguch(const QUrl& file);

   private:
    const QString surguch_executable_ =
        "/home/dv/surguch/surguch/cmake-build-debug/src/surguch";
};

}  // namespace core

#endif
