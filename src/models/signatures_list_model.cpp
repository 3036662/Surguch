#include "signatures_list_model.hpp"

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
   endResetModel();
}

QHash<int, QByteArray> SignaturesListModel::roleNames() const{
    return role_names_;
}

