#include "passwordbrokertest.h"

void PasswordBrokerTest::initTestCase(){
    masterPW = QCryptographicHash::hash(QString("masterPassword").toUtf8(), QCryptographicHash::Sha256);
    QDir dirDatabase(QCoreApplication::applicationDirPath() + "/database");
    dirDatabase.removeRecursively();

    DataEntryBuilder builder("apple");
    builder.addDetails("Just call up the website² and \"log\" in ?*?");
    builder.addWebsite("https://apple.com/?query=user-log_on#");
    builder.addUsername("user1");
    builder.addPassword(",~£:1Od33jy+lj");
    builder.addEmail("user1@apple.com");
    testEntry1 = builder.build(masterPW);

    DataEntryBuilder builder2("amazon");
    builder2.addDetails("Details regarding this amazon account");
    builder2.addEmail("user2@amazon.com");
    builder2.addPassword("AmazonPassword");
    testEntry2 = builder2.build(masterPW);

    DataEntryBuilder builder3("twitter");
    builder3.addUsername("dumb_user");
    builder3.addPassword("EvenDumberPassword");
    builder3.addWebsite("x.com");
    testEntry3 = builder3.build(masterPW);
}

void PasswordBrokerTest::testSingleton(){
    PasswordBroker& broker1 = PasswordBroker::getInstance();
    PasswordBroker& broker2 = PasswordBroker::getInstance();
    QCOMPARE(&broker1, &broker2);
    broker1.addEntry(testEntry1);
    QVERIFY(broker2.getEntryFromName("apple") == testEntry1);
    broker1.removeEntryById(testEntry1->getID());
    QVERIFY(broker1.getEntryFromName("apple").isNull());

}

void PasswordBrokerTest::testSimpleCallSequence(){
    PasswordBroker broker;
    broker.addEntry(testEntry1);
    broker.addEntry(testEntry2);
    broker.addEntry(testEntry3);
    QCOMPARE(broker.getEntryFromName("apple"), testEntry1);
    QCOMPARE(broker.getEntryFromName("amazon"), testEntry2);
    QCOMPARE(broker.getEntryFromName("twitter"), testEntry3);
    QVERIFY(broker.removeEntryById(testEntry2->getID()));
    QVERIFY(broker.getEntryFromName("amazon").isNull());
    QCOMPARE(broker.getEntryFromWebsite("x.com"), testEntry3);
    QCOMPARE(broker.getEntryFromId(testEntry3->getID()), testEntry3);
    QVERIFY(broker.removeEntryByName("apple"));
    QVERIFY(broker.removeEntryByName("twitter"));
    QVERIFY(!broker.removeEntryByName("twitter"));


}

void PasswordBrokerTest::testSingleStoreFetch(){
    PasswordBroker broker;
    QVERIFY(broker.fetchFileData());
    QVERIFY(broker.decryptData(masterPW));
    QVERIFY(broker.entryCount() == 0);
    QVERIFY(!broker.fileData.iv.isEmpty());
    QVERIFY(broker.fileData.mac.isEmpty());
    QVERIFY(broker.fileData.encryptedEntries.isEmpty());
    QVERIFY(!broker.fileData.iv.isNull());
    QVERIFY(!broker.fileData.mac.isNull());
    QVERIFY(!broker.fileData.encryptedEntries.isNull());
    broker.addEntry(testEntry1);
    broker.addEntry(testEntry2);
    broker.addEntry(testEntry3);
    QVERIFY(broker.encryptData(masterPW));
    QVERIFY(!broker.fileData.encryptedEntries.isEmpty());
    QVERIFY(!broker.fileData.mac.isEmpty());
    QVERIFY(broker.storeFileData());
    broker.removeEntryByName("apple");
    broker.removeEntryByName("amazon");
    broker.removeEntryByName("twitter");

    PasswordBroker broker2;
    QVERIFY(broker2.fetchFileData());
    QVERIFY(!broker2.fileData.iv.isEmpty());
    QVERIFY(!broker2.fileData.mac.isEmpty());
    QVERIFY(!broker2.fileData.encryptedEntries.isEmpty());
    QVERIFY(broker2.decryptData(masterPW));
    QCOMPARE(*broker2.getEntryFromName("apple").data(), *testEntry1.data());
    QCOMPARE(*broker2.getEntryFromName("amazon").data(), *testEntry2.data());
    QCOMPARE(*broker2.getEntryFromName("twitter").data(), *testEntry3.data());


    QDir dirDatabase(QCoreApplication::applicationDirPath() + "/database");
    dirDatabase.removeRecursively();
}

void PasswordBrokerTest::testMultipleStoreFetch(){
    PasswordBroker broker;
    QVERIFY(broker.fetchFileData());
    broker.addEntry(testEntry1);
    broker.addEntry(testEntry2);
    broker.addEntry(testEntry3);
    QVERIFY(broker.encryptData(masterPW));
    QVERIFY(broker.storeFileData());
    broker.removeEntryByName("apple");
    broker.removeEntryByName("amazon");
    broker.removeEntryByName("twitter");

    PasswordBroker broker2;
    QVERIFY(broker2.fetchFileData());
    QVERIFY(broker2.decryptData(masterPW));
    QCOMPARE(*broker2.getEntryFromName("apple").data(), *testEntry1.data());
    QCOMPARE(*broker2.getEntryFromName("amazon").data(), *testEntry2.data());
    QCOMPARE(*broker2.getEntryFromName("twitter").data(), *testEntry3.data());
    DataEntryModulator mod(broker2.getEntryFromName("apple"), masterPW);
    mod.changeUsername("appleUser");
    mod.saveChanges();
    broker2.encryptData(masterPW);
    broker2.storeFileData();

    PasswordBroker broker3;
    QVERIFY(broker3.fetchFileData());
    QVERIFY(broker3.decryptData(masterPW));
    broker3.getEntryFromName("apple")->decryptContent(masterPW);
    QCOMPARE(broker3.getEntryFromName("apple")->getUsername(), "appleUser");
    broker3.getEntryFromName("apple")->encryptContent(masterPW);
    broker3.encryptData(masterPW);
    broker3.storeFileData();


    QDir dirDatabase(QCoreApplication::applicationDirPath() + "/database");
    dirDatabase.removeRecursively();
}

void PasswordBrokerTest::testChangeMasterPW(){
    PasswordBroker broker;
    QVERIFY(testEntry1->decryptContent(masterPW));
    QCOMPARE(testEntry1->getPassword(), ",~£:1Od33jy+lj");
    QVERIFY(testEntry1->encryptContent(masterPW));
    broker.addEntry(testEntry1);
    broker.addEntry(testEntry2);
    broker.addEntry(testEntry3);
    QByteArray newMasterPW = QString("12345678901234567890123456789012").toUtf8();
    QVERIFY(broker.changerMasterPW(masterPW, newMasterPW));
    QVERIFY(broker.fetchFileData());
    QVERIFY(broker.encryptData(newMasterPW));
    QVERIFY(broker.storeFileData());

    PasswordBroker broker2;
    QVERIFY(broker2.fetchFileData());
    QVERIFY(broker2.decryptData(newMasterPW));
    QVERIFY(broker2.getEntryFromName("apple")->decryptContent(newMasterPW));
    QCOMPARE(broker2.getEntryFromName("apple")->getPassword(), ",~£:1Od33jy+lj");
    QVERIFY(broker2.getEntryFromName("apple")->encryptContent(newMasterPW));

    QVERIFY(broker.changerMasterPW(newMasterPW, masterPW));
    broker.removeEntryByName("apple");
    broker.removeEntryByName("amazon");
    broker.removeEntryByName("twitter");
    QDir dirDatabase(QCoreApplication::applicationDirPath() + "/database");
    dirDatabase.removeRecursively();

}

void PasswordBrokerTest::testLookupEntry(){
    PasswordBroker broker;
    broker.addEntry(testEntry1);
    broker.addEntry(testEntry2);
    broker.addEntry(testEntry3);
    QCOMPARE(broker.getEntryFromId(testEntry1->getID()), testEntry1);
    QCOMPARE(broker.getEntryFromName("amazon"), testEntry2);
    QCOMPARE(broker.getEntryFromWebsite("x.com"), testEntry3);
    QVERIFY(broker.removeEntryByName("apple"));
    QVERIFY(broker.removeEntryByName("amazon"));
    QVERIFY(broker.removeEntryByName("twitter"));
}

void PasswordBrokerTest::testWrongMasterPW(){
    PasswordBroker broker;
    QVERIFY(broker.fetchFileData());
    broker.addEntry(testEntry1);
    broker.addEntry(testEntry3);
    QVERIFY(broker.encryptData(masterPW));
    QVERIFY(broker.storeFileData());
    QVERIFY(broker.removeEntryByName("apple"));
    QVERIFY(!broker.removeEntryByName("amazon"));
    QVERIFY(broker.removeEntryByName("twitter"));

    QByteArray wrongMasterPW("12345678901234567890123456789012");
    PasswordBroker broker2;
    QVERIFY(broker2.fetchFileData());
    QVERIFY(!broker2.decryptData(wrongMasterPW));
    QVERIFY(broker2.decryptData(masterPW));
    broker2.addEntry(testEntry2);

    QVERIFY(broker2.encryptData(wrongMasterPW));

    QVERIFY(!broker2.decryptData(masterPW));

    QVERIFY(broker2.removeEntryByName("amazon"));
    QDir dirDatabase(QCoreApplication::applicationDirPath() + "/database");
    dirDatabase.removeRecursively();

}
