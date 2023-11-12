#include "messagehandler.h"


void MessageHandler::inform(const QString& msg, const QString& header){
    QMessageBox box;
    box.setIcon(QMessageBox::Information);
    box.setText(header);
    box.setInformativeText(msg);
    box.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    box.exec();
}

void MessageHandler::warn(const QString& msg, const QString& header){
    QMessageBox box;
    box.setIcon(QMessageBox::Warning);
    box.setText(header);
    box.setInformativeText(msg);
    box.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    box.exec();
}

void MessageHandler::critical(const QString& msg, const QString& header){
    QMessageBox box;
    box.setIcon(QMessageBox::Critical);
    box.setText(header);
    box.setInformativeText(msg);
    box.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    box.exec();
}
