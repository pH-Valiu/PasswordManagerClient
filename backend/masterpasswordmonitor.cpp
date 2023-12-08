#include "masterpasswordmonitor.h"
#include <windows.h>
#include <dpapi.h>
#include <QDebug>
#include <wincrypt.h>

MasterPasswordMonitor::MasterPasswordMonitor(const QSharedPointer<QByteArray>& masterPW){
    this->masterPW = masterPW;
    protect();
}

void MasterPasswordMonitor::setMasterPW(const QSharedPointer<QByteArray> &newMasterPW) {
    mutex.lock();
    unprotect();

    this->masterPW.clear();
    this->masterPW = newMasterPW;

    protect();
    mutex.unlock();
}

MasterPasswordMonitor::~MasterPasswordMonitor(){
    mutex.lock();
    unprotect();

    SecureZeroMemory(masterPW.get()->data(), masterPW.get()->size());
    masterPW.clear();

    mutex.unlock();
}


template<typename T, typename Consumer>
T MasterPasswordMonitor::performOperation(Consumer consumer){
    mutex.lock();
    unprotect();

    T returnValue = consumer(this->masterPW);

    protect();
    mutex.unlock();

    return returnValue;
}


void MasterPasswordMonitor::example(){
    auto f = [&](const QSharedPointer<QByteArray>& masterPW){
        return this->fun(masterPW, 10);
    };

    int ret = performOperation<int>(f);
    qDebug()<<"ret: "<<ret;
}

int MasterPasswordMonitor::fun(const QSharedPointer<QByteArray> &masterPW, int number){
    return masterPW.get()->size() + number;

}

bool MasterPasswordMonitor::protect(){
    DWORD bPW = masterPW->size();
    LPVOID pPW = (LPVOID) masterPW.get()->data();

    if(bPW != 32){
        return false;
    }

    int dMod = 0;
    if((dMod = bPW % CRYPTPROTECTMEMORY_BLOCK_SIZE)){
        bPW += (CRYPTPROTECTMEMORY_BLOCK_SIZE - dMod);
    }

    if(!CryptProtectMemory(pPW, bPW, CRYPTPROTECTMEMORY_SAME_PROCESS)){
        SecureZeroMemory(pPW, bPW);
        pPW = NULL;
        return false;
    }

    pPW = NULL;
    this->masterPW->resize(32);
    return S_OK;
}

bool MasterPasswordMonitor::unprotect(){
    DWORD bPW = masterPW->size();
    LPVOID pPW = (LPVOID) masterPW.get()->data();

    if(bPW != 32){
        return false;
    }

    int dMod = 0;
    if((dMod = bPW % CRYPTPROTECTMEMORY_BLOCK_SIZE)){
        bPW += (CRYPTPROTECTMEMORY_BLOCK_SIZE - dMod);
    }

    if(!CryptUnprotectMemory(pPW, bPW, CRYPTPROTECTMEMORY_SAME_PROCESS)){
        SecureZeroMemory(pPW, bPW);
        pPW = NULL;
        return false;
    }

    pPW = NULL;
    masterPW->resize(32);
    return S_OK;
}
