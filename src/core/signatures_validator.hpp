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

    void abort(){abort_recieved_=true;};

public slots:
    void validateSignatures(std::vector<core::RawSignature> raw_signatures_,QString file_source);

signals:
    void validationFinished();

private:
    bool abort_recieved_=false;
};

}// namespace core

#endif // SIGNATUES_VALIDATOR_HPP
