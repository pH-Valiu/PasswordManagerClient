#ifndef DATAENTRYBUILDERDIALOG_H
#define DATAENTRYBUILDERDIALOG_H

#include <QDialog>
#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QSharedPointer>

#include "dataentry.h"

class DataEntryBuilderDialog : public QDialog
{
    Q_OBJECT
public:
    DataEntryBuilderDialog(std::unique_ptr<DataEntryBuilder> builder, QWidget* parent);
    ~DataEntryBuilderDialog();
    void reject();
private:
    std::unique_ptr<DataEntryBuilder> builder;
    QLineEdit* nameEdit;
    QLineEdit* websiteEdit;
    QLineEdit* usernameEdit;
    QLineEdit* emailEdit;
    QLineEdit* passwordEdit;
    QTextEdit* detailsEdit;
    QPushButton* createButton;
    QPushButton* closeButton;
    bool xButtonPressed = true;

    void connectSignalSlots();

private slots:
    void create();
    void cancel();
signals:
    void closing();
    void built(QSharedPointer<DataEntry> entry);

};

#endif // DATAENTRYBUILDERDIALOG_H
