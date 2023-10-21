#include "passwordmanagermodel.h"

PasswordManagerModel::PasswordManagerModel() :
    QObject(nullptr),
    broker{PasswordBroker::getInstance()}{}

PasswordManagerModel& PasswordManagerModel::getInstance(){
    static PasswordManagerModel passwordManagerModel;
    return passwordManagerModel;
}

int PasswordManagerModel::showHideEntry(const QByteArray& id){
    QSharedPointer<DataEntry> dataEntry = broker.getEntryFromId(id);
    if(dataEntry){
        if(dataEntry->isPlain()){
            if(dataEntry->encryptContent(GLOBALES_TEMP::staticMasterPW)){
                return 1;
            }else{
                return -1;
            }
        }else{
            if(dataEntry->decryptContent(GLOBALES_TEMP::staticMasterPW)){
                return 2;
            }else{
                return -1;
            }
        }
    }
    return 0;
}

void PasswordManagerModel::addEntry(QSharedPointer<DataEntry> &entry){
    broker.addEntry(entry);
}

std::unique_ptr<DataEntryModulator> PasswordManagerModel::getModulator(const QByteArray& id){
    return std::make_unique<DataEntryModulator>(DataEntryModulator(broker.getEntryFromId(id), GLOBALES_TEMP::staticMasterPW));
}
