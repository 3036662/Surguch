#include "tree_item.hpp"

TreeItem::TreeItem(QVariantList data, TreeItem *parent)
    : item_data_(std::move(data)), parent_item_(parent)
{}

void TreeItem::appendChild(std::unique_ptr<TreeItem> &&child)
{
    child_items_.push_back(std::move(child));
}

TreeItem *TreeItem::child(int row)
{
    return row >= 0 && row < childCount() ? child_items_.at(row).get() : nullptr;
}

int TreeItem::childCount() const
{
    return int(child_items_.size());
}

int TreeItem::columnCount() const
{
    return int(item_data_.count());
}

QVariant TreeItem::data(int column) const
{
    return item_data_.value(column);
}

TreeItem *TreeItem::parentItem()
{
    return parent_item_;
}

int TreeItem::row() const
{
    if (parent_item_ == nullptr) {
        return 0;

    }
    const auto it = std::find_if(parent_item_->child_items_.cbegin(), parent_item_->child_items_.cend(),
                                 [this](const std::unique_ptr<TreeItem> &treeItem) {
                                     return treeItem.get() == this;
                                 });

    if (it != parent_item_->child_items_.cend()) {
        return std::distance(parent_item_->child_items_.cbegin(), it);
    }
    return -1;
}

