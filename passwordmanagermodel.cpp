#include "passwordmanagermodel.h"
#include "localbackup.h"

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
    return broker.removeEntryById(id);
}

std::unique_ptr<DataEntryEditor> PasswordManagerModel::getEditor(const QByteArray& id, const QSharedPointer<QByteArray>& masterPW){
    QSharedPointer<DataEntry> entry = broker.getEntryFromId(id);
    if(entry){
        if(masterPW){
            if(!masterPW.isNull()){
                return std::make_unique<DataEntryEditor>(DataEntryEditor(entry, masterPW->constData()));
            }
        }
    }
    return nullptr;
}

std::unique_ptr<DataEntryBuilder> PasswordManagerModel::getBuilder(const QSharedPointer<QByteArray>& masterPW){
    if(masterPW){
        if(!masterPW.isNull()){
            return std::make_unique<DataEntryBuilder>(DataEntryBuilder(masterPW->constData()));
        }
    }
    return nullptr;
}

QByteArray PasswordManagerModel::searchEntry(const QString& identifier){
    return broker.searchEntry(identifier);
}


bool PasswordManagerModel::startBroker(const QSharedPointer<QByteArray> &masterPW){
    if(masterPW){
        return broker.fetchFileData(masterPW->constData());
    }
    return false;
}

bool PasswordManagerModel::saveBroker(const QSharedPointer<QByteArray> &masterPW){
    if(masterPW){
        if(broker.storeFileData(masterPW->constData())){
            return LocalBackup::newLocalBackup();
        }
    }
    return false;
}

bool PasswordManagerModel::revertToOlderLocalBackup(const QString& folderName){
    return LocalBackup::revertToBackup(folderName);
}



QByteArray PasswordManagerModel::getUserMasterPWHash(){
    return broker.getUserMasterPWHash();
}

bool PasswordManagerModel::validateUserMasterPW(const QString& userMasterPW){
    return broker.validateUserMasterPW(userMasterPW);
}

bool PasswordManagerModel::setUserMasterPW(const QString &userMasterPW){
    return broker.setUserMasterPW(userMasterPW);
}

bool PasswordManagerModel::changeUserMasterPW(const QString &oldUserMasterPW, const QString &newUserMasterPW, const QSharedPointer<QByteArray> &oldDerivedMasterPW, const QSharedPointer<QByteArray> &newDerivedMasterPW){
    if(validateUserMasterPW(oldUserMasterPW)){
        if(setUserMasterPW(newUserMasterPW)){
            if(oldDerivedMasterPW && newDerivedMasterPW){
                if(broker.changerMasterPW(oldDerivedMasterPW->constData(), newDerivedMasterPW->constData())){
                    return true;
                }
            }
        }
    }
    return false;
}
