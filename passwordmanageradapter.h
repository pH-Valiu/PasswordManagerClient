#ifndef PASSWORDMANAGERADAPTER_H
#define PASSWORDMANAGERADAPTER_H

#include <QObject>
#include <QPointer>
#include "passwordmanagermodel.h"
#include "gui/passwordmanagerview.h"
#include "gui/dataentrywidget.h"
#include "gui/startupdialog.h"


class PasswordManagerAdapter : public QObject
{
    Q_OBJECT
public:
    PasswordManagerAdapter();
    ~PasswordManagerAdapter();
private:
    PasswordManagerModel& model;
    QPointer<PasswordManagerView> view;
    QPointer<StartupDialog> startupDialog;
    QSharedPointer<QByteArray> masterPW;

    void showMainWindow();
    void connectSignalSlots();
    bool unprotectMasterPW();
    bool protectMasterPW();
private slots:
    void handleSave();
    void handleAuthenticateCompleted(const QString& userMasterPW);
    void handleShow(const QByteArray& id, DataEntryWidget* widget);
    void handleEdit(const QByteArray& id, DataEntryWidget* widget);
    void handleDelete(const QByteArray& id, DataEntryWidget* widget);
    void handleCreate();
    void handleInsertion(QSharedPointer<DataEntry>);
    void handleSearch(const QString& identifier);
};

#endif // PASSWORDMANAGERADAPTER_H
