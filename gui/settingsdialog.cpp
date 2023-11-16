#include "settingsdialog.h"

#include <QLabel>

SettingsDialog::SettingsDialog(QWidget* parent) :
    QDialog{parent}
{
    mainLayout = new QVBoxLayout();

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



    //Missing Save Button!!!

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


    this->setWindowTitle("Settings");
    this->setLayout(mainLayout);
}
