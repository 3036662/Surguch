#ifndef TREE_ITEM_HPP
#define TREE_ITEM_HPP

#include <QList>
#include <QVariant>

#include <memory>


class TreeItem {
public:
    explicit TreeItem(QVariantList data,TreeItem *parentItem = nullptr);

    void appendChild(std::unique_ptr<TreeItem> &&child);

    TreeItem *child(int row);
    [[nodiscard]] int childCount() const;
    [[nodiscard]] int columnCount() const;
    [[nodiscard]] QVariant data(int column) const;
    [[nodiscard]] int row() const;
    TreeItem *parentItem();

    QVariant description() const { return m_description; }
    void setDescription(const QVariant &desc) { m_description = desc; }

    QString id() const {
        // Assuming ID is stored in column 4 (adjust as needed)
        return item_data_.size() > 4 ? item_data_[4].toString() : QString();
    }

private:
    std::vector<std::unique_ptr<TreeItem>> child_items_;
    QVariantList item_data_;
    TreeItem *parent_item_;
    QVariant m_description;
};





#endif //TREE_ITEM_HPP
