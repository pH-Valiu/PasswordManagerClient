#include "passwordbroker.h"



PasswordBroker::PasswordBroker()
{

}

PasswordBroker::~PasswordBroker(){
    foreach(QSharedPointer<DataEntry> entry, (*vector.get())){
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



