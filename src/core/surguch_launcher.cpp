#include "surguch_launcher.hpp"

#include <QDebug>
#include <QProcess>
#include <QUrl>

namespace core {

SurguchLauncher::SurguchLauncher(QObject *parent) : QObject{parent} {}

void SurguchLauncher::launchSurguch(const QUrl& file){
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    auto *process = new QProcess();
    connect(process, &QProcess::finished,
                    [process] { process->deleteLater(); });
    //  error
    connect(process, &QProcess::errorOccurred,
            [process](QProcess::ProcessError err) {
                qWarning() << "something went wrong in launcher " << err;
                process->close();
                process->deleteLater();
            });
    process->setProgram(surguch_executable_);
    process->setArguments({file.toLocalFile()});
    process->start(surguch_executable_, {file.toLocalFile()});
}


}  // namespace core