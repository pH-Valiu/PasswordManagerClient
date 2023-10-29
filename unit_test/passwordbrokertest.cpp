#include "passwordbrokertest.h"

void PasswordBrokerTest::initTestCase(){
    masterPW = QCryptographicHash::hash(QString("masterPassword").toUtf8(), QCryptographicHash::Sha256);
    QDir dirDatabase(QCoreApplication::applicationDirPath() + "/database");
    dirDatabase.removeRecursively();

    DataEntryBuilder builder(masterPW);
    builder.modulateName("apple");
    builder.modulateDetails("Just call up the website² and \"log\" in ?*?");
    builder.modulateWebsite("https://apple.com/?query=user-log_on#");
    builder.modulateUsername("user1");
    builder.modulatePassword(",~£:1Od33jy+lj");
    builder.modulateEmail("user1@apple.com");
    testEntry1 = builder.modulate();

    DataEntryBuilder builder2(masterPW);
    builder2.modulateName("amazon");
    builder2.modulateDetails("Details regarding this amazon account");
    builder2.modulateEmail("user2@amazon.com");
    builder2.modulatePassword("AmazonPassword");
    testEntry2 = builder2.modulate();

    DataEntryBuilder builder3(masterPW);
    builder3.modulateName("twitter");
    builder3.modulateUsername("dumb_user");
    builder3.modulatePassword("EvenDumberPassword");
    builder3.modulateWebsite("x.com");
    testEntry3 = builder3.modulate();
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
#ifdef EXECUTE_UNIT_TESTS
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
#endif

}

void PasswordBrokerTest::testSingleStoreFetch(){
#ifdef EXECUTE_UNIT_TESTS
    PasswordBroker broker;
    QVERIFY(broker.fetchFileData(masterPW));
    //QVERIFY(broker.decryptData(masterPW));
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
    //QVERIFY(broker.encryptData(masterPW));
    QVERIFY(broker.storeFileData(masterPW));
    QVERIFY(!broker.fileData.encryptedEntries.isEmpty());
    QVERIFY(!broker.fileData.mac.isEmpty());
    broker.removeEntryByName("apple");
    broker.removeEntryByName("amazon");
    broker.removeEntryByName("twitter");

    PasswordBroker broker2;
    QVERIFY(broker2.fetchFileData(masterPW));
    QVERIFY(!broker2.fileData.iv.isEmpty());
    QVERIFY(!broker2.fileData.mac.isEmpty());
    QVERIFY(!broker2.fileData.encryptedEntries.isEmpty());
    //QVERIFY(broker2.decryptData(masterPW));
    QCOMPARE(*broker2.getEntryFromName("apple").data(), *testEntry1.data());
    QCOMPARE(*broker2.getEntryFromName("amazon").data(), *testEntry2.data());
    QCOMPARE(*broker2.getEntryFromName("twitter").data(), *testEntry3.data());


    QDir dirDatabase(QCoreApplication::applicationDirPath() + "/database");
    dirDatabase.removeRecursively();
#endif

}

void PasswordBrokerTest::testMultipleStoreFetch(){
#ifdef EXECUTE_UNIT_TESTS
    PasswordBroker broker;
    QVERIFY(broker.fetchFileData(masterPW));
    broker.addEntry(testEntry1);
    broker.addEntry(testEntry2);
    broker.addEntry(testEntry3);
    //QVERIFY(broker.encryptData(masterPW));
    QVERIFY(broker.storeFileData(masterPW));
    broker.removeEntryByName("apple");
    broker.removeEntryByName("amazon");
    broker.removeEntryByName("twitter");

    PasswordBroker broker2;
    QVERIFY(broker2.fetchFileData(masterPW));
    //QVERIFY(broker2.decryptData(masterPW));
    QCOMPARE(*broker2.getEntryFromName("apple").data(), *testEntry1.data());
    QCOMPARE(*broker2.getEntryFromName("amazon").data(), *testEntry2.data());
    QCOMPARE(*broker2.getEntryFromName("twitter").data(), *testEntry3.data());
    DataEntryModulator mod(broker2.getEntryFromName("apple"), masterPW);
    mod.changeUsername("appleUser");
    mod.saveChanges();
    //broker2.encryptData(masterPW);
    broker2.storeFileData(masterPW);

    PasswordBroker broker3;
    QVERIFY(broker3.fetchFileData(masterPW));
    //QVERIFY(broker3.decryptData(masterPW));
    broker3.getEntryFromName("apple")->decryptContent(masterPW);
    QCOMPARE(broker3.getEntryFromName("apple")->getUsername(), "appleUser");
    broker3.getEntryFromName("apple")->encryptContent(masterPW);
    //broker3.encryptData(masterPW);
    broker3.storeFileData(masterPW);


    QDir dirDatabase(QCoreApplication::applicationDirPath() + "/database");
    dirDatabase.removeRecursively();
#endif

}

void PasswordBrokerTest::testChangeMasterPW(){
#ifdef EXECUTE_UNIT_TESTS
    PasswordBroker broker;
    QVERIFY(broker.fetchFileData(masterPW));
    QVERIFY(testEntry1->decryptContent(masterPW));
    QCOMPARE(testEntry1->getPassword(), ",~£:1Od33jy+lj");
    QVERIFY(testEntry1->encryptContent(masterPW));
    broker.addEntry(testEntry1);
    broker.addEntry(testEntry2);
    broker.addEntry(testEntry3);
    QByteArray newMasterPW = QString("12345678901234567890123456789012").toUtf8();
    QVERIFY(broker.changerMasterPW(masterPW, newMasterPW));
    //QVERIFY(broker.encryptData(newMasterPW));
    QVERIFY(broker.storeFileData(newMasterPW));

    PasswordBroker broker2;
    QVERIFY(broker2.fetchFileData(newMasterPW));
    //QVERIFY(broker2.decryptData(newMasterPW));
    QVERIFY(broker2.getEntryFromName("apple")->decryptContent(newMasterPW));
    QCOMPARE(broker2.getEntryFromName("apple")->getPassword(), ",~£:1Od33jy+lj");
    QVERIFY(broker2.getEntryFromName("apple")->encryptContent(newMasterPW));

    QVERIFY(broker.changerMasterPW(newMasterPW, masterPW));
    broker.removeEntryByName("apple");
    broker.removeEntryByName("amazon");
    broker.removeEntryByName("twitter");
    QDir dirDatabase(QCoreApplication::applicationDirPath() + "/database");
    dirDatabase.removeRecursively();
#endif

}

void PasswordBrokerTest::testLookupEntry(){
#ifdef EXECUTE_UNIT_TESTS
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
#endif

}

void PasswordBrokerTest::testWrongMasterPW(){
#ifdef EXECUTE_UNIT_TESTS
    PasswordBroker broker;
    QVERIFY(broker.fetchFileData(masterPW));
    broker.addEntry(testEntry1);
    broker.addEntry(testEntry3);
    //QVERIFY(broker.encryptData(masterPW));
    QVERIFY(broker.storeFileData(masterPW));
    QVERIFY(broker.removeEntryByName("apple"));
    QVERIFY(!broker.removeEntryByName("amazon"));
    QVERIFY(broker.removeEntryByName("twitter"));

    QByteArray wrongMasterPW("12345678901234567890123456789012");
    PasswordBroker broker2;
    QVERIFY(!broker2.fetchFileData(wrongMasterPW));
    //QVERIFY(!broker2.decryptData(wrongMasterPW));
    QVERIFY(broker2.fetchFileData(masterPW));
    broker2.addEntry(testEntry2);

    QVERIFY(broker2.encryptData(wrongMasterPW));

    QVERIFY(!broker2.decryptData(masterPW));

    QVERIFY(broker2.removeEntryByName("amazon"));
    QDir dirDatabase(QCoreApplication::applicationDirPath() + "/database");
    dirDatabase.removeRecursively();
#endif

}
