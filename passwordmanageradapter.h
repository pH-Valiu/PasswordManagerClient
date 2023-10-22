#ifndef PASSWORDMANAGERADAPTER_H
#define PASSWORDMANAGERADAPTER_H

#include <QObject>
#include "passwordmanagermodel.h"
#include "gui/passwordmanagerview.h"
#include "gui/dataentrywidget.h"


class PasswordManagerAdapter : public QObject
{
    Q_OBJECT
public:
    PasswordManagerAdapter();
    ~PasswordManagerAdapter();
private:
    PasswordManagerModel& model;
    PasswordManagerView* view;
    void connectSignalSlots();
private slots:
    void handleShow(const QByteArray& id, DataEntryWidget* widget);
    void handleEdit(const QByteArray& id, DataEntryWidget* widget);
    void handleDelete(const QByteArray& id, DataEntryWidget* widget);
};

#endif // PASSWORDMANAGERADAPTER_H
