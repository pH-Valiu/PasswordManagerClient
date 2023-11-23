#include "passwordmanagerview.h"
#include "dataentrymodulatordialog.h"

#include <QCoreApplication>
#include <QMessageBox>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QScrollBar>
#include <QIcon>

PasswordManagerView::PasswordManagerView(QWidget *parent)
    : QMainWindow{parent}
{
    mainWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout();

    QWidget* leftPanelWidget = new QWidget(mainWidget);
    QVBoxLayout* leftPanelLayout = new QVBoxLayout();
    leftPanelWidget->setMaximumWidth(250);

    leftPanelLayout->addStretch();
    leftPanelLayout->addStretch();

    QLabel* backupListName = new QLabel("Local Backups:", leftPanelWidget);
    QFont backupListNameFont("Arial", 20, QFont::Bold);
    backupListName->setFont(backupListNameFont);
    leftPanelLayout->addWidget(backupListName);

    backupList = new QListView(leftPanelWidget);
    backupList->setSpacing(5);
    backupList->setMinimumSize(QSize(200, 300));
    backupList->setMaximumSize(QSize(200, 900));

    backupListModel = new QStandardItemModel(backupList);
    backupList->setModel(backupListModel);
    leftPanelLayout->addWidget(backupList);


    QWidget* leftPanelBackupButtonWidget = new QWidget(leftPanelWidget);
    QHBoxLayout* leftPanelBackupButtonLayout = new QHBoxLayout();

    newLocalBackupButton = new QPushButton(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/new-backup.ico")), "", leftPanelBackupButtonWidget);
    newLocalBackupButton->setIconSize(QSize(64, 64));
    newLocalBackupButton->setFixedSize(72, 72);
    newLocalBackupButton->setToolTip("New local backup");
    remoteSynchronizeBackupButton = new QPushButton(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/remote.ico")), "", leftPanelBackupButtonWidget);
    remoteSynchronizeBackupButton->setIconSize(QSize(64, 64));
    remoteSynchronizeBackupButton->setFixedSize(72, 72);
    remoteSynchronizeBackupButton->setToolTip("Synchronize with cloud");

    leftPanelBackupButtonLayout->addWidget(newLocalBackupButton, 0 ,Qt::AlignLeft);
    leftPanelBackupButtonLayout->addSpacing(5);
    leftPanelBackupButtonLayout->addWidget(remoteSynchronizeBackupButton, 0 ,Qt::AlignLeft);
    leftPanelBackupButtonLayout->addStretch();
    leftPanelBackupButtonWidget->setLayout(leftPanelBackupButtonLayout);
    leftPanelLayout->addWidget(leftPanelBackupButtonWidget);


    leftPanelLayout->addSpacing(200);

    settingsButton = new QPushButton(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/settings.ico")),"", leftPanelWidget);
    settingsButton->setIconSize(QSize(64, 64));
    settingsButton->setFixedSize(72, 72);
    settingsButton->setToolTip("Settings");
    leftPanelLayout->addWidget(settingsButton);
    //leftPanelLayout->addStretch();
    leftPanelLayout->addStretch();
    leftPanelWidget->setLayout(leftPanelLayout);


    entriesWidget = new QWidget(mainWidget);
    entriesLayout = new QVBoxLayout();

    entriesHeaderWidget = new QWidget(entriesWidget);
    entriesHeaderLayout = new QHBoxLayout();


    addEntryButton = new QPushButton(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/add.ico")), "", entriesHeaderWidget);
    addEntryButton->setIconSize(QSize(32, 32));
    addEntryButton->setFixedSize(48, 48);
    addEntryButton->setToolTip("Add new data entry");
    saveButton = new QPushButton(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/save.ico")), "", entriesHeaderWidget);
    saveButton->setIconSize(QSize(32, 32));
    saveButton->setFixedSize(48, 48);
    saveButton->setToolTip("Save to disk");


    searchLineEdit = new QLineEdit(entriesHeaderWidget);
    searchLineEdit->setPlaceholderText("Search");
    searchLineEdit->setClearButtonEnabled(true);
    searchLineEdit->addAction(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/search.ico")), QLineEdit::LeadingPosition);
    entriesHeaderLayout->addWidget(addEntryButton, 0, Qt::AlignLeft);
    entriesHeaderLayout->addSpacing(5);
    entriesHeaderLayout->addWidget(saveButton, 0, Qt::AlignLeft);
    entriesHeaderLayout->addStretch();
    entriesHeaderLayout->addWidget(searchLineEdit, 0, Qt::AlignRight);
    entriesHeaderLayout->setSpacing(0);
    entriesHeaderWidget->setLayout(entriesHeaderLayout);



    scrollArea = new QScrollArea(entriesWidget);
    scrollAreaWidget = new QWidget(scrollArea);
    scrollAreaLayout = new QVBoxLayout();
    scrollAreaLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    scrollAreaWidget->setLayout(scrollAreaLayout);

    scrollArea->setWidget(scrollAreaWidget);


    entriesLayout->addWidget(entriesHeaderWidget);
    entriesLayout->addWidget(scrollArea);

    entriesWidget->setLayout(entriesLayout);

    mainLayout->addWidget(leftPanelWidget);
    mainLayout->addSpacing(5);
    mainLayout->addWidget(entriesWidget);
    mainWidget->setLayout(mainLayout);

    this->setCentralWidget(mainWidget);
    this->setContentsMargins(10, 10, 10, 10);
    this->setMinimumSize(800, 800);

    this->setWindowIcon(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/vault-shield.ico")));

    connectSignalSlots();

}

PasswordManagerView::~PasswordManagerView(){
    ///!!!
    /// incomplete
    //delete scrollAreaLayout;
    //delete scrollAreaWidget;
    //delete scrollArea;
    if(settingsDialog){
        settingsDialog.reset();
    }
    delete mainWidget;
}

void PasswordManagerView::connectSignalSlots(){
    connect(addEntryButton, &QPushButton::clicked, this, [&]{emit addEntryButtonClicked();});
    connect(saveButton, &QPushButton::clicked, this, [&]{emit saveButtonClicked();});
    connect(searchLineEdit, &QLineEdit::editingFinished, this, [&]{emit searchEntry(searchLineEdit->text());});
    connect(newLocalBackupButton, &QPushButton::clicked, this, [&]{emit newLocalBackupButtonClicked();});
    connect(backupList, &QAbstractItemView::activated, this, &PasswordManagerView::handleBackupClicked);
    connect(settingsButton, &QPushButton::clicked, this, &PasswordManagerView::handleSettingsClicked);
}

void PasswordManagerView::addDataEntryWidget(DataEntryWidget* dataEntryWidget){
    scrollAreaLayout->addWidget(dataEntryWidget);
    dataEntryWidget->setParent(scrollAreaWidget);
    dataEntryWidget->setObjectName("dataEntryWidget");
    scrollArea->update();
}

void PasswordManagerView::removeDataEntryWidget(DataEntryWidget* dataEntryWidget){
    scrollAreaLayout->removeWidget(dataEntryWidget);
    delete dataEntryWidget;
    scrollArea->update();
}

void PasswordManagerView::removeAllDataEntryWidgets(){
    //deletes all of its children as well
    //and removes itself from the children list of his parent scrollArea
    delete scrollAreaWidget;

    //reinstantiate scrollareaWidget and scrollAreaLayout
    scrollAreaWidget = new QWidget(scrollArea);
    scrollAreaLayout = new QVBoxLayout();
    scrollAreaLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    scrollAreaWidget->setLayout(scrollAreaLayout);

    //re-set parent/child relationship between scrollArea and scrollAreaWidget
    scrollArea->setWidget(scrollAreaWidget);
}

void PasswordManagerView::hideAllDataEntryWidgets(){
    foreach(DataEntryWidget* object, scrollArea->findChildren<DataEntryWidget *>("dataEntryWidget")){
        if(object){
            object->updateContent();
            object->switchShowButtonIcon(false);
        }
    }
}

void PasswordManagerView::createDataEntry(std::unique_ptr<DataEntryBuilder> builder){
    if(builder){
        this->setEnabled(false);
        DataEntryModulatorDialog* dialog = new DataEntryModulatorDialog("Create Entry: ", std::move(builder), this);
        connect(dialog, &DataEntryModulatorDialog::closing, this, [=]{
            this->setEnabled(true);
        });
        connect(dialog, &DataEntryModulatorDialog::modulated,this, [&](QSharedPointer<DataEntry> dataEntry){emit newEntry(dataEntry);});
        dialog->show();
    }
}

void PasswordManagerView::editDataEntry(std::unique_ptr<DataEntryEditor> editor, DataEntryWidget* widget){
    if(editor){
        this->setEnabled(false);
        DataEntryModulatorDialog* dialog = new DataEntryModulatorDialog("Edit Entry: ", std::move(editor), this);
        connect(dialog, &DataEntryModulatorDialog::closing, this, [=]{
            this->setEnabled(true);
            widget->updateContent();
        });
        dialog->show();
    }
}

void PasswordManagerView::scrollTo(const QByteArray& entryID){
    QList<QObject*> childList = scrollAreaWidget->children();
    for(qsizetype i=0; i<childList.size(); i++){
        DataEntryWidget* child = qobject_cast<DataEntryWidget*>(childList.at(i));
        if(child){
            if(child->getID() == entryID){
                QPoint p = child->mapTo(scrollAreaWidget, QPoint(0,0));
                scrollArea->verticalScrollBar()->setValue(p.y());
                break;
            }
        }
    }
}

void PasswordManagerView::setLocalBackups(const QList<QStandardItem* > &backupList){
    foreach (QStandardItem* item, backupList) {
        backupListModel->appendRow(item);
    }
}

void PasswordManagerView::addLocalBackup(QStandardItem *backupItem){
    backupListModel->insertRow(0, backupItem);
}

void PasswordManagerView::handleBackupClicked(const QModelIndex &backupIndex){
    QString backup = backupListModel->item(backupIndex.row())->text();

    QMessageBox msgBox;
    QString text = "Do you want to revert to backup:\n";
    text.append(backup).append("?\n");
    msgBox.setText(text);
    QAbstractButton* onlyRevert = msgBox.addButton("Revert", QMessageBox::YesRole);
    QAbstractButton* backupAndRevert = msgBox.addButton("New backup and revert", QMessageBox::AcceptRole);
    msgBox.addButton(QMessageBox::Cancel);
    msgBox.setWindowTitle("Are you sure?");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    msgBox.exec();
    if(msgBox.clickedButton() == onlyRevert){
        //only revert
        emit revertToLocalBackup(backup);

    }else if(msgBox.clickedButton() == backupAndRevert){
        //new backup and revert
        emit newLocalBackupButtonClicked();
        emit revertToLocalBackup(backup);

    }else{
        //cancel
    }

}

void PasswordManagerView::handleSettingsClicked(){
    this->setEnabled(false);
    settingsDialog = std::unique_ptr<SettingsDialog>(new SettingsDialog(this));
    connect(settingsDialog.get(), &SettingsDialog::passwordChanged, this, [&](const QByteArray& oldUMPW, const QByteArray& newUMPW){
        emit changeMasterPW(oldUMPW, newUMPW);
        this->update();
    });

    connect(settingsDialog.get(), &SettingsDialog::closing, this, [&]{
       this->setEnabled(true);
    });

    settingsDialog->show();
}

void PasswordManagerView::closeEvent(QCloseEvent *event){
    emit onClose();
    QMainWindow::closeEvent(event);
}
