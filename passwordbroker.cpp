#include "passwordbroker.h"



PasswordBroker::PasswordBroker()
{

}

PasswordBroker::~PasswordBroker(){
    foreach(QSharedPointer<PasswordBroker::DataEntry> entry, (*vector.get())){
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


PasswordBroker::DataEntry::DataEntry(){
    this->id = QUuid::createUuid().toByteArray();
    SystemFunction036(ivInner.data(), 16);
    SystemFunction036(ivMidKey.data(), 16);
    /*
    QByteArray plainMidKey = QPasswordDigestor::deriveKeyPbkdf2(QCryptographicHash::Sha256,
                                                                QByteArray::number(QRandomGenerator::global()->generate64()),
                                                                QString("some ineteresting salt").toUtf8(),
                                                                10000,
                                                                256);
                                                                */
}
