#include "mylabel.h"

MyLabel::MyLabel(const QString &text, QWidget *parent) :
    QLabel(parent)
{
    this->setText(text);
    setMouseTracking(true);

    connect(this, SIGNAL(clicked()), this, SLOT(slotClicked()));
    connect(this, SIGNAL(hovered()), this, SLOT(slotHovered()));
}

MyLabel::~MyLabel()
{
}

void MyLabel::mousePressEvent(QMouseEvent *event)
{
    emit(clicked());
}

void MyLabel::slotClicked()
{
    OperationsDialog odialog;
    odialog.exec();
    success = odialog.success;
    operationMode = odialog.operationMode;

    if(operationMode == 1)
    {
        file_idList = odialog.file_idList;
    }
    else if(operationMode == 2)
    {
        file_id = odialog.file_id;
        folder = odialog.folder;
    }

    if(success == true)
    {
        emit toUpdateMainWindow();
        emit toUpdateFrame5();
    }
}

void MyLabel::mouseMoveEvent(QMouseEvent *event)
{
    emit(hovered());
}

void MyLabel::slotHovered()
{
    this->setCursor(QCursor(Qt::PointingHandCursor));
}
