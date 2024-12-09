#include "signatures_list_model.hpp"
#include "core/signatures_validator.hpp"
#include <QCoreApplication>

SignaturesListModel::SignaturesListModel(QObject *parent)
    : QAbstractListModel(parent) {
  role_names_[SigInfoRole] = "sigInfo";
  role_names_[CheckStatusRole] = "checkStatus";
  role_names_[ValidRole] = "valid";
  role_names_[EmptyRole] = "empty";
  role_names_[SigData] = "sigData";
}

QVariant SignaturesListModel::headerData(int /*section*/,
                                         Qt::Orientation /*orientation*/,
                                         int /*role*/) const {
  return {};
}

int SignaturesListModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }
  return static_cast<int>(raw_signatures_.size());
}

QVariant SignaturesListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() > raw_signatures_.size() - 1) {
    return {};
  }
  switch (role) {
  case SigInfoRole:
    if (validation_results_.count(index.row()) > 0) {
      return validation_results_.at(index.row())->subj_common_name;
    }
    return "Signature " + QString::number(index.row());
    break;
  case CheckStatusRole:
    if (validation_results_.count(index.row()) > 0) {
      return true;
    }
    return false;
    break;
  case ValidRole:
    if (validation_results_.count(index.row()) > 0) {
      return validation_results_.at(index.row())->bres.check_summary;
    }
    return false;
    break;
  case EmptyRole:
    return raw_signatures_.at(index.row()).getSigData().empty();
    break;
  case SigData:
    if (validation_results_.count(index.row()) > 0) {
      return validation_results_.at(index.row())->toJson();
    }
    break;
  default:
    return {};
  }
  return {};
}

void SignaturesListModel::updateSigList(std::vector<core::RawSignature> sigs,
                                        const QString &file_source) {
  beginResetModel();
  validation_results_.clear();
  raw_signatures_ = std::move(sigs);
  QThread *worker_thread = nullptr;
  core::SignaturesValidator *validator = nullptr;
  if (!worker_threads_.empty() && curr_worker_index_ < worker_threads_.size() &&
      curr_worker_index_ < validators_.size()) {
    worker_thread = worker_threads_[curr_worker_index_].get();
    validator = validators_[curr_worker_index_].get();
  }
  if (worker_thread != nullptr && worker_thread->isRunning()) {
    if (validator != nullptr) {

      validator->abort();
    }
    worker_thread->requestInterruption();
    // worker_thread_->wait();
  }
  // qThreads and validator objects are stored in array, so it possible not to
  // wait for results if the document changed, just create a new thred and run
  // the verification
  worker_threads_.emplace_back(std::make_unique<QThread>());
  ++curr_worker_index_ = worker_threads_.size() - 1;
  worker_thread = worker_threads_[curr_worker_index_].get();
  qWarning() << "new worker_thread_ " << worker_thread;
  validators_.emplace_back(std::make_unique<core::SignaturesValidator>());
  validator = validators_[curr_worker_index_].get();
  validator->moveToThread(worker_thread);

  QObject::connect(
      QCoreApplication::instance(), &QCoreApplication::aboutToQuit, [this] {
        for (size_t i = 0; i < worker_threads_.size(); ++i) {
          if (worker_threads_[i] && worker_threads_[i]->isRunning()) {
            worker_threads_[i]->requestInterruption();
            worker_threads_[i]->wait();
          }
          if (i < validators_.size() && validators_[i]) {
            validators_[i]->abort();
          }
        }
      });

  QObject::connect(
      worker_thread, &QThread::started, [validator, file_source, this]() {
        validator->validateSignatures(raw_signatures_, file_source);
      });

  QObject::connect(
      validator, &core::SignaturesValidator::validationFinished,
      [this,
       worker_thread](core::DocStatusEnum::CommonDocCoverageStatus status) {
        if (curr_worker_index_ < worker_threads_.size() &&
            worker_thread == worker_threads_[curr_worker_index_].get()) {
          emit commonDocStatus(status);
        }
        qWarning() << "Finished validation";
        worker_thread->quit();
      });

  QObject::connect(
      validator, &core::SignaturesValidator::validatationResult,
      [this,
       worker_thread](std::shared_ptr<core::ValidationResult> validation_result,
                      size_t ind) {
        if (curr_worker_index_ < worker_threads_.size() &&
            worker_thread == worker_threads_[curr_worker_index_].get()) {
          qWarning() << "recieved validation result from validator"
                     << validators_[curr_worker_index_].get();
          saveValidationResult(std::move(validation_result), ind);
        }
      });

  QObject::connect(
      validator, &core::SignaturesValidator::validationFailedForSignature,
      [this, worker_thread](size_t ind) {
        if (curr_worker_index_ < worker_threads_.size() &&
            worker_thread == worker_threads_[curr_worker_index_].get()) {
          qWarning() << "validation failed for signature " << ind
                     << " from validator"
                     << validators_[curr_worker_index_].get();
          emit validationFailedForSignature(ind);
        }
      });

  // cleanup all threads that are finished
  for (size_t i = 0; i < worker_threads_.size(); ++i) {
    if (i != curr_worker_index_) {
      if (worker_threads_[i] && worker_threads_[i]->isFinished()) {
        if (curr_worker_index_ < validators_.size() && validators_[i]) {
          validators_[i].reset();
        }
      }
    }
  }
  worker_thread->start();
  endResetModel();
}

QHash<int, QByteArray> SignaturesListModel::roleNames() const {
  return role_names_;
}

void SignaturesListModel::saveValidationResult(
    std::shared_ptr<core::ValidationResult> validation_result, size_t ind) {
  validation_results_[ind] = std::move(validation_result);
  const QModelIndex qInd = index(static_cast<int>(ind), 0);
  qWarning() << "recieved validation result for index " << ind;
  emit dataChanged(qInd, qInd);
}

void SignaturesListModel::recoverDoc(qint64 sig_index) {
  const char *const expl = "[SignaturesListModel] recover doc failed";
  if (validation_results_.count(sig_index) == 0 ||
      !validation_results_.at(sig_index)->can_be_casted_to_full_coverage) {
    qWarning() << expl;
    return;
  }
  const core::RangesVector &branges =
      validation_results_.at(sig_index)->byteranges;
  const QString &file_path = validation_results_.at(sig_index)->file_path;
  if (branges.size() != 2 || file_path.isEmpty()) {
    qWarning() << expl;
    return;
  }
  if (recover_worker_ != nullptr || recover_thread_ != nullptr) {
    qWarning() << "recoverDoc is alreary running";
    return;
  }
  recover_worker_ = new core::FileRecoverWorker();
  recover_thread_ = new QThread();
  recover_worker_->moveToThread(recover_thread_);
  // start job
  QObject::connect(
      recover_thread_, &QThread::started, [file_path, branges, this]() {
        recover_worker_->recoverFileWithByteRange(file_path, branges);
      });
  // app closed
  QObject::connect(
      QCoreApplication::instance(), &QCoreApplication::aboutToQuit, [this]() {
        if (recover_thread_ != nullptr && recover_thread_->isRunning()) {
          recover_worker_->abort();
          recover_thread_->requestInterruption();
          recover_thread_->wait();
        }
      });
  // job is completed
  QObject::connect(recover_worker_, &core::FileRecoverWorker::recoverCompleted,
                   [this](const QString &res) {
                     if (!res.isEmpty()) {
                       emit fileRecovered(res);
                     }
                     recover_thread_->quit();
                   });
  // thread is finished
  QObject::connect(recover_thread_, &QThread::finished, [this]() {
    recover_worker_->deleteLater();
    recover_thread_->deleteLater();
    recover_worker_ = nullptr;
    recover_thread_ = nullptr;
  });
  recover_thread_->start();
}
