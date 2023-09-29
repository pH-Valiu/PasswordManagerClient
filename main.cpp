#include "view.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    view w;
    int i = 4;
    int ii = 5;
    w.show();
    return a.exec();

}
