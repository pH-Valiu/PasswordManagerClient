#include "dataentrymodulatordialog.h"
#include <QFont>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>


DataEntryModulatorDialog::DataEntryModulatorDialog(std::unique_ptr<DataEntryModulator> modulator, QWidget* parent) :
    QDialog{parent},
    mod{std::move(modulator)}
{
    QVBoxLayout* vbox = new QVBoxLayout();

    //name
    QWidget* nameWidget= new QWidget(this);
    QString dialogName = "Edit Entry: "+mod->getName();
    QLabel* topLabel = new QLabel(dialogName, nameWidget);
    QFont nameFont("Arial", 22, QFont::Bold);
    topLabel->setFont(nameFont);

    QHBoxLayout* hboxName = new QHBoxLayout();
    hboxName->addWidget(topLabel);

    nameWidget->setLayout(hboxName);

    vbox->addWidget(nameWidget);


    //horizontal line
    QFrame* horizontalLine = new QFrame(this);
    horizontalLine->setLineWidth(3);
    horizontalLine->setMidLineWidth(3);
    horizontalLine->setFrameShape(QFrame::HLine);
    horizontalLine->setFrameShadow(QFrame::Raised);
    vbox->addWidget(horizontalLine);


    //form
    QWidget* formWidget = new QWidget(this);
    QFormLayout* form = new QFormLayout();
    nameEdit = new QLineEdit(mod->getName(), formWidget);
    websiteEdit = new QLineEdit(mod->getWebsite(), formWidget);
    usernameEdit = new QLineEdit(mod->getUsername(), formWidget);
    emailEdit = new QLineEdit(mod->getEmail(), formWidget);
    passwordEdit = new QLineEdit(mod->getPassword(), formWidget);
    detailsEdit = new QTextEdit(mod->getDetails(), formWidget);
    form->addRow("Name:\t", nameEdit);
    form->addRow("Website:\t", websiteEdit);
    form->addRow("Username:\t", usernameEdit);
    form->addRow("Email:\t", emailEdit);
    form->addRow("Password:\t", passwordEdit);
    form->addRow("Details:\t", detailsEdit);

    formWidget->setLayout(form);
    vbox->addWidget(formWidget);


    //horizontal line2
    QFrame* horizontalLine2 = new QFrame(this);
    horizontalLine2->setLineWidth(3);
    horizontalLine2->setMidLineWidth(3);
    horizontalLine2->setFrameShape(QFrame::HLine);
    horizontalLine2->setFrameShadow(QFrame::Raised);
    vbox->addWidget(horizontalLine2);


    //button area
    QWidget* buttonArea = new QWidget(this);
    saveButton = new QPushButton("Save", buttonArea);
    saveButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    saveButton->setFixedSize(saveButton->sizeHint());
    saveButton->setDefault(true);
    saveButton->setAutoDefault(true);
    closeButton = new QPushButton("Close", buttonArea);
    closeButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    closeButton->setFixedSize(closeButton->sizeHint());
    closeButton->setDefault(false);
    closeButton->setAutoDefault(false);
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(saveButton);

    buttonArea->setLayout(buttonLayout);
    vbox->addWidget(buttonArea);


    this->setWindowTitle(dialogName);
    this->setLayout(vbox);

    connectSignalSlots();
}

void DataEntryModulatorDialog::connectSignalSlots(){
    connect(closeButton, &QPushButton::clicked, this, &DataEntryModulatorDialog::cancel);
    connect(saveButton, &QPushButton::clicked, this, &DataEntryModulatorDialog::save);
}

void DataEntryModulatorDialog::reject(){
    QMessageBox::StandardButton resBtn = QMessageBox::Close;
    if(xButtonPressed){
        resBtn = QMessageBox::question(
            this, "Are you sure?",
            tr("Are you sure?\n"),
            QMessageBox::Cancel | QMessageBox::Save | QMessageBox::Yes,
            QMessageBox::Save);
    }

    if (resBtn == QMessageBox::Yes) {
        cancel();
    }

    if (resBtn == QMessageBox::Save){
        save();
    }

    if (resBtn == QMessageBox::Close){
        QDialog::reject();
        emit closing();
    }
}

void DataEntryModulatorDialog::save(){
    mod->changeName(nameEdit->text());
    mod->changeWebsite(websiteEdit->text());
    mod->changeUsername(usernameEdit->text());
    mod->changeEmail(emailEdit->text());
    mod->changePassword(passwordEdit->text());
    mod->changeDetails(detailsEdit->toPlainText());
    mod->saveChanges();


    nameEdit->clear();
    websiteEdit->clear();
    usernameEdit->clear();
    emailEdit->clear();
    passwordEdit->clear();
    detailsEdit->clear();

    xButtonPressed = false;
    this->close();
    emit closing();
}

void DataEntryModulatorDialog::cancel(){
    mod->cancelChanges();

    nameEdit->clear();
    websiteEdit->clear();
    usernameEdit->clear();
    emailEdit->clear();
    passwordEdit->clear();
    detailsEdit->clear();

    xButtonPressed = false;
    this->close();
    emit closing();
}

DataEntryModulatorDialog::~DataEntryModulatorDialog(){
    mod->cancelChanges();
    delete nameEdit;
    delete websiteEdit;
    delete usernameEdit;
    delete emailEdit;
    delete passwordEdit;
    delete detailsEdit;
    delete saveButton;
    delete closeButton;
}

