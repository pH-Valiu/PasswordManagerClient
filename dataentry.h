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
    QJsonObject toJsonObject() const;//done
    bool decryptContent(const QByteArray& masterPW); //done
    bool encryptContent(const QByteArray& masterPW); //done
    bool isPlain() const                                        {return plain;}
    QString getName() const                                     {return name;}
    QByteArray getID() const                                    {return id;}
    QDateTime getLastChanged() const                            {return lastChanged;}
    std::optional<QString> getWebsite() const                   {return website;}
    std::optional<QString> getEMail() const                     {return email;}
    std::optional<QString> getUsername() const                  {return username;}
    std::optional<QString> getPassword() const                  {return password;}
    std::optional<QString> getDetails() const                   {return details;}
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
     * Returns nullptr if masterPW is not 32 bytes long
     */
    QSharedPointer<DataEntry> build(const QByteArray& masterPW);
    /**
     * @brief Creates a new DataEntry based on a json object and allocates it on the heap
     * @param QJsonObject containing keys: name, id, ivInner, ivMidKey, midKey, content, lastChanged, website
     *
     * midKey and content should be encoded in Base64, lastChanged should reflect a QDateTime
     * @return QSharedPointer to newly created DataEntry if the jsonObject contains every key/attribute
     *
     * Otherwise it returns nullptr
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
     *
     * A copy of the dataEntry will be decrypted in the modulator during its lifetime
     * A copy of the masterPW will be temporarily decrypted in the modulator during its lifetime
     *
     * It is being assumed that the dataEntry is not in plain mode during the usage of DataEntryModulator
     * @param dataEntry to be altered
     * @param masterPW to decrypt midKey
     */
    DataEntryModulator(QSharedPointer<DataEntry> dataEntry, const QByteArray& masterPW);
    ~DataEntryModulator();
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
    /**
     * @brief saveChanges() finished the modulator
     *
     * The copy of the dataEntry will be encrypted again and its content will be copied back into the original dataEntry
     */
    void saveChanges();
    void cancelChanges();

    QString getName()   {return dataEntryClone.getName();}
    QString getWebsite()   {return dataEntryClone.getWebsite().value_or("");}
    QString getUsername()   {return dataEntryClone.getUsername().value_or("");}
    QString getEmail()   {return dataEntryClone.getEMail().value_or("");}
    QString getPassword()   {return dataEntryClone.getPassword().value_or("");}
    QString getDetails()   {return dataEntryClone.getDetails().value_or("");}
private:
    QSharedPointer<DataEntry> dataEntry;
    DataEntry dataEntryClone;
    QByteArray masterPW;
    bool modified;
};

#endif // DATAENTRY_H
