#include "passwordmanagerview.h"
#include "gui/dataentrymodulatordialog.h"

PasswordManagerView::PasswordManagerView(QWidget *parent)
    : QMainWindow{parent}
{
    scrollAreaLayout = new QVBoxLayout();
    scrollAreaLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    scrollAreaWidget = new QWidget();
    scrollAreaWidget->setLayout(scrollAreaLayout);

    scrollArea = new QScrollArea();
    scrollArea->setWidget(scrollAreaWidget);

    this->setCentralWidget(scrollArea);
    this->setContentsMargins(10, 10, 10, 10);

}

PasswordManagerView::~PasswordManagerView(){
    delete scrollAreaLayout;
    delete scrollAreaWidget;
    delete scrollArea;
}

void PasswordManagerView::addDataEntryWidget(DataEntryWidget* dataEntryWidget){
    scrollAreaLayout->addWidget(dataEntryWidget);
    scrollArea->update();
}

void PasswordManagerView::editDataEntry(std::unique_ptr<DataEntryModulator> modulator){
    DataEntryModulatorDialog* dialog = new DataEntryModulatorDialog(std::move(modulator), this);
    dialog->show();
}
