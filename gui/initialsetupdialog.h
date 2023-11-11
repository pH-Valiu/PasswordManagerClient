#ifndef INITIALSETUPDIALOG_H
#define INITIALSETUPDIALOG_H

#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>

class InitialSetupDialog : public QDialog
{
    Q_OBJECT
public:
    InitialSetupDialog(QWidget* parent);
    ~InitialSetupDialog();
private:
    QLineEdit* passwordLineEdit;
    QPushButton* signInButton;
    QLabel* informationLabel;
private slots:
    void handleSignIn(const QString& userMasterPW);
signals:
    newUser(const QString& userMasterPW);
};

#endif // INITIALSETUPDIALOG_H
