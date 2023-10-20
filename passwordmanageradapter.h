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
    PasswordManagerModel* model;
    PasswordManagerView* view;
    void connectSignalSlots();
};

#endif // PASSWORDMANAGERADAPTER_H
