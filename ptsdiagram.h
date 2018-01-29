#ifndef PTSDIAGRAM_H
#define PTSDIAGRAM_H

#include <QMainWindow>

namespace Ui {
class ptsDiagram;
}

class ptsDiagram : public QMainWindow
{
    Q_OBJECT

public:
    explicit ptsDiagram(QWidget *parent = 0);
    ~ptsDiagram();

private:
    Ui::ptsDiagram *ui;
};

#endif // PTSDIAGRAM_H
