#include "passwordmanagermodel.h"

PasswordManagerModel::PasswordManagerModel()
{

}

PasswordManagerModel& PasswordManagerModel::getInstance(){
    static PasswordManagerModel passwordManagerModel;
    return passwordManagerModel;
}
