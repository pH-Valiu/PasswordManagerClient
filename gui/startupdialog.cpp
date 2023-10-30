#include "startupdialog.h"
#include <QBoxLayout>
#include <QFont>
#include <QCryptographicHash>
#include <QCoreApplication>

StartupDialog::StartupDialog(QWidget* parent, const QByteArray& hashedUserPW) :
    QDialog{parent},
    storedHashedUserPW{hashedUserPW}
{
    QVBoxLayout* mainLayout = new QVBoxLayout();

    mainLayout->addSpacing(30);

    QLabel* nameLabel = new QLabel("Vault Shield", this);
    QFont nameFont("Arial", 30, QFont::Bold, false);
    nameLabel->setFont(nameFont);
    nameLabel->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(nameLabel);

    mainLayout->addSpacing(20);

    informationLabel = new QLabel(this);
    informationLabel->setAlignment(Qt::AlignHCenter);
    informationLabel->setFont(QFont("Arial", 10, QFont::Normal, true));
    mainLayout->addWidget(informationLabel);


    QWidget* hboxWidget = new QWidget(this);
    QHBoxLayout* hbox = new QHBoxLayout();
    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setPlaceholderText("Enter Password");
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/enter.ico")), "", this);
    loginButton->setIconSize(QSize(32, 32));
    loginButton->setFixedSize(48, 48);
    hbox->addWidget(passwordLineEdit);
    hbox->addWidget(loginButton);
    hboxWidget->setLayout(hbox);
    mainLayout->addWidget(hboxWidget);



    connect(passwordLineEdit, &QLineEdit::returnPressed, this, [&]{authenticateUser(passwordLineEdit->text());});
    connect(loginButton, &QPushButton::clicked, this, [&]{authenticateUser(passwordLineEdit->text());});

    this->setFixedSize(300, 220);
    this->setLayout(mainLayout);
}

StartupDialog::~StartupDialog(){
    passwordLineEdit->clear();
}

void StartupDialog::authenticateUser(const QString &masterPW){
    QByteArray hashedPW = QCryptographicHash::hash(masterPW.toUtf8(), QCryptographicHash::Blake2b_512);

    if(this->storedHashedUserPW == hashedPW){
        emit userAuthenticated(masterPW);
        passwordLineEdit->clear();
        this->close();
    }else{
        informationLabel->setText("<a style= color:red>password incorrect</a>");
    }
}


