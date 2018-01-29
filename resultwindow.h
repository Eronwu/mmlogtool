#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

#include <QMainWindow>

namespace Ui {
class resultWindow;
}

class resultWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit resultWindow(QWidget *parent = 0);
    ~resultWindow();
    void setBrowserText(QString str);

private slots:
    void on_saveButton_clicked();

private:
    Ui::resultWindow *ui;
};

#endif // RESULTWINDOW_H
