#include "passwordmanageradapter.h"
#include <dataentry.h>
#include <windows.h>
#include <dpapi.h>
#include <wincrypt.h>
#include <QMessageBox>


PasswordManagerAdapter::PasswordManagerAdapter() :
    QObject(nullptr),
    model{PasswordManagerModel::getInstance()},
    view{new PasswordManagerView()}
{

    QString tempMasterPW = "12345678901234567890123456789012";

    masterPW = QSharedPointer<QByteArray>(new QByteArray(tempMasterPW.toUtf8()));
    protectMasterPW();

    /*
    qDebug()<<"[u_pr] pw: "<<masterPW.get()->toBase64();

    protectMasterPW();

    qDebug()<<"[pr] pw: "<<masterPW.get()->toBase64();

    unprotectMasterPW();

    qDebug()<<"[u_pr] pw: "<<masterPW.get()->toBase64();
    */



    unprotectMasterPW();
    QSharedPointer<DataEntry> testEntry1;
    DataEntryBuilder builder(masterPW->constData());
    builder.addName("Apple");
    builder.addDetails("Just call up the website² and \"log\" in ?*?");
    builder.addWebsite("https://apple.com/database?query=user-log_on#");
    builder.addUsername("user1");
    builder.addPassword(",~£:1Od33jy+lj");
    builder.addEmail("user1@apple.com");
    testEntry1 = builder.build();

    model.addEntry(testEntry1);


    DataEntryWidget* dataEntryWidget = new DataEntryWidget(testEntry1);

    QSharedPointer<DataEntry> testEntry2;
    DataEntryBuilder builder2(masterPW->constData());
    builder2.addName("Amazon");
    builder2.addDetails("Your online shipping service");
    builder2.addWebsite("https://amazon.com/");
    builder2.addUsername("user2");
    builder2.addPassword("passwort stark");
    builder2.addEmail("user2@amazon.com");
    testEntry2 = builder2.build();

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


    connectSignalSlots();
    view->show();
}

PasswordManagerAdapter::~PasswordManagerAdapter(){
    delete view;
    model.removeAllEntries();
}

void PasswordManagerAdapter::connectSignalSlots(){
    connect(view, &PasswordManagerView::addEntryButtonClicked, this, &PasswordManagerAdapter::handleCreate);
    connect(view, &PasswordManagerView::newEntry, this, &PasswordManagerAdapter::handleInsertion);

}

bool PasswordManagerAdapter::unprotectMasterPW(){
    DWORD bPW = masterPW->size();
    LPVOID pPW = (LPVOID) masterPW.get()->data();

    if(bPW != 32){
        return false;
    }

    int dMod = 0;
    if(!(dMod = bPW % CRYPTPROTECTMEMORY_BLOCK_SIZE)){
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
    return S_OK;

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
    view->editDataEntry(std::move(model.getModulator(id, masterPW)), widget);
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

