#include "advancedpushbutton.h"
#include <QMouseEvent>

AdvancedPushButton::AdvancedPushButton(QWidget* parent) : QPushButton{parent} {}

AdvancedPushButton::AdvancedPushButton(const QString &text, QWidget *parent) : QPushButton{text, parent} {}

AdvancedPushButton::AdvancedPushButton(const QIcon &icon, QWidget *parent) : QPushButton{icon, "", parent} {}

AdvancedPushButton::AdvancedPushButton(const QIcon &icon, const QString &text, QWidget *parent) : QPushButton{icon, text, parent} {}

void AdvancedPushButton::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        if(event->modifiers() == Qt::ShiftModifier){
            emit shiftClicked();
        }else{
            emit QPushButton::clicked(false);
        }
    }else{
        QPushButton::mousePressEvent(event);
    }
}
