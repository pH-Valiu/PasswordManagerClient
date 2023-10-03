#include "passwordbroker.h"



PasswordBroker::PasswordBroker() : vector(new QVector<QSharedPointer<DataEntry>>())
{
    //vector(new QVector<QSharedPointer<DataEntry>>());
}

PasswordBroker::~PasswordBroker(){
    qDebug()<<"deletion";
    foreach(QSharedPointer<DataEntry> entry, (*vector.get())){
        qDebug()<<"each";
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



