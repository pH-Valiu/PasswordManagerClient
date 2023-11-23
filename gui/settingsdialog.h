#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QBoxLayout>
#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class SettingsDialog : public QDialog
{
    Q_OBJECT
private:
    QVBoxLayout* mainLayout;
    QLineEdit* oldMasterPWLineEdit;
    QLineEdit* newMasterPWLineEdit;
    QLineEdit* newMasterPWRepeatLineEdit;
    QPushButton* changeMasterPWButton;


    QPushButton* integrityCheckButton;

    void setupChangeMasterPW();
    void setupIntegrityCheck();
public:
    SettingsDialog(QWidget* parent);
    ~SettingsDialog();

    void reject();
signals:
    void passwordChanged(const QByteArray& oldUserMasterPW, const QByteArray& newUserMasterPW);
    void requestIntegrityCheck();
    void closing();
};

#endif // SETTINGSDIALOG_H
