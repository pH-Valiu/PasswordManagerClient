#ifndef PASSWORDBROKER_H
#define PASSWORDBROKER_H
#include "dataentry.h"
#include "dataentrymodulator.h"




class PasswordBroker
{
public:
    PasswordBroker(PasswordBroker& other) = delete;
    void operator = (const PasswordBroker&) = delete;
    /**
     * @brief returns a reference to the only PasswordBroker instance at runtime
     *
     * Required Procedure:
     *
     * fetchFileData(), decryptData() (optional when fetching for the first time),
     *
     * encryptData(), storeFileData()
     * @return
     */
    static PasswordBroker& getInstance();
    /**
     * @brief fetchFileData() should only be called once per application
     *
     * storeFileData() works only if fetchFileData() has been called once and returned true
     * @param masterPW to decrypt fetched data
     * @return whether the operation was successful or not
     */
    bool fetchFileData(const QByteArray& masterPW);
    /**
     * @brief storeFileData can be called multiple times but only after fetchFileData() has been called once and returned true
     * @param masterPW to encrypt data
     * @return whether the operation was successful or not
     */
    bool storeFileData(const QByteArray& masterPW);
    bool changerMasterPW(const QByteArray& oldMasterPW, const QByteArray& newMasterPW);
    void addEntry(QSharedPointer<DataEntry>& dataEntry);
    /**
     * @brief removeEntryById
     * @param id
     * @return true if one or more entries were removed, false if none
     */
    bool removeEntryById(const QByteArray& id);
    /**
     * @brief removeAllEntries and deleted their containing data
     */
    void removeAllEntries();
    QSharedPointer<DataEntry> getEntryFromId(const QByteArray& id);
    QSharedPointer<DataEntry> getEntryFromName(const QString& name);
    QSharedPointer<DataEntry> getEntryFromWebsite(const QString& website);
    qsizetype entryCount() {return vector.size();}
    ~PasswordBroker();

    friend class PasswordBrokerTest;
#ifdef EXECUTE_UNIT_TESTS
#endif

private:
    PasswordBroker();
    QVector<QSharedPointer<DataEntry>> vector;
    bool encryptData(const QByteArray& masterPW);
    bool decryptData(const QByteArray& masterPW);
    bool fetchedFlag = false;
    struct FileData{
        QByteArray encryptedEntries;
        QByteArray iv;
        QByteArray mac;
    } fileData;


};

#endif // PASSWORDBROKER_H
