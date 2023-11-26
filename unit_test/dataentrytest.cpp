#include "dataentrytest.h"

#ifdef EXECUTE_UNIT_TESTS

void DataEntryTest::initTestCase(){
    masterPW = QSharedPointer<QByteArray>(new QByteArray(QCryptographicHash::hash(QString("masterPassword").toUtf8(), QCryptographicHash::Sha256)));
    DataEntryBuilder builder(masterPW);
    builder.modulateName("apple");
    builder.modulateDetails("Just call up the website² and \"log\" in ?*?");
    builder.modulateWebsite("https://apple.com/?query=user-log_on#");
    builder.modulateUsername("user1");
    builder.modulatePassword(",~£:1Od33jy+lj");
    builder.modulateEmail("user1@apple.com");
    appleEntry = builder.modulate();
}

void DataEntryTest::testBuilderFull(){

    QDateTime lastChanged = appleEntry->getLastChanged();
    QCOMPARE(appleEntry->getName(), "apple");
    QCOMPARE(appleEntry->getUsername(), "");
    QCOMPARE(appleEntry->getPassword(), "");
    QCOMPARE(appleEntry->getEMail(), "");
    QCOMPARE(appleEntry->getDetails(), "");
    QCOMPARE(appleEntry->getWebsite(), "https://apple.com/?query=user-log_on#");
    appleEntry->decryptContent(masterPW);
    QCOMPARE(appleEntry->getUsername(), "user1");
    QCOMPARE(appleEntry->getPassword(), ",~£:1Od33jy+lj");
    QCOMPARE(appleEntry->getEMail(), "user1@apple.com");
    QCOMPARE(appleEntry->getDetails(), "Just call up the website² and \"log\" in ?*?");
    appleEntry->encryptContent(masterPW);
    QCOMPARE(appleEntry->getUsername(), "");
    QCOMPARE(appleEntry->getPassword(), "");
    QCOMPARE(appleEntry->getEMail(), "");
    QCOMPARE(appleEntry->getDetails(), "");
    QDateTime lastChangedLater = appleEntry->getLastChanged();
    QCOMPARE(lastChanged, lastChangedLater);
}

void DataEntryTest::testBuilderPartial(){
    DataEntryBuilder builderAmazon(masterPW);
    builderAmazon.modulateName("amazon");
    builderAmazon.modulatePassword("PasswordAmazon");
    QSharedPointer<DataEntry> amazonEntry = builderAmazon.modulate();
    QCOMPARE(amazonEntry->getName(), "amazon");
    QCOMPARE(amazonEntry->getUsername(), "");
    QCOMPARE(amazonEntry->getPassword(), "");
    QCOMPARE(amazonEntry->getEMail(), "");
    QCOMPARE(amazonEntry->getDetails(), "");
    QCOMPARE(amazonEntry->getWebsite(), "");
    amazonEntry->decryptContent(masterPW);
    QCOMPARE(amazonEntry->getUsername(), "");
    QCOMPARE(amazonEntry->getPassword(), "PasswordAmazon");
    QCOMPARE(amazonEntry->getEMail(), "");
    QCOMPARE(amazonEntry->getDetails(), "");
    amazonEntry->encryptContent(masterPW);
    QCOMPARE(amazonEntry->getUsername(), "");
    QCOMPARE(amazonEntry->getPassword(), "");
    QCOMPARE(amazonEntry->getEMail(), "");
    QCOMPARE(amazonEntry->getDetails(), "");
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
    QCOMPARE(entry->getUsername(), "");
    QCOMPARE(entry->getPassword(), "");
    QCOMPARE(entry->getEMail(), "");
    QCOMPARE(entry->getDetails(), "");
    QCOMPARE(entry->getWebsite(), "https://apple.com/?query=user-log_on#");
    entry->decryptContent(masterPW);
    QCOMPARE(entry->getUsername(), "user1");
    QCOMPARE(entry->getPassword(), ",~£:1Od33jy+lj");
    QCOMPARE(entry->getEMail(), "user1@apple.com");
    QCOMPARE(entry->getDetails(), "Just call up the website² and \"log\" in ?*?");
    entry->encryptContent(masterPW);
    QCOMPARE(entry->getUsername(), "");
    QCOMPARE(entry->getPassword(), "");
    QCOMPARE(entry->getEMail(), "");
    QCOMPARE(entry->getDetails(), "");

    QCOMPARE(appleEntry->getID(), entry->getID().append(""));
    QDateTime lastChangedLater = entry->getLastChanged();
    QCOMPARE(lastChanged, lastChangedLater);
    file.remove();
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

    DataEntryBuilder failingBuilder(masterPW);
    failingBuilder.modulateName("fai~ling");
    failingBuilder.modulateWebsite("https://\\//");
    QSharedPointer<DataEntry> failedEntry = failingBuilder.modulate();
    QCOMPARE(failedEntry->getName(), failedEntry->getID());
    QCOMPARE(failedEntry->getWebsite(), "");

}

void DataEntryTest::testModulator(){

    DataEntryEditor appleMod(appleEntry, masterPW);
    QDateTime preLastChanged = appleEntry->getLastChanged();
    QCOMPARE(appleEntry->getEMail(), "");
    appleMod.modulateEmail("newMail@apple.com");
    QCOMPARE(appleEntry->getEMail(), "");
    QThread::currentThread()->sleep(1);
    appleMod.modulate();
    QVERIFY(preLastChanged != appleEntry->getLastChanged());
    QCOMPARE(appleEntry->getEMail(), "");
    appleEntry->decryptContent(masterPW);
    QCOMPARE(appleEntry->getEMail(), "newMail@apple.com");
    appleEntry->encryptContent(masterPW);

    DataEntryEditor appleMod2(appleEntry, masterPW);
    appleMod2.modulateEmail("user1@apple.com");
    appleMod2.modulate();

}

void DataEntryTest::testModulatorChangeMasterPW(){
    DataEntryEditor appleMod(appleEntry, masterPW);
    QDateTime preLastChanged = appleEntry->getLastChanged();
    QSharedPointer<QByteArray> newMasterPW = QSharedPointer<QByteArray>(new QByteArray("12345678901234567890123456789012"));
    appleMod.changeMasterPassword(newMasterPW);
    QThread::currentThread()->sleep(1);
    appleMod.modulate();
    QVERIFY(preLastChanged != appleEntry->getLastChanged());
    appleEntry->decryptContent(newMasterPW);
    QCOMPARE(appleEntry->getPassword(), ",~£:1Od33jy+lj");
    appleEntry->encryptContent(newMasterPW);
    appleEntry->decryptContent(newMasterPW);
    QCOMPARE(appleEntry->getEMail(), "user1@apple.com");
    appleEntry->encryptContent(newMasterPW);

    DataEntryEditor appleMod2(appleEntry, newMasterPW);
    appleMod2.changeMasterPassword(masterPW);
    appleMod2.modulate();
}

#endif




