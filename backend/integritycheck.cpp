#include "integritycheck.h"
#include "constants.h"
#include "qaesencryption.h"

void IntegrityCheckWorker::doWork(const QByteArray& masterPW, const QVector<QSharedPointer<DataEntry>>& entries){

    foreach(const QSharedPointer<DataEntry>& entry, entries){
        if(entry){
            if(!entry->checkIntegrity(masterPW)){
                emit this->workFinished(1);
                return;
            }
        }
    }

    emit this->workFinished(0);
    return;
}

IntegrityCheckController::IntegrityCheckController(){
    IntegrityCheckWorker* worker = new IntegrityCheckWorker;
    worker->moveToThread(&integrityCheckThread);

    connect(&integrityCheckThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &IntegrityCheckController::operate, worker, &IntegrityCheckWorker::doWork);
    connect(worker, &IntegrityCheckWorker::workFinished, this, [&](const int& retCode){
        SecureZeroMemory(this->masterPW.data(), this->masterPW.size());
       emit integrityCheckFinished(retCode);
    });

    integrityCheckThread.start();
}

IntegrityCheckController::~IntegrityCheckController(){
    integrityCheckThread.quit();
    integrityCheckThread.wait();
}

void IntegrityCheckController::checkIntegrity(const QSharedPointer<QByteArray>& masterPW, const QVector<QSharedPointer<DataEntry>> &entries){
    this->masterPW = *masterPW;
    emit operate(this->masterPW, entries);
}


