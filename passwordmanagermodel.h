#ifndef PASSWORDMANAGERMODEL_H
#define PASSWORDMANAGERMODEL_H

#include "passwordbroker.h"
#include <QObject>
#include <QPointer>


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
    std::unique_ptr<DataEntryModulator> getModulator(const QByteArray& id, const QSharedPointer<QByteArray>& masterPW);
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
    int showHideEntry(const QByteArray& id, QSharedPointer<QByteArray> masterPW);
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
    int showEntry(const QByteArray& id, QSharedPointer<QByteArray> masterPW);
    /**
     * @brief showEntry encrypts secret content of dataEntry using its id as the identifier
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

    int hideEntry(const QByteArray& id, QSharedPointer<QByteArray> masterPW);

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
private:
    PasswordManagerModel();
    PasswordBroker& broker;
    bool revertToOlderLocalBackup();



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

