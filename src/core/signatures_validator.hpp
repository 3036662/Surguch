#ifndef SIGNATUES_VALIDATOR_HPP
#define SIGNATUES_VALIDATOR_HPP

#include "csp_response.hpp"
#include "raw_signature.hpp"
#include <QObject>
#include <memory>

namespace core {

class SignaturesValidator : public QObject {
  Q_OBJECT
public:
  // explicit SignaturesValidator(QObject *parent = nullptr);

  ~SignaturesValidator() override = default;

  void abort() { abort_recieved_ = true; };

public slots:
  void validateSignatures(std::vector<core::RawSignature> raw_signatures_,
                          QString file_source);

signals:
  void validationFinished();

  void validatationResult(std::shared_ptr<CSPResponse> validation_result,
                          size_t index);

private:
  bool abort_recieved_ = false;
};

} // namespace core

#endif // SIGNATUES_VALIDATOR_HPP
