#include "passwordmanageradapter.h"

PasswordManagerAdapter::PasswordManagerAdapter() :
    QObject(nullptr),
    model{new PasswordManagerModel()},
    view{new PasswordManagerView()}
{

    QSharedPointer<DataEntry> testEntry1;
    QByteArray masterPW = QString("12345678901234567890123456789012").toUtf8();
    DataEntryBuilder builder("applfe");
    builder.addDetails("Just call up the website² and \"log\" in ?*?");
    builder.addWebsite("https://apple.com/database?query=user-log_on#");
    builder.addUsername("user1");
    builder.addPassword(",~£:1Od33jy+lj");
    builder.addEmail("user1@apple.com");
    testEntry1 = builder.build(masterPW);

    model->addEntry(testEntry1);


    DataEntryWidget* dataEntryWidget = new DataEntryWidget(testEntry1, masterPW);

    QSharedPointer<DataEntry> testEntry2;
    DataEntryBuilder builder2("amazon");
    builder2.addDetails("Jbbbust call up the website² and \"log\" in ?*?");
    builder2.addWebsite("://amazon.com/database?query=user-log_on#");
    builder2.addUsername("use21");
    builder2.addPassword(",~£:aaaaaaalj");
    builder2.addEmail("user122apple.com");
    testEntry2 = builder2.build(masterPW);

    model->addEntry(testEntry2);


    DataEntryWidget* dataEntryWidget2 = new DataEntryWidget(testEntry2, masterPW);
    view->addDataEntryWidget(dataEntryWidget);
    view->addDataEntryWidget(dataEntryWidget2);
    view->refresh();
    view->update();

    QVBoxLayout* vbox = new QVBoxLayout();
    QWidget* w = new QWidget();
    QScrollArea* sc = new QScrollArea();
    vbox->addWidget(dataEntryWidget);
    vbox->addWidget(dataEntryWidget2);
    w->setLayout(vbox);
    sc->setWidget(w);
    view->setCentralWidget(sc);



    connect(dataEntryWidget, &DataEntryWidget::showClicked, model, [=](const QByteArray& id){
        switch(model->showHideEntry(id)){
        case 1:
            qDebug()<<"case 1";
            dataEntryWidget->switchShowButtonIcon(false);
            break;
        case 2:
            qDebug()<<"case 2";
            dataEntryWidget->switchShowButtonIcon(true);
            qDebug()<<"out";
            break;
        }
    });
    connect(dataEntryWidget2, &DataEntryWidget::showClicked, model, &PasswordManagerModel::showHideEntry);
    connectSignalSlots();
    view->show();
}

PasswordManagerAdapter::~PasswordManagerAdapter(){
    delete view;
}

void PasswordManagerAdapter::connectSignalSlots(){

}

