#include "integritycheck.h"
#include "constants.h"
#include "qaesencryption.h"

void IntegrityCheckWorker::doWork(const QSharedPointer<QByteArray>& masterPW, const QVector<QSharedPointer<DataEntry>>& entries){
    //deep copy of masterPW
    this->masterPW = *masterPW;
    data.clear();

    foreach(const QSharedPointer<DataEntry>& entry, entries){
        data.append(entry->getIntegrityData());
    }

    QAESEncryption crypter(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);
    QByteArray decryptedMidKey, decryptedMidKeyHash;

    for(int i=0; i<data.size(); i++){
        SecureZeroMemory(decryptedMidKey.data(), decryptedMidKey.size());

        struct DataEntry::IntegrityCheckData currentData = data.at(i);
        decryptedMidKey = crypter.removePadding(crypter.decode(currentData.ICMidKey, this->masterPW, currentData.ICivMidKey));

        decryptedMidKeyHash = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Blake2b_512,
                                                                 decryptedMidKey,
                                                                 currentData.ICMidKeySalt,
                                                                 SECURITY_CONSTANTS::DATA_ENTRY_MID_KEY_HASH_ITERATIONS,
                                                                 64);

        if(decryptedMidKeyHash != currentData.ICMidKeyHash){
            //hashes are not equal
            //emit false
            emit this->workFinished(1);
            return;
        }
    }

    SecureZeroMemory(decryptedMidKey.data(), decryptedMidKey.size());
    SecureZeroMemory(this->masterPW.data(), this->masterPW.size());
    data.clear();

    emit this->workFinished(0);}


IntegrityCheckController::IntegrityCheckController(){
    IntegrityCheckWorker* worker = new IntegrityCheckWorker;
    worker->moveToThread(&integrityCheckThread);

    connect(&integrityCheckThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &IntegrityCheckController::operate, worker, &IntegrityCheckWorker::doWork);
    connect(worker, &IntegrityCheckWorker::workFinished, this, [&](const int& retCode){
       emit integrityCheckFinished(retCode);
    });

    integrityCheckThread.start();
}

IntegrityCheckController::~IntegrityCheckController(){
    integrityCheckThread.quit();
    integrityCheckThread.wait();
}

void IntegrityCheckController::checkIntegrity(const QSharedPointer<QByteArray> &masterPW, const QVector<QSharedPointer<DataEntry>> &entries){
    emit operate(masterPW, entries);
}


