#include "passwordbroker.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QMutableListIterator>
#include <QMessageAuthenticationCode>


PasswordBroker::PasswordBroker() : vector(new QVector<QSharedPointer<DataEntry>>()){
}

PasswordBroker::~PasswordBroker(){
    for(qsizetype i=0; i<vector->size(); i++){
        vector->at(i)->clearData();
    }
    QMutableListIterator<QSharedPointer<DataEntry>> it(*vector.get());
    while(it.hasNext()){
        it.next().clear();
    }
    vector->clear();
}

QSharedPointer<PasswordBroker> PasswordBroker::getInstance(){
    static QSharedPointer<PasswordBroker> broker = QSharedPointer<PasswordBroker>();
    if(broker.isNull()){
        broker = QSharedPointer<PasswordBroker>(new PasswordBroker());
    }
    return broker;
}

QString PasswordBroker::fetchFileData(){
    QString applicationDirPath = QCoreApplication::applicationDirPath();
    QDir mainDirectory(applicationDirPath);
    mainDirectory.mkdir("database");


    QFile fileIv(applicationDirPath + "/database/iv");
    if(!fileIv.exists()){
        srand(QTime::currentTime().msec());
        int rnd = rand();
        fileData.iv.append((unsigned char*) &rnd);
        fileData.iv.resize(16);
        if(fileIv.open(QIODevice::WriteOnly)){
            QTextStream ivInput(&fileIv);
            ivInput << QString::fromUtf8(fileData.iv.toBase64());
            ivInput.flush();
            fileIv.close();
        }else{
            //IV FILE COULD NOT BE OPENED
            //ABORT
            return "Internal Error: IV file could not be opened: " + fileIv.errorString();
        }
    }else{
        if(fileIv.open(QIODevice::ReadOnly)){
            QTextStream ivInput(&fileIv);
            fileData.iv = QByteArray::fromBase64(ivInput.readAll().toUtf8());
            fileIv.close();
            if(fileData.iv.size() != 16){
                //IV SIZE IS NOT CORRECT
                //ABORT
                //IV FILE IS CORRUPTED
                return "Internal Error: IV size is not correct and might be corrupted";
            }
        }else{
            //IV FILE COULD NOT BE OPENED
            //ABORT
            return "Internal Error: IV file could not be opened: " + fileIv.errorString();
        }
    }


    QFile fileMAC(applicationDirPath + "/database/mac");
    if(!fileMAC.exists()){
        if(fileMAC.open(QIODevice::WriteOnly)){
            fileMAC.close();
        }else{
            //IV FILE COULD NOT BE OPENED
            //ABORT
            return "Internal Error: MAC file could not be opened: " + fileMAC.errorString();
        }
    }else{
        if(fileMAC.open(QIODevice::ReadOnly)){
            QTextStream macInput(&fileMAC);
            fileData.mac = QByteArray::fromBase64(macInput.readAll().toUtf8());
            fileMAC.close();
        }else{
            //MAC FILE COULD NOT BE OPENED
            //ABORT
            return "Internal Error: MAC file could not be opened: " + fileMAC.errorString();
        }
    }

    QFile fileEntries(applicationDirPath + "/database/dataEntries");
    if(!fileEntries.exists()){
        if(fileEntries.open(QIODevice::WriteOnly)){
            fileEntries.close();
        }else{
            //DATAENTRIES FILE COULD NOT BE OPENEND
            //ABORT
             return "Internal Error: DataEntries file could not be opened: " + fileEntries.errorString();
        }
    }else{
        if(fileData.mac.isNull() ){//|| fileData.mac.isEmpty()){
            //MAC FILE DOES NOT EXIST WHILE ENTRIES FILE EXIST
            //MISSING MAC
            //ABORT
            return "Internal Error: MAC file does not exist while DataEntries file does exist";
        }else{
            if(fileEntries.open(QIODevice::ReadOnly)){
                QTextStream entriesInput(&fileEntries);
                fileData.encryptedEntries = QByteArray::fromBase64(entriesInput.readAll().toUtf8());
                fileEntries.close();
                //Integrity of file has to be checked
                //in decryptData that is
            }else{
                //DATAENTRIES FILE COULD NOT BE OPENEND
                //ABORT
                return "Internal Error: DataEntries file could not be opened: " + fileEntries.errorString();
            }
        }
    }
    return "";
}

QString PasswordBroker::storeFileData(){
    QString applicationDirPath = QCoreApplication::applicationDirPath();
    QDir mainDirectory(applicationDirPath);
    mainDirectory.mkdir("database");


    QFile fileIv(applicationDirPath + "/database/iv");
    if(fileIv.exists()){
        fileIv.resize(0);
    }
    if(fileIv.open(QIODevice::WriteOnly)){
        QTextStream ivInput(&fileIv);
        if(fileData.iv.size() != 16){
            //IV SIZE IS NOT CORRECT
            //ABORT
            //IV FILE IS CORRUPTED
            return "Internal Error: IV size is not correct and might be corrupted";
        }
        ivInput << QString::fromUtf8(fileData.iv.toBase64());
        ivInput.flush();
        fileIv.close();
    }else{
        //IV FILE COULD NOT BE OPENED
        //ABORT
        return "Internal Error: IV file could not be opened: " + fileIv.errorString();
    }


    QFile fileMAC(applicationDirPath + "/database/mac");
    if(fileMAC.exists()){
        fileMAC.resize(0);
    }
    if(fileMAC.open(QIODevice::WriteOnly)){
        QTextStream macInput(&fileMAC);
        macInput << QString::fromUtf8(fileData.mac.toBase64());
        macInput.flush();
        fileMAC.close();
    }else{
        //IV FILE COULD NOT BE OPENED
        //ABORT
        return "Internal Error: MAC file could not be opened: " + fileMAC.errorString();
    }

    QFile fileEntries(applicationDirPath + "/database/dataEntries");
    if(fileEntries.exists()){
        fileEntries.resize(0);
    }
    if(fileEntries.open(QIODevice::WriteOnly)){
        QTextStream entriesInput(&fileEntries);
        entriesInput << QString::fromUtf8(fileData.encryptedEntries.toBase64());
        entriesInput.flush();
        fileEntries.close();
    }else{
        //DATAENTRIES FILE COULD NOT BE OPENEND
        //ABORT
        return "Internal Error: DataEntries file could not be opened: " + fileEntries.errorString();
    }
    return "";
}

QString PasswordBroker::encryptData(const QByteArray& masterPW){
    QJsonArray jsonArray;
    for(qsizetype i=0; i<vector->size(); i++){
        jsonArray.append(vector->at(i)->toJsonObject());

    }
    QByteArray jsonDoc = QJsonDocument(jsonArray).toJson(QJsonDocument::Indented);

    QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
    QByteArray encryptedDoc = crypter.encode(jsonDoc, masterPW, fileData.iv);
    if(encryptedDoc.isNull() || encryptedDoc.isEmpty()){
        return "Internal Error: encryption of json data failed";
    }

    fileData.encryptedEntries = encryptedDoc;

    fileData.mac = QMessageAuthenticationCode::hash(encryptedDoc, masterPW, QCryptographicHash::Sha256);

    return "";
}

QString PasswordBroker::decryptData(const QByteArray& masterPW){
    if(fileData.encryptedEntries.isNull() || fileData.encryptedEntries.isEmpty()){
        return "Internal Error: No data entries available to decrypt";
    }
    if(fileData.mac.isNull() || fileData.mac.isEmpty()){
        return "Internal Error: No MAC available to verify authentity";
    }

    QByteArray computedMAC = QMessageAuthenticationCode::hash(fileData.encryptedEntries, masterPW, QCryptographicHash::Sha256);

    if(computedMAC != fileData.mac){
        return "Security Error: computed MAC unequal to fetched MAC from file";
    }

    //MACs are equal

    QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
    QByteArray decryptedEntries = crypter.removePadding(crypter.decode(fileData.encryptedEntries, masterPW, fileData.iv));

    if(decryptedEntries.isNull() || decryptedEntries.isEmpty()){
        return "Internal Error: decryption of encrypted data entries failed";
    }


    QJsonParseError error;
    QJsonDocument json = QJsonDocument::fromJson(decryptedEntries, &error);
    if(error.error != QJsonParseError::NoError){
        return "JSON Error: " + error.errorString();
    }
    if(!json.isArray()){
        return "JSON Error: json file is not an array";
    }
    QJsonArray jsonArray = json.array();
    for(qsizetype i=0; i<jsonArray.size(); i++){
        if(!jsonArray.at(i).isObject()){
            return "JSON Error: array value was not an object";
        }
        QSharedPointer<DataEntry> entry = DataEntryBuilder::fromJsonObject(jsonArray.at(i).toObject());
        if(entry.isNull()){
            return "JSON Error: json object did not contain all required keys";
        }
        vector->append(entry);

    }
    return "";
}

QString PasswordBroker::changerMasterPW(const QByteArray& oldMasterPW, const QByteArray& newMasterPW){
    //Maybe try catch block for error catching and user notification?

    for(qsizetype i=0; i<vector->size(); i++){
        DataEntryModulator modulator(vector->at(i), oldMasterPW);
        if(!modulator.changeMasterPassword(newMasterPW)){
            modulator.saveChanges();
            return "Changing master password failed at " + vector->at(i)->getName();
        }
        modulator.saveChanges();
    }
    return "";
}

void PasswordBroker::addEntry(QSharedPointer<DataEntry> dataEntry){
    vector->append(dataEntry);
}

bool PasswordBroker::removeEntry(const QByteArray& id){
    int entryRemoved = vector->removeIf([&](QSharedPointer<DataEntry> entry){
            return entry->getID() == id;
    });
    return entryRemoved;
}

QSharedPointer<DataEntry> PasswordBroker::getEntryFromId(const QByteArray& id){
    for(qsizetype i=0; i<vector->size(); i++){
        if(vector->at(i)->getID() == id){
            return vector->at(i);
        }
    }
    return nullptr;
}

QSharedPointer<DataEntry> PasswordBroker::getEntryFromName(const QString& name){
    for(qsizetype i=0; i<vector->size(); i++){
        if(vector->at(i)->getName() == name){
            return vector->at(i);
        }
    }
    return nullptr;
}

QSharedPointer<DataEntry> PasswordBroker::getEntryFromWebsite(const QString& website){
    for(qsizetype i=0; i<vector->size(); i++){
        if(vector->at(i)->getWebsite() == website){
            return vector->at(i);
        }
    }
    return nullptr;
}



