#ifndef PASSWORDBROKER_H
#define PASSWORDBROKER_H
#include <QString>
#include <QSharedPointer>
#include <QDateTime>
#include <QRandomGenerator>
#include <QPasswordDigestor>
#include <QUuid>
#include "windows.h"

extern "C" {
DECLSPEC_IMPORT BOOLEAN WINAPI SystemFunction036(PVOID RandomBuffer, ULONG RandomBufferLength);
}

class PasswordBroker
{
public:
    class DataEntry {
    public:
        DataEntry();
        void decryptContent(QSharedPointer<QByteArray> pointerToMasterPW);
        void setName(QString name)                  {this->name=name;}
        void setWebsite(QString website)            {this->website=website;}
        void setEMail(QString email)                {this->email=email;}
        void setUsername(QString username)          {this->username=username;}
        void setPassword(QString password)          {this->password=password;}
        void setDetails(QString details)            {this->details=details;}
        QString getName()                           {return name;}
        QByteArray getID()                          {return id;}
        QDateTime getLastChanged()                  {return lastchanged;}
        std::optional<QString> getWebsite()         {return website.isEmpty() ? std::nullopt : std::optional<QString>(website);}
        std::optional<QString> getEMail()           {return email.isEmpty() ? std::nullopt : std::optional<QString>(email);}
        std::optional<QString> getUsername()        {return username.isEmpty() ? std::nullopt : std::optional<QString>(username);}
        std::optional<QString> getPassword()        {return password.isEmpty() ? std::nullopt : std::optional<QString>(password);}
        std::optional<QString> getDetails()         {return details.isEmpty() ? std::nullopt : std::optional<QString>(details);}
        void clearData(){
            name.clear();id=0;website.clear();lastchanged=QDateTime();ivInner.clear();ivMidKey.clear();
            midKey.clear();encryptedContent.clear();email.clear();password.clear();details.clear();
        }
    private:
        QString name;           //mandatory
        QByteArray id;          //auto.
        QString website;        //opt.
        QDateTime lastchanged;  //auto.
        QByteArray ivInner;     //auto.
        QByteArray ivMidKey;    //auto.
        QByteArray midKey;      //auto.
        QByteArray encryptedContent;
        QString email;          //opt.
        QString username;       //opt.
        QString password;       //mandatory
        QString details;        //opt.
    };


    static QSharedPointer<PasswordBroker> getInstance();
    //void addEntry(PasswordBroker::DataEntry);
    void newEntry();
    QSharedPointer<DataEntry> getEntryFromId(QByteArray id);
    QSharedPointer<DataEntry> getEntryFromName(QString name);
    QSharedPointer<DataEntry> getEntryFromWebsite(QString website);
    ~PasswordBroker();
private:
    PasswordBroker();
    QScopedPointer<QVector<QSharedPointer<DataEntry>>> vector;

};

#endif // PASSWORDBROKER_H
