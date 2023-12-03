#ifndef PASSWORDMANAGERADAPTER_H
#define PASSWORDMANAGERADAPTER_H

#include <QObject>
#include <QPointer>
#include <QTimer>
#include "passwordmanagermodel.h"
#include "passwordmanagerview.h"
#include "dataentrywidget.h"
#include "startupdialog.h"
#include "initialsetupdialog.h"


class PasswordManagerAdapter : public QObject
{
    Q_OBJECT
public:
    PasswordManagerAdapter();
    ~PasswordManagerAdapter();
    int start();
private:
    PasswordManagerModel& model;
    std::unique_ptr<PasswordManagerView> view;
    std::unique_ptr<StartupDialog> startupDialog;
    std::unique_ptr<InitialSetupDialog> initialSetupDialg;
    QSharedPointer<QByteArray> masterPW;
    std::unique_ptr<QTimer> logoutTimer;

    void showMainWindow();
    void connectSignalSlots();
    bool unprotectMasterPW();
    bool protectMasterPW();
private slots:
    void handleSave();
    void handleNewUser(const QByteArray& userMasterPW);
    void handleAuthenticateCompleted(const QByteArray& userMasterPW);
    void handleCopyButtonClicked(const QByteArray& id, const DataEntry::SecureDataType& type);
    void handleShow(const QByteArray& id, DataEntryWidget* widget);
    void handleEdit(const QByteArray& id, DataEntryWidget* widget);
    void handleDelete(const QByteArray& id, DataEntryWidget* widget);
    void handleMove(const QByteArray& id, DataEntryWidget* widget, bool up, bool toTheTop);
    void handleCreate();
    void handleInsertion(QSharedPointer<DataEntry>);
    void handleSearch(const QString& identifier);
    void handleNewLocalBackup();
    void handleRevertToLocalBackup(const QString& backup);
    void handleChangeMasterPW(const QByteArray& oldUserMasterPW, const QByteArray& newUserMasterPW);
    void handleStartIntegrityCheck();
    void handleMainWindowClose();
    void logout();
signals:
};

#endif // PASSWORDMANAGERADAPTER_H
