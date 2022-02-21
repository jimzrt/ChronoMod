#ifndef RESOURCEENTRYMODEL_H
#define RESOURCEENTRYMODEL_H

#include "resourceentry.h"

#include <QAbstractTableModel>
#include <QLocale>

enum ResourceEntryModelRole {
    EntryHasModificationRole = Qt::UserRole + 1

};

class ResourceEntryModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit ResourceEntryModel(QObject* parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void setModelData(std::vector<ResourceEntry>* ressource_entry_list);

private:
    std::vector<ResourceEntry>* ressource_entry_list = nullptr;
    //std::vector<RessourceEntry> empty_entry_list;
    QLocale locale;
};

#endif // RESOURCEENTRYMODEL_H
