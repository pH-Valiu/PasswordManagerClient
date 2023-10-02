#include "dataentry.h"

DataEntry::DataEntry(){}

QJsonObject DataEntry::toJsonObject(){
    QMap<QString, QVariant> map;
    map.insert("name", getName());
    map.insert("id", QString::fromUtf8(getID()));
    map.insert("ivInner", QString::fromUtf8(getIvInner().toBase64()));
    map.insert("ivMidKey", QString::fromUtf8(getIvMidKey().toBase64()));
    map.insert("midKey", QString::fromUtf8(getMidKey().toBase64()));
    map.insert("lastChanged", getLastChanged());
    map.insert("website", getWebsite().value_or(QString("")));
    map.insert("content", QString::fromUtf8(getContent().toBase64()));

    return QJsonObject::fromVariantMap(map);
}

bool DataEntry::decryptContent(const QByteArray& masterPW){
    if(!this->encryptedContent.isEmpty() && masterPW.size() == 32){
        QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
        QByteArray decryptedMidKey = crypter.removePadding(crypter.decode(this->midKey, masterPW, this->ivMidKey));
        QByteArray decryptedJson = crypter.removePadding(crypter.decode(this->encryptedContent, decryptedMidKey, this->ivInner));
        QMap<QString, QVariant> map = QJsonDocument::fromJson(decryptedJson).object().toVariantMap();

        QString username = map.value("username", "").toString();
        if(username.isEmpty()){this->username = std::nullopt;}else{this->username = username;}

        QString email = map.value("email", "").toString();
        if(email.isEmpty()){this->email = std::nullopt;}else{this->email = email;}

        QString password = map.value("password", "").toString();
        if(password.isEmpty()){this->password = std::nullopt;}else{this->password = password;}

        QString details = map.value("details", "").toString();
        if(details.isEmpty()){this->details = std::nullopt;}else{this->details = details;}

        this->encryptedContent.clear();
        decryptedMidKey.clear();
        return true;
    }else{
        return false;
    }
}

bool DataEntry::encryptContent(const QByteArray& masterPW){
    if(this->encryptedContent.isEmpty() && masterPW.size() == 32){
        QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
        QByteArray decryptedMidKey = crypter.removePadding(crypter.decode(this->midKey, masterPW, this->ivMidKey));

        QMap<QString, QVariant> map;
        map.insert("username", this->username.value_or(QString("")));
        map.insert("email", this->email.value_or(QString("")));
        map.insert("password", this->password.value_or(QString("")));
        map.insert("details", this->details.value_or(QString("")));
        QByteArray contentAsJson = QJsonDocument(QJsonObject::fromVariantMap(map)).toJson(QJsonDocument::Compact);
        QByteArray encryptedContent = crypter.encode(contentAsJson, decryptedMidKey, this->ivInner);
        this->setContent(encryptedContent);
        this->clearConfidential();
        decryptedMidKey.clear();
        return true;
    }else{
        return false;
    }
}





DataEntryBuilder::DataEntryBuilder(const QString& name){
    dataEntry = QSharedPointer<DataEntry>(new DataEntry());
    dataEntry->setID(QUuid::createUuid().toByteArray());
    if(regexNaming.match(name).hasMatch()){
        dataEntry->setName(name);
    }else{
        dataEntry->setName(dataEntry->getID());
    }
    srand(QTime::currentTime().minute());
    QByteArray ivInner, ivMidKey;
    int rnd = rand();
    ivInner.append((unsigned char*) &rnd);
    rnd = rand();
    ivMidKey.append((unsigned char*) &rnd);
    ivInner.resize(16);
    ivMidKey.resize(16);
    dataEntry->setIvInner(ivInner);
    dataEntry->setIvMidKey(ivMidKey);
    //dataEntry->setIvInner(QString("1234567890123456").toUtf8());
    //dataEntry->setIvMidKey(QString("0987654321123456").toUtf8());
}
DataEntryBuilder::~DataEntryBuilder(){
    this->dataEntry.clear();
}
QSharedPointer<DataEntry> DataEntryBuilder::build(const QByteArray& masterPW){
    if(masterPW.size() == 32){
        QByteArray plainMidKey = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256,
                                                                    QByteArray::number(QRandomGenerator::global()->generate64()),
                                                                    QString("some ineteresting salt").toUtf8(),
                                                                    10000,
                                                                    32);

        QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
        QByteArray encryptedMidKey = crypter.encode(plainMidKey, masterPW, dataEntry->getIvMidKey());
        dataEntry->setMidKey(encryptedMidKey);

        QMap<QString, QVariant> map;
        map.insert("username", dataEntry->getUsername().value_or(QString("")));
        map.insert("email", dataEntry->getEMail().value_or(QString("")));
        map.insert("password", dataEntry->getPassword().value_or(QString("")));
        map.insert("details", dataEntry->getDetails().value_or(QString("")));
        QByteArray contentAsJson = QJsonDocument(QJsonObject::fromVariantMap(map)).toJson(QJsonDocument::Compact);
        QByteArray encryptedContent = crypter.encode(contentAsJson, plainMidKey, dataEntry->getIvInner());
        dataEntry->setContent(encryptedContent);


        contentAsJson.clear();
        plainMidKey.clear();
        dataEntry->clearConfidential();

        dataEntry->setLastChanged(QDateTime::currentDateTime());
        return dataEntry;
    }else{
        return QSharedPointer<DataEntry>();
    }
}
QSharedPointer<DataEntry> DataEntryBuilder::fromJsonObject(const QJsonObject& jsonObject){
    QMap<QString, QVariant> map = jsonObject.toVariantMap();
    if(map.contains("name") && map.contains("id") && map.contains("ivInner")
            && map.contains("ivMidKey") && map.contains("midKey") && map.contains("content")
            && map.contains("lastChanged") && map.contains("website"))
    {
        QSharedPointer<DataEntry> entry = QSharedPointer<DataEntry>(new DataEntry());
        entry->setName(map.value("name", "").toString());
        entry->setID(map.value("id", "").toString().toUtf8());
        entry->setIvInner(QByteArray::fromBase64(map.value("ivInner", "").toString().toUtf8()));
        entry->setIvMidKey(QByteArray::fromBase64(map.value("ivMidKey", "").toString().toUtf8()));
        entry->setMidKey(QByteArray::fromBase64(map.value("midKey", "").toString().toUtf8()));
        entry->setContent(QByteArray::fromBase64(map.value("content", "").toString().toUtf8()));
        entry->setLastChanged(map.value("lastChanged", "").toDateTime());
        if(map.value("website", "").toString().isEmpty()){
            entry->setWebsite(std::nullopt);
        }else{
            entry->setWebsite(map.value("website", "").toString());
        }
        return entry;
    }else{
        //JSON object is incomplete
        //one or more keys (attributes) are missing
        return QSharedPointer<DataEntry>();
    }

}

void DataEntryBuilder::addWebsite(const QString& website){
    if(regexNaming.match(website).hasMatch()){
        dataEntry->setWebsite(website);
    }else{
        dataEntry->setWebsite(std::nullopt);
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
    this->modified = false;
    this->masterPW = masterPW;
}
DataEntryModulator::~DataEntryModulator(){
    saveChanges();
}
void DataEntryModulator::saveChanges(){
    bool encryptionWorked = this->dataEntry->encryptContent(this->masterPW);
    if(encryptionWorked && modified) dataEntry->setLastChanged(QDateTime::currentDateTime());

    this->modified = false;
    this->dataEntry.clear();
    this->masterPW.clear();
}

void DataEntryModulator::changeName(const QString& name){
    dataEntry->setName(name);
    modified = true;
}
void DataEntryModulator::changeWebsite(const QString& website){
    dataEntry->setWebsite(website);
    modified = true;
}
void DataEntryModulator::changeEmail(const QString& email){
    dataEntry->setEMail(email);
    modified = true;
}
void DataEntryModulator::changeUsername(const QString& username){
    dataEntry->setUsername(username);
    modified = true;
}
void DataEntryModulator::changePassword(const QString& password){
    dataEntry->setPassword(password);
    modified = true;
}
void DataEntryModulator::changeDetails(const QString& details){
    dataEntry->setDetails(details);
    modified = true;
}
bool DataEntryModulator::changeMasterPassword(const QByteArray& newMasterPW){
    if(masterPW.size() == 32 && newMasterPW.size() == 32){
        QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
        QByteArray oldDecryptedMidKey = crypter.removePadding(crypter.decode(dataEntry->getMidKey(), masterPW, dataEntry->getIvMidKey()));
        QByteArray newEncryptedMidKey = crypter.encode(oldDecryptedMidKey, newMasterPW, dataEntry->getIvMidKey());

        dataEntry->setMidKey(newEncryptedMidKey);
        masterPW = newMasterPW;
        return true;
    }else{
        return false;
    }
}
