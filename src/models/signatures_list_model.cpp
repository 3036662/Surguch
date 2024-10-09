#include "signatures_list_model.hpp"
#include "core/signatures_validator.hpp"
#include <QCoreApplication>

SignaturesListModel::SignaturesListModel(QObject *parent)
    : QAbstractListModel(parent) {
  role_names_[SigInfoRole] = "sigInfo";
  role_names_[CheckStatusRole] = "checkStatus";
  role_names_[ValidRole] = "valid";
  role_names_[EmptyRole] = "empty";
  role_names_[SigData]="sigData";
}

QVariant SignaturesListModel::headerData(int section,
                                         Qt::Orientation orientation,
                                         int role) const {
  return {};
}

int SignaturesListModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }
  return raw_signatures_.size();
}

QVariant SignaturesListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() > raw_signatures_.size() - 1) {
    return {};
  }
  switch (role) {
  case SigInfoRole:
    if (validation_results_.count(index.row())>0){
        return validation_results_.at(index.row())->subj_common_name;
    }
    return "Signature " + QString::number(index.row());
    break;
  case CheckStatusRole:
    if (validation_results_.count(index.row())>0){
          return true;
    }
    return false;
    break;
  case ValidRole:
    if (validation_results_.count(index.row())>0){
            return validation_results_.at(index.row())->bres.check_summary;
    }
    return false;
    break;
  case EmptyRole:
    return raw_signatures_.at(index.row()).getSigData().empty();
    break;
  case SigData:
    if (validation_results_.count(index.row())>0){
        return validation_results_.at(index.row())->toJson();
    }
    break;
  }
  return {};
}

void SignaturesListModel::updateSigList(std::vector<core::RawSignature> sigs,
                                        QString file_source) {
  beginResetModel();
  validation_results_.clear();
  raw_signatures_ = std::move(sigs);
  if (worker_thread_ != nullptr && worker_thread_->isRunning()) {
    validator_->abort();
    worker_thread_->requestInterruption();
    // worker_thread_->wait();
  }
  worker_thread_ = new QThread();
  validator_ = new core::SignaturesValidator();
  validator_->moveToThread(worker_thread_);

  QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                   [this] {
                     validator_->abort();
                     worker_thread_->requestInterruption();
                     worker_thread_->wait();
                   });

  QObject::connect(worker_thread_, &QThread::started, [this, file_source]() {
    emit validator_->validateSignatures(raw_signatures_, file_source);
  });
  QObject::connect(validator_, &core::SignaturesValidator::validationFinished,
                   [this] {
                     qWarning() << "Finished validation";
                     worker_thread_->quit();
                   });

  QObject::connect(validator_, &core::SignaturesValidator::validatationResult,
                   this, &SignaturesListModel::saveValidationResult);

  QObject::connect(worker_thread_, &QThread::finished, validator_,
                   &core::SignaturesValidator::deleteLater);

  QObject::connect(worker_thread_, &QThread::finished, worker_thread_,
                   &QThread::deleteLater);

  QObject::connect(worker_thread_, &QThread::finished, [this] {
    worker_thread_ = nullptr;
    qWarning() << "Thread finish recieved";
  });
  ;

  worker_thread_->start();
  // TODO(Oleg) check if everything is deleted

  endResetModel();
}

QHash<int, QByteArray> SignaturesListModel::roleNames() const {
  return role_names_;
}

void SignaturesListModel::saveValidationResult(
    std::shared_ptr<core::CSPResponse> validation_result, size_t ind) {
  validation_results_[ind] = std::move(validation_result);
  const QModelIndex qInd = index(static_cast<int>(ind), 0);
  qWarning() << "recieved validation result for index " << ind;
  emit dataChanged(qInd, qInd);
}
