#include "passwordmanagerview.h"
#include "gui/dataentrymodulatordialog.h"

PasswordManagerView::PasswordManagerView(QWidget *parent)
    : QMainWindow{parent}
{
    scrollArea = new QScrollArea(this);
    scrollAreaWidget = new QWidget(scrollArea);
    scrollAreaLayout = new QVBoxLayout(scrollAreaWidget);
    scrollAreaLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    scrollAreaWidget->setLayout(scrollAreaLayout);

    scrollArea->setWidget(scrollAreaWidget);

    this->setCentralWidget(scrollArea);
    this->setContentsMargins(10, 10, 10, 10);
    this->setMinimumSize(600, 800);

}

PasswordManagerView::~PasswordManagerView(){
    delete scrollAreaLayout;
    delete scrollAreaWidget;
    delete scrollArea;
}

void PasswordManagerView::addDataEntryWidget(DataEntryWidget* dataEntryWidget){
    scrollAreaLayout->addWidget(dataEntryWidget);
    dataEntryWidget->setParent(scrollAreaWidget);
    scrollArea->update();
}

void PasswordManagerView::removeDataEntryWidget(DataEntryWidget* dataEntryWidget){
    scrollAreaLayout->removeWidget(dataEntryWidget);
    delete dataEntryWidget;
    scrollArea->update();
}

void PasswordManagerView::editDataEntry(std::unique_ptr<DataEntryModulator> modulator, DataEntryWidget* widget){
    if(modulator){
        this->setEnabled(false);
        DataEntryModulatorDialog* dialog = new DataEntryModulatorDialog(std::move(modulator), this);
        connect(dialog, &DataEntryModulatorDialog::closing, this, [=]{
            this->setEnabled(true);
            widget->updateContent();
            delete dialog;
        });
        dialog->show();
    }
}
