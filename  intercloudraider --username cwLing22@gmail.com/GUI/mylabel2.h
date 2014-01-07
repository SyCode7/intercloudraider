#ifndef MYLABEL2_H
#define MYLABEL2_H

#include <QWidget>
#include <QLabel>

#include <QUrl>
#include <QDesktopServices>

class MyLabel2 : public QLabel
{
    Q_OBJECT

    QString uri;

public:
    MyLabel2(int n, const QString &text = "", QWidget *parent = 0);
    ~MyLabel2();
    
signals:
    void clicked();

    void hovered();
    
public slots:
    void slotClicked();

    void slotHovered();

protected:
    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

private:
    int n;
    int helpMode;
    
};

#endif // MYLABEL2_H
