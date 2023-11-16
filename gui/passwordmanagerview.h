#ifndef PASSWORDMANAGERVIEW_H
#define PASSWORDMANAGERVIEW_H

#include <QMainWindow>
#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QListView>
#include <QStandardItemModel>
#include <QScrollArea>
#include "dataentrywidget.h"
#include "dataentrymodulator.h"
#include "settingsdialog.h"

class PasswordManagerView : public QMainWindow
{
    Q_OBJECT
public:
    PasswordManagerView(QWidget *parent = nullptr);
    ~PasswordManagerView();
    void editDataEntry(std::unique_ptr<DataEntryEditor>, DataEntryWidget*);
    void createDataEntry(std::unique_ptr<DataEntryBuilder>);
    void scrollTo(const QByteArray& entryID);
    void setLocalBackups(const QList<QStandardItem* >& backupList);
    void addLocalBackup(QStandardItem* backupItem);
    void addDataEntryWidget(DataEntryWidget*);
    /**
     * @brief removeDataEntryWidget removes the widget from the layout and deletes it afterwards
     */
    void removeDataEntryWidget(DataEntryWidget*);
    void removeAllDataEntryWidgets();
    void hideAllDataEntryWidgets();
    void closeEvent(QCloseEvent* event);
private:
    QWidget* mainWidget;
    QListView* backupList;
    QStandardItemModel* backupListModel;
    QPushButton* settingsButton;
    QPushButton* newLocalBackupButton;
    QPushButton* remoteSynchronizeBackupButton;
    QWidget* scrollAreaWidget;
    QVBoxLayout* scrollAreaLayout;
    QScrollArea* scrollArea;
    QWidget* entriesWidget;
    QVBoxLayout* entriesLayout;
    QWidget* entriesHeaderWidget;
    QHBoxLayout* entriesHeaderLayout;
    QPushButton* addEntryButton;
    QPushButton* saveButton;
    QLineEdit* searchLineEdit;

    std::unique_ptr<SettingsDialog> settingsDialog;


    void connectSignalSlots();
private slots:
    void handleBackupClicked(const QModelIndex& backupIndex);
    void handleSettingsClicked();



signals:
    void addEntryButtonClicked();
    void searchEntry(QString identifier);
    void newEntry(QSharedPointer<DataEntry>);
    void saveButtonClicked();
    void revertToLocalBackup(QString backup);
    void newLocalBackupButtonClicked();
    void onClose();
};

#endif // PASSWORDMANAGERVIEW_H
