#include "passwordmanagerview.h"
#include "gui/dataentrymodulatordialog.h"

#include <QCoreApplication>
#include <QIcon>

PasswordManagerView::PasswordManagerView(QWidget *parent)
    : QMainWindow{parent}
{
    entriesWidget = new QWidget(this);
    entriesLayout = new QVBoxLayout();



    scrollArea = new QScrollArea(entriesWidget);
    scrollAreaWidget = new QWidget(scrollArea);
    scrollAreaLayout = new QVBoxLayout();
    scrollAreaLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    scrollAreaWidget->setLayout(scrollAreaLayout);

    scrollArea->setWidget(scrollAreaWidget);



    entriesHeaderWidget = new QWidget(entriesWidget);
    entriesHeaderLayout = new QHBoxLayout();



    addEntryButton = new QPushButton(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/add.ico")),"",entriesHeaderWidget);
    addEntryButton->setIconSize(QSize(32, 32));
    addEntryButton->setFixedSize(48, 48);
    searchLineEdit = new QLineEdit(entriesHeaderWidget);
    searchLineEdit->setPlaceholderText("Search");
    searchLineEdit->setClearButtonEnabled(true);
    searchLineEdit->addAction(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/search.ico")), QLineEdit::LeadingPosition);
    entriesHeaderLayout->addWidget(addEntryButton, 0, Qt::AlignLeft);
    entriesHeaderLayout->addWidget(searchLineEdit, 0, Qt::AlignBaseline | Qt::AlignRight);
    entriesHeaderWidget->setLayout(entriesHeaderLayout);

    entriesLayout->addWidget(entriesHeaderWidget);
    entriesLayout->addWidget(scrollArea);

    entriesWidget->setLayout(entriesLayout);

    this->setCentralWidget(entriesWidget);
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
