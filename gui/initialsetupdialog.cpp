#include "initialsetupdialog.h"
#include <QRegularExpression>
#include <QBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QCoreApplication>


InitialSetupDialog::InitialSetupDialog(QWidget* parent) :
    QDialog{parent}
{
    QVBoxLayout* mainLayout = new QVBoxLayout();

    mainLayout->addSpacing(30);

    QLabel* nameLabel = new QLabel("Vault Shield", this);
    QFont nameFont("Arial", 30, QFont::Bold, false);
    nameLabel->setFont(nameFont);
    nameLabel->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(nameLabel);

    mainLayout->addSpacing(20);

    QLabel* descriptiveLabel = new QLabel("Please set your master password!", this);
    QFont descriptionFont("Arial", 14, QFont::Normal, false);
    descriptiveLabel->setFont(descriptionFont);
    descriptiveLabel->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(descriptiveLabel);

    mainLayout->addSpacing(3);

    informationLabel = new QLabel(this);
    informationLabel->setAlignment(Qt::AlignHCenter);
    informationLabel->setFont(QFont("Arial", 10, QFont::Normal, true));
    mainLayout->addWidget(informationLabel);


    QWidget* hboxWidget = new QWidget(this);
    QHBoxLayout* hbox = new QHBoxLayout();
    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setPlaceholderText("Enter Password");
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    signInButton = new QPushButton(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/enter.ico")), "", this);
    signInButton->setIconSize(QSize(32, 32));
    signInButton->setFixedSize(48, 48);
    hbox->addWidget(passwordLineEdit);
    hbox->addWidget(signInButton);
    hboxWidget->setLayout(hbox);
    mainLayout->addWidget(hboxWidget);



    connect(passwordLineEdit, &QLineEdit::returnPressed, this, [&]{handleSignIn(passwordLineEdit->text());});
    connect(signInButton, &QPushButton::clicked, this, [&]{handleSignIn(passwordLineEdit->text());});

    this->setMinimumSize(QSize(400, 300));
    this->setMaximumSize(QSize(400, 450));
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    this->setLayout(mainLayout);
}

InitialSetupDialog::~InitialSetupDialog(){
    passwordLineEdit->clear();
}

void InitialSetupDialog::handleSignIn(const QString& userMasterPW){
    bool regexFine = true;
    QString startTag = "<a style= color:red>";
    QString endTag = "</a>";
    QString errorHeader = "Password must contain:<br>";


    /*
    if(!QRegularExpression("^.{10,}$").match(userMasterPW).hasMatch()){
        QString errorNum = " - At least 10 characters";
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

    */


    if(regexFine){
        emit newUser(userMasterPW.toUtf8());
        passwordLineEdit->clear();
        this->close();
    }else{
        informationLabel->setText(errorHeader);
        //error displayed in informationLabel
    }

}
