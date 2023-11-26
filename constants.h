#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QByteArray>


//Tune variables for (security vs. performance)


namespace SECURITY_CONSTANTS{
    static const int MASTER_PW_PBKDF_ITERATIONS = 600000;
    static const int USER_MASTER_PW_HASH_ITERATIONS = 600000;
    static const int DATA_ENTRY_MID_KEY_HASH_ITERATIONS = 600000;
}


#endif // CONSTANTS_H
