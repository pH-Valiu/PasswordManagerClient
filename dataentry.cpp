#include "dataentry.h"

DataEntry::DataEntry(){}

QJsonObject DataEntry::toJsonObject(){
    QMap<QString, QVariant> map;
    map.insert("name", getName());
    map.insert("id", QString::fromUtf8(getID()));
    map.insert("ivInner", QString::fromUtf8(getIvInner()));
    map.insert("ivMidKey", QString::fromUtf8(getIvMidKey()));
    map.insert("midKey", QString::fromUtf8(getMidKey()));
    map.insert("lastChanged", getLastChanged());
    map.insert("website", getWebsite().value_or(QString("")));
    map.insert("content", QString::fromUtf8(getContent()));

    return QJsonObject::fromVariantMap(map);
}

void DataEntry::decryptContent(const QByteArray& masterPW){
    if(!this->encryptedContent.isEmpty()){
        QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
        QByteArray decryptedMidKey = crypter.removePadding(crypter.decode(this->midKey, masterPW, this->ivMidKey));
        QByteArray decryptedJson = QByteArray::fromBase64(crypter.removePadding(crypter.decode(this->encryptedContent, decryptedMidKey, this->ivInner)), QByteArray::Base64Encoding);
        QMap<QString, QVariant> map = QJsonDocument::fromJson(decryptedJson).object().toVariantMap();

        QString username = map.value("username").toString();
        if(username.isEmpty()){this->username = std::nullopt;}else{this->username = username;}

        QString email = map.value("email").toString();
        if(email.isEmpty()){this->email = std::nullopt;}else{this->email = email;}

        QString password = map.value("password").toString();
        if(password.isEmpty()){this->password = std::nullopt;}else{this->password = password;}

        QString details = map.value("details").toString();
        if(details.isEmpty()){this->details = std::nullopt;}else{this->details = details;}

        this->encryptedContent.clear();
        decryptedMidKey.clear();

    }

}

void DataEntry::encryptContent(const QByteArray& masterPW){
    if(this->encryptedContent.isEmpty()){
        QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
        QByteArray decryptedMidKey = crypter.removePadding(crypter.decode(this->midKey, masterPW, this->ivMidKey));

        QMap<QString, QVariant> map;
        map.insert("username", this->username.value_or(QString("")));
        map.insert("email", this->email.value_or(QString("")));
        map.insert("password", this->password.value_or(QString("")));
        map.insert("details", this->details.value_or(QString("")));
        QByteArray contentAsJson = QJsonDocument(QJsonObject::fromVariantMap(map)).toJson(QJsonDocument::Compact);
        QByteArray encryptedContent = crypter.encode(contentAsJson, decryptedMidKey, this->ivInner).toBase64(QByteArray::Base64Encoding);
        this->setContent(encryptedContent);
        this->clearConfidential();
        decryptedMidKey.clear();
    }
}


DataEntryBuilder::DataEntryBuilder(const QString& name){
    dataEntry = QSharedPointer<DataEntry>(new DataEntry());
    dataEntry->setName(name);
    dataEntry->setID(QUuid::createUuid().toByteArray());
    QByteArray iv;
    SystemFunction036(iv.data(), 16);
    dataEntry->setIvInner(iv);

    iv.clear();
    SystemFunction036(iv.data(), 16);
    dataEntry->setIvMidKey(iv);
}
DataEntryBuilder::~DataEntryBuilder(){
    this->dataEntry.clear();
}

QSharedPointer<DataEntry> DataEntryBuilder::build(const QByteArray& masterPW){
    QByteArray plainMidKey = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256,
                                                                QByteArray::number(QRandomGenerator::global()->generate64()),
                                                                QString("some ineteresting salt").toUtf8(),
                                                                10000,
                                                                256);
    QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
    QByteArray encryptedMidKey = crypter.encode(plainMidKey, masterPW, dataEntry->getIvMidKey());
    dataEntry->setMidKey(encryptedMidKey);

    QMap<QString, QVariant> map;
    map.insert("username", dataEntry->getUsername().value_or(QString("")));
    map.insert("email", dataEntry->getEMail().value_or(QString("")));
    map.insert("password", dataEntry->getPassword().value_or(QString("")));
    map.insert("details", dataEntry->getDetails().value_or(QString("")));
    QByteArray contentAsJson = QJsonDocument(QJsonObject::fromVariantMap(map)).toJson(QJsonDocument::Compact);
    QByteArray encryptedContent = crypter.encode(contentAsJson, plainMidKey, dataEntry->getIvInner()).toBase64(QByteArray::Base64Encoding);
    dataEntry->setContent(encryptedContent);


    contentAsJson.clear();
    plainMidKey.clear();
    dataEntry->clearConfidential();

    dataEntry->setLastChanged(QDateTime::currentDateTime());
    return dataEntry;
}

QSharedPointer<DataEntry> DataEntryBuilder::fromJsonObject(const QJsonObject& jsonObject){
    QMap<QString, QVariant> map = jsonObject.toVariantMap();

    QSharedPointer<DataEntry> entry = QSharedPointer<DataEntry>(new DataEntry());
    entry->setName(map.value("name").toString());
    entry->setID(map.value("id").toString().toUtf8());
    entry->setIvInner(map.value("ivInner").toString().toUtf8());
    entry->setIvMidKey(map.value("ivMidKey").toString().toUtf8());
    entry->setMidKey(map.value("midKey").toString().toUtf8());
    entry->setContent(map.value("content").toString().toUtf8());
    entry->setLastChanged(map.value("lastChanged").toDateTime());
    if(map.value("website").toString().isEmpty()){
        entry->setWebsite(std::nullopt);
    }else{
        entry->setWebsite(map.value("website").toString());
    }
    return entry;
}

void DataEntryBuilder::addWebsite(const QString& website){
    if(website.isEmpty()){
        dataEntry->setWebsite(std::nullopt);
    }else{
        dataEntry->setWebsite(website);
    }
}
void DataEntryBuilder::addEmail(const QString& email){
    if(email.isEmpty()){
        dataEntry->setEMail(std::nullopt);
    }else{
        dataEntry->setEMail(email);
    }
}
void DataEntryBuilder::addUsername(const QString& username){
    if(username.isEmpty()){
        dataEntry->setUsername(std::nullopt);
    }else{
        dataEntry->setUsername(username);
    }
}
void DataEntryBuilder::addPassword(const QString& password){
    if(password.isEmpty()){
        dataEntry->setPassword(std::nullopt);
    }else{
        dataEntry->setPassword(password);
    }
}
void DataEntryBuilder::addDetails(const QString& details){
    if(details.isEmpty()){
        dataEntry->setDetails(std::nullopt);
    }else{
        dataEntry->setDetails(details);
    }
}



DataEntryModulator::DataEntryModulator(QSharedPointer<DataEntry> dataEntry, const QByteArray& masterPW){
    this->dataEntry = QSharedPointer<DataEntry>(dataEntry);
    dataEntry->decryptContent(masterPW);
    this->masterPW = masterPW;
}
DataEntryModulator::~DataEntryModulator(){
    saveChanges();
}
void DataEntryModulator::saveChanges(){
    this->dataEntry->encryptContent(this->masterPW);
    this->dataEntry.clear();
    this->masterPW.clear();
}

void DataEntryModulator::changeName(const QString& name){
    dataEntry->setName(name);
}
void DataEntryModulator::changeWebsite(const QString& website){
    dataEntry->setWebsite(website);
}
void DataEntryModulator::changeEmail(const QString& email){
    dataEntry->setEMail(email);
}
void DataEntryModulator::changeUsername(const QString& username){
    dataEntry->setUsername(username);
}
void DataEntryModulator::changePassword(const QString& password){
    dataEntry->setPassword(password);
}
void DataEntryModulator::changeDetails(const QString& details){
    dataEntry->setDetails(details);
}
