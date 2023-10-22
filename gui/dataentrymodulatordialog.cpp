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
    QString dialogName = "Edit Entry: "+mod->getName();
    QLabel* topLabel = new QLabel(dialogName);
    QFont nameFont("Arial", 22, QFont::Bold);
    topLabel->setFont(nameFont);

    QHBoxLayout* hboxName = new QHBoxLayout();
    hboxName->addWidget(topLabel);

    QWidget* nameWidget= new QWidget();
    nameWidget->setLayout(hboxName);

    vbox->addWidget(nameWidget);


    //horizontal line
    QFrame* horizontalLine = new QFrame;
    horizontalLine->setLineWidth(3);
    horizontalLine->setMidLineWidth(3);
    horizontalLine->setFrameShape(QFrame::HLine);
    horizontalLine->setFrameShadow(QFrame::Raised);
    vbox->addWidget(horizontalLine);


    //form
    QFormLayout* form = new QFormLayout();
    nameEdit = new QLineEdit(mod->getName());
    websiteEdit = new QLineEdit(mod->getWebsite());
    usernameEdit = new QLineEdit(mod->getUsername());
    emailEdit = new QLineEdit(mod->getEmail());
    passwordEdit = new QLineEdit(mod->getPassword());
    detailsEdit = new QTextEdit(mod->getDetails());
    form->addRow("Name:\t", nameEdit);
    form->addRow("Website:\t", websiteEdit);
    form->addRow("Username:\t", usernameEdit);
    form->addRow("Email:\t", emailEdit);
    form->addRow("Password:\t", passwordEdit);
    form->addRow("Details:\t", detailsEdit);

    QWidget* formWidget = new QWidget();
    formWidget->setLayout(form);
    vbox->addWidget(formWidget);


    //horizontal line2
    QFrame* horizontalLine2 = new QFrame;
    horizontalLine2->setLineWidth(3);
    horizontalLine2->setMidLineWidth(3);
    horizontalLine2->setFrameShape(QFrame::HLine);
    horizontalLine2->setFrameShadow(QFrame::Raised);
    vbox->addWidget(horizontalLine2);


    //button area
    saveButton = new QPushButton("Save");
    saveButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    saveButton->setFixedSize(saveButton->sizeHint());
    closeButton = new QPushButton("Close");
    closeButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    closeButton->setFixedSize(closeButton->sizeHint());
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(saveButton);

    QWidget* buttonArea = new QWidget();
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
        emit closing();
        QDialog::reject();
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
    emit closing();
    this->close();
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
    emit closing();
    this->close();
}

DataEntryModulatorDialog::~DataEntryModulatorDialog(){
    delete nameEdit;
    delete websiteEdit;
    delete usernameEdit;
    delete emailEdit;
    delete passwordEdit;
    delete detailsEdit;
    delete saveButton;
    delete closeButton;
    mod->cancelChanges();
}

