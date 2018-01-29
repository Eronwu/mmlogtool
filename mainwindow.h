#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qfile.h>
#include "versiondialog.h"
#include "resultwindow.h"
#include <QtCharts/QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

#define MAX_DIV_PLAYBACK_NUM 500
#define DATA_RAW_MAX 5000

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

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
    QFile logFile;
    QFile keyWordFile;
    QFile playbackLogFile[MAX_DIV_PLAYBACK_NUM];
    QString strResult;
    int intPlayTime;

    bool openLogFile();
    bool openKeywordXML();
    bool playbackIntegrityCheck(QFile *logFile, int playbackNo, int allPlayTime);
    bool analyzeLogFile();
    int getLogPtsData(int data[][2], const QString ptsType);
    QChart *createLineChart();
    void showDiagram();
    long long logTimeConvertMs(const QString logTimeStr);
};

#endif // MAINWINDOW_H
