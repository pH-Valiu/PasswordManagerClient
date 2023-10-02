#include "dataentrytest.h"


void DataEntryTest::initTestCase(){
    masterPW = QCryptographicHash::hash(QString("masterPassword").toUtf8(), QCryptographicHash::Sha256);
    DataEntryBuilder builder("apple");
    builder.addDetails("Just call up the website² and \"log\" in ?*?");
    builder.addWebsite("https://apple.com/?query=user-log_on#");
    builder.addUsername("user1");
    builder.addPassword(",~£:1Od33jy+lj");
    builder.addEmail("user1@apple.com");
    appleEntry = builder.build(masterPW);
}

void DataEntryTest::testBuilderFull(){

    QDateTime lastChanged = appleEntry->getLastChanged();
    QCOMPARE(appleEntry->getName(), "apple");
    QCOMPARE(appleEntry->getUsername(), std::nullopt);
    QCOMPARE(appleEntry->getPassword(), std::nullopt);
    QCOMPARE(appleEntry->getEMail(), std::nullopt);
    QCOMPARE(appleEntry->getDetails(), std::nullopt);
    QCOMPARE(appleEntry->getWebsite(), "https://apple.com/?query=user-log_on#");
    appleEntry->decryptContent(masterPW);
    QCOMPARE(appleEntry->getUsername(), "user1");
    QCOMPARE(appleEntry->getPassword(), ",~£:1Od33jy+lj");
    QCOMPARE(appleEntry->getEMail(), "user1@apple.com");
    QCOMPARE(appleEntry->getDetails(), "Just call up the website² and \"log\" in ?*?");
    appleEntry->encryptContent(masterPW);
    QCOMPARE(appleEntry->getUsername(), std::nullopt);
    QCOMPARE(appleEntry->getPassword(), std::nullopt);
    QCOMPARE(appleEntry->getEMail(), std::nullopt);
    QCOMPARE(appleEntry->getDetails(), std::nullopt);
    QDateTime lastChangedLater = appleEntry->getLastChanged();
    QCOMPARE(lastChanged, lastChangedLater);
}

void DataEntryTest::testBuilderJSON(){
    QFile file("DataEntryBuilder.test");
    file.resize(0);
    if(file.open(QIODevice::WriteOnly)){
        QTextStream textStream(&file);
        QByteArray json = QJsonDocument(appleEntry->toJsonObject()).toJson();
        textStream << json;
        file.close();
    }

    QByteArray json;
    if(file.open(QIODevice::ReadOnly)){
        QTextStream textStream(&file);
        json = file.readAll();
        file.close();
    }
    QSharedPointer<DataEntry> entry = DataEntryBuilder::fromJsonObject(QJsonDocument::fromJson(json).object());
    qDebug() << "id after: "<<QString::fromUtf8(entry.data()->getID());

    entry.data()->decryptContent(masterPW);
    qDebug() << "content: "<<QString::fromUtf8(entry.data()->getContentPublic()) << "\n"
             << "username: "<< entry.data()->getUsername().value_or(QString("no value")) <<"\n"
             << "password: "<<entry.data()->getPassword().value_or(QString("no value"));
    QCOMPARE(entry->getPassword(), QString(",~£:1Od33jy+lj"));
    entry.data()->encryptContent(masterPW);
    qDebug() << "content: "<<QString::fromUtf8(entry.data()->getContentPublic()) << "\n"
             << "username: "<< entry.data()->getUsername().value_or(QString("no value")) <<"\n"
             << "password: "<<entry.data()->getPassword().value_or(QString("no value"));

    entry.data()->decryptContent(masterPW);
    qDebug() << "content: "<<QString::fromUtf8(entry.data()->getContentPublic()) << "\n"
             << "username: "<< entry.data()->getUsername().value_or(QString("no value")) <<"\n"
             << "password: "<<entry.data()->getPassword().value_or(QString("no value"));
    entry.data()->encryptContent(masterPW);
    qDebug() << "content: "<<QString::fromUtf8(entry.data()->getContentPublic()) << "\n"
             << "username: "<< entry.data()->getUsername().value_or(QString("no value")) <<"\n"
             << "password: "<<entry.data()->getPassword().value_or(QString("no value"));

}


    /*

        QTextStream textStream(&file);
        DataEntryBuilder builder("apple");
        builder.addPassword("megaPasswort");
        builder.addUsername("user1");
        //builder.addWebsite("apple.com");
        QSharedPointer<DataEntry> apple = builder.build(masterPW);
        qDebug()<<"size of masterPW: "<<masterPW.size();


        qDebug() << "id prior: "<<QString::fromUtf8(apple.data()->getID());

    }



    if(file.open(QIODevice::ReadOnly)){
        QTextStream textStream(&file);
        QByteArray json = file.readAll();
        file.close();
        QSharedPointer<DataEntry> entry = DataEntryBuilder::fromJsonObject(QJsonDocument::fromJson(json).object());
        qDebug() << "id after: "<<QString::fromUtf8(entry.data()->getID());

        entry.data()->decryptContent(masterPW);
        qDebug() << "content: "<<QString::fromUtf8(entry.data()->getContentPublic()) << "\n"
                 << "username: "<< entry.data()->getUsername().value_or(QString("no value")) <<"\n"
                 << "password: "<<entry.data()->getPassword().value_or(QString("no value"));
        QCOMPARE(entry->getPassword(), QString("megaPasswort"));
        entry.data()->encryptContent(masterPW);
        qDebug() << "content: "<<QString::fromUtf8(entry.data()->getContentPublic()) << "\n"
                 << "username: "<< entry.data()->getUsername().value_or(QString("no value")) <<"\n"
                 << "password: "<<entry.data()->getPassword().value_or(QString("no value"));

        entry.data()->decryptContent(masterPW);
        qDebug() << "content: "<<QString::fromUtf8(entry.data()->getContentPublic()) << "\n"
                 << "username: "<< entry.data()->getUsername().value_or(QString("no value")) <<"\n"
                 << "password: "<<entry.data()->getPassword().value_or(QString("no value"));
        entry.data()->encryptContent(masterPW);
        qDebug() << "content: "<<QString::fromUtf8(entry.data()->getContentPublic()) << "\n"
                 << "username: "<< entry.data()->getUsername().value_or(QString("no value")) <<"\n"
                 << "password: "<<entry.data()->getPassword().value_or(QString("no value"));
    }

    QRegularExpression regexNaming = QRegularExpression(R"(^([a-z]|[A-Z]|[0-9]| |\$|\#|\-|\_|\.|\+|\!|\*|\'|\(|\)|\,|\/|\&|\?|\=|\:|\%)+$)");    QString test("https://amazon.com//Gd=450//_$-%+!*#()&?:%'");//
    QString test2("d f455 sdf-s,gf4,.q2r.dfs#'df43+*+");
    QRegularExpressionMatch match = regexNaming.match(test2);
    qDebug()<<"re is valid?: "<<regexNaming.isValid();
    qDebug()<<"regex match?: "<<match.hasMatch();
    */


