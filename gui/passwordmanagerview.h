#ifndef PASSWORDMANAGERVIEW_H
#define PASSWORDMANAGERVIEW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include "gui/dataentrywidget.h"

class PasswordManagerView : public QMainWindow
{
    Q_OBJECT
public:
    PasswordManagerView(QWidget *parent = nullptr);
    ~PasswordManagerView();
    void editDataEntry(std::unique_ptr<DataEntryModulator>, DataEntryWidget* widget);
public slots:
    void addDataEntryWidget(DataEntryWidget*);
    void removeDataEntryWidget(DataEntryWidget*);
private:
    QWidget* scrollAreaWidget;
    QVBoxLayout* scrollAreaLayout;
    QScrollArea* scrollArea;

signals:

};

#endif // PASSWORDMANAGERVIEW_H
