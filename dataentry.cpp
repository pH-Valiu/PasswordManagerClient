#include "dataentry.h"
#include "qaesencryption.h"

QRegularExpression DataEntryBuilder::regexNaming = QRegularExpression(R"(^([a-z]|[A-Z]|[0-9]| |\$|\#|\-|\_|\.|\+|\!|\*|\'|\(|\)|\,|\/|\&|\?|\=|\:|\%)+$)");

DataEntry::DataEntry(){
    qDebug()<<"entry created";
}

DataEntry::~DataEntry(){
    clearData();
    qDebug()<<"entry deleted";
}

QJsonObject DataEntry::toJsonObject() const{
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
    //function won't notify you about wrong masterPW
    if(!this->encryptedContent.isNull() && !this->encryptedContent.isEmpty() && masterPW.size() == 32 && !plain){
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
        plain = true;
        return true;
    }else{
        return false;
    }
}

bool DataEntry::encryptContent(const QByteArray& masterPW){
    //this function won't notify you about wrong masterPW
    if((this->encryptedContent.isNull() || this->encryptedContent.isEmpty()) && masterPW.size() == 32 && plain){
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
        plain = false;
        return true;
    }else{
        return false;
    }
}

bool DataEntry::operator==(const DataEntry& other) const{
    if(this->name != other.name) return false;
    if(this->id != other.id) return false;
    if(this->lastChanged != other.lastChanged) return false;
    if(this->ivInner != other.ivInner) return false;
    if(this->ivMidKey != other.ivMidKey) return false;
    if(this->midKey != other.midKey) return false;
    if(this->website != other.website) return false;
    if(this->encryptedContent != other.encryptedContent) return false;
    if(this->email != other.email) return false;
    if(this->username != other.username) return false;
    if(this->password != other.password) return false;
    if(this->details != other.details) return false;
    return true;
}

QString DataEntry::showDiff(DataEntry& d1, DataEntry& d2){
    QString string("diff:\n");
    if(d1.getName() != d2.getName()){
        string+= "name: " + d1.getName() + " vs "+d2.getName()+"\n";
    }
    if(d1.getID() != d2.getID()){
        string+= "id: " + d1.getID().toBase64() + " vs "+d2.getID().toBase64()+"\n";
    }
    if(d1.getLastChanged() != d2.getLastChanged()){
        string+= "lastChanged: " + d1.getLastChanged().toString() + " vs "+d2.getLastChanged().toString()+"\n";
    }
    if(d1.getIvInner() != d2.getIvInner()){
        string+= "ivInner: " + d1.getIvInner().toBase64() + " vs "+d2.getIvInner().toBase64()+"\n";
    }
    if(d1.getIvMidKey() != d2.getIvMidKey()){
        string+= "ivMidKey: " + d1.getIvMidKey().toBase64() + " vs "+d2.getIvMidKey().toBase64()+"\n";
    }
    if(d1.getMidKey() != d2.getMidKey()){
        string+= "midKey: " + d1.getMidKey().toBase64() + " vs "+d2.getMidKey().toBase64()+"\n";
    }
    if(d1.getWebsite() != d2.getWebsite()){
        string+= "website: " + d1.getWebsite().value_or("") + " vs "+d2.getWebsite().value_or("")+"\n";
    }
    if(d1.getContent() != d2.getContent()){
        string+= "content: " + d1.getContent().toBase64() + " vs "+d2.getContent().toBase64()+"\n";
    }
    if(d1.getEMail() != d2.getEMail()){
        string+= "email: " + d1.getEMail().value_or("") + " vs "+d2.getEMail().value_or("")+"\n";
    }
    if(d1.getUsername() != d2.getUsername()){
        string+= "username: " + d1.getUsername().value_or("") + " vs "+d2.getUsername().value_or("")+"\n";
    }
    if(d1.getPassword() != d2.getPassword()){
        string+= "password: " + d1.getPassword().value_or("") + " vs "+d2.getPassword().value_or("")+"\n";
    }
    if(d1.getDetails() != d2.getDetails()){
        string+= "details: " + d1.getDetails().value_or("") + " vs "+d2.getDetails().value_or("")+"\n";
    }
    return string;

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
        dataEntry->plain = false;


        contentAsJson.clear();
        plainMidKey.clear();
        dataEntry->clearConfidential();

        dataEntry->setLastChanged(QDateTime::currentDateTime());
        return dataEntry;
    }else{
        return nullptr;
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
        return nullptr;
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
    if(!dataEntry.isNull()){
        this->dataEntry = QSharedPointer<DataEntry>(dataEntry);
        dataEntryClone = DataEntry(*dataEntry.get());

        this->masterPW = masterPW;
        this->modified = false;
        dataEntryClone.decryptContent(masterPW);
    }
}

DataEntryModulator::~DataEntryModulator(){
    this->dataEntry.clear();
    this->dataEntryClone.clearData();
    SecureZeroMemory(masterPW.data(), masterPW.size());
    this->masterPW.clear();
}

void DataEntryModulator::saveChanges(){
    bool encryptionWorked = this->dataEntryClone.encryptContent(masterPW);
    if(encryptionWorked && modified){
        dataEntry->setName(dataEntryClone.getName());
        dataEntry->setWebsite(dataEntryClone.getWebsite());
        dataEntry->setLastChanged(QDateTime::currentDateTime());
        dataEntry->setMidKey(dataEntryClone.getMidKey());
        dataEntry->setContent(dataEntryClone.getContent());
    }

    this->dataEntryClone.clearData();
    SecureZeroMemory(masterPW.data(), masterPW.size());
    this->masterPW.clear();
    this->modified = false;

}

void DataEntryModulator::cancelChanges(){
    this->dataEntryClone.clearData();
    SecureZeroMemory(masterPW.data(), masterPW.size());
    this->masterPW.clear();
    this->modified = false;
}

void DataEntryModulator::changeName(const QString& name){
    dataEntryClone.setName(name);
    modified = true;
}
void DataEntryModulator::changeWebsite(const QString& website){
    dataEntryClone.setWebsite(website);
    modified = true;
}
void DataEntryModulator::changeEmail(const QString& email){
    dataEntryClone.setEMail(email);
    modified = true;
}
void DataEntryModulator::changeUsername(const QString& username){
    dataEntryClone.setUsername(username);
    modified = true;
}
void DataEntryModulator::changePassword(const QString& password){
    dataEntryClone.setPassword(password);
    modified = true;
}
void DataEntryModulator::changeDetails(const QString& details){
    dataEntryClone.setDetails(details);
    modified = true;
}
bool DataEntryModulator::changeMasterPassword(const QByteArray& newMasterPW){
    //if oldMasterPW is incorrect this function will not notify you
    //it will appear as if everything worked correctly
    if(this->masterPW.size() == 32 && newMasterPW.size() == 32){
        QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
        QByteArray oldDecryptedMidKey = crypter.removePadding(crypter.decode(dataEntry->getMidKey(), this->masterPW, dataEntry->getIvMidKey()));
        QByteArray newEncryptedMidKey = crypter.encode(oldDecryptedMidKey, newMasterPW, dataEntry->getIvMidKey());

        dataEntryClone.setMidKey(newEncryptedMidKey);
        this->masterPW = newMasterPW;
        modified = true;
        return true;
    }else{
        return false;
    }
}
