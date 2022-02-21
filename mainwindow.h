#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Patch.h"
#include "WorkerThread.h"
#include "resourcebin.h"
#include "resourceentry.h"
#include "resourceentrymodel.h"

#include <QFutureWatcher>
#include <QMainWindow>
#include <QTemporaryDir>

QT_BEGIN_NAMESPACE
namespace Ui {

class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    WorkerThread workerThread;

    void reload();
signals:
    void ressourceBin_loaded();
    void patchLoaded();
    void modificationUnset();

private slots:
    void on_actionOpen_Archive_triggered();

    void on_actionExtract_All_triggered();

    void on_actionLoad_Patch_triggered();

    void on_actionSave_triggered();

private:
    Ui::MainWindow* ui;
    //std::vector<RessourceBinHeaderEntry> entry_list;
    std::shared_ptr<ResourceBin> ressourceBin;
    void extract_entries(const std::vector<ResourceEntry>& entries);
    //QFutureWatcher<void> taskWatcher;
    std::unique_ptr<QThread> qthread;
    std::unordered_map<std::string, Patch> patchMap;
    QTemporaryDir tempDir;
    void loadRessourceBin(const QString& filepath);
};
#endif // MAINWINDOW_H
