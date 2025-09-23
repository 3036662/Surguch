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

    void deleteItem(QUuid id);

    void deleteChildren();

   private:
    std::vector<std::shared_ptr<TreeItem>> child_items_;
    FileData file_data_;
    TreeItem *parent_item_;
    QUuid uid_;
};

#endif  // TREE_ITEM_HPP
