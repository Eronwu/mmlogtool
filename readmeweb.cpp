#include "readmeweb.h"
#include "ui_readmeweb.h"
#include <QtWebView/QtWebView>

ReadMeWeb::ReadMeWeb(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReadMeWeb)
{
    ui->setupUi(this);

    setWindowTitle("README");
}

ReadMeWeb::~ReadMeWeb()
{
    delete ui;
}
