#ifndef LOCALBACKUP_H
#define LOCALBACKUP_H

#include "messagehandler.h"
#include <QCoreApplication>
#include <QString>
#include <QFile>
#include <QStringList>
#include <QDir>


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
     * @brief getAllLocalBackups reads from localbackup folder
     * @return QList containing each backups name (timestamp of the backup) or empty directory in error cases
     */
    static QList<QString> getAllLocalBackups(){
        QDir workingDirectory = QDir(QCoreApplication::applicationDirPath());
        if(!workingDirectory.exists()){
            //ERROR
            MessageHandler::critical("Working Directory does not exist?", "Critical Error:");
            return QList<QString>();
        }

        workingDirectory.mkdir("local-backups");
        QDir localBackupDirectory = QDir(QCoreApplication::applicationDirPath().append("/local-backups/"));

        if(!localBackupDirectory.exists()){
            //ERROR
            return QList<QString>();
        }

        QStringList nameFilters;
        nameFilters << "*";
        return localBackupDirectory.entryList(nameFilters, QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);

    }

    /**
     * @brief getOneBackupNewer searches for the next newer/younger backup than the one given
     * @param folderName representing the base backup that is 'one' older than the backup being searched for
     * @return empty QString if there is no newer backup or the name of the one newer/younger backup
     */
    static QString getOneBackupNewer(const QString& folderName){
        QDir workingDirectory = QDir(QCoreApplication::applicationDirPath());
        if(!workingDirectory.exists()){
            //ERROR
            MessageHandler::critical("Working Directory does not exist?", "Critical Error:");
            return "";
        }

        workingDirectory.mkdir("local-backups");
        QDir localBackupDirectory = QDir(QCoreApplication::applicationDirPath().append("/local-backups/"));

        if(!localBackupDirectory.exists()){
            //ERROR
            MessageHandler::warn("There do not exist any local backups?");
            return "";
        }
        QStringList nameFilters;
        nameFilters << "*";

        QList<QString> allBackups = localBackupDirectory.entryList(nameFilters, QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time);
        int currentIndex = allBackups.indexOf(folderName);
        if(--currentIndex >= 0){
            return allBackups.at(currentIndex);
        }else{
            return "";
        }
    }
    /**
     * @brief newLocalBackup() copies iv, mac, dataEntries, pw files into a new folder with the current time as the folders name
     * @return if the creation of the backup was successful, it returns the name of the backup, otherwise it returns an empty QString
     */
    static QString newLocalBackup(){
        QDir workingDirectory = QDir(QCoreApplication::applicationDirPath());
        if(!workingDirectory.exists()){
            //ERROR
            MessageHandler::critical("Working Directory does not exist?", "Critical Error:");
            return "";
        }

        QString databasePath = QCoreApplication::applicationDirPath().append("/database/");
        QFile filePW(databasePath + "pw");
        QString pwData;
        if(!filePW.exists()){
            //ERROR
            MessageHandler::warn("PW file doesn't exist");
            return "";
        }
        if(filePW.open(QIODevice::ReadOnly)){
            QTextStream pwInput(&filePW);
            pwData = pwInput.readAll();
            filePW.close();
        }else{
            //PW FILE COULD NOT BE OPENED
            //ABORT
            MessageHandler::warn("PW file could not be opened: " + filePW.errorString());
            return "";
        }


        QFile fileIv(databasePath + "iv");
        QString ivData;
        if(!fileIv.exists()){
            //ERROR
            MessageHandler::warn("IV file doesn't exist");
            return "";
        }
        if(fileIv.open(QIODevice::ReadOnly)){
            QTextStream ivInput(&fileIv);
            ivData = ivInput.readAll();
            fileIv.close();
        }else{
            //IV FILE COULD NOT BE OPENED
            //ABORT
            MessageHandler::warn("IV file could not be opened: " + fileIv.errorString());
            return "";
        }


        QFile fileMAC(databasePath + "mac");
        QString macData;
        if(!fileMAC.exists()){
            MessageHandler::warn("MAC file doesn't exist");
            return "";
        }
        if(fileMAC.open(QIODevice::ReadOnly)){
            QTextStream macInput(&fileMAC);
            macData = macInput.readAll();
            fileMAC.close();
        }else{
            //MAC FILE COULD NOT BE OPENED
            //ABORT
            MessageHandler::warn("MAC file could not be opened: " + fileMAC.errorString());
            return "";
        }


        QFile fileEntries(databasePath + "dataEntries");
        QString dataEntriesData;
        if(!fileEntries.exists()){
            MessageHandler::warn("DataEntries file doesn't exist");
            return "";
        }
        if(fileEntries.open(QIODevice::ReadOnly)){
            QTextStream entriesInput(&fileEntries);
            dataEntriesData = entriesInput.readAll();
            fileEntries.close();
        }else{
            //DATAENTRIES FILE COULD NOT BE OPENEND
            //ABORT
            MessageHandler::warn("DataEntries file could not be opened: " + fileEntries.errorString());
            return "";
        }



        workingDirectory.mkdir("local-backups");
        QDir backupDirectory = QDir(QCoreApplication::applicationDirPath().append("/local-backups/"));

        QString backupName = QDateTime::currentDateTime().toString("dd.MM.yyyy hh-mm-ss-zzz");
        QString backupPath = QCoreApplication::applicationDirPath().append("/local-backups/").append(backupName).append("/");
        backupDirectory.mkdir(backupName);

        QFile filePWCopy(backupPath + "pw");
        if(filePWCopy.open(QIODevice::WriteOnly)){
            QTextStream stream(&filePWCopy);
            stream << pwData;
            stream.flush();
            filePWCopy.close();
        }else{
            MessageHandler::warn("PW backup file could not be opened: " + filePWCopy.errorString());
            return "";
        }

        QFile fileIvCopy(backupPath + "iv");
        if(fileIvCopy.open(QIODevice::WriteOnly)){
            QTextStream stream(&fileIvCopy);
            stream << ivData;
            stream.flush();
            fileIvCopy.close();
        }else{
            MessageHandler::warn("IV backup file could not be opened: " + fileIvCopy.errorString());
            return "";
        }

        QFile fileMacCopy(backupPath + "mac");
        if(fileMacCopy.open(QIODevice::WriteOnly)){
            QTextStream stream(&fileMacCopy);
            stream << macData;
            stream.flush();
            fileMacCopy.close();
        }else{
            MessageHandler::warn("MAC backup file could not be opened: " + fileMacCopy.errorString());
            return "";
        }

        QFile fileDataEntriesCopy(backupPath + "dataEntries");
        if(fileDataEntriesCopy.open(QIODevice::WriteOnly)){
            QTextStream stream(&fileDataEntriesCopy);
            stream << dataEntriesData;
            stream.flush();
            fileDataEntriesCopy.close();
        }else{
            MessageHandler::warn("DataEntries backup file could not be opened: " + fileDataEntriesCopy.errorString());
            return "";
        }


        return backupName;
    }

    /**
     * @brief revertToBackup() overwrites the iv, mac, dataEntries, pw file in the database folder
     *
     * with the three corresponding files by the same names in the folder given as parameter
     * @param the name of the folder containing the iv, mac, dataEntries, pw file to revert to
     * @return  whether the reversal to the other backup was successful or not
     */
    static bool revertToBackup(const QString& folderName){
        QDir workingDirectory = QDir(QCoreApplication::applicationDirPath());
        if(!workingDirectory.exists()){
            //ERROR
            MessageHandler::critical("Working Directory does not exist?", "Critical Error:");
            return false;
        }

        QDir localBackupDirectory = QDir(QCoreApplication::applicationDirPath().append("/local-backups"));
        if(!localBackupDirectory.exists()){
            //ERROR
            MessageHandler::warn("Backup directory does not yet exist");
            return false;
        }

        QDir selectedLocalBackup = QDir(QCoreApplication::applicationDirPath().append("/local-backups/").append(folderName));
        if(!selectedLocalBackup.exists()){
            //ERROR
            MessageHandler::critical("Selected backup directory does not exist", "Critical Error:");
            return false;
        }



        QString backupPath = QCoreApplication::applicationDirPath().append("/local-backups/").append(folderName).append("/");

        QFile filePWBackup(backupPath + "pw");
        QString pwDataCopy;
        if(!filePWBackup.exists()){
            MessageHandler::warn("PW backup file doesn't exist");
            return false;
        }
        if(filePWBackup.open(QIODevice::ReadOnly)){
            QTextStream stream(&filePWBackup);
            pwDataCopy = stream.readAll();
            filePWBackup.close();
        }else{
            MessageHandler::warn("PW backup file could not be opened: " + filePWBackup.errorString());
            return false;
        }


        QFile fileIvBackup(backupPath + "iv");
        QString ivDataCopy;
        if(!fileIvBackup.exists()){
            MessageHandler::warn("IV backup file doesn't exist");
            return false;
        }
        if(fileIvBackup.open(QIODevice::ReadOnly)){
            QTextStream stream(&fileIvBackup);
            ivDataCopy = stream.readAll();
            fileIvBackup.close();
        }else{
            MessageHandler::warn("IV backup file could not be opened: " + fileIvBackup.errorString());
            return false;
        }


        QFile fileMacBackup(backupPath + "mac");
        QString macDataCopy;
        if(!fileMacBackup.exists()){
            MessageHandler::warn("MAC backup file doesn't exist");
            return false;
        }
        if(fileMacBackup.open(QIODevice::ReadOnly)){
            QTextStream stream(&fileMacBackup);
            macDataCopy = fileMacBackup.readAll();
            fileMacBackup.close();
        }else{
            MessageHandler::warn("MAC backup file could not be opened: " + fileMacBackup.errorString());
            return false;
        }


        QFile fileDataEntriesBackup(backupPath + "dataEntries");
        QString dataEntriesDataCopy;
        if(!fileDataEntriesBackup.exists()){
            MessageHandler::warn("DataEntries backup file doesn't exist");
            return false;
        }
        if(fileDataEntriesBackup.open(QIODevice::ReadOnly)){
            QTextStream stream(&fileDataEntriesBackup);
            dataEntriesDataCopy = fileDataEntriesBackup.readAll();
            fileDataEntriesBackup.close();
        }else{
            MessageHandler::warn("DataEntries backup file could not be opened: " + fileDataEntriesBackup.errorString());
            return false;
        }


        workingDirectory.mkdir("database");

        QString databasePath = QCoreApplication::applicationDirPath().append("/database/");
        QFile filePW(databasePath + "pw");
        if(filePW.exists()){
            filePW.resize(0);
        }
        if(filePW.open(QIODevice::WriteOnly)){
            QTextStream pwInput(&filePW);
            pwInput << pwDataCopy;
            pwInput.flush();
            filePW.close();
        }else{
            //PW FILE COULD NOT BE OPENED
            //ABORT
            MessageHandler::warn("PW file could not be opened: " + filePW.errorString());
            return false;
        }


        QFile fileIv(databasePath + "iv");
        if(fileIv.exists()){
            fileIv.resize(0);
        }
        if(fileIv.open(QIODevice::WriteOnly)){
            QTextStream ivInput(&fileIv);
            ivInput << ivDataCopy;
            ivInput.flush();
            fileIv.close();
        }else{
            //IV FILE COULD NOT BE OPENED
            //ABORT
            MessageHandler::warn("IV file could not be opened: " + fileIv.errorString());
            return false;
        }


        QFile fileMAC(databasePath + "mac");
        if(fileMAC.exists()){
            fileMAC.resize(0);
        }
        if(fileMAC.open(QIODevice::WriteOnly)){
            QTextStream macInput(&fileMAC);
            macInput << macDataCopy;
            macInput.flush();
            fileMAC.close();
        }else{
            //IV FILE COULD NOT BE OPENED
            //ABORT
            MessageHandler::warn("MAC file could not be opened: " + fileMAC.errorString());
            return false;
        }


        QFile fileEntries(databasePath + "dataEntries");
        if(fileEntries.exists()){
            fileEntries.resize(0);
        }
        if(fileEntries.open(QIODevice::WriteOnly)){
            QTextStream entriesInput(&fileEntries);
            entriesInput << dataEntriesDataCopy;
            entriesInput.flush();
            fileEntries.close();
        }else{
            //DATAENTRIES FILE COULD NOT BE OPENEND
            //ABORT
            MessageHandler::warn("DataEntries file could not be opened: " + fileEntries.errorString());
            return false;
        }


        return true;
    }
private:
    LocalBackup();
};

#endif // LOCALBACKUP_H
