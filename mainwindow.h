#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qfile.h>
#include "versiondialog.h"
#include "resultwindow.h"
#include <QtCharts/QChartGlobal>
#include <QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>

QT_BEGIN_NAMESPACE
class QComboBox;
class QCheckBox;
QT_END_NAMESPACE

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

enum {
    AXISX_USE_TIME,
    AXISX_USE_PTS,
};

enum {
    AXISY_NONE,
    AXISY_CK_KEEP,
    AXISY_IN_PTS,
    AXISY_OUT_PTS,
    AXISY_AUD_IN_PTS,
    AXISY_AUD_OUT_PTS,
    AXISY_DMS_FLIP,
    AXISY_RENDER_FRAME,
    AXISY_SHOW_PTS,
};
/*
 * TODO: omx's get and push msg
 */
static const struct {
    const int num;
    const char *name;
    const char *searchWord;
    const char *searchWord2;
} ptsTypeTable[] = {
    { AXISY_NONE, "NONE", "NONE", NULL },
    { AXISY_CK_KEEP, "ck_keep", "ck_keep, pts:", NULL },
    { AXISY_IN_PTS, "in_pts", " in_pts:", NULL },
    { AXISY_OUT_PTS, "out_pts", " out_pts:", NULL },
    { AXISY_AUD_IN_PTS, "aud_in_pts", " aud_in_pts: ", NULL },
    { AXISY_AUD_OUT_PTS, "aud_out_pts", " aud_out_pts: ", NULL },
    { AXISY_DMS_FLIP, "dms_flip", " dms_flip [", " pts = " },
    { AXISY_RENDER_FRAME, "render_frame", "render_frame ", " pts = " },
    { AXISY_SHOW_PTS, "showInPts", "showPts", NULL },
};

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

private Q_SLOTS:
    void freshChartsUI();
    void addCharts();

private:
    //私有的class 变量命名之前要加m_prfix
    //布尔型变量建议不要使用否定定义 bFound // SUGGESTED
    //common words 禁止缩写 comp instead of compute
    //get/set, add/remove, create/destroy, start/stop, insert/delete
    Ui::MainWindow *ui;
    VersionDialog *verDlg;
    resultWindow *keywordWin;
    resultWindow *rltWin;
    QWidget *chartsWin;
    QFile logFile;
    QFile keyWordFile;
    QFile playbackLogFile[MAX_DIV_PLAYBACK_NUM];
    bool bLogNoError[MAX_DIV_PLAYBACK_NUM];
    QString strResult;
    int intPlayTime;
    QComboBox *axisX;
    QComboBox *axisY;
    QComboBox *axisY2;
    QComboBox *axisY3;
    QChartView *ptsChartView;
    QGridLayout *baseLayout;
    QHBoxLayout *settingsLayout;
    QPushButton *btn;

    bool openLogFile();
    bool openKeywordXML();
    bool playbackIntegrityCheck(QFile *logFile, int playbackNo, int allPlayTime);
    bool analyzeLogFile();
    int getLogPtsData(int data[][2], const int ptsType);
    QChart *createLineChart();
    void showDiagram();
    long long logTimeConvertMs(const QString logTimeStr);
    void initCharts();
};

#endif // MAINWINDOW_H
