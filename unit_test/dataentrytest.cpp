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

void DataEntryTest::testBuilderPartial(){
    DataEntryBuilder builderAmazon("amazon");
    builderAmazon.addPassword("PasswordAmazon");
    QSharedPointer<DataEntry> amazonEntry = builderAmazon.build(masterPW);
    QCOMPARE(amazonEntry->getName(), "amazon");
    QCOMPARE(amazonEntry->getUsername(), std::nullopt);
    QCOMPARE(amazonEntry->getPassword(), std::nullopt);
    QCOMPARE(amazonEntry->getEMail(), std::nullopt);
    QCOMPARE(amazonEntry->getDetails(), std::nullopt);
    QCOMPARE(amazonEntry->getWebsite(), std::nullopt);
    amazonEntry->decryptContent(masterPW);
    QCOMPARE(amazonEntry->getUsername(), std::nullopt);
    QCOMPARE(amazonEntry->getPassword(), "PasswordAmazon");
    QCOMPARE(amazonEntry->getEMail(), std::nullopt);
    QCOMPARE(amazonEntry->getDetails(), std::nullopt);
    amazonEntry->encryptContent(masterPW);
    QCOMPARE(amazonEntry->getUsername(), std::nullopt);
    QCOMPARE(amazonEntry->getPassword(), std::nullopt);
    QCOMPARE(amazonEntry->getEMail(), std::nullopt);
    QCOMPARE(amazonEntry->getDetails(), std::nullopt);
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
    QDateTime lastChanged = appleEntry->getLastChanged();
    QCOMPARE(entry->getName(), "apple");
    QCOMPARE(entry->getUsername(), std::nullopt);
    QCOMPARE(entry->getPassword(), std::nullopt);
    QCOMPARE(entry->getEMail(), std::nullopt);
    QCOMPARE(entry->getDetails(), std::nullopt);
    QCOMPARE(entry->getWebsite(), "https://apple.com/?query=user-log_on#");
    entry->decryptContent(masterPW);
    QCOMPARE(entry->getUsername(), "user1");
    QCOMPARE(entry->getPassword(), ",~£:1Od33jy+lj");
    QCOMPARE(entry->getEMail(), "user1@apple.com");
    QCOMPARE(entry->getDetails(), "Just call up the website² and \"log\" in ?*?");
    entry->encryptContent(masterPW);
    QCOMPARE(entry->getUsername(), std::nullopt);
    QCOMPARE(entry->getPassword(), std::nullopt);
    QCOMPARE(entry->getEMail(), std::nullopt);
    QCOMPARE(entry->getDetails(), std::nullopt);

    QCOMPARE(appleEntry->getID(), entry->getID().append(""));
    QDateTime lastChangedLater = entry->getLastChanged();
    QCOMPARE(lastChanged, lastChangedLater);
}

void DataEntryTest::testRegex(){
    QString test("https://amazon.com//Gd=450//_$-%+!*#()&?:%'");
    QString test2("d f455 sdf-s,gf4,.q2r.dfs#'df43+*+");
    QString testFail("abc\"ABC\"-123");
    QString testFail2("abc§8§");
    QString testFail3("abc~sdas~");
    QCOMPARE(true, DataEntryBuilder::regexNaming.match(test).hasMatch());
    QCOMPARE(true, DataEntryBuilder::regexNaming.match(test2).hasMatch());
    QCOMPARE(false, DataEntryBuilder::regexNaming.match(testFail).hasMatch());
    QCOMPARE(false, DataEntryBuilder::regexNaming.match(testFail2).hasMatch());
    QCOMPARE(false, DataEntryBuilder::regexNaming.match(testFail3).hasMatch());

    DataEntryBuilder failingBuilder("fai~ling");
    failingBuilder.addWebsite("https://\\//");
    QSharedPointer<DataEntry> failedEntry = failingBuilder.build(masterPW);
    QCOMPARE(failedEntry->getName(), failedEntry->getID());
    QCOMPARE(failedEntry->getWebsite(), std::nullopt);

}

void DataEntryTest::testModulator(){
    DataEntryModulator appleMod(appleEntry, masterPW);
    QDateTime preLastChanged = appleEntry->getLastChanged();
    QCOMPARE(appleEntry->getEMail(), "user1@apple.com");
    appleMod.changeEmail("newMail@apple.com");
    appleMod.saveChanges();
    QVERIFY(preLastChanged != appleEntry->getLastChanged());
    QCOMPARE(appleEntry->getEMail(), std::nullopt);
    appleEntry->decryptContent(masterPW);
    QCOMPARE(appleEntry->getEMail(), "newMail@apple.com");
    appleEntry->encryptContent(masterPW);

    DataEntryModulator appleMod2(appleEntry, masterPW);
    appleMod2.changeEmail("user1@apple.com");
    appleMod2.saveChanges();

}


void DataEntryTest::testModulatorChangeMasterPW(){
    DataEntryModulator appleMod(appleEntry, masterPW);
    QDateTime preLastChanged = appleEntry->getLastChanged();
    appleMod.changeMasterPassword(QString("12345678901234567890123456789012").toUtf8());
    appleMod.saveChanges();
    QVERIFY(preLastChanged == appleEntry->getLastChanged());
    appleEntry->decryptContent(QString("12345678901234567890123456789012").toUtf8());
    QCOMPARE(appleEntry->getPassword(), ",~£:1Od33jy+lj");
    appleEntry->encryptContent(QString("12345678901234567890123456789012").toUtf8());
    appleEntry->decryptContent(QString("12345678901234567890123456789012").toUtf8());
    QCOMPARE(appleEntry->getEMail(), "user1@apple.com");
    appleEntry->encryptContent(QString("12345678901234567890123456789012").toUtf8());

    DataEntryModulator appleMod2(appleEntry, QString("12345678901234567890123456789012").toUtf8());
    appleMod2.changeMasterPassword(masterPW);
    appleMod2.saveChanges();
}




