#ifndef PASSWORDMANAGERVIEW_H
#define PASSWORDMANAGERVIEW_H

#include <QMainWindow>
#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QScrollArea>
#include "gui/dataentrywidget.h"
#include "dataentrymodulator.h"

class PasswordManagerView : public QMainWindow
{
    Q_OBJECT
public:
    PasswordManagerView(QWidget *parent = nullptr);
    ~PasswordManagerView();
    void editDataEntry(std::unique_ptr<DataEntryEditor>, DataEntryWidget*);
    void createDataEntry(std::unique_ptr<DataEntryBuilder>);
    void scrollTo(const QByteArray& entryID);
public slots:
    void addDataEntryWidget(DataEntryWidget*);
    /**
     * @brief removeDataEntryWidget removes the widget from the layout and deletes it afterwards
     */
    void removeDataEntryWidget(DataEntryWidget*);
private:
    QWidget* scrollAreaWidget;
    QVBoxLayout* scrollAreaLayout;
    QScrollArea* scrollArea;
    QWidget* entriesWidget;
    QVBoxLayout* entriesLayout;
    QWidget* entriesHeaderWidget;
    QHBoxLayout* entriesHeaderLayout;
    QPushButton* addEntryButton;
    QPushButton* saveButton;
    QLineEdit* searchLineEdit;


    void connectSignalSlots();


signals:
    void addEntryButtonClicked();
    void searchEntry(QString identifier);
    void newEntry(QSharedPointer<DataEntry>);
    void saveButtonClicked();
};

#endif // PASSWORDMANAGERVIEW_H
