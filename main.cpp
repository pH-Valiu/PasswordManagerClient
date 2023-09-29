#include "view.h"
#include "qaesencryption.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    view w;
    QAESEncryption qaes(QAESEncryption::AES_256, QAESEncryption::CBC, QAESEncryption::PKCS7);

    w.show();
    return a.exec();

}
