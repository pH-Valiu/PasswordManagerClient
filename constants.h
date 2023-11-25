#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QByteArray>


//Tune variables for (security vs. performance)


namespace SECURITY_CONSTANTS{
    static const int MASTER_PW_PBKDF_ITERATIONS = 600000;
    static const int USER_MASTER_PW_HASH_ITERATIONS = 600000;
    static const int DATA_ENTRY_MID_KEY_HASH_ITERATIONS = 600000;
    static const QByteArray USER_MASTER_PW_HASH_SALT("P9Mm&4HmKkD#*jj6FsGvwo*kPh3%3nUN2Tri7!j%4u!^fJVC!8RQeGJ$bzDYHvZd");
    static const QByteArray MASTER_PW_PBKDF_SALT("pK*PSb%hB$dnuBqcG&i!hcruyvpgMKYvXe4q8c#NU*uM%WYc$Yk%QCD36oG2j4tq");
}


#endif // CONSTANTS_H
