#include "dataentry.h"
#include "qaesencryption.h"

DataEntry::DataEntry(){}

DataEntry::~DataEntry(){
    clearData();
}

QJsonObject DataEntry::toJsonObject() const{
    QMap<QString, QVariant> map;
    map.insert("name", getName());
    map.insert("id", QString::fromUtf8(getID()));
    map.insert("ivInner", QString::fromUtf8(getIvInner().toBase64()));
    map.insert("ivMidKey", QString::fromUtf8(getIvMidKey().toBase64()));
    map.insert("midKey", QString::fromUtf8(getMidKey().toBase64()));
    map.insert("midKeyHash", QString::fromUtf8(getMidKeyHash().toBase64()));
    map.insert("midKeySalt", QString::fromUtf8(getMidKeySalt().toBase64()));
    map.insert("lastChanged", getLastChanged());
    map.insert("website", getWebsite());
    map.insert("content", QString::fromUtf8(getContent().toBase64()));

    return QJsonObject::fromVariantMap(map);
}

bool DataEntry::decryptContent(const QSharedPointer<QByteArray>& masterPW){
    if(!this->encryptedContent.isNull() && !this->encryptedContent.isEmpty() && masterPW && masterPW->size() == 32 && !plain){
        QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
        QByteArray decryptedMidKey = crypter.removePadding(crypter.decode(this->midKey, *masterPW, this->ivMidKey));

        //check decryptedMidKey with midKeyHash
        QByteArray decryptedMidKeyHash = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Blake2b_512,
                                                                            decryptedMidKey,
                                                                            this->midKeySalt,
                                                                            SECURITY_CONSTANTS::DATA_ENTRY_MID_KEY_HASH_ITERATIONS,
                                                                            64);
        if(decryptedMidKeyHash != this->midKeyHash){
            //hashes do not align
            //return false
            decryptedMidKey.clear();
            decryptedMidKeyHash.clear();
            return false;
        }


        QByteArray decryptedJson = crypter.removePadding(crypter.decode(this->encryptedContent, decryptedMidKey, this->ivInner));

        QJsonParseError* parseError;
        QJsonDocument contentAsJsonDoc = QJsonDocument::fromJson(decryptedJson, parseError);
        if(parseError && parseError->error != QJsonParseError::NoError){
            //ERROR while parsing
            //return false
            decryptedMidKey.clear();
            return false;
        }
        QMap<QString, QVariant> map = contentAsJsonDoc.object().toVariantMap();

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
        decryptedMidKeyHash.clear();
        plain = true;
        return true;
    }else{
        return false;
    }
}

bool DataEntry::encryptContent(const QSharedPointer<QByteArray>& masterPW){
    if((this->encryptedContent.isNull() || this->encryptedContent.isEmpty()) && masterPW && masterPW->size() == 32 && plain){
        QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
        QByteArray decryptedMidKey = crypter.removePadding(crypter.decode(this->midKey, *masterPW, this->ivMidKey));

        //check decryptedMidKey with midKeyHash
        QByteArray decryptedMidKeyHash = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Blake2b_512,
                                                                            decryptedMidKey,
                                                                            this->midKeySalt,
                                                                            SECURITY_CONSTANTS::DATA_ENTRY_MID_KEY_HASH_ITERATIONS,
                                                                            64);
        if(decryptedMidKeyHash != this->midKeyHash){
            //hashes did not align
            //return false
            decryptedMidKey.clear();
            decryptedMidKeyHash.clear();
            return false;
        }


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
        decryptedMidKeyHash.clear();
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
    if(this->midKeyHash != other.midKeyHash) return false;
    if(this->midKeySalt != other.midKeySalt) return false;
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
    if(d1.getMidKeyHash() != d2.getMidKeyHash()){
        string+= "midKeyHash: " + d1.getMidKeyHash().toBase64() + " vs "+d2.getMidKeyHash().toBase64()+"\n";
    }
    if(d1.getMidKeySalt() != d2.getMidKeySalt()){
        string+= "midKeySalt: " + d1.getMidKeySalt().toBase64() + " vs "+d2.getMidKeySalt().toBase64()+"\n";
    }
    if(d1.getWebsite() != d2.getWebsite()){
        string+= "website: " + d1.getWebsite()+ " vs "+d2.getWebsite()+"\n";
    }
    if(d1.getContent() != d2.getContent()){
        string+= "content: " + d1.getContent().toBase64() + " vs "+d2.getContent().toBase64()+"\n";
    }
    if(d1.getEMail() != d2.getEMail()){
        string+= "email: " + d1.getEMail() + " vs "+d2.getEMail()+"\n";
    }
    if(d1.getUsername() != d2.getUsername()){
        string+= "username: " + d1.getUsername() + " vs "+d2.getUsername()+"\n";
    }
    if(d1.getPassword() != d2.getPassword()){
        string+= "password: " + d1.getPassword() + " vs "+d2.getPassword()+"\n";
    }
    if(d1.getDetails() != d2.getDetails()){
        string+= "details: " + d1.getDetails() + " vs "+d2.getDetails()+"\n";
    }
    return string;

}


