#include "dataentrywidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>

DataEntryWidget::DataEntryWidget(QSharedPointer<DataEntry> dataEntry, QWidget *parent)
    : QWidget{parent}, dataEntry{dataEntry}
{
    QVBoxLayout* entryLayout = new QVBoxLayout;
    entryLayout->setContentsMargins(5, 5, 5, 5);
    QHBoxLayout* topButtonLayout = new QHBoxLayout;
    QPushButton* saveButton = new QPushButton("save");
    QPushButton* editButton = new QPushButton("edit");
    QPushButton* showButton = new QPushButton("show");
    QPushButton* deleteButton = new QPushButton("delete");
    topButtonLayout->addWidget(saveButton);
    topButtonLayout->addWidget(editButton);
    topButtonLayout->addWidget(showButton);
    topButtonLayout->addWidget(deleteButton);
    QWidget* buttonPanel = new QWidget;
    buttonPanel->setLayout(topButtonLayout);
    topButtonLayout->setContentsMargins(10, 5, 10, 5);
    entryLayout->addWidget(buttonPanel);

    QFrame* horizontalLine = new QFrame;
    horizontalLine->setObjectName(QString::fromUtf8("HorizontalLine"));
    horizontalLine->setGeometry(QRect(320, 150, 118, 3));
    horizontalLine->setFrameShape(QFrame::HLine);
    horizontalLine->setFrameShadow(QFrame::Sunken);

    entryLayout->addWidget(horizontalLine);

    this->setLayout(entryLayout);
}
