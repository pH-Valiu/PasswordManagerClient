#include "passwordmanageradapter.h"
#include <dataentry.h>


PasswordManagerAdapter::PasswordManagerAdapter() :
    QObject(nullptr),
    model{PasswordManagerModel::getInstance()},
    view{new PasswordManagerView()}
{

    QSharedPointer<DataEntry> testEntry1;
    DataEntryBuilder builder("Apple");
    builder.addDetails("Just call up the website² and \"log\" in ?*?");
    builder.addWebsite("https://apple.com/database?query=user-log_on#");
    builder.addUsername("user1");
    builder.addPassword(",~£:1Od33jy+lj");
    builder.addEmail("user1@apple.com");
    testEntry1 = builder.build(GLOBALES_TEMP::staticMasterPW);

    model.addEntry(testEntry1);


    DataEntryWidget* dataEntryWidget = new DataEntryWidget(testEntry1, GLOBALES_TEMP::staticMasterPW);

    QSharedPointer<DataEntry> testEntry2;
    DataEntryBuilder builder2("Amazon");
    builder2.addDetails("Your online shipping service");
    builder2.addWebsite("https://amazon.com/");
    builder2.addUsername("user2");
    builder2.addPassword("passwort stark");
    builder2.addEmail("user2@amazon.com");
    testEntry2 = builder2.build(GLOBALES_TEMP::staticMasterPW);

    model.addEntry(testEntry2);


    DataEntryWidget* dataEntryWidget2 = new DataEntryWidget(testEntry2, GLOBALES_TEMP::staticMasterPW);
    view->addDataEntryWidget(dataEntryWidget);
    view->addDataEntryWidget(dataEntryWidget2);



    connect(dataEntryWidget, &DataEntryWidget::showClicked, this, &PasswordManagerAdapter::handleShow);
    connect(dataEntryWidget2, &DataEntryWidget::showClicked, this, &PasswordManagerAdapter::handleShow);
    connect(dataEntryWidget, &DataEntryWidget::editClicked, this, &PasswordManagerAdapter::handleEdit);
    connect(dataEntryWidget2, &DataEntryWidget::editClicked, this, &PasswordManagerAdapter::handleEdit);


    connectSignalSlots();
    view->show();
}

PasswordManagerAdapter::~PasswordManagerAdapter(){
    delete view;
}

void PasswordManagerAdapter::connectSignalSlots(){

}

void PasswordManagerAdapter::handleShow(const QByteArray& id, DataEntryWidget* widget){
    switch(model.showHideEntry(id)){
    case 1:
        widget->switchShowButtonIcon(false);
        break;
    case 2:
        widget->switchShowButtonIcon(true);
        break;
    }
    widget->updateContent();
}

void PasswordManagerAdapter::handleEdit(const QByteArray& id, DataEntryWidget* widget){
    //view->setEnabled(false);
    view->editDataEntry(model.getModulator(id));
    widget->updateContent();
}

