#ifndef DATAENTRYWIDGET_H
#define DATAENTRYWIDGET_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "dataentry.h"

class DataEntryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataEntryWidget(QSharedPointer<const DataEntry> dataEntry, const QByteArray& masterPW, QWidget *parent = nullptr);
    void switchShowButtonIcon(bool eyeClosed) const;
private:
    QSharedPointer<const DataEntry> dataEntry;
    bool plain = false;
    QLabel* name;
    QLabel* website;
    QLabel* username;
    QLabel* email;
    QLabel* password;
    QLabel* details;
    QLabel* lastChanged;

    QPushButton* editButton;
    QPushButton* showButton;
    QPushButton* deleteButton;
    QPushButton* usernameCopyButton;
    QPushButton* emailCopyButton;
    QPushButton* passwordCopyButton;
    QPushButton* detailsCopyButton;

    void setupButtonPanel(QVBoxLayout* entryLayout);
    void setupHeaderPanel(QVBoxLayout* entryLayout);
    void setupContentPanel(QVBoxLayout* entryLayout);
    void connectSignalSlots();
    void updateContent();
protected:
    virtual void paintEvent(QPaintEvent*);
signals:
    showClicked(const QByteArray& id);
    editClicked(const QByteArray& id);
    deleteClicked(const QByteArray& id);

};

#endif // DATAENTRYWIDGET_H
