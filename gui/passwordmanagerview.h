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
#include "gui/dataentrywidget.h"
#include "dataentrymodulator.h"

class PasswordManagerView : public QMainWindow
{
    Q_OBJECT
public:
    PasswordManagerView(QWidget *parent = nullptr);
    ~PasswordManagerView();
    void editDataEntry(std::unique_ptr<DataEntryEditor>, DataEntryWidget*);
    void createDataEntry(std::unique_ptr<DataEntryBuilder>);
    void scrollTo(const QByteArray& entryID);
public slots:
    void setLocalBackups(const QList<QStandardItem* >& backupList);
    void addLocalBackup(QStandardItem* backupItem);
    void addDataEntryWidget(DataEntryWidget*);
    /**
     * @brief removeDataEntryWidget removes the widget from the layout and deletes it afterwards
     */
    void removeDataEntryWidget(DataEntryWidget*);
    void removeAllDataEntryWidgets();
    void hideAllDataEntryWidgets();
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


    void connectSignalSlots();
    void handleBackupClicked(const QModelIndex& backupIndex);


signals:
    void addEntryButtonClicked();
    void searchEntry(QString identifier);
    void newEntry(QSharedPointer<DataEntry>);
    void saveButtonClicked();
    void revertToLocalBackup(QString backup);
    void newLocalBackupButtonClicked();
};

#endif // PASSWORDMANAGERVIEW_H
