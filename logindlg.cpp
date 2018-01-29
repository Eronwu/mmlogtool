#include "logindlg.h"
#include "ui_logindlg.h"

LoginDlg::LoginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDlg)
{
    ui->setupUi(this);
    ui->hellowLab->setText(tr("HELLO WORLD"));
    ui->buttonBox->setHidden(true);
//    ui->inEdit->setEchoMode(QLineEdit::Password);
//    connect(ui->inEdit, SIGNAL(textChanged(QString)), ui->hellowLab, SLOT(setText(QString)));

    QLabel teLab;
    teLab.move(100,50);
    teLab.setText(tr("test"));

}

LoginDlg::~LoginDlg()
{
    delete ui;
}

void LoginDlg::on_inEdit_textChanged(const QString &arg1)
{
    if (arg1 == "123")
    {
        ui->hellowLab->setText(tr("passwd correct!"));
        ui->buttonBox->setHidden(false);
    }
    else if (arg1.length() > 6)
    {
        ui->inEdit->clear();
        ui->inEdit->setFocus();
        ui->hellowLab->setText("pls input passwd again!");
    }
    else
    {
//        ui->hellowLab->setText(itoa(rand(),));
//        ui->hellowLab->setText(tr("HELLO WORLD"));
    }
}
