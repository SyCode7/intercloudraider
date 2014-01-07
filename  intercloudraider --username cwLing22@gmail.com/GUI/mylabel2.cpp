#include "mylabel2.h"
#include <QDebug>

MyLabel2::MyLabel2(int n, const QString &text, QWidget *parent) :
    QLabel(parent)
{
    this->setText(text);
    setMouseTracking(true);

    uri = "https://code.google.com/p/intercloudraider/";

    connect(this, SIGNAL(clicked()), this, SLOT(slotClicked()));
    connect(this, SIGNAL(hovered()), this, SLOT(slotHovered()));

    helpMode = n;
}

MyLabel2::~MyLabel2()
{
}

void MyLabel2::mousePressEvent(QMouseEvent *event)
{
    emit(clicked());
}

void MyLabel2::slotClicked()
{
    if(helpMode == 1)
    {
        uri.append("wiki/ManualPage#Functions_of_InterCloud_RAIDer");
        QUrl url = uri;
        QDesktopServices::openUrl(url);

    }
    else if(helpMode == 2)
    {
        uri.append("wiki/Registration#Key_and_Secret");
        QUrl url = uri;
        QDesktopServices::openUrl(url);
    }
}

void MyLabel2::mouseMoveEvent(QMouseEvent *event)
{
    emit(hovered());
}

void MyLabel2::slotHovered()
{
    this->setCursor(QCursor(Qt::PointingHandCursor));
}
