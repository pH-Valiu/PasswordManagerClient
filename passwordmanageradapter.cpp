#include "passwordmanageradapter.h"
#include <dataentry.h>
#include <windows.h>
#include <dpapi.h>
#include <wincrypt.h>
#include <QMessageBox>
#include <QPasswordDigestor>
#include "gui/startupdialog.h"


PasswordManagerAdapter::PasswordManagerAdapter() :
    QObject(nullptr),
    model{PasswordManagerModel::getInstance()},
    view{new PasswordManagerView()}
{

    /*

    */
    model.setUserMasterPW("1234");

    startupDialog = new StartupDialog(view, model.getUserMasterPWHash());
    connect(startupDialog, &StartupDialog::userAuthenticated, this, &PasswordManagerAdapter::handleAuthenticateCompleted);
    startupDialog->show();


    /*


    const QVector<QSharedPointer<DataEntry>> list = model.getAllEntries();
    for(qsizetype i=0; i<list.size(); i++){
        DataEntryWidget* dataEntryWidget = new DataEntryWidget(list.at(i));
        connect(dataEntryWidget, &DataEntryWidget::editClicked, this, &PasswordManagerAdapter::handleEdit);
        connect(dataEntryWidget, &DataEntryWidget::deleteClicked, this, &PasswordManagerAdapter::handleDelete);
        connect(dataEntryWidget, &DataEntryWidget::showClicked, this, &PasswordManagerAdapter::handleShow);
        view->addDataEntryWidget(dataEntryWidget);
    }
    */


    /*
    unprotectMasterPW();
    QSharedPointer<DataEntry> testEntry1;
    DataEntryBuilder builder(masterPW->constData());
    builder.modulateName("Apple");
    builder.modulateDetails("Just call up the website² and \"log\" in ?*?");
    builder.modulateWebsite("https://apple.com/database?query=user-log_on#");
    builder.modulateUsername("user1");
    builder.modulatePassword(",~£:1Od33jy+lj");
    builder.modulateEmail("user1@apple.com");
    testEntry1 = builder.modulate();

    model.addEntry(testEntry1);


    DataEntryWidget* dataEntryWidget = new DataEntryWidget(testEntry1);

    QSharedPointer<DataEntry> testEntry2;
    DataEntryBuilder builder2(masterPW->constData());
    builder2.modulateName("Amazon");
    builder2.modulateDetails("Your online shipping service");
    builder2.modulateWebsite("https://amazon.com/");
    builder2.modulateUsername("user2");
    builder2.modulatePassword("passwort stark");
    builder2.modulateEmail("user2@amazon.com");
    testEntry2 = builder2.modulate();

    model.addEntry(testEntry2);

    protectMasterPW();


    DataEntryWidget* dataEntryWidget2 = new DataEntryWidget(testEntry2);
    view->addDataEntryWidget(dataEntryWidget);
    view->addDataEntryWidget(dataEntryWidget2);



    connect(dataEntryWidget, &DataEntryWidget::showClicked, this, &PasswordManagerAdapter::handleShow);
    connect(dataEntryWidget2, &DataEntryWidget::showClicked, this, &PasswordManagerAdapter::handleShow);
    connect(dataEntryWidget, &DataEntryWidget::editClicked, this, &PasswordManagerAdapter::handleEdit);
    connect(dataEntryWidget2, &DataEntryWidget::editClicked, this, &PasswordManagerAdapter::handleEdit);
    connect(dataEntryWidget, &DataEntryWidget::deleteClicked, this, &PasswordManagerAdapter::handleDelete);
    connect(dataEntryWidget2, &DataEntryWidget::deleteClicked, this, &PasswordManagerAdapter::handleDelete);
    */



    /*
    unprotectMasterPW();
    model.saveBroker(masterPW);
    protectMasterPW();
    */
}

PasswordManagerAdapter::~PasswordManagerAdapter(){
    //QSharedPointer::clear()
    masterPW.clear();
    if(!startupDialog.isNull()){
        delete startupDialog;
    }
    if(!view.isNull()){
        delete view;

    }
    model.removeAllEntries();
}

void PasswordManagerAdapter::showMainWindow(){
    unprotectMasterPW();
    model.startBroker(masterPW);
    protectMasterPW();


    const QVector<QSharedPointer<DataEntry>> list = model.getAllEntries();

    for(qsizetype i=0; i<list.size(); i++){
        DataEntryWidget* dataEntryWidget = new DataEntryWidget(list.at(i));
        connect(dataEntryWidget, &DataEntryWidget::editClicked, this, &PasswordManagerAdapter::handleEdit);
        connect(dataEntryWidget, &DataEntryWidget::deleteClicked, this, &PasswordManagerAdapter::handleDelete);
        connect(dataEntryWidget, &DataEntryWidget::showClicked, this, &PasswordManagerAdapter::handleShow);
        view->addDataEntryWidget(dataEntryWidget);
    }

    connectSignalSlots();
    view->show();
}

void PasswordManagerAdapter::connectSignalSlots(){
    connect(view, &PasswordManagerView::addEntryButtonClicked, this, &PasswordManagerAdapter::handleCreate);
    connect(view, &PasswordManagerView::newEntry, this, &PasswordManagerAdapter::handleInsertion);
    connect(view, &PasswordManagerView::searchEntry, this, &PasswordManagerAdapter::handleSearch);
    connect(view, &PasswordManagerView::saveButtonClicked, this, &PasswordManagerAdapter::handleSave);

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
    model.saveBroker(masterPW);
    protectMasterPW();
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
            "Do you really want to delete data entry: "+widget->getName()+"?\n",
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

