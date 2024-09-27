#include "signatures_list_model.hpp"
#include "core/signatures_validator.hpp"

SignaturesListModel::SignaturesListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    role_names_[SigInfoRole]="sigInfo";
    role_names_[CheckStatusRole]="checkStatus";
    role_names_[ValidRole]="valid";
    role_names_[EmptyRole]="empty";
}

QVariant SignaturesListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   return {};
}

int SignaturesListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()){
        return 0;
    }
    return raw_signatures_.size();
}




QVariant SignaturesListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row()>raw_signatures_.size()-1){
        return {};
    }
    switch(role){
        case SigInfoRole:
            return "Signature "+ QString::number(index.row());
            break;
        case CheckStatusRole:
            return false;
            break;
        case ValidRole:
            return false;
            break;
        case EmptyRole:
            return raw_signatures_.at(index.row()).getSigData().empty();
    }
    return {};
}

void SignaturesListModel::updateSigList(std::vector<core::RawSignature> sigs){
   beginResetModel();
   raw_signatures_=std::move(sigs);
   if (worker_thread_ && worker_thread_->isRunning()){
       worker_thread_->requestInterruption();
       worker_thread_->wait();
   }
   worker_thread_=std::make_unique<QThread>();
   core::SignaturesValidator* validator=new core::SignaturesValidator();
   validator->moveToThread(worker_thread_.get());
   QObject::connect(worker_thread_.get(), &QThread::started, [this,validator]() {
          emit validator->validateSignatures(raw_signatures_);
   });
   QObject::connect(worker_thread_.get(),
                    &QThread::finished, []{
       qWarning()<<"Finish recieved";
   });;
   QObject::connect(validator,&core::SignaturesValidator::validationFinished,[this]{
       qWarning()<<"Finished validation";
        worker_thread_->quit();
   });

   QObject::connect(worker_thread_.get(),
                    &QThread::finished, validator, &core::SignaturesValidator::deleteLater);

   worker_thread_->start();
   //TODO(Oleg) check if everything is deleted

   endResetModel();
}

QHash<int, QByteArray> SignaturesListModel::roleNames() const{
    return role_names_;
}

