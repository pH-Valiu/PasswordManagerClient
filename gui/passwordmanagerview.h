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
    QWidget* scrollAreaWidget;
    QVBoxLayout* scrollAreaLayout;
    QScrollArea* scrollArea;
public slots:
    void refresh();
    void addDataEntryWidget(DataEntryWidget*);
private:

signals:

};

#endif // PASSWORDMANAGERVIEW_H
