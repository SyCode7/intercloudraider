#ifndef MYLABEL_H
#define MYLABEL_H

#include <QWidget>
#include <QLabel>

#include "GUI/operationsdialog.h"

class MyLabel : public QLabel
{
    Q_OBJECT

public:
    MyLabel(const QString &text = "", QWidget *parent = 0);
    ~MyLabel();

    bool success;
    int operationMode;

    // backup:
    QList<int> file_idList;

    // retrieve:
    int file_id;
    QString folder;
    
signals:
    void clicked();

    void hovered();

    void toUpdateMainWindow();

    void toUpdateFrame5();

public slots:
    void slotClicked();

    void slotHovered();

protected:
    void mousePressEvent(QMouseEvent *event);
    
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // MYLABEL_H
