#include "passwordmanageradapter.h"
#include <dataentry.h>
#include <QMessageBox>


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
    connect(dataEntryWidget, &DataEntryWidget::deleteClicked, this, &PasswordManagerAdapter::handleDelete);
    connect(dataEntryWidget2, &DataEntryWidget::deleteClicked, this, &PasswordManagerAdapter::handleDelete);


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
    case 0:
        //id was not findable
        return;
    }
    widget->updateContent();
}

void PasswordManagerAdapter::handleEdit(const QByteArray& id, DataEntryWidget* widget){
    switch(model.hideEntry(id)){
    case 1:
        widget->switchShowButtonIcon(false);
        break;
    case -1:
        //widgets is already in hideMode
        break;
    case 0:
        //id was not findable
        return;
    }
    view->editDataEntry(model.getModulator(id), widget);
}

void PasswordManagerAdapter::handleDelete(const QByteArray& id, DataEntryWidget* widget){
    QMessageBox::StandardButton resBtn =  QMessageBox::question(
            widget, "Are you sure?",
            "Do you really want to delete data entry: "+widget->getName()+"?\n",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No);


    if (resBtn == QMessageBox::Yes) {
        //delete
        model.removeEntry(id);
        view->removeDataEntryWidget(widget);
        view->update();
    }

    if (resBtn == QMessageBox::No){
        //do nothing
    }
}

