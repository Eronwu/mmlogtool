#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include "logindlg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
#if 0
    LoginDlg dlg;
    if (dlg.exec() == QDialog::Accepted)
    {
        w.show();
        return a.exec();
    } else
    {
        QMessageBox::warning(&dlg, "警告",
                             "非mstar人员速速离场",
                             QMessageBox::Ok);
//        qWarning("what?"); //the same as printf
    }

    return 0;
#else
    w.show();
    return a.exec();
#endif
}
