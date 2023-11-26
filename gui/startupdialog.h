#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

class StartupDialog : public QDialog
{
    Q_OBJECT
public:
    StartupDialog(QWidget* parent, const QByteArray& hashedUserPW, const QByteArray& salt);
    ~StartupDialog();
    void passwordCorrect();
    void passwordIncorrect();
private:
    QLineEdit* passwordLineEdit;
    QPushButton* loginButton;
    QLabel* informationLabel;
    QByteArray storedHashedUserPW;
    QByteArray storedSalt;
private slots:
    void authenticateUser(const QString& userMasterPW);
signals:
    void userAuthenticated(const QByteArray& userMasterPW);
};

#endif // STARTUPDIALOG_H
