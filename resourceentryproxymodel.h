#ifndef RESOURCEENTRYPROXYMODEL_H
#define RESOURCEENTRYPROXYMODEL_H

#include <QItemSelection>
#include <QSortFilterProxyModel>

class ResourceEntryProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit ResourceEntryProxyModel(QObject* parent = nullptr);
    bool filterAcceptsRow(int source_row,
        const QModelIndex& source_parent) const override;
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
    //    QVariant headerData(int section, Qt::Orientation orientation,
    //                        int role) const;

public slots:
    void setCurrentSearchString(const QString& searchString);
    void setOnlyModified(int state);

private:
    QString currentSearchString;
    int onlyModified = 0;
};

#endif // RESOURCEENTRYPROXYMODEL_H
