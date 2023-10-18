#ifndef DATAENTRYWIDGET_H
#define DATAENTRYWIDGET_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "dataentry.h"

class DataEntryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataEntryWidget(QSharedPointer<DataEntry> dataEntry, QWidget *parent = nullptr);
private:
    QSharedPointer<DataEntry> dataEntry;
    QLabel* name;
    QLabel* website;
    QLabel* username;
    QLabel* email;
    QLabel* password;
    QLabel* details;
    QLabel* lastChanged;

    QPushButton* editButton;
    QPushButton* showButton;
    QPushButton* deleteButton;
    QPushButton* usernameCopyButton;
    QPushButton* emailCopyButton;
    QPushButton* passwordCopyButton;
    QPushButton* detailsCopyButton;
protected:
    virtual void paintEvent(QPaintEvent*);
signals:

};

#endif // DATAENTRYWIDGET_H
