#ifndef TREE_ITEM_HPP
#define TREE_ITEM_HPP

#include <QUuid>
#include <QVariantList>
#include <QJsonObject>
#include <memory>

struct CheckResult {
    uint32_t file_id = 0;
    bool check_summary = false;
};

struct FileData {
    bool encrypted = false;
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
    std::optional<bool> has_check_result = false;
    std::optional<std::vector<CheckResult>> check_results;
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

    void deleteItem(QUuid id);

    void deleteChildren();

   private:
    std::vector<std::shared_ptr<TreeItem>> child_items_;
    FileData file_data_;
    TreeItem *parent_item_;
    QUuid uid_;
};

#endif  // TREE_ITEM_HPP
