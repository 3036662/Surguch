#ifndef TREE_ITEM_HPP
#define TREE_ITEM_HPP

#include <QUuid>
#include <QVariantList>
#include <memory>

struct FileData {
    bool has_check_result = false;
    int id = 0;
    int size = 0;
    int last_modified = 0;
    int ref_id_size = 0;
    int mrpa_id_size = 0;
    std::vector<int> ref_ids;
    std::vector<int> mrpa_ids;
    QString name;
    QString type;
    QString full_path;
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

    void deleteItem(QUuid id);

    void deleteChildren();

   private:
    std::vector<std::shared_ptr<TreeItem>> child_items_;
    FileData file_data_;
    TreeItem *parent_item_;
    QUuid uid_;
};

#endif  // TREE_ITEM_HPP
