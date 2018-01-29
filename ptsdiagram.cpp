#include "ptsdiagram.h"
#include "ui_ptsdiagram.h"
#include <QChartView>

ptsDiagram::ptsDiagram(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ptsDiagram)
{
    ui->setupUi(this);
    QChartView chartView = new QChartView(this);

}

ptsDiagram::~ptsDiagram()
{
    delete ui;
}
