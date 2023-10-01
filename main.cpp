#include "view.h"
#include "passwordbroker.h"
#include "qaesencryption.h"

#include <QFile>
#include <QTextStream>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    view w;

    QByteArray masterPW = QString("masterPasswort").toUtf8();
    masterPW = QCryptographicHash::hash(masterPW, QCryptographicHash::Sha256);
    QFile file("temp.txt");
    if(file.open(QIODevice::ReadWrite)){
        QTextStream textStream(&file);
        DataEntryBuilder builder("apple");
        builder.addPassword("megaPasswort");
        builder.addUsername("user1");
        builder.addWebsite("apple.com");
        QSharedPointer<DataEntry> apple = builder.build(masterPW);
        qDebug()<<"size of masterPW: "<<masterPW.size();
        QByteArray json = QJsonDocument(apple.data()->toJsonObject()).toJson();
        textStream << json;
        file.close();

        qDebug() << "id prior: "<<QString::fromUtf8(apple.data()->getID());

    }

    if(file.open(QIODevice::ReadOnly)){
        QTextStream textStream(&file);
        QByteArray json = file.readAll();
        file.close();
        QSharedPointer<DataEntry> entry = DataEntryBuilder::fromJsonObject(QJsonDocument::fromJson(json).object());
        qDebug() << "id after: "<<QString::fromUtf8(entry.data()->getID());

        entry.data()->decryptContent(masterPW);
        qDebug() << "content: "<<QString::fromUtf8(entry.data()->getContentPublic()) << "\n"
                 << "username: "<< entry.data()->getUsername().value_or(QString("no value")) <<"\n"
                 << "password: "<<entry.data()->getPassword().value_or(QString("no value"));
        entry.data()->encryptContent(masterPW);
        qDebug() << "content: "<<QString::fromUtf8(entry.data()->getContentPublic()) << "\n"
                 << "username: "<< entry.data()->getUsername().value_or(QString("no value")) <<"\n"
                 << "password: "<<entry.data()->getPassword().value_or(QString("no value"));

        entry.data()->decryptContent(masterPW);
        qDebug() << "content: "<<QString::fromUtf8(entry.data()->getContentPublic()) << "\n"
                 << "username: "<< entry.data()->getUsername().value_or(QString("no value")) <<"\n"
                 << "password: "<<entry.data()->getPassword().value_or(QString("no value"));
        entry.data()->encryptContent(masterPW);
        qDebug() << "content: "<<QString::fromUtf8(entry.data()->getContentPublic()) << "\n"
                 << "username: "<< entry.data()->getUsername().value_or(QString("no value")) <<"\n"
                 << "password: "<<entry.data()->getPassword().value_or(QString("no value"));
    }

    w.show();
    return a.exec();

}
