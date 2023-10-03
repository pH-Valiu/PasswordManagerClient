#include "passwordbroker.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>


PasswordBroker::PasswordBroker() : vector(new QVector<QSharedPointer<DataEntry>>()){
    QString mainDirectory = QCoreApplication::applicationDirPath();
    QDir database(mainDirectory.append("/database"));
    if(!database.exists()){
        database.mkpath(mainDirectory.append("/database"));
    }

    QFile fileIv(mainDirectory.append("/database/iv"));
    if(!database.exists("iv")){
        srand(QTime::currentTime().msec());
        int rnd = rand();
        iv.append((unsigned char*) &rnd);
        iv.resize(16);
        if(fileIv.open(QIODevice::WriteOnly)){
            QTextStream ivInput(&fileIv);
            ivInput << QString::fromUtf8(iv.toBase64());
            ivInput.flush();
            fileIv.close();
        }else{
            //IV FILE COULD NOT BE OPENED
            //ABORT
        }
    }else{
        if(fileIv.open(QIODevice::ReadOnly)){
            QTextStream ivInput(&fileIv);
            iv = QByteArray::fromBase64(ivInput.readAll().toUtf8());
            fileIv.close();
            if(iv.size() != 16){
                //IV SIZE IS NOT CORRECT
                //ABORT
                //IV FILE IS CORRUPTED
            }
        }else{
            //IV FILE COULD NOT BE OPENED
            //ABORT
        }
    }

    QFile fileEntries(mainDirectory.append("/database/dataEntries"));
    if(!database.exists("dataEntries")){
        if(fileEntries.open(QIODevice::WriteOnly)){
            fileEntries.close();
        }else{
            //DATAENTRIES FILE COULD NOT BE OPENEND
            //ABORT
        }
    }else{
        if(fileEntries.open(QIODevice::ReadOnly)){
            QTextStream entriesInput(&fileEntries);
            encryptedEntries = QByteArray::fromBase64(entriesInput.readAll().toUtf8());
            fileEntries.close();
            //Integrity of file has to be checked
        }else{
            //DATAENTRIES FILE COULD NOT BE OPENEND
            //ABORT
        }
    }
}

PasswordBroker::~PasswordBroker(){
    foreach(QSharedPointer<DataEntry> entry, (*vector.get())){
        entry.data()->clearData();
        entry.clear();
    }
}

QSharedPointer<PasswordBroker> PasswordBroker::getInstance(){
    static QSharedPointer<PasswordBroker> broker = QSharedPointer<PasswordBroker>();
    if(broker.isNull()){
        broker = QSharedPointer<PasswordBroker>(new PasswordBroker());
    }
    return broker;
}

bool PasswordBroker::encryptData(const QByteArray& masterPW){
    return false;
}

bool PasswordBroker::decryptData(const QByteArray& masterPW){
    return false;
}

bool PasswordBroker::changerMasterPW(const QByteArray& oldMasterPW, const QByteArray& newMasterPW){
    //Maybe try catch block for error catching and user notification?

    for(qsizetype i=0; i<vector->size(); i++){
        DataEntryModulator modulator(vector->at(i), oldMasterPW);
        if(!modulator.changeMasterPassword(newMasterPW)){
            modulator.saveChanges();
            return false;
        }
        modulator.saveChanges();
    }
    return true;
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



