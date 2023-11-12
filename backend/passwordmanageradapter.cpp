#include "passwordmanageradapter.h"
#include <dataentry.h>
#include <windows.h>
#include <dpapi.h>
#include <wincrypt.h>
#include <QMessageBox>
#include <QApplication>
#include <QPasswordDigestor>
#include "messagehandler.h"
#include "startupdialog.h"


PasswordManagerAdapter::PasswordManagerAdapter() :
    QObject(nullptr),
    model{PasswordManagerModel::getInstance()}
{
}

PasswordManagerAdapter::~PasswordManagerAdapter(){
    //QSharedPointer::clear()
    masterPW.clear();


    if(startupDialog){
        startupDialog.reset();
    }
    if(initialSetupDialg){
        initialSetupDialg.reset();
    }
    if(view){
        view.reset();
    }

    model.removeAllEntries();
}

int PasswordManagerAdapter::start(){
    QByteArray storedHashedUserPW = model.getUserMasterPWHash();

    if(!storedHashedUserPW.isEmpty()){
        //Show StartupDialog
        startupDialog = std::unique_ptr<StartupDialog>(new StartupDialog(nullptr, model.getUserMasterPWHash()));
        connect(startupDialog.get(), &StartupDialog::userAuthenticated, this, &PasswordManagerAdapter::handleAuthenticateCompleted);
        startupDialog->show();
        return 0;
    }else{
        //Show InitialSetupDialog
        initialSetupDialg = std::unique_ptr<InitialSetupDialog>(new InitialSetupDialog(nullptr));
        connect(initialSetupDialg.get(), &InitialSetupDialog::newUser, this, &PasswordManagerAdapter::handleNewUser);
        initialSetupDialg->show();
        return 1;
    }
}

void PasswordManagerAdapter::showMainWindow(){
    view = std::unique_ptr<PasswordManagerView>(new PasswordManagerView(nullptr));

    unprotectMasterPW();
    if(!model.startBroker(masterPW)){
        protectMasterPW();
        QApplication::exit(-1);
        return;
    }
    protectMasterPW();



    //add each DataEntry intp PasswordManagerView after creation of DataEntryWidget
    const QVector<QSharedPointer<DataEntry>> list = model.getAllEntries();

    for(qsizetype i=0; i<list.size(); i++){
        DataEntryWidget* dataEntryWidget = new DataEntryWidget(list.at(i));
        connect(dataEntryWidget, &DataEntryWidget::editClicked, this, &PasswordManagerAdapter::handleEdit);
        connect(dataEntryWidget, &DataEntryWidget::deleteClicked, this, &PasswordManagerAdapter::handleDelete);
        connect(dataEntryWidget, &DataEntryWidget::showClicked, this, &PasswordManagerAdapter::handleShow);
        view->addDataEntryWidget(dataEntryWidget);
    }

    //set local backups in PasswordManagerView
    QList<QStandardItem*> localBackupList;
    QIcon backupIcon(QCoreApplication::applicationDirPath().append("/gui/ico/backup.ico"));
    foreach(const QString& backup, model.getlAllLocalBackups()){
        if(backup != "." && backup != ".."){
            QStandardItem* item = new QStandardItem(backupIcon, backup);
            item->setEditable(false);
            localBackupList.append(item);
        }
    }
    view->setLocalBackups(localBackupList);


    connectSignalSlots();
    view->show();
}

void PasswordManagerAdapter::connectSignalSlots(){
    connect(view.get(), &PasswordManagerView::addEntryButtonClicked, this, &PasswordManagerAdapter::handleCreate);
    connect(view.get(), &PasswordManagerView::newEntry, this, &PasswordManagerAdapter::handleInsertion);
    connect(view.get(), &PasswordManagerView::searchEntry, this, &PasswordManagerAdapter::handleSearch);
    connect(view.get(), &PasswordManagerView::saveButtonClicked, this, &PasswordManagerAdapter::handleSave);
    connect(view.get(), &PasswordManagerView::revertToLocalBackup, this, &PasswordManagerAdapter::handleRevertToLocalBackup);
    connect(view.get(), &PasswordManagerView::newLocalBackupButtonClicked, this, &PasswordManagerAdapter::handleNewLocalBackup);
    connect(view.get(), &PasswordManagerView::onClose, this, &PasswordManagerAdapter::handleMainWindowClose);
}

bool PasswordManagerAdapter::unprotectMasterPW(){
    DWORD bPW = masterPW->size();
    LPVOID pPW = (LPVOID) masterPW.get()->data();

    if(bPW != 32){
        return false;
    }

    int dMod = 0;
    if((dMod = bPW % CRYPTPROTECTMEMORY_BLOCK_SIZE)){
        bPW += (CRYPTPROTECTMEMORY_BLOCK_SIZE - dMod);
    }

    if(!CryptUnprotectMemory(pPW, bPW, CRYPTPROTECTMEMORY_SAME_PROCESS)){
        SecureZeroMemory(pPW, bPW);
        pPW = NULL;
        return false;
    }

    pPW = NULL;
    masterPW->resize(32);
    return S_OK;
}

bool PasswordManagerAdapter::protectMasterPW(){
    DWORD bPW = masterPW->size();
    LPVOID pPW = (LPVOID) masterPW.get()->data();

    if(bPW != 32){
        return false;
    }

    int dMod = 0;
    if((dMod = bPW % CRYPTPROTECTMEMORY_BLOCK_SIZE)){
        bPW += (CRYPTPROTECTMEMORY_BLOCK_SIZE - dMod);
    }

    if(!CryptProtectMemory(pPW, bPW, CRYPTPROTECTMEMORY_SAME_PROCESS)){
        SecureZeroMemory(pPW, bPW);
        pPW = NULL;
        return false;
    }

    pPW = NULL;
    this->masterPW->resize(32);
    return S_OK;

}

void PasswordManagerAdapter::handleSave(){
    unprotectMasterPW();
    bool flag = model.saveBroker(masterPW);
    protectMasterPW();

    if(flag){
        MessageHandler::inform("Successfully saved the data entries to the disk.");
    }else{
        MessageHandler::warn("Saving of PasswordManagerModel failed!");
    }
}

void PasswordManagerAdapter::handleNewUser(const QString &userMasterPW){
    this->masterPW = QSharedPointer<QByteArray>(new QByteArray(QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256,
                                                                                                  userMasterPW.toUtf8(),
                                                                                                  "HKRsdn14n8(Sb§$ ,:,3;",
                                                                                                  10000,
                                                                                                  32)));
    this->masterPW->resize(32);

    model.setUserMasterPW(userMasterPW);

    protectMasterPW();
    showMainWindow();
}

void PasswordManagerAdapter::handleAuthenticateCompleted(const QString& userMasterPW){
    this->masterPW = QSharedPointer<QByteArray>(new QByteArray(QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256,
                                                            userMasterPW.toUtf8(),
                                                            "HKRsdn14n8(Sb§$ ,:,3;",
                                                            10000,
                                                            32)));
    this->masterPW->resize(32);

    protectMasterPW();
    showMainWindow();
}

void PasswordManagerAdapter::handleShow(const QByteArray& id, DataEntryWidget* widget){
    unprotectMasterPW();
    switch(model.showHideEntry(id, masterPW)){
    case 1:
        widget->switchShowButtonIcon(false);
        break;
    case 2:
        widget->switchShowButtonIcon(true);
        break;
    case -2:
        //masterPW was null
        break;
    case 0:
        //id was not findable
        break;

    }
    protectMasterPW();
    widget->updateContent();
}

void PasswordManagerAdapter::handleCreate(){
    unprotectMasterPW();
    view->createDataEntry(std::move(model.getBuilder(masterPW)));
    protectMasterPW();
}

void PasswordManagerAdapter::handleInsertion(QSharedPointer<DataEntry> dataEntry){
    model.addEntry(dataEntry);
    DataEntryWidget* dataEntryWidget = new DataEntryWidget(dataEntry);
    connect(dataEntryWidget, &DataEntryWidget::editClicked, this, &PasswordManagerAdapter::handleEdit);
    connect(dataEntryWidget, &DataEntryWidget::deleteClicked, this, &PasswordManagerAdapter::handleDelete);
    connect(dataEntryWidget, &DataEntryWidget::showClicked, this, &PasswordManagerAdapter::handleShow);
    view->addDataEntryWidget(dataEntryWidget);
}

void PasswordManagerAdapter::handleEdit(const QByteArray& id, DataEntryWidget* widget){
    unprotectMasterPW();
    switch(model.hideEntry(id, masterPW)){
    case 1:
        widget->switchShowButtonIcon(false);
        break;
    case -1:
        //widgets is already in hideMode
        break;
    case -2:
        //masterPW was null
        break;
    case 0:
        //id was not findable
        break;
    }
    view->editDataEntry(std::move(model.getEditor(id, masterPW)), widget);
    protectMasterPW();
}

void PasswordManagerAdapter::handleDelete(const QByteArray& id, DataEntryWidget* widget){
    QMessageBox::StandardButton resBtn =  QMessageBox::question(
            widget, "Are you sure?",
            "Do you really want to delete data entry:\n"+widget->getName()+"?\n",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No);


    if (resBtn == QMessageBox::Yes) {
        //delete

        unprotectMasterPW();
        switch(model.hideEntry(id, masterPW)){
        case 1:
            widget->switchShowButtonIcon(false);
            break;
        case -1:
            //widgets is already in hideMode
            break;
        case -2:
            //masterPW was null
            break;
        case 0:
            //id was not findable
            break;
        }
        protectMasterPW();

        model.removeEntry(id);
        view->removeDataEntryWidget(widget);
        view->update();
    }

    if (resBtn == QMessageBox::No){
        //do nothing
    }
}

void PasswordManagerAdapter::handleSearch(const QString& identifier){
    QByteArray id = model.searchEntry(identifier);
    if(!id.isEmpty()){
        view->scrollTo(id);
    }
}

void PasswordManagerAdapter::handleNewLocalBackup(){
    //block user input
    view->setEnabled(false);

    //hide all DataEntries as well as DataEntryWidgets
    //save brokers fileData into database directory
    unprotectMasterPW();
    model.hideAllEntries(masterPW);
    bool retFlag = model.saveBroker(masterPW);
    protectMasterPW();
    view->hideAllDataEntryWidgets();

    //create new local backup
    QString currentBackupName;
    if(retFlag){
        currentBackupName = model.newLocalBackup();

        //add new local backup to localBackupList in PasswordManagerView
        if(!currentBackupName.isNull() && !currentBackupName.isEmpty()){
            QStandardItem* newListItem = new QStandardItem(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/backup.ico")), currentBackupName);
            newListItem->setEditable(false);
            view->addLocalBackup(newListItem);
        }else{
            MessageHandler::inform("Could not create a new local backup");
        }
    }else{
        MessageHandler::warn("Could not create a new local backup because saving of PasswordBroker failed.");
    }


    //reenable user input
    view->setEnabled(true);
}

void PasswordManagerAdapter::handleRevertToLocalBackup(const QString &backup){
    //block input from user
    view->setEnabled(false);

    //hide all DataEntries as well as DataEntryWidgets
    unprotectMasterPW();
    model.hideAllEntries(masterPW);
    protectMasterPW();
    view->hideAllDataEntryWidgets();

    //clear view of any "old" DataEntries and DataEntryWidgets
    view->removeAllDataEntryWidgets();


    bool restartDidntWorkFlag = false;
    QString backupToRevertTo = backup;
    do{
        //clear model/broker of any "old" DataEntries
        model.removeAllEntries();

        //revert to older local backup
        if(!model.revertToOlderLocalBackup(backupToRevertTo)){
            MessageHandler::critical("Reverting to backup: "+backupToRevertTo+" failed.\nPlease approach an administrator and try reverting to an even older backup.", "Password-Loss-Probability Error:");
        }

        //restart broker (fetchFileData)
        unprotectMasterPW();
        if(!model.startBroker(masterPW)){
            protectMasterPW();
            restartDidntWorkFlag = true;

            //go one backup newer until restart works
            //OR CRASH INDEFINATELY!!!
            QString newerBackup = model.getOneBackupNewer(backupToRevertTo);
            if(!newerBackup.isEmpty()){
                MessageHandler::warn("Reverting to backup: "+backupToRevertTo+" failed.\nTrying to revert to newer backup: "+newerBackup);
                backupToRevertTo = newerBackup;
            }else{
                MessageHandler::critical("Reverting to backup: "+backupToRevertTo+" failed.\nNo more newer backups available to revert to.\nPlease consult an administrator.\nIf you have changed your master password lately, restart the password manager and use your old master password to log in.\nThen change your master password back to your current one and create a new backup.\nExiting application now.", "Password-Loss-Probability Error:");
                QApplication::exit(-1);
                return;
            }
        }else{
            protectMasterPW();
            restartDidntWorkFlag = false;
        }
    }while(restartDidntWorkFlag);


    //refill PasswordManagerView with "new" DataEntryWidgets
    const QVector<QSharedPointer<DataEntry>> list = model.getAllEntries();

    for(qsizetype i=0; i<list.size(); i++){
        DataEntryWidget* dataEntryWidget = new DataEntryWidget(list.at(i));
        connect(dataEntryWidget, &DataEntryWidget::editClicked, this, &PasswordManagerAdapter::handleEdit);
        connect(dataEntryWidget, &DataEntryWidget::deleteClicked, this, &PasswordManagerAdapter::handleDelete);
        connect(dataEntryWidget, &DataEntryWidget::showClicked, this, &PasswordManagerAdapter::handleShow);
        view->addDataEntryWidget(dataEntryWidget);
    }
    MessageHandler::inform("Successfully reverted to backup:\n"+backupToRevertTo);

    //reenable user input
    view->setEnabled(true);
}

void PasswordManagerAdapter::handleMainWindowClose(){
    unprotectMasterPW();
    bool flag = model.saveBroker(masterPW);
    protectMasterPW();

    if(!flag){
        MessageHandler::warn("Saving of PasswordManagerModel failed!");
    }
}
