/* File: tree_item.hpp
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

#ifndef TREE_ITEM_HPP
#define TREE_ITEM_HPP

#include <QJsonObject>
#include <QUuid>
#include <QVariantList>
#include <memory>

// possible node types
enum NodeTypes {
    Zip,
    Dir,
    File,
    Sig,
    Asig,
    Mrpa,
};

struct CheckResult {
    int file_id = 0;
    bool check_summary = false;
};

// struct to hold all needed data about node from libmrpa
struct FileData {
    NodeTypes type;
    bool encrypted = false;
    int id = 0;
    int size = 0;
    int last_modified = 0;
    int ref_id_size = 0;
    int mrpa_id_size = 0;
    std::vector<int> ref_ids;
    std::vector<int> mrpa_ids;
    QString name;
    QString full_path;

    // most of nodes could have relations with MRPA or Signs
    std::optional<bool> has_check_result = false;
    std::vector<CheckResult> check_results;
    QString sig_text;
    QString sig_color;
    QString mrpa_text;
    QString mrpa_color;

    // MRPA specific fields
    std::optional<bool> time_valid = false;
    std::optional<QJsonObject> mrpa_data;
};

class TreeItem {
   public:
    explicit TreeItem(FileData data, QUuid uid, TreeItem *parentItem = nullptr);

    void appendChild(std::shared_ptr<TreeItem> &&child);

    TreeItem *child(int row);
    [[nodiscard]] int childCount() const;
    [[nodiscard]] int columnCount() const;
    [[nodiscard]] int row() const;
    [[nodiscard]] TreeItem *parentItem() const;
    [[nodiscard]] QUuid uid() const;
    [[nodiscard]] FileData data() const;
    [[nodiscard]] bool contains(const QString &full_path) const;

    void setSigStats(const QString &text, const QString &color);
    void setMrpaStats(const QString &text, const QString &color);

    /// @brief delete item
    void deleteItem(QUuid id);

    /// @brief delete items childs
    void deleteChildren();

   private:
    std::vector<std::shared_ptr<TreeItem>> child_items_;
    FileData file_data_;
    TreeItem *parent_item_;
    QUuid uid_;
};

#endif  // TREE_ITEM_HPP
