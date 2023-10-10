#include "dataentrywidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <QPainter>

DataEntryWidget::DataEntryWidget(QSharedPointer<DataEntry> dataEntry, QWidget *parent)
    : QWidget{parent}, dataEntry{dataEntry}
{
    QVBoxLayout* entryLayout = new QVBoxLayout;
    entryLayout->setContentsMargins(5, 5, 5, 5);
    //entryLayout->setAlignment(Qt::AlignHCenter);
    QHBoxLayout* topButtonLayout = new QHBoxLayout;
    QPushButton* saveButton = new QPushButton("save");
    QPushButton* editButton = new QPushButton;
    QBitmap editBmp("edit.bmp");

    QIcon buttonEditIcon(editBmp);
    editButton->setIcon(buttonEditIcon);
    editButton->setIconSize(editBmp.rect().size());
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
    horizontalLine->setLineWidth(3);
    horizontalLine->setMidLineWidth(3);
    horizontalLine->setFrameShape(QFrame::HLine);
    horizontalLine->setFrameShadow(QFrame::Raised);

    entryLayout->addWidget(horizontalLine);

    this->setLayout(entryLayout);
}


void DataEntryWidget::paintEvent(QPaintEvent* event){
    QPainter painter(this);

    painter.setPen(QPen(QColor(qRgb(125, 125, 159))));
    painter.drawRoundedRect(0, 0, width()-2, height()-2, 7, 7);

    QWidget::paintEvent(event);
}
