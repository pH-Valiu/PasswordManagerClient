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
#include "constants.h"

class DataEntry {
public:
    struct IntegrityCheckData{
        QByteArray ICMidKey;
        QByteArray ICMidKeySalt;
        QByteArray ICivMidKey;
        QByteArray ICMidKeyHash;
    };
    ~DataEntry();
    QJsonObject toJsonObject() const;//done
    /**
     * @brief decryptContent decrypts midKey with the masterPW, derives its hash (midKeySalt) and compares it with midKeyHash
     *
     * If not successfull -> return false, otherwise
     *
     * Decrypt encryptedContent using plainMidKey and try to parse it as QJSonDocument
     *
     * If not successfull (parse error) -> return false, otherwise
     *
     * Fill DataEntry::confidentialData with data from decrypted json-content
     * @param masterPW already derived from the user masterPW
     * @return true if everything worked as planned, false if hashes did not align, or parse error occurred, or masterPW had the wrong size
     */
    bool decryptContent(const QSharedPointer<QByteArray>& masterPW); //done
    /**
     * @brief encryptContent decrypts midKey with the masterPW, derives its hash (midKeySalt) and compares it with midKeyHash
     *
     * If not successfull -> return false, otherwise
     *
     * Create QJsonDocument from DataEntry::confidentialData and encrypt it with plainMidKey
     * Update encryptedContent to that result
     * @param masterPW already derived from the user masterPW
     * @return true if everything worked as planned, false if hashes did not align, or masterPW had the wrong size
     */
    bool encryptContent(const QSharedPointer<QByteArray>& masterPW); //done
    bool isPlain() const                                        {return plain;}
    QString getName() const                                     {return name;}
    QByteArray getID() const                                    {return id;}
    QDateTime getLastChanged() const                            {return lastChanged;}
    QString getWebsite() const                   {return website.value_or("");}
    QString getEMail() const                     {return email.value_or("");}
    QString getUsername() const                  {return username.value_or("");}
    QString getPassword() const                  {return password.value_or("");}
    QString getDetails() const                   {return details.value_or("");}
    const IntegrityCheckData getIntegrityData() const {
        return {midKey, midKeySalt, ivMidKey, midKeyHash};
    }
    void clearData(){
        name.clear();id=0;website.reset();lastChanged=QDateTime();ivInner.clear();
        ivMidKey.clear();midKey.clear();midKeyHash.clear();midKeySalt.clear();
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
    QByteArray midKeyHash;  //auto.
    QByteArray midKeySalt;  //auto.
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
    void setMidKeyHash(const QByteArray& midKeyHash)            {this->midKeyHash=midKeyHash;}
    void setMidKeySalt(const QByteArray& midKeySalt)            {this->midKeySalt=midKeySalt;}
    void setIvInner(const QByteArray& ivInner)                  {this->ivInner=ivInner;}
    void setIvMidKey(const QByteArray& ivMidKey)                {this->ivMidKey=ivMidKey;}
    void setID(const QByteArray& id)                            {this->id=id;}
    void setContent(const QByteArray& content)                  {this->encryptedContent=content;}
    QByteArray getMidKey() const                                {return midKey;}
    QByteArray getMidKeyHash() const                            {return midKeyHash;}
    QByteArray getMidKeySalt() const                            {return midKeySalt;}
    QByteArray getContent() const                               {return encryptedContent;}
    QByteArray getIvInner() const                               {return ivInner;}
    QByteArray getIvMidKey() const                              {return ivMidKey;}
    static QString showDiff(DataEntry& d1, DataEntry& d2);
};






#endif // DATAENTRY_H
