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
#include <windows.h>

class DataEntry {
public:
    ~DataEntry();
    QJsonObject toJsonObject() const;//done
    bool decryptContent(const QByteArray& masterPW); //done
    bool encryptContent(const QByteArray& masterPW); //done
    bool isPlain() const                                        {return plain;}
    QString getName() const                                     {return name;}
    QByteArray getID() const                                    {return id;}
    QDateTime getLastChanged() const                            {return lastChanged;}
    QString getWebsite() const                   {return website.value_or("");}
    QString getEMail() const                     {return email.value_or("");}
    QString getUsername() const                  {return username.value_or("");}
    QString getPassword() const                  {return password.value_or("");}
    QString getDetails() const                   {return details.value_or("");}
    void clearData(){
        name.clear();id=0;website.reset();lastChanged=QDateTime();ivInner.clear();ivMidKey.clear(); midKey.clear();
        SecureZeroMemory(encryptedContent.data(), encryptedContent.size());
        encryptedContent.clear();
        clearConfidential();
    }
    void clearConfidential(){
        if(email.has_value()){
            SecureZeroMemory(email.value().data(), email.value().size());
            email.value().clear();
        }
        if(username.has_value()){
            SecureZeroMemory(username.value().data(), username.value().size());
            username.value().clear();
        }
        if(password.has_value()){
            SecureZeroMemory(password.value().data(), password.value().size());
            password.value().clear();
        }
        if(details.has_value()){
            SecureZeroMemory(details.value().data(), details.value().size());
            details.value().clear();
        }
        email.reset();username.reset();password.reset();details.reset();
    }
    bool operator == (const DataEntry&) const;
    friend class DataEntryBuilder;
    friend class DataEntryEditor;
    friend class DataEntryModulator;
#ifdef EXECUTE_UNIT_TESTS
    friend class PasswordBrokerTest;
    friend class DataEntryTest;
#endif

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
    bool plain=false;             //auto.

    void setName(const QString& name)                           {this->name=name;}
    void setWebsite(const std::optional<QString>& website)      {this->website=website;}
    void setEMail(const std::optional<QString>& email)          {this->email=email;}
    void setUsername(const std::optional<QString>& username)    {this->username=username;}
    void setPassword(const std::optional<QString>& password)    {this->password=password;}
    void setDetails(const std::optional<QString>& details)      {this->details=details;}
    void setLastChanged(const QDateTime& lastChanged)           {this->lastChanged=lastChanged;}
    void setMidKey(const QByteArray& midKey)                    {this->midKey=midKey;}
    void setIvInner(const QByteArray& ivInner)                  {this->ivInner=ivInner;}
    void setIvMidKey(const QByteArray& ivMidKey)                {this->ivMidKey=ivMidKey;}
    void setID(const QByteArray& id)                            {this->id=id;}
    void setContent(const QByteArray& content)                  {this->encryptedContent=content;}
    QByteArray getMidKey() const                                {return midKey;}
    QByteArray getContent() const                               {return encryptedContent;}
    QByteArray getIvInner() const                               {return ivInner;}
    QByteArray getIvMidKey() const                              {return ivMidKey;}
    static QString showDiff(DataEntry& d1, DataEntry& d2);
};






#endif // DATAENTRY_H
