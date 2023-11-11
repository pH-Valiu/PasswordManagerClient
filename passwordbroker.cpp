#include "passwordbroker.h"
#include "messagehandler.h"
#include "qaesencryption.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QMutableListIterator>
#include <QMessageAuthenticationCode>




PasswordBroker::PasswordBroker() : vector(){
}

PasswordBroker::~PasswordBroker(){
    for(qsizetype i=0; i<vector.size(); i++){
        vector.at(i)->clearData();
    }
    QMutableListIterator<QSharedPointer<DataEntry>> it(vector);
    while(it.hasNext()){
        it.next().clear();
    }
    vector.clear();
}

PasswordBroker& PasswordBroker::getInstance(){
    static PasswordBroker broker;
    return broker;
}

bool PasswordBroker::fetchFileData(const QByteArray& masterPW){
    QString applicationDirPath = QCoreApplication::applicationDirPath();
    QDir mainDirectory(applicationDirPath);
    mainDirectory.mkdir("database");


    QFile fileIv(applicationDirPath + "/database/iv");
    if(!fileIv.exists() || fileIv.size() == 0){
        if(fileIv.open(QIODevice::WriteOnly)){
            srand(QTime::currentTime().msec());
            int rnd = rand();
            fileData.iv.resize(0);
            fileData.iv.append((unsigned char*) &rnd);
            fileData.iv.resize(16);
            fileIv.close();
        }else{
            //IV FILE COULD NOT BE OPENED
            //ABORT
            MessageHandler::warn("IV file could not be opened: " + fileIv.errorString());
            return false;
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
                MessageHandler::critical("IV size is not correct and might be corrupted");
                return false;
            }
        }else{
            //IV FILE COULD NOT BE OPENED
            //ABORT
            MessageHandler::warn("IV file could not be opened: " + fileIv.errorString());
            return false;
        }
    }



    QFile fileMAC(applicationDirPath + "/database/mac");
    if(!fileMAC.exists() || fileMAC.size() == 0){
        if(fileMAC.open(QIODevice::WriteOnly)){
            fileData.mac.resize(0);
            fileMAC.close();
        }else{
            //IV FILE COULD NOT BE OPENED
            //ABORT
            MessageHandler::warn("MAC file could not be opened: " + fileMAC.errorString());
            return false;
        }
    }else{
        if(fileMAC.open(QIODevice::ReadOnly)){
            QTextStream macInput(&fileMAC);
            fileData.mac = QByteArray::fromBase64(macInput.readAll().toUtf8());
            fileMAC.close();
        }else{
            //MAC FILE COULD NOT BE OPENED
            //ABORT
            MessageHandler::warn("MAC file could not be opened: " + fileMAC.errorString());
            return false;
        }
    }


    QFile fileEntries(applicationDirPath + "/database/dataEntries");
    if(!fileEntries.exists() || fileEntries.size() == 0){
        if(fileEntries.open(QIODevice::WriteOnly)){
            fileData.encryptedEntries.resize(0);
            fileEntries.close();
        }else{
            //DATAENTRIES FILE COULD NOT BE OPENEND
            //ABORT
            MessageHandler::warn("DataEntries file could not be opened: " + fileEntries.errorString());
            return false;
        }
    }else{
        if(fileData.mac.isNull() ){//|| fileData.mac.isEmpty()){
            //MAC FILE DOES NOT EXIST WHILE ENTRIES FILE EXIST
            //MISSING MAC
            //ABORT
            MessageHandler::critical("MAC file does not exist while DataEntries file does exist");
            return false;
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
                MessageHandler::warn("DataEntries file could not be opened: " + fileEntries.errorString());
                return false;
            }
        }
    }
    if(decryptData(masterPW)){
        fetchedFlag = true;
        return true;
    }else{
        return false;
    }
}

bool PasswordBroker::storeFileData(const QByteArray& masterPW){
    QString applicationDirPath = QCoreApplication::applicationDirPath();
    QDir mainDirectory(applicationDirPath);
    mainDirectory.mkdir("database");


    if(fetchedFlag){
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
                MessageHandler::critical("IV size is not correct and might be corrupted");
                return false;
            }
            ivInput << QString::fromUtf8(fileData.iv.toBase64());
            ivInput.flush();
            fileIv.close();
        }else{
            //IV FILE COULD NOT BE OPENED
            //ABORT
            MessageHandler::warn("IV file could not be opened: " + fileIv.errorString());
            return false;
        }

        if(encryptData(masterPW)){
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
                MessageHandler::warn("MAC file could not be opened: " + fileMAC.errorString());
                return false;
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
                MessageHandler::warn("DataEntries file could not be opened: " + fileEntries.errorString());
                return false;
            }
            return true;
        }else{
            return false;
        }
    }else{
        MessageHandler::critical("Trying to save data to file before files have ever been fetched", "Data Integrity Error:");
        return false;
    }
}

bool PasswordBroker::encryptData(const QByteArray& masterPW){
    if(fileData.iv.isNull() || fileData.iv.isEmpty()){
        MessageHandler::warn("No IV available to encrypt data");
        return false;
    }
    QJsonArray jsonArray;
    for(qsizetype i=0; i<vector.size(); i++){
        jsonArray.append(vector.at(i)->toJsonObject());

    }
    QByteArray jsonDoc = QJsonDocument(jsonArray).toJson(QJsonDocument::Indented);

    QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
    QByteArray encryptedDoc = crypter.encode(jsonDoc, masterPW, fileData.iv);
    if(encryptedDoc.isNull() || encryptedDoc.isEmpty()){
        MessageHandler::warn("Encryption of json data failed");
        return false;
    }

    fileData.encryptedEntries = encryptedDoc;

    fileData.mac = QMessageAuthenticationCode::hash(encryptedDoc, masterPW, QCryptographicHash::Sha256);

    return true;
}

bool PasswordBroker::decryptData(const QByteArray& masterPW){
    if(fileData.encryptedEntries.isNull()){
        MessageHandler::warn("Encrypted entries variable hasn't been initialized");
        return false;
    }
    if(fileData.mac.isNull()){
        MessageHandler::warn("MAC variable hasn't been initialized");
        return false;
    }
    if(fileData.iv.isNull() || fileData.iv.isEmpty()){
        MessageHandler::warn("No IV available to decrypt data");
        return false;
    }

    //No decryption if encryptedEntries is empty
    if(!fileData.encryptedEntries.isEmpty()){
        QByteArray computedMAC = QMessageAuthenticationCode::hash(fileData.encryptedEntries, masterPW, QCryptographicHash::Sha256);

        if(computedMAC != fileData.mac){
            MessageHandler::critical("Computed MAC unequal to fetched MAC from file");
            return false;
        }

        //MACs are equal

        QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
        QByteArray decryptedEntries = crypter.removePadding(crypter.decode(fileData.encryptedEntries, masterPW, fileData.iv));

        if(decryptedEntries.isNull() || decryptedEntries.isEmpty()){
            MessageHandler::warn("Decryption of encrypted data entries failed");
            return false;
        }


        QJsonParseError error;
        QJsonDocument json = QJsonDocument::fromJson(decryptedEntries, &error);
        if(error.error != QJsonParseError::NoError){
            MessageHandler::warn(error.errorString(), "JSON Error:");
            return false;
        }
        if(!json.isArray()){
            MessageHandler::warn("JSON file is not an array", "JSON Error:");
            return false;
        }
        QJsonArray jsonArray = json.array();
        for(qsizetype i=0; i<jsonArray.size(); i++){
            if(!jsonArray.at(i).isObject()){
                MessageHandler::warn("Array value was not an object", "JSON Error:");
                return false;
            }
            QSharedPointer<DataEntry> entry = DataEntryBuilder::fromJsonObject(jsonArray.at(i).toObject());
            if(entry.isNull()){
                MessageHandler::warn("JSON object did not contain all required 'keys'", "JSON Error:");
                return false;
            }
            vector.append(entry);

        }
    }
    return true;
}

bool PasswordBroker::changerMasterPW(const QByteArray& oldMasterPW, const QByteArray& newMasterPW){
    for(qsizetype i=0; i<vector.size(); i++){
        DataEntryEditor editor(vector.at(i), oldMasterPW);
        if(!editor.changeMasterPassword(newMasterPW)){
            editor.cancel();
            MessageHandler::critical("Changing master password failed at: " +vector.at(i)->getName(), "Password Error:");
            return false;
        }
        editor.modulate();
    }
    return true;
}

void PasswordBroker::addEntry(QSharedPointer<DataEntry>& dataEntry){
    vector.append(dataEntry);
}

bool PasswordBroker::removeEntryById(const QByteArray& id){
    for(qsizetype i=0; i<vector.size(); i++){
        if(vector.at(i)->getID() == id){
            vector.remove(i);
            return true;
        }
    }
    return false;
}
void PasswordBroker::removeAllEntries(){
    for(qsizetype i=0; i<vector.size(); i++){
        vector[i].clear();
    }
    vector.clear();
}

QSharedPointer<DataEntry> PasswordBroker::getEntryFromId(const QByteArray& id){
    for(qsizetype i=0; i<vector.size(); i++){
        if(vector.at(i)->getID() == id){
            return vector.at(i);
        }
    }
    return nullptr;
}

QSharedPointer<DataEntry> PasswordBroker::getEntryFromName(const QString& name){
    for(qsizetype i=0; i<vector.size(); i++){
        if(vector.at(i)->getName() == name){
            return vector.at(i);
        }
    }
    return nullptr;
}

QSharedPointer<DataEntry> PasswordBroker::getEntryFromWebsite(const QString& website){
    for(qsizetype i=0; i<vector.size(); i++){
        if(vector.at(i)->getWebsite() == website){
            return vector.at(i);
        }
    }
    return nullptr;
}

QByteArray PasswordBroker::searchEntry(const QString& identifier){
    for(qsizetype i=0; i<vector.size(); i++){
        if(vector.at(i)->getName().contains(identifier)){
            return vector.at(i)->getID();
        }
    }
    for(qsizetype i=0; i<vector.size(); i++){
        if(vector.at(i)->getWebsite().contains(identifier)){
            return vector.at(i)->getID();
        }
    }
    return QByteArray();
}


QByteArray PasswordBroker::getUserMasterPWHash(){
    QString applicationPath = QCoreApplication::applicationDirPath();
    QDir mainDirectory(applicationPath + "/database");

    if(mainDirectory.exists()){

        QFile hashedPWFile(applicationPath + "/database/pw");
        if(!hashedPWFile.exists() || hashedPWFile.size() == 0){
            //PW FILE DOES NOT EXIST
            //MessageHandler::critical("Password file does not yet exist!");
        }
        else if(hashedPWFile.open(QIODevice::ReadOnly)){
            QTextStream pwInput(&hashedPWFile);
            QByteArray storedHashedPW = QByteArray::fromBase64(pwInput.readAll().toUtf8());
            hashedPWFile.close();
            return storedHashedPW;

        }else{
            //PW FILE COULD NOT BE OPENED
            //ABORT
            MessageHandler::warn("Password file could not be opened: " + hashedPWFile.errorString());
        }
    }
    return QByteArray();
}

bool PasswordBroker::validateUserMasterPW(const QString& userMasterPW){

    QString applicationPath = QCoreApplication::applicationDirPath();
    QDir mainDirectory(applicationPath + "/database");

    if(mainDirectory.exists()){

        QFile hashedPWFile(applicationPath + "/database/pw");
        if(!hashedPWFile.exists() || hashedPWFile.size() == 0){
            //PW FILE DOES NOT EXIST
            MessageHandler::critical("Password file does not yet exist!");
            return false;
        }
        else if(hashedPWFile.open(QIODevice::ReadOnly)){
            QTextStream pwInput(&hashedPWFile);
            QByteArray storedHashedPW = QByteArray::fromBase64(pwInput.readAll().toUtf8());
            hashedPWFile.close();
            QByteArray hashedPW = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Blake2b_512,
                                                                      userMasterPW.toUtf8(),
                                                                      "1dn9vm-sadm4t§$:F;$§§f3)&46²€af",
                                                                      50,
                                                                      64);

            if(storedHashedPW == hashedPW){
                return true;
            }

        }else{
            //PW FILE COULD NOT BE OPENED
            //ABORT
            MessageHandler::warn("Password file could not be opened: " + hashedPWFile.errorString());
            return false;
        }
    }else{
        return false;
    }
    return false;
}

bool PasswordBroker::setUserMasterPW(const QString& userMasterPW){
    QString applicationDirPath = QCoreApplication::applicationDirPath();
    QDir mainDirectory(applicationDirPath);

    mainDirectory.mkdir("database");

    QFile hashedPWFile(applicationDirPath + "/database/pw");
    if(hashedPWFile.exists()){
        hashedPWFile.resize(0);
    }
    if(hashedPWFile.open(QIODevice::WriteOnly)){
        QTextStream pwInput(&hashedPWFile);
        QByteArray hashedPW = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Blake2b_512,
                                                                 userMasterPW.toUtf8(),
                                                                 "1dn9vm-sadm4t§$:F;$§§f3)&46²€af",
                                                                 50,
                                                                 64);

        pwInput << QString::fromUtf8(hashedPW.toBase64());
        pwInput.flush();
        hashedPWFile.close();
        return true;
    }else{
        //PW FILE COULD NOT BE OPENED
        //ABORT
        MessageHandler::warn("Password file could not be opened: " + hashedPWFile.errorString());
        return false;
    }
}
