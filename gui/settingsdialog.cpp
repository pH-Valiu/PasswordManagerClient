#include "settingsdialog.h"

#include <QLabel>
#include <QCoreApplication>

SettingsDialog::SettingsDialog(QWidget* parent) :
    QDialog{parent}
{
    mainLayout = new QVBoxLayout();


    setupChangeMasterPW();

    this->setWindowTitle("Settings");
    this->setLayout(mainLayout);
    this->setWindowIcon(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/settings.ico")));
}

SettingsDialog::~SettingsDialog(){
    oldMasterPWLineEdit->clear();
    newMasterPWLineEdit->clear();
    newMasterPWRepeatLineEdit->clear();
}

void SettingsDialog::setupChangeMasterPW(){

    QWidget* chMasterPWWidget = new QWidget(this);
    QVBoxLayout* chMasterPWLayout = new QVBoxLayout();
    QLabel* chMasterPWLabel = new QLabel("Change master password:", chMasterPWWidget);
    QFont masterPWFont("Arial", 15, QFont::Normal, false);
    chMasterPWLabel->setFont(masterPWFont);
    chMasterPWLayout->addStretch();
    chMasterPWLayout->addWidget(chMasterPWLabel);
    chMasterPWLayout->setSpacing(5);

    oldMasterPWLineEdit = new QLineEdit(chMasterPWWidget);
    oldMasterPWLineEdit->setEchoMode(QLineEdit::Password);
    oldMasterPWLineEdit->setPlaceholderText("Old master password:");
    chMasterPWLayout->addWidget(oldMasterPWLineEdit);

    newMasterPWLineEdit = new QLineEdit(chMasterPWWidget);
    newMasterPWLineEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    newMasterPWLineEdit->setPlaceholderText("New master password:");
    chMasterPWLayout->addWidget(newMasterPWLineEdit);

    newMasterPWRepeatLineEdit = new QLineEdit(chMasterPWWidget);
    newMasterPWRepeatLineEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    newMasterPWRepeatLineEdit->setPlaceholderText("Repeat new master passowrd:");
    chMasterPWLayout->addWidget(newMasterPWRepeatLineEdit);


    changeMasterPWButton = new QPushButton("Save", chMasterPWWidget);
    changeMasterPWButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    chMasterPWLayout->addWidget(changeMasterPWButton);


    connect(changeMasterPWButton, &QPushButton::clicked, this, [&]{
        if(!oldMasterPWLineEdit->text().isEmpty()){
            if(newMasterPWLineEdit->text() == newMasterPWRepeatLineEdit->text()){
                emit passwordChanged(oldMasterPWLineEdit->text().toUtf8(), newMasterPWLineEdit->text().toUtf8());
                oldMasterPWLineEdit->clear();
                newMasterPWLineEdit->clear();
                newMasterPWRepeatLineEdit->clear();
            }else{
                newMasterPWLineEdit->clear();
                newMasterPWRepeatLineEdit->clear();
                newMasterPWLineEdit->setFocus();
            }
        }
    });


    chMasterPWWidget->setLayout(chMasterPWLayout);
    mainLayout->addWidget(chMasterPWWidget);

    //horizontal line
    QFrame* horizontalLine = new QFrame(this);
    horizontalLine->setLineWidth(3);
    horizontalLine->setMidLineWidth(3);
    horizontalLine->setFrameShape(QFrame::HLine);
    horizontalLine->setFrameShadow(QFrame::Raised);
    mainLayout->addWidget(horizontalLine);
    mainLayout->addStretch();
    mainLayout->addStretch();
}

void SettingsDialog::reject(){
    emit closing();
    QDialog::reject();
}
