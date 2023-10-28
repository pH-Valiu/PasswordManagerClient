#include "dataentrybuilderdialog.h"
#include <QFont>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QRegularExpressionValidator>

DataEntryBuilderDialog::DataEntryBuilderDialog(std::unique_ptr<DataEntryBuilder> bud, QWidget* parent) :
    QDialog{parent},
    builder{std::move(bud)}
{
    QVBoxLayout* vbox = new QVBoxLayout();

    //name
    QWidget* nameWidget= new QWidget(this);
    QLabel* topLabel = new QLabel("Create new entry:", nameWidget);
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
    QRegularExpressionValidator* lineEditValidator = new QRegularExpressionValidator(DataEntryBuilder::regexNaming, formWidget);
    nameEdit = new QLineEdit(formWidget);
    nameEdit->setValidator(lineEditValidator);
    websiteEdit = new QLineEdit(formWidget);
    websiteEdit->setValidator(lineEditValidator);

    usernameEdit = new QLineEdit(formWidget);
    emailEdit = new QLineEdit(formWidget);
    passwordEdit = new QLineEdit(formWidget);
    detailsEdit = new QTextEdit(formWidget);
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
    createButton = new QPushButton("Create", buttonArea);
    createButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    createButton->setFixedSize(createButton->sizeHint());
    createButton->setDefault(true);
    createButton->setAutoDefault(true);
    closeButton = new QPushButton("Close", buttonArea);
    closeButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    closeButton->setFixedSize(closeButton->sizeHint());
    closeButton->setDefault(false);
    closeButton->setAutoDefault(false);
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(createButton);

    buttonArea->setLayout(buttonLayout);
    vbox->addWidget(buttonArea);


    this->setWindowTitle("Create new entry");
    this->setLayout(vbox);

    connectSignalSlots();
}

void DataEntryBuilderDialog::connectSignalSlots(){
    connect(closeButton, &QPushButton::clicked, this, &DataEntryBuilderDialog::cancel);
    connect(createButton, &QPushButton::clicked, this, &DataEntryBuilderDialog::create);
}

void DataEntryBuilderDialog::reject(){
    QMessageBox::StandardButton resBtn = QMessageBox::Close;
    if(xButtonPressed){
        resBtn = QMessageBox::question(
            this, "Are you sure?",
            tr("Are you sure?\n"),
            QMessageBox::Cancel | QMessageBox::Save | QMessageBox::Yes,
            QMessageBox::Cancel);
    }

    if (resBtn == QMessageBox::Yes) {
        cancel();
    }

    if (resBtn == QMessageBox::Save){
        create();
    }

    if (resBtn == QMessageBox::Close){
        QDialog::reject();
        emit closing();
    }
}

void DataEntryBuilderDialog::create(){
    builder->addName(nameEdit->text());
    builder->addWebsite(websiteEdit->text());
    builder->addUsername(usernameEdit->text());
    builder->addEmail(emailEdit->text());
    builder->addPassword(passwordEdit->text());
    builder->addDetails(detailsEdit->toPlainText());
    emit built(builder->build());

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

void DataEntryBuilderDialog::cancel(){
    //since builder->build() won't have been called,
    //the dataEntry inside of builder will have ref=1
    //and therefore on destructor call of builder the dataEntry will be cleared
    //and also deleted since ref=0
    builder->deleteMasterPW();

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

DataEntryBuilderDialog::~DataEntryBuilderDialog(){
    //same argument as in cancel()
    builder->deleteMasterPW();

    delete nameEdit;
    delete websiteEdit;
    delete usernameEdit;
    delete emailEdit;
    delete passwordEdit;
    delete detailsEdit;
    delete createButton;
    delete closeButton;

}
