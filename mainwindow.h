#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qfile.h>
#include "versiondialog.h"
#include "resultwindow.h"
#include "ptsdiagram.h"

#define MAX_DIV_PLAYBACK_NUM 500

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool openLogFile();
    bool openKeywordXML();
    bool playbackIntegrityCheck(QFile *logFile, int playbackNo, int allPlayTime);
    bool analyzeLogFile();
    void showDiagram();

private slots:
    void on_actionABOUT_triggered();

    void on_fileOpenButton_clicked();

    void on_startButton_clicked();

    void on_outResultButton_clicked();

    void on_inDataButton_clicked();

    void on_outDiagramButton_clicked();

private:
    Ui::MainWindow *ui;
    VersionDialog *verDlg;
    resultWindow *keywordWin;
    resultWindow *rltWin;
    ptsDiagram *ptsWin;
    QFile logFile;
    QFile keyWordFile;
    QFile playbackLogFile[MAX_DIV_PLAYBACK_NUM];
    QString strResult;
    int intPlayTime;

};

#endif // MAINWINDOW_H
