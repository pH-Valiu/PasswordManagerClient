#include "view.h"
#include "ui_view.h"
#include "gui/dataentrywidget.h"
#include "dataentry.h"
#include <QHBoxLayout>
#include <QScrollArea>
#include <QListView>
#include <QLabel>

view::view(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::view)
{
    ui->setupUi(this);

    QSharedPointer<DataEntry> testEntry1;
    QByteArray masterPW = QString("12345678901234567890123456789012").toUtf8();
    DataEntryBuilder builder("apple");
    builder.addDetails("Just call up the website² and \"log\" in ?*?");
    builder.addWebsite("https://apple.com/?query=user-log_on#");
    builder.addUsername("user1");
    builder.addPassword(",~£:1Od33jy+lj");
    builder.addEmail("user1@apple.com");
    testEntry1 = builder.build(masterPW);
    DataEntryWidget* dataEntryWidget = new DataEntryWidget(testEntry1);
    //QWidget* main = new QWidget;
    setCentralWidget(dataEntryWidget);

    QScrollArea* dataEntriesList = new QScrollArea;

    QListView* oneDataEntry = new QListView;

    QLabel* labelName = new QLabel;
    labelName->setText("apple");

    QLabel* timestamp = new QLabel;
    timestamp->setText("Montag");

    QVBoxLayout* dataEntriesScrollLayout = new QVBoxLayout;


    dataEntriesList->setLayout(dataEntriesScrollLayout);



    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(5, 5, 5, 5);


}

view::~view()
{
    delete ui;
}

