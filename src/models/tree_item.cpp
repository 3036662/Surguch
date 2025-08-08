#include "tree_item.hpp"

TreeItem::TreeItem(FileData data, QUuid id,TreeItem *parent)
    : file_data_(std::move(data)), uid_(id), parent_item_(parent) {}

void TreeItem::appendChild(std::shared_ptr<TreeItem> &&child) {
    child_items_.push_back(std::move(child));
}

TreeItem *TreeItem::child(int row) {
    return row >= 0 && row < childCount() ? child_items_.at(row).get()
                                          : nullptr;
}

int TreeItem::childCount() const { return int(child_items_.size()); }

int TreeItem::columnCount() const { return 5; }

TreeItem *TreeItem::parentItem() const { return parent_item_; }

int TreeItem::row() const {
    if (parent_item_ == nullptr) {
        return 0;
    }
    const auto it = std::find_if(
        parent_item_->child_items_.cbegin(), parent_item_->child_items_.cend(),
        [this](const std::shared_ptr<TreeItem> &treeItem) {
            return treeItem.get() == this;
        });

    if (it != parent_item_->child_items_.cend()) {
        return std::distance(parent_item_->child_items_.cbegin(), it);
    }
    return -1;
}

FileData TreeItem::data() const{
    return file_data_;
}


QUuid TreeItem::uid() const{
    return uid_;
}


void TreeItem::deleteItem(QUuid id) {
    child_items_.erase(
        std::remove_if(child_items_.begin(), child_items_.end(),
                       [id](const std::shared_ptr<TreeItem> &item_ptr) {
                           return item_ptr->uid_ == id;
                       }),
        child_items_.end());
}

void TreeItem::deleteChildren(){
    child_items_.clear();
}

