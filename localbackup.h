#ifndef LOCALBACKUP_H
#define LOCALBACKUP_H

#include <QString>


/**
 * @brief The LocalBackup class delivers two methods:
 *
 * 1. Creating a new backup
 *
 * 2. Reverting to an older backup via the folder name
 *
 * To work as expected it is assumed that PasswordBroker data has been encrypted and stored to the drive in beforehand
 *
 * The local backups can be found in its own folder relative to the current database in use
 */
class LocalBackup
{
public:
    /**
     * @brief newLocalBackup() copies iv, mac, dataEntries files into a new folder with the current time as the folders name
     * @return whether the creating of the backup was successful or not
     */
    static bool newLocalBackup();
    /**
     * @brief revertToBackup() overwrites the iv, mac, dataEntries file in the database folder
     *
     * with the three corresponding files by the same names in the folder given as parameter
     * @param the name of the folder containing the iv, mac, dataEntries file to revert to
     * @return  whether the reversal to the other backup was successful or not
     */
    static bool revertToBackup(const QString& folderName);
private:
    LocalBackup();
};

#endif // LOCALBACKUP_H
