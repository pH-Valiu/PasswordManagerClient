#include "dataentrywidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QClipboard>
#include <QFrame>
#include <QPainter>

DataEntryWidget::DataEntryWidget(QSharedPointer<const DataEntry> dataEntry, QWidget *parent)
    : QWidget{parent}, dataEntry{dataEntry}
{
    //main VBoxLayout
    QVBoxLayout* entryLayout = new QVBoxLayout();
    entryLayout->setContentsMargins(5, 5, 5, 5);

    //button panel
    setupButtonPanel(entryLayout);

    //header panel
    setupHeaderPanel(entryLayout);

    //space between header and content
    entryLayout->addSpacing(10);

    //content panel
    setupContentPanel(entryLayout);

    //horizontal line 2
    QFrame* horizontalLine2 = new QFrame(this);
    horizontalLine2->setLineWidth(3);
    horizontalLine2->setMidLineWidth(3);
    horizontalLine2->setFrameShape(QFrame::HLine);
    horizontalLine2->setFrameShadow(QFrame::Raised);
    entryLayout->addWidget(horizontalLine2);

    //lastChanged label
    lastChanged = new QLabel(dataEntry->getLastChanged().toString(), this);
    lastChanged->setAlignment(Qt::AlignRight);
    QFont lastChangedFont("Arial", 8, QFont::ExtraLight, true);
    lastChanged->setFont(lastChangedFont);
    entryLayout->addWidget(lastChanged);


    this->setLayout(entryLayout);
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    this->setMinimumSize(400, 400);

    //connect Buttons with Methods
    connectSignalSlots();
}

DataEntryWidget::~DataEntryWidget(){
    //Clear reference binding from QSharedPointer to the DataEntry*
    dataEntry.clear();

    name->clear();
    website->clear();
    username->clear();
    email->clear();
    password->clear();
    details->clear();
    delete name;
    delete website;
    delete username;
    delete email;
    delete password;
    delete details;
    delete lastChanged;
    delete editButton;
    delete showButton;
    delete deleteButton;
    delete usernameCopyButton;
    delete emailCopyButton;
    delete passwordCopyButton;
    delete detailsCopyButton;
}


void DataEntryWidget::setupButtonPanel(QVBoxLayout* entryLayout){
    //button area
    QWidget* buttonPanel = new QWidget(this);
    QHBoxLayout* topButtonLayout = new QHBoxLayout();
    topButtonLayout->setContentsMargins(10, 5, 10, 5);
    editButton = new QPushButton(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/edit.ico")), "", buttonPanel);
    editButton->setIconSize(QSize(32, 32));
    editButton->setFixedSize(48, 48);
    showButton = new QPushButton(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/show.ico")), "", buttonPanel);
    showButton->setIconSize(QSize(32, 32));
    showButton->setFixedSize(48, 48);
    deleteButton = new QPushButton(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/delete.ico")), "", buttonPanel);
    deleteButton->setIconSize(QSize(32, 32));
    deleteButton->setFixedSize(48, 48);
    topButtonLayout->addWidget(editButton);
    topButtonLayout->addWidget(showButton);
    topButtonLayout->addWidget(deleteButton);

    buttonPanel->setLayout(topButtonLayout);
    buttonPanel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    entryLayout->addWidget(buttonPanel);

    //horizontal line
    QFrame* horizontalLine = new QFrame(this);
    horizontalLine->setLineWidth(3);
    horizontalLine->setMidLineWidth(3);
    horizontalLine->setFrameShape(QFrame::HLine);
    horizontalLine->setFrameShadow(QFrame::Raised);
    entryLayout->addWidget(horizontalLine);
}

void DataEntryWidget::setupHeaderPanel(QVBoxLayout *entryLayout){
    //header
    QWidget* headerWidget = new QWidget(this);
    QVBoxLayout* headerLayout = new QVBoxLayout();
    name = new QLabel(dataEntry->getName(), headerWidget);
    name->setAlignment(Qt::AlignHCenter);
    QFont nameFont("Arial", 22, QFont::Bold);
    name->setFont(nameFont);
    headerLayout->addWidget(name);

    QString websiteS = "<a style= color:black href=\"" + dataEntry->getWebsite().value_or("") +"\">"+dataEntry->getWebsite().value_or("")+"</a>";
    website = new QLabel(websiteS, headerWidget);
    website->setAlignment(Qt::AlignHCenter);
    website->setTextInteractionFlags(Qt::TextBrowserInteraction);
    website->setOpenExternalLinks(true);
    QFont websiteFont("Arial", 10, QFont::Normal, true);
    website->setFont(websiteFont);
    headerLayout->addWidget(website);


    headerWidget->setLayout(headerLayout);
    headerWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    entryLayout->addWidget(headerWidget);

}

void DataEntryWidget::setupContentPanel(QVBoxLayout *entryLayout){
    //Content
    QWidget* contentWidget = new QWidget(this);
    QWidget* t1lWid = new QWidget(contentWidget);
    QWidget* t1rWid = new QWidget(contentWidget);
    QFont contentFont("Arial", 12, QFont::Normal, false);
    username = new QLabel("Username:\t****", t1lWid);
    username->setFont(contentFont);
    username->setTextInteractionFlags(Qt::TextSelectableByMouse);
    username->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    email = new QLabel("Email:\t****", t1lWid);
    email->setFont(contentFont);
    email->setTextInteractionFlags(Qt::TextSelectableByMouse);
    email->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    password = new QLabel("Password:\t****", t1lWid);
    password->setFont(contentFont);
    password->setTextInteractionFlags(Qt::TextSelectableByMouse);
    password->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    details = new QLabel("Details:\t****", t1lWid);
    details->setFont(contentFont);
    details->setTextInteractionFlags(Qt::TextSelectableByMouse);
    details->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    details->setWordWrap(true);


    QIcon copyIcon(QCoreApplication::applicationDirPath().append("/gui/ico/copy.ico"));
    usernameCopyButton = new QPushButton(copyIcon, "", t1rWid);
    usernameCopyButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    usernameCopyButton->setToolTip("copy username");
    emailCopyButton = new QPushButton(copyIcon, "", t1rWid);
    emailCopyButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    emailCopyButton->setToolTip("copy email");
    passwordCopyButton = new QPushButton(copyIcon, "", t1rWid);
    passwordCopyButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    passwordCopyButton->setToolTip("copy password");
    detailsCopyButton = new QPushButton(copyIcon, "", t1rWid);
    detailsCopyButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    detailsCopyButton->setToolTip("copy details");

    QHBoxLayout* contentLayout = new QHBoxLayout();
    QVBoxLayout* t1l = new QVBoxLayout();
    t1l->setContentsMargins(10, 15, 10, 15);
    t1l->addWidget(username);
    t1l->addWidget(email);
    t1l->addWidget(password);
    t1l->addWidget(details);
    t1lWid->setLayout(t1l);

    QVBoxLayout* t1r = new QVBoxLayout();
    t1r->setContentsMargins(10, 15, 10, 15);
    t1r->setAlignment(Qt::AlignRight);
    t1r->addWidget(usernameCopyButton);
    t1r->addWidget(emailCopyButton);
    t1r->addWidget(passwordCopyButton);
    t1r->addWidget(detailsCopyButton);
    t1r->setSizeConstraint(QLayout::SetMinimumSize);
    t1rWid->setLayout(t1r);

    contentLayout->addWidget(t1lWid);
    contentLayout->addWidget(t1rWid);



    contentWidget->setLayout(contentLayout);
    contentWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    entryLayout->addWidget(contentWidget);
}

void DataEntryWidget::connectSignalSlots(){
    connect(usernameCopyButton, &QPushButton::clicked, this, [&]{QGuiApplication::clipboard()->setText(dataEntry->getUsername().value_or(""));});
    connect(emailCopyButton , &QPushButton::clicked, this, [&]{QGuiApplication::clipboard()->setText(dataEntry->getEMail().value_or(""));});
    connect(passwordCopyButton, &QPushButton::clicked, this, [&]{QGuiApplication::clipboard()->setText(dataEntry->getPassword().value_or(""));});
    connect(detailsCopyButton, &QPushButton::clicked, this, [&]{QGuiApplication::clipboard()->setText(dataEntry->getDetails().value_or(""));});
    connect(showButton, &QPushButton::clicked, this, [&]{emit showClicked(dataEntry->getID(), this);});
    connect(editButton, &QPushButton::clicked, this, [&]{emit editClicked(dataEntry->getID(), this);});
    connect(deleteButton, &QPushButton::clicked, this, [&]{emit deleteClicked(dataEntry->getID(), this);});
}

void DataEntryWidget::switchShowButtonIcon(bool eyeClosed) const{
    if(eyeClosed){
        showButton->setIcon(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/dont-show.ico")));
    }else{
        showButton->setIcon(QIcon(QCoreApplication::applicationDirPath().append("/gui/ico/show.ico")));
    }
}

void DataEntryWidget::updateContent(){
    name->setText(dataEntry->getName());
    website->setText("<a style= color:black href=\""+dataEntry->getWebsite().value_or("")+"\">"+dataEntry->getWebsite().value_or("")+"</a>");
    lastChanged->setText(dataEntry->getLastChanged().toString());

    QString uText = "Username:\t"+dataEntry->getUsername().value_or("****");
    username->setText(uText);
    QString eText = "Email:\t"+dataEntry->getEMail().value_or("****");
    email->setText(eText);
    QString pText = "Password:\t"+dataEntry->getPassword().value_or("****");
    password->setText(pText);
    QString dText = "Details:\t"+dataEntry->getDetails().value_or("****");
    details->setText(dText);
}

void DataEntryWidget::paintEvent(QPaintEvent* event){
    QPainter painter(this);

    painter.setPen(QPen(QColor(qRgb(125, 125, 159))));
    painter.drawRoundedRect(0, 0, width()-2, height()-2, 7, 7);

    QWidget::paintEvent(event);
}
