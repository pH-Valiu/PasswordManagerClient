#include "passwordmanagermodel.h"

PasswordManagerModel::PasswordManagerModel() :
    QObject(nullptr),
    broker{PasswordBroker::getInstance()}{}

PasswordManagerModel& PasswordManagerModel::getInstance(){
    static PasswordManagerModel passwordManagerModel;
    return passwordManagerModel;
}

int PasswordManagerModel::showHideEntry(const QByteArray& id, const QSharedPointer<QByteArray>& masterPW){
    QSharedPointer<DataEntry> dataEntry = broker.getEntryFromId(id);
    if(dataEntry){
        if(!masterPW.isNull()){
            if(dataEntry->isPlain()){
                if(dataEntry->encryptContent(masterPW->constData())){
                    return 1;
                }else{
                    return -1;
                }
            }else{
                if(dataEntry->decryptContent(masterPW->constData())){
                    return 2;
                }else{
                    return -1;
                }
            }
        }else{
            return -2;
        }
    }
    return 0;
}

int PasswordManagerModel::showEntry(const QByteArray& id, const QSharedPointer<QByteArray>& masterPW){
    QSharedPointer<DataEntry> dataEntry = broker.getEntryFromId(id);
    if(dataEntry){
        if(!masterPW.isNull()){
            if(dataEntry->decryptContent(masterPW->constData())){
                return 1;
            }else{
                return -1;
            }
        }else{
            return -2;
        }
    }
    return 0;
}

int PasswordManagerModel::hideEntry(const QByteArray& id, const QSharedPointer<QByteArray>& masterPW){
    QSharedPointer<DataEntry> dataEntry = broker.getEntryFromId(id);
    if(dataEntry){
        if(!masterPW.isNull()){
            if(dataEntry->encryptContent(masterPW->constData())){
                return 1;
            }else{
                return -1;
            }
        }else{
            return -2;
        }
    }
    return 0;
}

void PasswordManagerModel::addEntry(QSharedPointer<DataEntry>& entry){
    broker.addEntry(entry);
}

bool PasswordManagerModel::removeEntry(const QByteArray& id){
    broker.removeEntryById(id);

    return 1;
}

//effb748
std::unique_ptr<DataEntryModulator> PasswordManagerModel::getModulator(const QByteArray& id, const QSharedPointer<QByteArray>& masterPW){
    QSharedPointer<DataEntry> entry = broker.getEntryFromId(id);
    if(entry){
        if(masterPW){
            if(!masterPW.isNull()){
                return std::make_unique<DataEntryModulator>(DataEntryModulator(entry, masterPW->constData()));
            }
        }
    }
    return nullptr;

}
