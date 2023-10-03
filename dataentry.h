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
    bool decryptContent(const QByteArray& masterPW); //done
    bool encryptContent(const QByteArray& masterPW); //done
    QByteArray getContentPublic(){return encryptedContent;} //TO BE REMOVED
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
    /**
     * @brief Creates a new DataEntry and allocates it on the heap
     * @param name for the DataEntry
     */
    DataEntryBuilder(const QString& name);
    ~DataEntryBuilder();
    /**
     * @brief Builds the new DataEntry and encrypts its content (email, user, password, details)
     * @param masterPW to encrypt midKey, has to be exactly 32 byte long
     * @return QSharedPointer to newly created DataEntry with encrypted content
     *
     * Returns empty QSharedPointer containing nullptr if masterPW is not 32 bytes long
     */
    QSharedPointer<DataEntry> build(const QByteArray& masterPW);
    /**
     * @brief Creates a new DataEntry based on a json object and allocates it on the heap
     * @param QJsonObject containing keys: name, id, ivInner, ivMidKey, midKey, content, lastChanged, website
     *
     * midKey and content should be encoded in Base64, lastChanged should reflect a QDateTime
     * @return QSharedPointer to newly created DataEntry if the jsonObject contains every key/attribute
     *
     * Otherwise it returns empty QSharedPointer containing nullptr
     */
    static QSharedPointer<DataEntry> fromJsonObject(const QJsonObject& jsonObject);
    /**
     * @brief addWebsite
     * @param website must only contain letters: a-z A-Z 0-9 #$-_.+!*'(),/&?=:% or whitespace
     */
    void addWebsite(const QString& website);
    void addEmail(const QString& email);
    void addUsername(const QString& username);
    void addPassword(const QString& password);
    void addDetails(const QString& details);
    static QRegularExpression regexNaming;
private:
    QSharedPointer<DataEntry> dataEntry;

};


class DataEntryModulator{
public:
    /**
     * @brief Constructs a DataEntryModulator to change attributes of dataEntry
     *
     * Only works when masterPW is 32 bytes long
     * @param dataEntry to be altered
     * @param masterPW to decrypt midKey
     */
    DataEntryModulator(QSharedPointer<DataEntry> dataEntry, const QByteArray& masterPW);
    void changeName(const QString& name);
    void changeWebsite(const QString& website);
    void changeEmail(const QString& email);
    void changeUsername(const QString& username);
    void changePassword(const QString& password);
    void changeDetails(const QString& details);
    /**
     * @brief changeMasterPassword to newMasterPW, midKey stays the same
     * @param newMasterPW has to be 32 bytes long
     * @return true if the change was successfull, false if key sizes were not 32 bytes
     */
    bool changeMasterPassword(const QByteArray& newMasterPW);
    void saveChanges();
private:
    QSharedPointer<DataEntry> dataEntry;
    QByteArray masterPW;
    bool modified;
};

#endif // DATAENTRY_H
