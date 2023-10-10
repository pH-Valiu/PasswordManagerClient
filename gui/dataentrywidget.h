#ifndef DATAENTRYWIDGET_H
#define DATAENTRYWIDGET_H

#include <QObject>
#include <QWidget>
#include "dataentry.h"

class DataEntryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataEntryWidget(QSharedPointer<DataEntry> dataEntry, QWidget *parent = nullptr);
private:
    QSharedPointer<DataEntry> dataEntry;
protected:
    virtual void paintEvent(QPaintEvent*);
signals:

};

#endif // DATAENTRYWIDGET_H
