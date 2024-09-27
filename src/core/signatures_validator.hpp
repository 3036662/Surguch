#ifndef SIGNATUES_VALIDATOR_HPP
#define SIGNATUES_VALIDATOR_HPP

#include <QObject>
#include "raw_signature.hpp"


namespace core{

class SignaturesValidator : public QObject
{
    Q_OBJECT
public:
    explicit SignaturesValidator(QObject *parent = nullptr);

    ~SignaturesValidator() override;

public slots:
    void validateSignatures(std::vector<core::RawSignature> raw_signatures_);

signals:
    void validationFinished();
};

}// namespace core

#endif // SIGNATUES_VALIDATOR_HPP
