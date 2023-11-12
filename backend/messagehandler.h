#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QString>
#include <QMessageBox>


class MessageHandler
{
public:
    static void inform(const QString& msg, const QString& header = "Information:");
    static void warn(const QString& msg, const QString& header = "Internal Error:");
    static void critical(const QString& msg, const QString& header = "Security Error:");
private:
    MessageHandler(){}
};

#endif // MESSAGEHANDLER_H
