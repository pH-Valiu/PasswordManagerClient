#ifndef PASSWORDMANAGERMODEL_H
#define PASSWORDMANAGERMODEL_H

#include "passwordbroker.h"
#include <QObject>
#include <QList>
#include <QString>
#include <QSharedPointer>
#include "dataentry.h"


//!!! Maybe remove Q_OBJECT since not needed
class PasswordManagerModel : public QObject
{
    Q_OBJECT
public:
    PasswordManagerModel(PasswordManagerModel& other) = delete;
    void operator = (const PasswordManagerModel&) = delete;
    /**
     * @brief returns a reference to the only PasswordManagerModel instance at runtime
     * @return
     */
    static PasswordManagerModel& getInstance();
    std::unique_ptr<DataEntryEditor> getEditor(const QByteArray& id, const QSharedPointer<QByteArray>& masterPW);
    std::unique_ptr<DataEntryBuilder> getBuilder(const QSharedPointer<QByteArray>& masterPW);
    /**
     * @brief showHideEntry en/decrypts secret content of dataEntry using its id as the identifier
     * @param the id of the dataEntry
     * @param pointer to the masterPW
     * @return 0 if id was not findable
     *
     * -1 if dataEntry could not be en/decrypted
     *
     * -2 if masterPW was pointer to null
     *
     * 1 if dataEntry got encrypted
     *
     * 2 if dataEntry got decrypted
     */
    int showHideEntry(const QByteArray& id, const QSharedPointer<QByteArray>& masterPW);
    /**
     * @brief showEntry decrypts secret content of dataEntry using its id as the identifier
     * @param id of the dataEntry
     * @param pointer to the masterPW
     * @return 0 if id was not findable
     *
     * -1 if dataEntry could not be decrypted
     *
     * -2 if masterPW was pointer to null
     *
     * 1 if dataEntry got decrypted
     */
    int showEntry(const QByteArray& id, const QSharedPointer<QByteArray>& masterPW);
    /**
     * @brief hideEntry encrypts secret content of dataEntry using its id as the identifier
     * @param id of the dataEntry
     * @param pointer to the masterPW
     * @return 0 if id was not findable
     *
     * -1 if dataEntry could not be encrypted
     *
     * -2 if masterPW was pointer to null
     *
     * 1 if dataEntry got encrypted
     */
    int hideEntry(const QByteArray& id, const QSharedPointer<QByteArray>& masterPW);

    /**
     * @brief hideAllEntries tries to perform encryptData() on each entry inside of PasswordManagerBroker
     * @param pointer to the mastePW
     * @return number of entries encrypted, can be different from all entries count
     */
    int hideAllEntries(const QSharedPointer<QByteArray>& masterPW);

    /**
     * @brief getEntry
     * @param id of the entry to get
     * @return pointer to the entry, or nullptr if id was not found
     */
    QSharedPointer<DataEntry> getEntry(const QByteArray& id);

    void addEntry(QSharedPointer<DataEntry>& entry);
    /**
     * @brief removeEntry removes the data entry with id as its identifier
     *
     * If there is still another QSharedPointer floating around pointing to the entry,
     *
     * the entrie's data won't get deleted
     * @param identifier id for the data entry
     * @return whether the data data entry got removed or not
     */
    bool removeEntry(const QByteArray& id);
    void removeAllEntries()     {broker.removeAllEntries();}

    /**
     * @brief moveEntry moves the DataEntry specified by id upwards or downwards in the internal QList
     * @param id specifying the DataEntry to be moved
     * @param up whether the DataEntry should be moved upwards (true) or downwards (false)
     * @param toTheLimit whether the DataEntry should be moved to edge (true) or not (false)
     * @return the index the entry has been moved to, or -1 if entry was not findable
     */
    int moveEntry(const QByteArray& id, bool up, bool toTheLimit);
    const QVector<QSharedPointer<DataEntry>> getAllEntries(){return broker.getAllEntries();}
    /**
     * @brief searchEntry
     * @param identifier
     * @return The id of the entry if it was found, otherwise an empty QByteArray
     */
    QByteArray searchEntry(const QString& identifier);

    /**
     * @brief startBroker performs broker.fetchFileData()
     * @param pointer to the masterPW
     * @return false if masterPW is nullptr and otherwise broker.fetchFileData()
     */
    bool startBroker(const QSharedPointer<QByteArray>& masterPW);
    /**
     * @brief saveBroker performs broker.storeFileData()
     * @param pointer to the masterPW
     * @return false if masterPW is nullptr and otherwise broker.storeFileData()
     */
    bool saveBroker(const QSharedPointer<QByteArray>& masterPW);
    /**
     * @brief newLocalBackup performs LocalBackup::newLocalBackup()
     * @return return value of LocalBackup::newLocalBackup()
     */
    QString newLocalBackup();
    /**
     * @brief revertToOlderLocalBackup performs LocalBackup::reverToBackup(folderName)
     * @param folderName of the local backup
     * @return return value of LocalBackup::reverToBackup()
     */
    bool revertToOlderLocalBackup(const QString& folderName);
    /**
     * @brief getlAllLocalBackups performs LocalBackup::getAllLocalBackups()
     * @return return value of LocalBackup::getAllLocalBackups()
     */
    QList<QString> getlAllLocalBackups();
    QString getOneBackupNewer(const QString& currentBackup);

    QByteArray getMasterPWSalt();
    bool validateUserMasterPW(const QByteArray& userMasterPW);
    QPair<QByteArray, QByteArray> getUserMasterPWHash();
    bool setUserMasterPW(const QByteArray& userMasterPW, const QByteArray& masterPWSalt);
    bool changeUserMasterPW(const QByteArray& oldUserMasterPW, const QByteArray& newUserMasterPW, const QByteArray& oldMasterPWSalt, const QByteArray& newMasterPWSalt, const QSharedPointer<QByteArray>& oldDerivedMasterPW, const QSharedPointer<QByteArray>& newDerivedMasterPW);
private:
    PasswordManagerModel();
    PasswordBroker& broker;



};

#endif // PASSWORDMANAGERMODEL_H

//Fragen die geklärt werden müsse:
//WIE, WO und WANN ensteht ein neues lokales Backup?
//-> wann?: beim schließen des programms/log out UND beim revertieren zu einem früheren backup
//-> wo?: backup folder in PWD
//-> wie?: kopie des kompletten database folder mit timestamp als name (backend implemented)

//WIE revertiere ich zu einem früheren Backup?
//-> wie?: auf auswahl eines backup in einer liste (widget ListView)


//WAS passiert beim revertieren zu einem früheren Backup?
//-> was?: saven des derzeitigen PasswordBrokers (neuer BackUpFolder eintrag)
//-> was?: überschreibung des derzeitigen databse ordners mit dem ausgewählten backup ordner

//WAS passiert beim revertieren mit der derzeitigen database? (ist diese dann ein neues backup?)
//-> was?: ja

