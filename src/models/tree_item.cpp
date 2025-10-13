/* File: tree_item.cpp
Copyright (C) Basealt LLC,  2025
Author: Daniil-Viktor Ratkin, <ratkinda@basealt.ru>

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "tree_item.hpp"

TreeItem::TreeItem(FileData data, QUuid id, TreeItem *parent)
    : file_data_(std::move(data)), uid_(id), parent_item_(parent) {}

void TreeItem::appendChild(std::shared_ptr<TreeItem> &&child) {
    child_items_.push_back(std::move(child));
}

TreeItem *TreeItem::child(int row) {
    return row >= 0 && row < childCount() ? child_items_.at(row).get()
                                          : nullptr;
}

int TreeItem::childCount() const { return int(child_items_.size()); }

int TreeItem::columnCount() const { return 1; }

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

FileData TreeItem::data() const { return file_data_; }

QUuid TreeItem::uid() const { return uid_; }

bool TreeItem::contains(const QString &full_path) const {
    return std::any_of(child_items_.begin(), child_items_.end(),
                       [&full_path](const std::shared_ptr<TreeItem> &treeItem) {
                           return full_path == treeItem->data().full_path;
                       });
}

void TreeItem::setSigStats(const QString &text, const QString &color) {
    file_data_.sig_text = text;
    file_data_.sig_color = color;
}

void TreeItem::setMrpaStats(const QString &text, const QString &color) {
    file_data_.mrpa_text = text;
    file_data_.mrpa_color = color;
}

void TreeItem::deleteItem(QUuid id) {
    child_items_.erase(
        std::remove_if(child_items_.begin(), child_items_.end(),
                       [id](const std::shared_ptr<TreeItem> &item_ptr) {
                           return item_ptr->uid_ == id;
                       }),
        child_items_.end());
}

void TreeItem::deleteChildren() { child_items_.clear(); }
