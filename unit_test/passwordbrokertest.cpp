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
    QSharedPointer<PasswordBroker> broker1 = PasswordBroker::getInstance();
    QSharedPointer<PasswordBroker> broker2 = PasswordBroker::getInstance();
    QCOMPARE(broker1.data(), broker2.data());
    broker1->addEntry(testEntry1);
    QVERIFY(broker2->getEntryFromName("apple") == testEntry1);

}

void PasswordBrokerTest::testSimpleCallSequence(){
    QSharedPointer<PasswordBroker> broker = PasswordBroker::getInstance();
    broker->addEntry(testEntry1);
    broker->addEntry(testEntry2);
    broker->addEntry(testEntry3);

}

void PasswordBrokerTest::testMultipleStoreFetch(){

}

void PasswordBrokerTest::testChangeMasterPW(){

}

void PasswordBrokerTest::testLookupEntry(){

}

void PasswordBrokerTest::testWrongMasterPW(){

}
