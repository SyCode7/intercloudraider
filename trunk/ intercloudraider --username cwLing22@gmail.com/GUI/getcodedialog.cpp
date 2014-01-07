#include "getcodedialog.h"
#include "ui_getcodedialog.h"

GetCodeDialog::GetCodeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GetCodeDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    setWindowTitle("Proceed to next step");
    this->resize(500, 100);
}

GetCodeDialog::~GetCodeDialog()
{
    delete ui;
}

void GetCodeDialog::on_buttonBox_accepted()
{
    code = ui->lineEdit->text();
}
