#include "resultwindow.h"
#include "ui_resultwindow.h"
#include <QFile>
#include <QFileDialog>
#include <qmessagebox.h>

resultWindow::resultWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::resultWindow)
{
    ui->setupUi(this);
}

resultWindow::~resultWindow()
{
    delete ui;
}

void resultWindow::setBrowserText(QString str)
{
    if (!str.isEmpty())
        ui->showBrowser->setText(str);
}

void resultWindow::on_saveButton_clicked()
{
    QFile resultFile;
    QString resultFileName = QFileDialog::getSaveFileName(this,
                                                          tr("save as"),
                                                          tr("AnalyzeResult.txt"));
    if (resultFileName.isEmpty())
        return;

    resultFile.setFileName(resultFileName);
    if (!resultFile.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("多文档编辑器"),
                             tr("无法写入文件 %1, \n %2")
                             .arg(resultFileName).arg(resultFile.errorString()));
        return;
    }

    QTextStream out(&resultFile);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << ui->showBrowser->toPlainText();
    QApplication::restoreOverrideCursor();
}
