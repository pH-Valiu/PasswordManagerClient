#include "settingsdialog.h"

#include <QLabel>
#include <QCoreApplication>
#include <QRegularExpression>

SettingsDialog::SettingsDialog(QWidget* parent) :
    QDialog{parent}
{
    mainLayout = new QVBoxLayout();


    setupChangeMasterPW();
    setupIntegrityCheck();

    this->setWindowTitle("Settings");
    this->setLayout(mainLayout);
    this->setWindowIcon(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/settings.ico")));
}

SettingsDialog::~SettingsDialog(){
    oldMasterPWLineEdit->clear();
    newMasterPWLineEdit->clear();
    newMasterPWRepeatLineEdit->clear();
}

void SettingsDialog::setupIntegrityCheck(){
    QWidget* integrityCheckWidget = new QWidget(this);
    QVBoxLayout* integrityCheckLayout = new QVBoxLayout();
    QLabel* integrityCheckLabel = new QLabel("Request integrity check:", integrityCheckWidget);
    QFont integrityCheckFont("Arial", 15, QFont::Normal, false);
    integrityCheckLabel->setFont(integrityCheckFont);
    integrityCheckLayout->addStretch();

    integrityCheckButton = new QPushButton("Start", integrityCheckWidget);

    integrityCheckLayout->addWidget(integrityCheckLabel);
    integrityCheckLayout->addSpacing(5);
    integrityCheckLayout->addWidget(integrityCheckButton);

    connect(integrityCheckButton, &QPushButton::clicked, this, [&]{emit requestIntegrityCheck();});

    integrityCheckWidget->setLayout(integrityCheckLayout);
    mainLayout->addWidget(integrityCheckWidget);

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

void SettingsDialog::setupChangeMasterPW(){

    QWidget* chMasterPWWidget = new QWidget(this);
    QVBoxLayout* chMasterPWLayout = new QVBoxLayout();
    QLabel* chMasterPWLabel = new QLabel("Change master password:", chMasterPWWidget);
    QFont masterPWFont("Arial", 15, QFont::Normal, false);
    chMasterPWLabel->setFont(masterPWFont);
    chMasterPWLayout->addStretch();
    chMasterPWLayout->addWidget(chMasterPWLabel);
    chMasterPWLayout->setSpacing(5);

    changePasswordInformationLabel = new QLabel(chMasterPWWidget);
    changePasswordInformationLabel->setAlignment(Qt::AlignHCenter);
    changePasswordInformationLabel->setFont(QFont("Arial", 10, QFont::Normal, true));
    chMasterPWLayout->addWidget(changePasswordInformationLabel);


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

                bool regexFine = true;
                QString startTag = "<a style= color:red>";
                QString endTag = "</a>";
                QString errorHeader = "Password must contain:<br>";



                QString userMasterPW = newMasterPWLineEdit->text();
                if(!QRegularExpression("^.{12,}$").match(userMasterPW).hasMatch()){
                    QString errorNum = " - At least 12 characters";
                    errorHeader.append(startTag).append(errorNum).append(endTag).append("<br>");
                    regexFine = false;
                }
                if(!QRegularExpression(".*[a-z]+.*").match(userMasterPW).hasMatch()){
                    QString errorLowerCase = " - At least one lowercase letter";
                    errorHeader.append(startTag).append(errorLowerCase).append(endTag).append("<br>");
                    regexFine = false;
                }
                if(!QRegularExpression(".*[A-Z]+.*").match(userMasterPW).hasMatch()){
                    QString errorUpperCase = " - At least one uppercase letter";
                    errorHeader.append(startTag).append(errorUpperCase).append(endTag).append("<br>");
                    regexFine = false;
                }
                if(!QRegularExpression(".*[0-9]+.*").match(userMasterPW).hasMatch()){
                    QString errorDigit = " - At least one digit\n";
                    errorHeader.append(startTag).append(errorDigit).append(endTag).append("<br>");
                    regexFine = false;
                }
                if(!QRegularExpression(".*[^a-zA-Z0-9].*").match(userMasterPW).hasMatch()){
                    QString errorSpecial = " - At least one special character";
                    errorHeader.append(startTag).append(errorSpecial).append(endTag).append("<br>");
                    regexFine = false;
                }



                if(regexFine){
                    emit passwordChanged(oldMasterPWLineEdit->text().toUtf8(), newMasterPWLineEdit->text().toUtf8());
                    oldMasterPWLineEdit->clear();
                    newMasterPWLineEdit->clear();
                    newMasterPWRepeatLineEdit->clear();
                }else{
                    changePasswordInformationLabel->setText(errorHeader);
                    newMasterPWLineEdit->clear();
                    newMasterPWRepeatLineEdit->clear();
                    newMasterPWLineEdit->setFocus();
                    //error displayed in informationLabel
                }
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
