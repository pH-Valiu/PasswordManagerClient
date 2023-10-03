#include "passwordbroker.h"



PasswordBroker::PasswordBroker() : vector(new QVector<QSharedPointer<DataEntry>>())
{
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



