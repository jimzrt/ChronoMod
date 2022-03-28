#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Patch.h"
#include "WorkerThread.h"
#include "resourcebin.h"
#include "resourceentry.h"
#include "resourceentrymodel.h"

#include <QFutureWatcher>
#include <QMainWindow>
#include <QSettings>
#include <QTemporaryDir>

QT_BEGIN_NAMESPACE
namespace Ui {

class MainWindow;
}
QT_END_NAMESPACE

enum class PatchLoaded {
    Manual,
    FromPatch
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    WorkerThread workerThread;

    void reload();
signals:
    void ressourceBin_loaded();
    void patchLoaded(PatchLoaded);
    void modificationUnset();
    void error_message(QString errorMessage);

private slots:
    void on_actionOpen_Archive_triggered();

    void on_actionExtract_All_triggered();

    void on_actionLoad_Patch_triggered();

    void on_actionSave_triggered();

    void on_actionReplace_Font_triggered();

    void on_actionSave_Patch_triggered();

    void on_actionApply_Defilter_triggered();

    void on_actionCheck_for_Updates_triggered();

private:
    Ui::MainWindow* ui;
    std::vector<uint32_t> decryption_key;
    void open_archives(const QString& chronoFileName, const QString& resourceBinFileName);
    // std::vector<RessourceBinHeaderEntry> entry_list;
    std::shared_ptr<ResourceBin> ressourceBin;
    void extract_entries(const std::vector<ResourceEntry>& entries);
    // QFutureWatcher<void> taskWatcher;
    std::unique_ptr<QThread> qthread;
    std::unordered_map<std::string, Patch> patchMap;
    QTemporaryDir tempDir;
    QPixmap previewImage1;
    QPixmap previewImage2;
    void loadRessourceBin(const QString& filepath);
    void hidePreviews();
    void refreshSelection();
    QSettings settings;
    ;
    void try_open_steambinaries();
    void resizePreviewImages();
    void replaceFont(ResourceEntry& fontEntry);
    void checkUpdate();
    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
};
#endif // MAINWINDOW_H
