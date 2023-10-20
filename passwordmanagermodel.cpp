#include "passwordmanagermodel.h"

PasswordManagerModel::PasswordManagerModel() :
    QObject(nullptr),
    broker{PasswordBroker::getInstance()}{}

PasswordManagerModel& PasswordManagerModel::getInstance(){
    static PasswordManagerModel passwordManagerModel;
    return passwordManagerModel;
}

int PasswordManagerModel::showHideEntry(const QByteArray& id){
    qDebug()<<"called";
    QSharedPointer<DataEntry> dataEntry = broker.getEntryFromId(id);
    if(dataEntry){
        if(dataEntry->isPlain()){
            if(dataEntry->encryptContent(GLOBALES_TEMP::staticMasterPW)){

                return 1;
            }
        }else{
            if(dataEntry->decryptContent(GLOBALES_TEMP::staticMasterPW)){
                return 2;
            }
        }
    }
    return 0;
}

void PasswordManagerModel::addEntry(QSharedPointer<DataEntry> &entry){
    broker.addEntry(entry);
}
