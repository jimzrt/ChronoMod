#include "resourceentryproxymodel.h"
#include "resourceentrymodel.h"

ResourceEntryProxyModel::ResourceEntryProxyModel(QObject* parent)
    : QSortFilterProxyModel { parent }
    , currentSearchString("")
{
}

void ResourceEntryProxyModel::setCurrentSearchString(const QString& searchString)
{
    if (searchString == currentSearchString) {
        return;
    }
    currentSearchString = searchString;
    invalidate();
}

void ResourceEntryProxyModel::setOnlyModified(int state)
{
    if (onlyModified == state) {
        return;
    }
    onlyModified = state;
    invalidate();
}

bool ResourceEntryProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{

    if (!onlyModified && currentSearchString.isEmpty()) {
        return true;
    };
    bool hasModification = sourceModel()->index(source_row, 0).data(EntryHasModificationRole).toBool();
    if (onlyModified && currentSearchString.isEmpty()) {
        return hasModification;
    }
    bool containsSearchString = sourceModel()->index(source_row, 2, source_parent).data().toString().toLower().contains(currentSearchString.toLower());
    if (!onlyModified) {
        return containsSearchString;
    }
    return containsSearchString && hasModification;
}

bool ResourceEntryProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    if (left.column() == 0 && right.column() == 0) {
        return sourceModel()->data(left).toUInt() < sourceModel()->data(right).toUInt();
    }
    if (left.column() == 1 && right.column() == 1) {
        return sourceModel()->data(left, Qt::UserRole).toUInt() < sourceModel()->data(right, Qt::UserRole).toUInt();
    }
    if (left.column() == 2 && right.column() == 2) {
        return QString::localeAwareCompare(sourceModel()->data(left).toString(), sourceModel()->data(right).toString()) < 0;
    }

    return true;
}

//QVariant RessourceEntryProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    return sourceModel()->headerData(section, orientation, role);
//}
