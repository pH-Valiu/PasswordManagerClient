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
    StartupDialog(QWidget* parent, const QByteArray& hashedUserPW);
    ~StartupDialog();
    void passwordCorrect();
    void passwordIncorrect();
private:
    QLineEdit* passwordLineEdit;
    QPushButton* loginButton;
    QLabel* informationLabel;
    QByteArray storedHashedUserPW;
private slots:
    void authenticateUser(const QString& masterPW);
signals:
    void userAuthenticated(const QString& userMasterPW);
};

#endif // STARTUPDIALOG_H
