#include "resourceentrymodel.h"

#include <QFont>

ResourceEntryModel::ResourceEntryModel(QObject* parent)
    : QAbstractTableModel(parent)

{
}

void ResourceEntryModel::setModelData(std::vector<ResourceEntry>* ressource_entry_list)
{
    beginResetModel();
    this->ressource_entry_list = ressource_entry_list;
    endResetModel();
}

QVariant ResourceEntryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return {};
    }

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Index");
        case 1:
            return tr("Size");
        case 2:
            return tr("Path");
        default:
            return {};
        }
    }
    return {};
}

int ResourceEntryModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    if (ressource_entry_list == nullptr) {
        return 0;
    }
    return ressource_entry_list->size();
}

int ResourceEntryModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant ResourceEntryModel::data(const QModelIndex& index, int role) const
{

    if (!index.isValid()) {
        return {};
    }
    if (index.row() >= (int)ressource_entry_list->size() || index.row() < 0) {
        return {};
    }
    if (role == Qt::DisplayRole) {
        if (ressource_entry_list == nullptr) {
            return {};
        }
        const auto& entry = ressource_entry_list->at(index.row());
        switch (index.column()) {
        case 0:
            return index.row();
        case 1:
            return locale.formattedDataSize(entry.entry_length);
        case 2:
            return QString::fromStdString(entry.path);
        default:
            return {};
        }
    }
    if (role == Qt::FontRole) {
        if (ressource_entry_list == nullptr) {
            return {};
        }
        const auto& entry = ressource_entry_list->at(index.row());
        if (entry.hasReplacement) {
            QFont font;
            font.setBold(true);
            return font;
        }
    }
    if (role == EntryHasModificationRole) {
        const auto& entry = ressource_entry_list->at(index.row());
        return entry.hasReplacement;
    }
    if (role == Qt::UserRole && index.column() == 1) {
        const auto& entry = ressource_entry_list->at(index.row());
        return entry.entry_length;
    }

    if (role == Qt::TextAlignmentRole) {
        return { Qt::AlignLeft | Qt::AlignVCenter };
    }
    if (role == Qt::ToolTipRole) {
        const auto& entry = ressource_entry_list->at(index.row());
        return QString::fromStdString(entry.path);
    }
    return {};
}
