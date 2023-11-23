#ifndef DATAENTRYWIDGET_H
#define DATAENTRYWIDGET_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "dataentry.h"
#include "advancedpushbutton.h"

class DataEntryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DataEntryWidget(QSharedPointer<const DataEntry> dataEntry, QWidget *parent = nullptr);
    ~DataEntryWidget();
    QString getName()       {return name->text();}
    QByteArray getID()      {return dataEntry->getID();}
public slots:
    /**
     * @brief switchShowButtonIcon calls updateContent() internally
     * @param eyeClosed
     */
    void switchShowButtonIcon(bool eyeClosed);
    void updateContent();
private:
    QSharedPointer<const DataEntry> dataEntry;
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
    AdvancedPushButton* upButton;
    AdvancedPushButton* downButton;


    void setupButtonPanel(QVBoxLayout* entryLayout);
    void setupHeaderPanel(QVBoxLayout* entryLayout);
    void setupContentPanel(QVBoxLayout* entryLayout);
    void connectSignalSlots();
protected:
    virtual void paintEvent(QPaintEvent*);
signals:
    void showClicked(const QByteArray& id, DataEntryWidget* self);
    void editClicked(const QByteArray& id, DataEntryWidget* self);
    void deleteClicked(const QByteArray& id, DataEntryWidget* self);
    void upClicked(bool toTheTop);
    void downClicked(bool toTheBottom);

};

#endif // DATAENTRYWIDGET_H
