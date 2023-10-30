#include "dataentrymodulator.h"
#include "qaesencryption.h"

QRegularExpression DataEntryModulator::regexNaming = QRegularExpression(R"(^([a-z]|[A-Z]|[0-9]| |\$|\#|\-|\_|\.|\+|\!|\*|\'|\(|\)|\,|\/|\&|\?|\=|\:|\%)+$)");

DataEntryModulator::DataEntryModulator(){
    dataEntry = nullptr;
    masterPW.clear();
}

DataEntryModulator::~DataEntryModulator(){
    this->dataEntry.clear();
    SecureZeroMemory(masterPW.data(), masterPW.size());
}


DataEntryBuilder::DataEntryBuilder(const QByteArray &masterPW){
    this->masterPW = masterPW;

    dataEntry = QSharedPointer<DataEntry>(new DataEntry());
    dataEntry->setID(QUuid::createUuid().toByteArray());


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
}

QSharedPointer<DataEntry> DataEntryBuilder::modulate(){
    if(this->masterPW.size() == 32){
        QByteArray plainMidKey = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256,
                                                                    QByteArray::number(QRandomGenerator::global()->generate64()),
                                                                    QString("some ineteresting salt").toUtf8(),
                                                                    10000,
                                                                    32);

        QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
        QByteArray encryptedMidKey = crypter.encode(plainMidKey, this->masterPW, dataEntry->getIvMidKey());
        dataEntry->setMidKey(encryptedMidKey);

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




DataEntryEditor::DataEntryEditor(const QSharedPointer<DataEntry> &dataEntry, const QByteArray &masterPW){
    if(!dataEntry.isNull()){
        this->dataEntry = dataEntry;

        //deep copy of dataEntry
        dataEntryClone = std::make_unique<DataEntry>(*dataEntry.get());

        this->masterPW = masterPW;
        this->modified = false;
        dataEntryClone->decryptContent(masterPW);
    }
}
DataEntryEditor::~DataEntryEditor(){
    dataEntryClone.reset();
}

QSharedPointer<DataEntry> DataEntryEditor::modulate(){
    QSharedPointer<DataEntry> returnEntry = nullptr;
    bool encryptionWorked = this->dataEntryClone->encryptContent(masterPW);
    if(encryptionWorked && modified){
        dataEntry->setName(dataEntryClone->getName());
        dataEntry->setWebsite(dataEntryClone->getWebsite());
        dataEntry->setLastChanged(QDateTime::currentDateTime());
        dataEntry->setMidKey(dataEntryClone->getMidKey());
        dataEntry->setContent(dataEntryClone->getContent());

        returnEntry = dataEntry;
    }

    this->dataEntryClone->clearData();
    this->modified = false;

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

bool DataEntryEditor::changeMasterPassword(const QByteArray& newMasterPW){
    //if oldMasterPW is incorrect this function will not notify you
    //it will appear as if everything worked correctly
    if(this->masterPW.size() == 32 && newMasterPW.size() == 32){
        QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
        QByteArray oldDecryptedMidKey = crypter.removePadding(crypter.decode(dataEntryClone->getMidKey(), this->masterPW, dataEntryClone->getIvMidKey()));
        QByteArray newEncryptedMidKey = crypter.encode(oldDecryptedMidKey, newMasterPW, dataEntryClone->getIvMidKey());

        dataEntryClone->setMidKey(newEncryptedMidKey);
        this->masterPW = newMasterPW;
        modified = true;
        return true;
    }else{
        return false;
    }
}
