#include "localbackup.h"
#include "messagehandler.h"
#include <QDir>
#include <QFile>
#include <QCoreApplication>

bool LocalBackup::newLocalBackup(){
    QDir workingDirectory = QDir(QCoreApplication::applicationDirPath());
    if(!workingDirectory.exists()){
        //ERROR
        MessageHandler::critical("Working Directory does not exist?", "Critical Error:");
        return false;
    }


    QString databasePath = QCoreApplication::applicationDirPath().append("/database/");
    QFile fileIv(databasePath + "iv");
    QString ivData;
    if(!fileIv.exists()){
        //ERROR
        MessageHandler::warn("IV file doesn't exist");
        return false;
    }
    if(fileIv.open(QIODevice::ReadOnly)){
        QTextStream ivInput(&fileIv);
        ivData = ivInput.readAll();
        fileIv.close();
    }else{
        //IV FILE COULD NOT BE OPENED
        //ABORT
        MessageHandler::warn("IV file could not be opened: " + fileIv.errorString());
        return false;
    }

    QFile fileMAC(databasePath + "mac");
    QString macData;
    if(!fileMAC.exists()){
        MessageHandler::warn("MAC file doesn't exist");
        return false;
    }
    if(fileMAC.open(QIODevice::ReadOnly)){
        QTextStream macInput(&fileMAC);
        macData = macInput.readAll();
        fileMAC.close();
    }else{
        //IV FILE COULD NOT BE OPENED
        //ABORT
        MessageHandler::warn("MAC file could not be opened: " + fileMAC.errorString());
        return false;
    }

    QFile fileEntries(databasePath + "dataEntries");
    QString dataEntriesData;
    if(!fileEntries.exists()){
        MessageHandler::warn("DataEntries file doesn't exist");
        return false;
    }
    if(fileEntries.open(QIODevice::ReadOnly)){
        QTextStream entriesInput(&fileEntries);
        dataEntriesData = entriesInput.readAll();
        fileEntries.close();
    }else{
        //DATAENTRIES FILE COULD NOT BE OPENEND
        //ABORT
        MessageHandler::warn("DataEntries file could not be opened: " + fileEntries.errorString());
        return false;
    }



    workingDirectory.mkdir("local-backups");
    QDir backupDirectory = QDir(QCoreApplication::applicationDirPath().append("/local-backups/"));

    QString backupName = QDateTime::currentDateTime().toString("dd.MM.yyyy hh-mm-ss-zzz");
    QString backupPath = QCoreApplication::applicationDirPath().append("/local-backups/").append(backupName).append("/");
    backupDirectory.mkdir(backupName);


    QFile fileIvCopy(backupPath + "iv");
    if(fileIvCopy.open(QIODevice::WriteOnly)){
        QTextStream stream(&fileIvCopy);
        stream << ivData;
        stream.flush();
        fileIvCopy.close();
    }else{
        MessageHandler::warn("IV backup file could not be opened: " + fileIvCopy.errorString());
        return false;
    }

    QFile fileMacCopy(backupPath + "mac");
    if(fileMacCopy.open(QIODevice::WriteOnly)){
        QTextStream stream(&fileMacCopy);
        stream << macData;
        stream.flush();
        fileMacCopy.close();
    }else{
        MessageHandler::warn("MAC backup file could not be opened: " + fileMacCopy.errorString());
        return false;
    }

    QFile fileDataEntriesCopy(backupPath + "dataEntries");
    if(fileDataEntriesCopy.open(QIODevice::WriteOnly)){
        QTextStream stream(&fileDataEntriesCopy);
        stream << dataEntriesData;
        stream.flush();
        fileDataEntriesCopy.close();
    }else{
        MessageHandler::warn("DataEntries backup file could not be opened: " + fileDataEntriesCopy.errorString());
        return false;
    }


    return true;
}

bool LocalBackup::revertToBackup(const QString& folderName){
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
