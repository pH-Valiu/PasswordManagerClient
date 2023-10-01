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
    QJsonObject toJsonObject();//done
    void decryptContent(const QByteArray& masterPW); //done
    void encryptContent(const QByteArray& masterPW); //done
    QString getName()                                           {return name;}
    QByteArray getID()                                          {return id;}
    QDateTime getLastChanged()                                  {return lastChanged;}
    std::optional<QString> getWebsite()                         {return website;}
    std::optional<QString> getEMail()                           {return email;}
    std::optional<QString> getUsername()                        {return username;}
    std::optional<QString> getPassword()                        {return password;}
    std::optional<QString> getDetails()                         {return details;}
    void clearData(){
        name.clear();id=0;website.reset();lastChanged=QDateTime();ivInner.clear();ivMidKey.clear(); midKey.clear();encryptedContent.clear();
        clearConfidential();
    }
    void clearConfidential(){
        email.reset();username.reset();password.reset();details.reset();
    }
    friend class DataEntryBuilder;
    friend class DataEntryModulator;

private:
    DataEntry();
    QString name;           //mandatory
    QByteArray id;          //auto.
    QDateTime lastChanged;  //auto.
    QByteArray ivInner;     //auto.
    QByteArray ivMidKey;    //auto.
    QByteArray midKey;      //auto.
    std::optional<QString> website;        //opt.
    QByteArray encryptedContent;
    std::optional<QString> email;          //opt.
    std::optional<QString> username;       //opt.
    std::optional<QString> password;       //mandatory
    std::optional<QString> details;        //opt.

    void setName(const QString& name)                           {this->name=name;}
    void setWebsite(const std::optional<QString>& website)      {this->website=website;}
    void setEMail(const std::optional<QString>& email)          {this->email=email;}
    void setUsername(const std::optional<QString>& username)    {this->username=username;}
    void setPassword(const std::optional<QString>& password)    {this->password=password;}
    void setDetails(const std::optional<QString>& details)      {this->details=details;}
    void setLastChanged(const QDateTime& lastChanged)           {this->lastChanged=lastChanged;}
    void setMidKey(const QByteArray& midKey)                    {this->midKey=midKey;}
    void setContent(const QByteArray& content)                  {this->encryptedContent=content;}
    void setIvInner(const QByteArray& ivInner)                  {this->ivInner=ivInner;}
    void setIvMidKey(const QByteArray& ivMidKey)                {this->ivMidKey=ivMidKey;}
    void setID(const QByteArray& id)                            {this->id=id;}
    QByteArray getMidKey()                                      {return midKey;}
    QByteArray getContent()                                     {return encryptedContent;}
    QByteArray getIvInner()                                     {return ivInner;}
    QByteArray getIvMidKey()                                    {return ivMidKey;}
};


class DataEntryBuilder {
public:
    DataEntryBuilder(const QString& name);
    ~DataEntryBuilder();
    QSharedPointer<DataEntry> build(const QByteArray& masterPW);
    static QSharedPointer<DataEntry> fromJsonObject(const QJsonObject& jsonObject);
    void addWebsite(const QString& website);
    void addEmail(const QString& email);
    void addUsername(const QString& username);
    void addPassword(const QString& password);
    void addDetails(const QString& details);


private:
    QSharedPointer<DataEntry> dataEntry;
};


class DataEntryModulator{
public:
    DataEntryModulator(QSharedPointer<DataEntry> dataEntry, const QByteArray& masterPW);
    ~DataEntryModulator();
    void changeName(const QString& name);
    void changeWebsite(const QString& website);
    void changeEmail(const QString& email);
    void changeUsername(const QString& username);
    void changePassword(const QString& password);
    void changeDetails(const QString& details);
    void saveChanges();
private:
    QSharedPointer<DataEntry> dataEntry;
    QByteArray masterPW;
};

#endif // DATAENTRY_H
