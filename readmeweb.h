#ifndef READMEWEB_H
#define READMEWEB_H

#include <QMainWindow>

namespace Ui {
class ReadMeWeb;
}

class ReadMeWeb : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReadMeWeb(QWidget *parent = 0);
    ~ReadMeWeb();

private:
    Ui::ReadMeWeb *ui;
};

#endif // READMEWEB_H
