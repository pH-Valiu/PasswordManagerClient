#ifndef DATAENTRYMODULATOR_H
#define DATAENTRYMODULATOR_H

#include <QString>
#include <QSharedPointer>
#include "dataentry.h"

class DataEntryModulator
{
public:
    DataEntryModulator();
    virtual ~DataEntryModulator() = 0;
    DataEntryModulator(DataEntryModulator&&) = default;
    DataEntryModulator& operator = (DataEntryModulator&&) = default;

    virtual QSharedPointer<DataEntry> modulate() = 0;
    virtual void cancel() = 0;

    virtual void modulateName(const QString& name) = 0;
    virtual void modulateWebsite(const QString& website) = 0;
    virtual void modulateUsername(const QString& username) = 0;
    virtual void modulateEmail(const QString& email) = 0;
    virtual void modulatePassword(const QString& password) = 0;
    virtual void modulateDetails(const QString& details) = 0;
    virtual QString getName() = 0;
    virtual QString getWebsite() = 0;
    virtual QString getUsername() = 0;
    virtual QString getEmail() = 0;
    virtual QString getPassword() = 0;
    virtual QString getDetails() = 0;
    static QRegularExpression regexNaming;
protected:
    QSharedPointer<DataEntry> dataEntry;
    //This masterPW (although it's a shared pointer) does not point to the same QByteArray object as the masterPW in
    //the PasswordManagerAdapter
    QSharedPointer<QByteArray> masterPW;
};




class DataEntryBuilder : public DataEntryModulator
{
public:
    DataEntryBuilder(const QSharedPointer<QByteArray> &masterPW);
    DataEntryBuilder(DataEntryBuilder&&) = default;
    DataEntryBuilder& operator = (DataEntryBuilder&&) = default;
    ~DataEntryBuilder();

    QSharedPointer<DataEntry> modulate();
    void cancel();

    void modulateName(const QString& name);
    void modulateWebsite(const QString& website);
    void modulateUsername(const QString& username);
    void modulateEmail(const QString& email);
    void modulatePassword(const QString& password);
    void modulateDetails(const QString& details);
    QString getName();
    QString getWebsite();
    QString getUsername();
    QString getEmail();
    QString getPassword();
    QString getDetails();

    static QSharedPointer<DataEntry> fromJsonObject(const QJsonObject &jsonObject);
};




class DataEntryEditor : public DataEntryModulator
{
public:
    DataEntryEditor(const QSharedPointer<DataEntry>& dataEntry, const QSharedPointer<QByteArray>& masterPW);
    DataEntryEditor(DataEntryEditor&&) = default;
    DataEntryEditor& operator = (DataEntryEditor&&) = default;
    ~DataEntryEditor();

    /**
     * @brief modulate
     * @return nullptr if masterPW passed in constructor was wrong, QSharedPointer<DataEntry> pointing to the modified dataEntry if everything worked
     */
    QSharedPointer<DataEntry> modulate();
    void cancel();

    void modulateName(const QString& name);
    void modulateWebsite(const QString& website);
    void modulateUsername(const QString& username);
    void modulateEmail(const QString& email);
    void modulatePassword(const QString& password);
    void modulateDetails(const QString& details);
    QString getName();
    QString getWebsite();
    QString getUsername();
    QString getEmail();
    QString getPassword();
    QString getDetails();

    /**
     * @brief changeMasterPassword decrypts midKey and compares its dervied hash value (midKeySalt) with midKeyHash
     *
     * If the comparision was unsuccessfull -> return false, otherwise
     *
     * Enrypt plainMidKey with newMasterPW and set it in dataEntryClone
     *
     * @param newMasterPW already derived from the user masterPW
     * @return true if the change was successfull, false if it wasn't (hashes did not align) or masterPW passed in constructor was wrong
     */
    bool changeMasterPassword(const QSharedPointer<QByteArray> &newMasterPW);
private:
    std::unique_ptr<DataEntry> dataEntryClone = nullptr;
    bool modified;
    bool passwordOk;
};

#endif // DATAENTRYMODULATOR_H
