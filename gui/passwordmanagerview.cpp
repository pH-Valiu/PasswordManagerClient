#include "passwordmanagerview.h"

PasswordManagerView::PasswordManagerView(QWidget *parent)
    : QMainWindow{parent}
{
    scrollAreaLayout = new QVBoxLayout();
    QWidget* ww = new QWidget();
    scrollAreaLayout->addWidget(ww);


    scrollAreaWidget = new QWidget();

    scrollAreaWidget->setLayout(scrollAreaLayout);

    scrollArea = new QScrollArea();
    scrollArea->setWidget(scrollAreaWidget);

    this->setCentralWidget(scrollArea);
    this->setContentsMargins(10, 10, 10, 10);

}

PasswordManagerView::~PasswordManagerView(){
    delete scrollAreaLayout;
}

void PasswordManagerView::refresh(){
    scrollAreaWidget->show();
    scrollArea->show();
}

void PasswordManagerView::addDataEntryWidget(DataEntryWidget* dataEntryWidget){
    scrollAreaLayout->addWidget(dataEntryWidget);
    //refresh();
}
