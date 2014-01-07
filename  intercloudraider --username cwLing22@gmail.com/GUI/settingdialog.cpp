#include "settingdialog.h"
#include "ui_settingdialog.h"

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    setWindowTitle("Background Parameter");
    this->setFixedSize(535, 420);

    MyLabel2 *helplabel = new MyLabel2(2, " For more information, please click here", this);

    helplabel->setStyleSheet("QLabel{color : blue}");
    QFont font;
    font.setUnderline(true);
    helplabel->setFont(font);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(ui->label_3);
    layout->addWidget(ui->label_4);
    layout->addWidget(helplabel);

    ui->frame->setLayout(layout);

    // Retrieve information from database setting.db.
    DatabaseControllerSetting dbcontr;
    if(dbcontr.checkParameterKeySecret())
    {
        key_secret = dbcontr.retrieveKeySecret();

        ui->lineEdit->setText(key_secret[0]);
        ui->lineEdit_2->setText(key_secret[1]);
        ui->lineEdit_3->setText(key_secret[2]);
        ui->lineEdit_4->setText(key_secret[3]);
        ui->lineEdit_5->setText(key_secret[4]);
        ui->lineEdit_6->setText(key_secret[5]);
    }
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::on_lineEdit_textEdited(const QString &arg1)
{
    key_secret[0] = arg1;
}

void SettingDialog::on_lineEdit_2_textEdited(const QString &arg1)
{
    key_secret[1] = arg1;
}

void SettingDialog::on_lineEdit_3_textEdited(const QString &arg1)
{
    key_secret[2] = arg1;
}

void SettingDialog::on_lineEdit_4_textEdited(const QString &arg1)
{
    key_secret[3] = arg1;
}

void SettingDialog::on_lineEdit_5_textEdited(const QString &arg1)
{
    key_secret[4] = arg1;
}

void SettingDialog::on_lineEdit_6_textEdited(const QString &arg1)
{
    key_secret[5] = arg1;
}

void SettingDialog::on_buttonBox_accepted()
{
    // update the changed values:
    DatabaseControllerSetting dbcontr;
    dbcontr.updateParameterKeySecret(key_secret[0], key_secret[1],
                                             key_secret[2], key_secret[3],
                                             key_secret[4], key_secret[5]);
}
