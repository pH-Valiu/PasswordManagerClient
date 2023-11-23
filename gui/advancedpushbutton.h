#ifndef ADVANCEDPUSHBUTTON_H
#define ADVANCEDPUSHBUTTON_H

#include <QPushButton>
#include <QObject>

class AdvancedPushButton : public QPushButton
{
    Q_OBJECT
public:
    AdvancedPushButton(QWidget* parent = nullptr);
    AdvancedPushButton(const QString& text, QWidget* parent = nullptr);
    AdvancedPushButton(const QIcon &icon, const QString &text, QWidget* parent = nullptr);
    AdvancedPushButton(const QIcon &icon, QWidget* parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *event);
signals:
    void shiftClicked();
};

#endif // ADVANCEDPUSHBUTTON_H
