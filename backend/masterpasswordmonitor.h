#ifndef MASTERPASSWORDMONITOR_H
#define MASTERPASSWORDMONITOR_H

#include <QObject>
#include <QRecursiveMutex>
#include <QSharedPointer>


class MasterPasswordMonitor
{
public:
    MasterPasswordMonitor(const QSharedPointer<QByteArray>& masterPW);
    void setMasterPW(const QSharedPointer<QByteArray>& newMasterPW);
    ~MasterPasswordMonitor();

    /**
     * @brief performOperation takes: T (*function)(const QSharedPointer<QByteArray>&) as a parameter and returns sth of type T<br>
     * It can also take a lambda function as in [&/=](const QSharedPointer<QByteArray>& masterPW){ return doSth(masterPW, arg1(&/=), arg2); } where the return type is T as well
     * @param Consumer: either lambda function or pointer to function
     * @return sth of type T
     */
    template<typename T, typename Consumer>
    T performOperation(Consumer c);
    void example();
    int fun(const QSharedPointer<QByteArray>& masterPW, int number);
private:
    QSharedPointer<QByteArray> masterPW;
    QRecursiveMutex mutex;
    bool protect();
    bool unprotect();
};

#endif // MASTERPASSWORDMONITOR_H
