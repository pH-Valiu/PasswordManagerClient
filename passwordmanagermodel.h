#ifndef PASSWORDMANAGERMODEL_H
#define PASSWORDMANAGERMODEL_H


class PasswordManagerModel
{
public:
    PasswordManagerModel(PasswordManagerModel& other) = delete;
    void operator = (const PasswordManagerModel&) = delete;
    /**
     * @brief returns a reference to the only PasswordManagerModel instance at runtime
     * @return
     */
    static PasswordManagerModel& getInstance();
private:
    PasswordManagerModel();
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

