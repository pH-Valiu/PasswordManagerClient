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
    qDebug()<<"[PMM sHE] masterPW unprotected: "<<*masterPW;
    QSharedPointer<DataEntry> dataEntry = broker.getEntryFromId(id);
    if(dataEntry){
        if(masterPW){
            if(dataEntry->isPlain()){
                if(dataEntry->encryptContent(masterPW)){
                    return 1;
                }else{
                    return -1;
                }
            }else{
                if(dataEntry->decryptContent(masterPW)){
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
        if(masterPW){
            if(dataEntry->decryptContent(masterPW)){
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
        if(masterPW){
            if(dataEntry->encryptContent(masterPW)){
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

int PasswordManagerModel::hideAllEntries(const QSharedPointer<QByteArray> &masterPW){
    int i=0;
    if(masterPW){
        foreach(const QSharedPointer<DataEntry>& entry, broker.getAllEntries()){
            if(entry->encryptContent(masterPW)){
                i++;
            }
        }
    }
    return i;
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
        if(masterPW && masterPW->size() == 32){
            return std::make_unique<DataEntryEditor>(DataEntryEditor(entry, masterPW));
        }
    }
    return nullptr;
}

std::unique_ptr<DataEntryBuilder> PasswordManagerModel::getBuilder(const QSharedPointer<QByteArray>& masterPW){
    if(masterPW && masterPW->size() == 32){
        return std::make_unique<DataEntryBuilder>(DataEntryBuilder(masterPW));
    }
    return nullptr;
}

QByteArray PasswordManagerModel::searchEntry(const QString& identifier){
    return broker.searchEntry(identifier);
}


bool PasswordManagerModel::startBroker(const QSharedPointer<QByteArray> &masterPW){
    if(masterPW){
        return broker.fetchFileData(masterPW);
    }
    return false;
}

QList<QString> PasswordManagerModel::getlAllLocalBackups(){
    return LocalBackup::getAllLocalBackups();
}

QString PasswordManagerModel::getOneBackupNewer(const QString &currentBackup){
    return LocalBackup::getOneBackupNewer(currentBackup);
}

bool PasswordManagerModel::saveBroker(const QSharedPointer<QByteArray> &masterPW){
    if(masterPW){
        return broker.storeFileData(masterPW);
    }
    return false;
}

QString PasswordManagerModel::newLocalBackup(){
    return LocalBackup::newLocalBackup();
}

bool PasswordManagerModel::revertToOlderLocalBackup(const QString& folderName){
    return LocalBackup::revertToBackup(folderName);
}



QByteArray PasswordManagerModel::getUserMasterPWHash(){
    return broker.getUserMasterPWHash();
}

bool PasswordManagerModel::validateUserMasterPW(const QByteArray& userMasterPW){
    return broker.validateUserMasterPW(userMasterPW);
}

bool PasswordManagerModel::setUserMasterPW(const QByteArray &userMasterPW){
    return broker.setUserMasterPW(userMasterPW);
}

bool PasswordManagerModel::changeUserMasterPW(const QByteArray &oldUserMasterPW, const QByteArray &newUserMasterPW, const QSharedPointer<QByteArray> &oldDerivedMasterPW, const QSharedPointer<QByteArray> &newDerivedMasterPW){
    if(validateUserMasterPW(oldUserMasterPW)){
        if(setUserMasterPW(newUserMasterPW)){
            if(oldDerivedMasterPW && newDerivedMasterPW){
                if(broker.changerMasterPW(oldDerivedMasterPW, newDerivedMasterPW)){
                    return true;
                }
            }
        }
    }
    return false;
}
