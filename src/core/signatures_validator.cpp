#include "signatures_validator.hpp"
#include <QDebug>
#include <QThread>

namespace core {

// SignaturesValidator::SignaturesValidator(QObject *parent) : QObject{parent} {
//   qWarning() << "Validator constructed";
// }

void SignaturesValidator::validateSignatures(
    std::vector<core::RawSignature> raw_signatures_,QString file_source) {
  bool aborted = false;
  for (size_t i = 0; i < raw_signatures_.size(); ++i) {
    const auto &sig = raw_signatures_[i];
    if (abort_recieved_ ||
        QThread::currentThread()->isInterruptionRequested()) {
      qWarning() << "Validation abort";
      aborted = true;
      break;
    }
    qWarning() << "validating signatures " << raw_signatures_.size();
    std::shared_ptr<CSPResponse> result;
    // do not parse empty sigbatures
    if (sig.getSigData().empty() || sig.getByteRanges().empty()){
        continue;
    }
    try {
      result = std::make_shared<CSPResponse>(sig, file_source.toStdString());
    } catch (const std::exception &ex) {
      qWarning() << ex.what();
    }
    if (result) {
      emit validatationResult(result, i);
    }
  }
  if (aborted) {
    qWarning() << "quit without sending results";
    QThread::currentThread()->quit();
  } else {
    emit validationFinished();
  }
}

} // namespace core
