#ifndef DATAENTRYMODULATORDIALOG_H
#define DATAENTRYMODULATORDIALOG_H

#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>

#include "dataentry.h"

class DataEntryModulatorDialog : public QDialog
{
    Q_OBJECT
public:
    DataEntryModulatorDialog(std::unique_ptr<DataEntryModulator> modulator, QWidget* parent);
    ~DataEntryModulatorDialog();
    void reject();

private:
    std::unique_ptr<DataEntryModulator> mod;
    QLineEdit* nameEdit;
    QLineEdit* websiteEdit;
    QLineEdit* usernameEdit;
    QLineEdit* emailEdit;
    QLineEdit* passwordEdit;
    QTextEdit* detailsEdit;
    QPushButton* saveButton;
    QPushButton* closeButton;
    bool closeDialog = true;

    void connectSignalSlots();
private slots:
    void save();
    void cancel();
};

#endif // DATAENTRYMODULATORDIALOG_H
