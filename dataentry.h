#ifndef DATAENTRY_H
#define DATAENTRY_H

#include <QString>
#include <QSharedPointer>
#include <QDateTime>
#include <QUuid>
#include <QRandomGenerator>
#include <QPasswordDigestor>
#include <QJsonObject>
#include <QJsonDocument>
#include "windows.h"
#include "qaesencryption.h"

extern "C" {
DECLSPEC_IMPORT BOOLEAN WINAPI SystemFunction036(PVOID RandomBuffer, ULONG RandomBufferLength);
}

class DataEntry {
public:
    void decryptContent(QSharedPointer<QByteArray> pointerToMasterPW);
    void encryptContent(QSharedPointer<QByteArray> pointerToMasterPW);
    QJsonObject toJsonObject();
    DataEntry fromJsonObject(QJsonObject);
    void setName(const QString& name)                  {this->name=name;}
    void setWebsite(const QString& website)            {this->website=website;}
    void setEMail(const QString& email)                {this->email=email;}
    void setUsername(const QString& username)          {this->username=username;}
    void setPassword(const QString& password)          {this->password=password;}
    void setDetails(const QString& details)            {this->details=details;}
    QString getName()                           {return name;}
    QByteArray getID()                          {return id;}
    QDateTime getLastChanged()                  {return lastchanged;}
    std::optional<QString> getWebsite()         {return website.isEmpty() ? std::nullopt : std::optional<QString>(website);}
    std::optional<QString> getEMail()           {return email.isEmpty() ? std::nullopt : std::optional<QString>(email);}
    std::optional<QString> getUsername()        {return username.isEmpty() ? std::nullopt : std::optional<QString>(username);}
    std::optional<QString> getPassword()        {return password.isEmpty() ? std::nullopt : std::optional<QString>(password);}
    std::optional<QString> getDetails()         {return details.isEmpty() ? std::nullopt : std::optional<QString>(details);}
    void clearData(){
        name.clear();id=0;website.clear();lastchanged=QDateTime();ivInner.clear();ivMidKey.clear(); midKey.clear();encryptedContent.clear();
        clearConfidential();
    }
    void clearConfidential(){
        email.clear();username.clear();password.clear();details.clear();
    }

    friend class DataEntryBuilder;
private:
    DataEntry();
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

    void setMidKey(const QByteArray& midKey)            {this->midKey=midKey;}
    void setContent(const QByteArray& content)          {this->encryptedContent=content;}
    QByteArray getMidKey()                              {return midKey;}
    QByteArray getContent()                             {return encryptedContent;}
};


class DataEntryBuilder {
public:
    DataEntryBuilder(const QString& name, const QString& password);
    QSharedPointer<DataEntry> build(QSharedPointer<QByteArray> pointerToMasterPW);
    void addWebsite(const QString& website);
    void addEmail(const QString& email);
    void addUsername(const QString& username);
    void addDetails(const QString& details);
private:
    QSharedPointer<DataEntry> dataEntry;
};

#endif // DATAENTRY_H
