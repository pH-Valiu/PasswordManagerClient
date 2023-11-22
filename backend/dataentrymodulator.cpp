#include "dataentrymodulator.h"
#include "qaesencryption.h"
#include <QSharedPointer>

QRegularExpression DataEntryModulator::regexNaming = QRegularExpression(R"(^([a-z]|[A-Z]|[0-9]| |\$|\#|\-|\_|\.|\+|\!|\*|\'|\(|\)|\,|\/|\&|\?|\=|\:|\%)+$)");

DataEntryModulator::DataEntryModulator(){
    this->dataEntry = QSharedPointer<DataEntry>(nullptr);
    this->masterPW = QSharedPointer<QByteArray>(nullptr);
}

DataEntryModulator::~DataEntryModulator(){
    this->dataEntry.clear();
    this->masterPW.clear();
}


DataEntryBuilder::DataEntryBuilder(const QSharedPointer<QByteArray> &masterPW){
    //deep copy of the masterPW (new heap allocation)
    this->masterPW = QSharedPointer<QByteArray>(new QByteArray(*masterPW));

    dataEntry = QSharedPointer<DataEntry>(new DataEntry());
    dataEntry->setID(QUuid::createUuid().toByteArray());


    srand(QTime::currentTime().minute());
    QByteArray ivInner, ivMidKey, midKeySalt;
    int rnd = rand();
    ivInner.append((unsigned char*) &rnd);
    rnd = rand();
    ivMidKey.append((unsigned char*) &rnd);
    rnd = rand();
    midKeySalt.append((unsigned char*) &rnd);

    midKeySalt.resize(16);
    ivInner.resize(16);
    ivMidKey.resize(16);
    dataEntry->setIvInner(ivInner);
    dataEntry->setIvMidKey(ivMidKey);
    dataEntry->setMidKeySalt(midKeySalt);
}
DataEntryBuilder::~DataEntryBuilder(){
}

QSharedPointer<DataEntry> DataEntryBuilder::modulate(){
    if(this->masterPW && this->masterPW->size() == 32){
        QByteArray plainMidKey = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256,
                                                                    QByteArray::number(QRandomGenerator::global()->generate64()),
                                                                    QString("some ineteresting salt").toUtf8(),
                                                                    SECURITY_CONSTANTS::DATA_ENTRY_MID_KEY_PBKDF_ITERATIONS,
                                                                    32);

        QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
        QByteArray encryptedMidKey = crypter.encode(plainMidKey, *masterPW, dataEntry->getIvMidKey());
        dataEntry->setMidKey(encryptedMidKey);

        QByteArray plainMidKeyHash = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Blake2b_512,
                                                                        plainMidKey,
                                                                        dataEntry->getMidKeySalt(),
                                                                        SECURITY_CONSTANTS::DATA_ENTRY_MID_KEY_HASH_ITERATIONS,
                                                                        64);
        dataEntry->setMidKeyHash(plainMidKeyHash);




        QMap<QString, QVariant> map;
        map.insert("username", dataEntry->getUsername());
        map.insert("email", dataEntry->getEMail());
        map.insert("password", dataEntry->getPassword());
        map.insert("details", dataEntry->getDetails());
        QByteArray contentAsJson = QJsonDocument(QJsonObject::fromVariantMap(map)).toJson(QJsonDocument::Compact);
        QByteArray encryptedContent = crypter.encode(contentAsJson, plainMidKey, dataEntry->getIvInner());
        dataEntry->setContent(encryptedContent);
        dataEntry->plain = false;


        contentAsJson.clear();
        plainMidKey.clear();
        plainMidKeyHash.clear();
        dataEntry->clearConfidential();

        dataEntry->setLastChanged(QDateTime::currentDateTime());
        return dataEntry;
    }else{
        return nullptr;
    }
}
void DataEntryBuilder::cancel(){
    dataEntry->clearConfidential();
}

//static method
QSharedPointer<DataEntry> DataEntryBuilder::fromJsonObject(const QJsonObject &jsonObject){
    QMap<QString, QVariant> map = jsonObject.toVariantMap();
    if(map.contains("name") && map.contains("id") && map.contains("ivInner")
        && map.contains("ivMidKey") && map.contains("midKey")
        && map.contains("midKeyHash") && map.contains("midKeySalt")
        && map.contains("content")
        && map.contains("lastChanged") && map.contains("website"))
    {
        QSharedPointer<DataEntry> entry = QSharedPointer<DataEntry>(new DataEntry());
        entry->setName(map.value("name", "").toString());
        entry->setID(map.value("id", "").toString().toUtf8());
        entry->setIvInner(QByteArray::fromBase64(map.value("ivInner", "").toString().toUtf8()));
        entry->setIvMidKey(QByteArray::fromBase64(map.value("ivMidKey", "").toString().toUtf8()));
        entry->setMidKey(QByteArray::fromBase64(map.value("midKey", "").toString().toUtf8()));
        entry->setMidKeyHash(QByteArray::fromBase64(map.value("midKeyHash", "").toString().toUtf8()));
        entry->setMidKeySalt(QByteArray::fromBase64(map.value("midKeySalt", "").toString().toUtf8()));
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

void DataEntryBuilder::modulateName(const QString &name){
    if(regexNaming.match(name).hasMatch()){
        dataEntry->setName(name);
    }else{
        dataEntry->setName(dataEntry->getID());
    }
}
void DataEntryBuilder::modulateWebsite(const QString& website){
    if(regexNaming.match(website).hasMatch()){
        dataEntry->setWebsite(website);
    }else{
        dataEntry->setWebsite(std::nullopt);
    }
}
void DataEntryBuilder::modulateEmail(const QString& email){
    if(email.isEmpty()){
        dataEntry->setEMail(std::nullopt);
    }else{
        dataEntry->setEMail(email);
    }
}
void DataEntryBuilder::modulateUsername(const QString& username){
    if(username.isEmpty()){
        dataEntry->setUsername(std::nullopt);
    }else{
        dataEntry->setUsername(username);
    }
}
void DataEntryBuilder::modulatePassword(const QString& password){
    if(password.isEmpty()){
        dataEntry->setPassword(std::nullopt);
    }else{
        dataEntry->setPassword(password);
    }
}
void DataEntryBuilder::modulateDetails(const QString& details){
    if(details.isEmpty()){
        dataEntry->setDetails(std::nullopt);
    }else{
        dataEntry->setDetails(details);
    }
}

QString DataEntryBuilder::getName(){
    return dataEntry->getName();
}
QString DataEntryBuilder::getWebsite(){
    return dataEntry->getWebsite();
}
QString DataEntryBuilder::getUsername(){
    return dataEntry->getUsername();
}
QString DataEntryBuilder::getEmail(){
    return dataEntry->getEMail();
}
QString DataEntryBuilder::getPassword(){
    return dataEntry->getPassword();
}
QString DataEntryBuilder::getDetails(){
    return dataEntry->getDetails();
}




DataEntryEditor::DataEntryEditor(const QSharedPointer<DataEntry> &dataEntry, const QSharedPointer<QByteArray> &masterPW){
    if(!dataEntry.isNull()){
        this->dataEntry = dataEntry;

        //deep copy of dataEntry
        dataEntryClone = std::make_unique<DataEntry>(*dataEntry.get());

        //deep copy of masterPW (new heap allocation)
        this->masterPW = QSharedPointer<QByteArray>(new QByteArray(*masterPW));
        this->modified = false;
        this->passwordOk = true;
        if(!dataEntryClone->decryptContent(masterPW)){
            this->passwordOk = false;
        }
    }
}
DataEntryEditor::~DataEntryEditor(){
    dataEntryClone.reset();
}

QSharedPointer<DataEntry> DataEntryEditor::modulate(){
    QSharedPointer<DataEntry> returnEntry = nullptr;
    if(passwordOk){
        bool encryptionWorked = this->dataEntryClone->encryptContent(masterPW);
        if(encryptionWorked && modified){
            dataEntry->setName(dataEntryClone->getName());
            dataEntry->setWebsite(dataEntryClone->getWebsite());
            dataEntry->setLastChanged(QDateTime::currentDateTime());
            dataEntry->setMidKey(dataEntryClone->getMidKey());
            dataEntry->setMidKeyHash(dataEntryClone->getMidKeyHash());
            dataEntry->setMidKeySalt(dataEntryClone->getMidKeySalt());
            dataEntry->setContent(dataEntryClone->getContent());

            returnEntry = dataEntry;
        }
        this->dataEntryClone->clearData();
        this->modified = false;

    }

    return returnEntry;
}
void DataEntryEditor::cancel(){
    this->dataEntryClone->clearData();
}

void DataEntryEditor::modulateName(const QString& name){
    dataEntryClone->setName(name);
    modified = true;
}
void DataEntryEditor::modulateWebsite(const QString& website){
    dataEntryClone->setWebsite(website);
    modified = true;
}
void DataEntryEditor::modulateEmail(const QString& email){
    dataEntryClone->setEMail(email);
    modified = true;
}
void DataEntryEditor::modulateUsername(const QString& username){
    dataEntryClone->setUsername(username);
    modified = true;
}
void DataEntryEditor::modulatePassword(const QString& password){
    dataEntryClone->setPassword(password);
    modified = true;
}
void DataEntryEditor::modulateDetails(const QString& details){
    dataEntryClone->setDetails(details);
    modified = true;
}

QString DataEntryEditor::getName(){
    return dataEntryClone->getName();
}
QString DataEntryEditor::getWebsite(){
    return dataEntryClone->getWebsite();
}
QString DataEntryEditor::getUsername(){
    return dataEntryClone->getUsername();
}
QString DataEntryEditor::getEmail(){
    return dataEntryClone->getEMail();
}
QString DataEntryEditor::getPassword(){
    return dataEntryClone->getPassword();
}
QString DataEntryEditor::getDetails(){
    return dataEntryClone->getDetails();
}

bool DataEntryEditor::changeMasterPassword(const QSharedPointer<QByteArray>& newMasterPW){
    if(passwordOk){
        if(this->masterPW && this->masterPW->size() == 32 && newMasterPW && newMasterPW->size() == 32){
            QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
            QByteArray oldDecryptedMidKey = crypter.removePadding(crypter.decode(dataEntryClone->getMidKey(), *masterPW, dataEntryClone->getIvMidKey()));
            QByteArray oldDecryptedMidKeyHash = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Blake2b_512,
                                                                                   oldDecryptedMidKey,
                                                                                   dataEntryClone->getMidKeySalt(),
                                                                                   SECURITY_CONSTANTS::DATA_ENTRY_MID_KEY_HASH_ITERATIONS,
                                                                                   64);
            if(oldDecryptedMidKeyHash != dataEntryClone->getMidKeyHash()){
                //derived hash != stored hash
                //return false
                oldDecryptedMidKey.clear();
                oldDecryptedMidKeyHash.clear();
                return false;
            }
            QByteArray newEncryptedMidKey = crypter.encode(oldDecryptedMidKey, *newMasterPW, dataEntryClone->getIvMidKey());
            dataEntryClone->setMidKey(newEncryptedMidKey);

            oldDecryptedMidKey.clear();
            oldDecryptedMidKeyHash.clear();
            newEncryptedMidKey.clear();

            this->masterPW = newMasterPW;
            modified = true;
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}
