#include "dataentry.h"


DataEntry::DataEntry(){
    this->id = QUuid::createUuid().toByteArray();
    SystemFunction036(ivInner.data(), 16);
    SystemFunction036(ivMidKey.data(), 16);

}

DataEntryBuilder::DataEntryBuilder(const QString& name, const QString& password){
    dataEntry = QSharedPointer<DataEntry>(new DataEntry());
    dataEntry->setName(name);
    dataEntry->setPassword(password);
}

QSharedPointer<DataEntry> DataEntryBuilder::build(QSharedPointer<QByteArray> pointerToMasterPW){
    QByteArray plainMidKey = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256,
                                                                QByteArray::number(QRandomGenerator::global()->generate64()),
                                                                QString("some ineteresting salt").toUtf8(),
                                                                10000,
                                                                256);
    QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
    QByteArray encryptedMidKey = crypter.encode(plainMidKey, *pointerToMasterPW.get(), dataEntry->ivMidKey);
    dataEntry->setMidKey(encryptedMidKey);

    QMap<QString, QVariant> map;
    map.insert("username", dataEntry->getUsername().value_or(QString("")));
    map.insert("email", dataEntry->getEMail().value_or(QString("")));
    map.insert("password", dataEntry->getPassword().value_or(QString("")));
    map.insert("details", dataEntry->getDetails().value_or(QString("")));
    QByteArray contentAsJson = QJsonDocument(QJsonObject::fromVariantMap(map)).toJson(QJsonDocument::Compact);
    QByteArray encryptedContent = crypter.encode(contentAsJson, plainMidKey, dataEntry->ivInner).toBase64(QByteArray::Base64Encoding);
    dataEntry->setContent(encryptedContent);


    contentAsJson.clear();
    plainMidKey.clear();
    dataEntry->clearConfidential();

    return dataEntry;
}

void DataEntryBuilder::addWebsite(const QString& website){
    dataEntry->setWebsite(website);
}
void DataEntryBuilder::addEmail(const QString& email){
    dataEntry->setEMail(email);
}
void DataEntryBuilder::addUsername(const QString& username){
    dataEntry->setUsername(username);
}
void DataEntryBuilder::addDetails(const QString& details){
    dataEntry->setDetails(details);
}
