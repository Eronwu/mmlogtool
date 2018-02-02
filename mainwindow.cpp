#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <qpainter.h>
#include <qtextstream.h>
#include <qfile.h>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>


#define FOR_RELEASE 0

/*
 * TODO:
 * 1, 任务栏图标在分析过程中闪烁，使用：pannelIcon = new QSystemTrayIcon(this);
                                  pannelIcon->setIcon(QIcon("tray.ico"));
      不同颜色图标交换切换即可；
      参考：http://blog.csdn.net/mingzznet/article/details/17202689
 * 2，HOVER事件，当移动到一些空间上面进行注释显示
 * 参考http://blog.163.com/zh_ch_0412@126/blog/static/16409046620124222494859/
 * 3, 建议加入一个mediainfo的功能，可以单独增加一个按钮显示播放了几个码流，
 *    每个码流的基本信息，比如分辨率，音视频格式，I/P，帧率之类的
 *
 */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    verDlg(new VersionDialog),
    keywordWin(new resultWindow),
    rltWin(new resultWindow),
    chartsWin(new QWidget),
    intPlayTime(0),
    axisX(new QComboBox()),
    axisY(new QComboBox()),
    axisY2(new QComboBox()),
    axisY3(new QComboBox()),
    ptsChartView(NULL),
    baseLayout(new QGridLayout()),
    settingsLayout(new QHBoxLayout()),
    btn(new QPushButton())
{
    ui->setupUi(this);
    setWindowTitle(tr("MSTAR 一键LOG分析工具"));
    clearAllStatus();
    initCharts();

//    ui->baojiCheckBox->setStyleSheet("QPushButton:hover{}"
#if 0
    QAction *openAction = new QAction(tr("&Open"), this);
    QIcon myIcon(":/new/myimages/mstarLogo.png");
    openAction->setIcon(myIcon);
    openAction->setShortcut(QKeySequence(tr("Ctrl+M")));
    ui->menuMENU->addAction(openAction);
#endif
}

MainWindow::~MainWindow()
{
    clearAllStatus();
    delete verDlg;
    delete keywordWin;
    delete rltWin;
    delete ui;
}

void MainWindow::clearAllStatus()
{
//    if (logFile.exists())
//        logFile.close();
    if (keyWordFile.exists())
        keyWordFile.close();
    for (int i=0; i<intPlayTime; i++)
    {
        if (!playbackLogFile[i].exists())
            continue;
        playbackLogFile[i].close();
        //delete ok log
        if (!(bLogNoError[i] == false && ui->keepLogCheckBox->isChecked()))
            playbackLogFile[i].remove();
    }
    strResult.clear();
    intPlayTime = 0;
    ui->outResultBrowser->setPlainText(NULL);
    ui->inDataBrowser->setText(tr(NULL));
    ui->outDiagramButton->setVisible(false);
    ui->outDiagramlabel->setVisible(false);
    ui->outDiagramSpinBox->setVisible(false);
}

void MainWindow::initCharts()
{
    const int ptsTypeTableRow = sizeof(ptsTypeTable) / sizeof(ptsTypeTable[0]);
    for (int i=0; i<ptsTypeTableRow; i++)
    {
        if (i != 0)
            axisY->addItem(ptsTypeTable[i].name, ptsTypeTable[i].num);
        axisY2->addItem(ptsTypeTable[i].name, ptsTypeTable[i].num);
        axisY3->addItem(ptsTypeTable[i].name, ptsTypeTable[i].num);
    }
//    for (int i=0; i<ptsTypeTableRow; i++)
        axisX->addItem("dafaultTIME", AXISX_USE_TIME);
        axisX->addItem("currentPTS", AXISX_USE_PTS);

    chartsWin->connect(axisX, SIGNAL(currentIndexChanged(int)), this, SLOT(freshChartsUI()));
    chartsWin->connect(axisY, SIGNAL(currentIndexChanged(int)), this, SLOT(freshChartsUI()));
    chartsWin->connect(axisY2, SIGNAL(currentIndexChanged(int)), this, SLOT(freshChartsUI()));
    chartsWin->connect(axisY3, SIGNAL(currentIndexChanged(int)), this, SLOT(freshChartsUI()));
    settingsLayout->addWidget(new QLabel(tr("x坐标:")));
    settingsLayout->addWidget(axisX);
    settingsLayout->addWidget(new QLabel(tr("y坐标:")));
    settingsLayout->addWidget(axisY);
    settingsLayout->addWidget(new QLabel(tr("y2坐标:")));
    settingsLayout->addWidget(axisY2);
    settingsLayout->addWidget(new QLabel(tr("y3坐标:")));
    settingsLayout->addWidget(axisY3);
    btn->setText(tr("异常LOG显示"));
    settingsLayout->addWidget(btn);
    chartsWin->connect(btn, SIGNAL(clicked(bool)), this, SLOT(addCharts()));
    settingsLayout->addStretch();
    baseLayout->addLayout(settingsLayout, 0, 0, 1, 8);
}

bool MainWindow::openLogFile()
{
    QString logFileName = QFileDialog::getOpenFileName(this);
    qDebug() << logFileName;
    if (logFileName.isEmpty())
    {
        qDebug() << "logfileName is NULL!";
        return false;
    }
    if (logFile.exists())
    {
        qDebug() << "logfile exits, will close first";
        logFile.close();
    }
    logFile.setFileName(logFileName);
    if (!logFile.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("多文档编辑器"),
                             tr("无法读取文件 %1, \n %2")
                             .arg(logFileName).arg(logFile.errorString()));
        return false;
    }
//    QTextStream logStream(&logFile);
    QApplication::setOverrideCursor(Qt::WaitCursor);
//    ui->temLogBrowser->setPlainText(logStream.readAll());
    QApplication::restoreOverrideCursor();
//    setWindowTitle(QFileInfo(logFileName).canonicalFilePath());
    setWindowTitle(QFileInfo(logFileName).fileName());
    ui->openedLabel->setStyleSheet("QLable{color: red;}");
    ui->openedLabel->setText(tr("打开成功"));
    ui->outDiagramButton->setVisible(false);
    ui->outDiagramlabel->setVisible(false);
    ui->outDiagramSpinBox->setVisible(false);

    return true;
}

bool MainWindow::openKeywordXML()
{
    if (keyWordFile.exists())
    {
        //in case of somebody modified xml
        qDebug() << "keywordfile already opened,will open again";
        keyWordFile.close();
    }
#if FOR_RELEASE
    keyWordFile.setFileName("./keyword.xml");
#else
    keyWordFile.setFileName("D:/Qt/workstudio/build-test-Desktop_Qt_5_9_0_MinGW_32bit-Debug/debug/keyword.xml");
#endif
    if (!keyWordFile.open(QFile::ReadOnly|QFile::Text))
    {
        QMessageBox::warning(this, tr("多文档编辑器"),
                             tr("找不到 %1, \n 请将该文件放到本工具同级目录")
                             .arg(tr("keyword.xml")));//.arg(logFile.errorString()));//%2
        return false;
    }
    return true;
}

bool MainWindow::playbackIntegrityCheck(QFile *divLogFile, int playbackNo, int allPlayTime)
{
    QString logLine;
    // TODO: 也提取到另一个文件注入
    /* TODO: 因为两个播放过程的尾巴结束有交集，暂时使用这种方法处理
    ** 即，勾选选项 有交集的log忽略destructor之后的条目核对，无交集log分析到destructor Leave
    ** 或者看看是否有已定义数组增加元素方法，但如果采用文件注入就不要管这些了
    */
    static const QString checkListStr[] = { "MstPlayer::constructor",
                                          "MstPlayer::constructor Leave",
                                          "MstPlayer::setDataSource",
                                          "MstPlayer::prepareAsync",
                                          "MstPlayer::prepared",
                                          "MstPlayer::start",
                                          "MstPlayer::reset",
                                          "MstPlayer::destructor",
                                          "MstPlayer::destructor Leave"};
    int checkListLength = sizeof(checkListStr) / sizeof(checkListStr[0]);
    int i = 0;
    int ignoreLine = 0;

    divLogFile->seek(0);
    strResult += tr("\n[playback %1 integrity checking]\n").arg(playbackNo);
    if (ui->baojiCheckBox->isChecked()) checkListLength -= 1;
    while (i < checkListLength) {
        logLine = divLogFile->readLine();
        if (logLine.contains(checkListStr[i]))
            i++;
        if (divLogFile->atEnd())
        {
            if (playbackNo == 0 && i == ignoreLine)
            {
                i++;
                ignoreLine++;
                divLogFile->seek(0);
            }
            else
                break;
        }
    }

    if (i == checkListLength)
    {
        strResult += tr("[playback %1 integrity check OK]\n").arg(playbackNo);
        return true;
    }
    else if (i == 0 && allPlayTime == playbackNo + 1)
    {
        strResult += tr("---this part is not a playback\n");
        return true;
    }
    else
    {
//        if (playbackNo == 0)
//            strResult += tr("---WARNING:%1 part may no has integrity log or playback\n").arg(playbackNo);
//        for (int j=i; j<checkListLength-1; j++)
//            strResult += tr("playback %1 has no \"%2\"\n").arg(playbackNo).arg(checkListStr[j]);
        strResult += tr("---ERROR:playback %1 stuck before \"%2\"\n").arg(playbackNo).arg(checkListStr[i]);
        return false;
    }
}

bool MainWindow::analyzeLogFile()
{
    clearAllStatus();
    if (!logFile.isOpen())
    {
        qDebug() << "enter analyze file logfile no exits";
#if FOR_RELEASE
        QMessageBox::warning(this, tr("提示"), tr("log文件未打开！"));
        return false;
#else
        logFile.setFileName("D:/Qt/workstudio/build-test-Desktop_Qt_5_9_0_MinGW_32bit-Debug/test_log/avs2localmmOK.log");
        if (!logFile.open(QFile::ReadOnly | QFile::Text))
        {
            QMessageBox::warning(this, tr("多文档编辑器"),
                                 tr("无法读取文件 %1, \n %2")
                                 .arg(tr("define log")).arg(logFile.errorString()));
            return false;
        }
#endif
    }

    if (!openKeywordXML())
        return false;

    ui->outResultBrowser->setPlainText(tr("AI analyzing... hold on for a second"));
    ui->inDataBrowser->setText(tr("AI analyzing... hold on for a second"));
    QApplication::setOverrideCursor(Qt::WaitCursor);
    intPlayTime = 1; // default has 1 playback
    while (!logFile.atEnd())
    {
        if (logFile.readLine().contains(")MstPlayer::constructor"))
            intPlayTime++;
    }

    if (intPlayTime > MAX_DIV_PLAYBACK_NUM)
    {
        QMessageBox::warning(this, tr("警告！"),
                             tr("log播放次数%1超过程序分析限制，请联系修改").arg(intPlayTime));
        return false;
    }

    logFile.seek(0);
    for (int i=0; i<intPlayTime; i++)
    {
        qDebug() << tr("create file %1").arg(i);
        playbackLogFile[i].setFileName(tr("./divLog%1.log").arg(i));
        if (!playbackLogFile[i].open(QFile::ReadOnly | QFile::WriteOnly | QFile::Text))
        {
            QMessageBox::warning(this, tr("多文档编辑器"),
                                 tr("无法创建文件 %1, \n %2")
                                 .arg(tr("./divLog%1.log").arg(i)).arg(logFile.errorString()));
            return false;
        }
        QTextStream out(&playbackLogFile[i]);
        QString sLine;
        QString sKeyword = ui->baojiCheckBox->isChecked()?")MstPlayer::destructor":"MstPlayer::destructor Leave";
        do {
            sLine = logFile.readLine();
            out << sLine;
        } while (!(sLine.contains(sKeyword) || logFile.atEnd()));
    }

    int intPlaybackNo = 0;
    strResult.clear();
    strResult += tr("****** maybe %1 playback in all ******\n").arg(intPlayTime);

    memset(bLogNoError, true, sizeof(bLogNoError));
    while (intPlaybackNo < intPlayTime)
    {
        QString strKeyWordLine;
        QString strKeyWordType;
        QString strKeyWord;
        QString strKeyWordMsg;
        QString strLogLine;
        int intKeyWordValue = 0;

        strResult += tr("\n*** start analyze %1 playback error log ***\n").arg(intPlaybackNo);
        // playback integrity checking
        bLogNoError[intPlaybackNo] &= playbackIntegrityCheck(&playbackLogFile[intPlaybackNo], intPlaybackNo, intPlayTime);

        //play log error checking
        /***********************循环扫描改成check xml 优化效率？*****************/
        keyWordFile.seek(0);
    #if 1
        // from openKeywordXML
        while (!keyWordFile.atEnd())
        {
            playbackLogFile[intPlaybackNo].seek(0);
            strKeyWordLine.clear();
            strKeyWordLine = keyWordFile.readLine().trimmed();
            if (strKeyWordLine == NULL)
                continue;
            else if (strKeyWordLine.at(0) == "#")
                continue;
    #else
            int i = 1;
            while (i--) {
            //    if (!ui->testSearchEdit->text().isEmpty())
            //        keyWordStr = ui->testSearchEdit->text();
            //    else
                    strKeyWordLine = "<2><MsVdecVacancy: ><need to check the vacancy value whether larger than a certain num which can know display module error><50>";
    #endif
            strKeyWordType = strKeyWordLine.section(QRegExp("[<>]"), 0, 0, QString::SectionSkipEmpty).trimmed();
            strKeyWord = strKeyWordLine.section(QRegExp("[<>]"), 1, 1, QString::SectionSkipEmpty).trimmed();
            strKeyWordMsg = strKeyWordLine.section(QRegExp("[<>]"), 2, 2, QString::SectionSkipEmpty).trimmed();
            if (strKeyWordType.toInt() > 0) {
                intKeyWordValue = strKeyWordLine.section(QRegExp("[<>]"), 3, 3, QString::SectionSkipEmpty).trimmed().toInt();
    //            qDebug() << tr("4:%1").arg(intKeyWordValue);
            }
    //            qDebug() << "1:"+strKeyWordType;
    //            qDebug() << "2:"+strKeyWord;
    //            qDebug() << "3:"+strKeyWordMsg;
            ui->inDataBrowser->insertPlainText(strKeyWordType + " " + strKeyWord + '\n');
            ui->inDataBrowser->moveCursor(QTextCursor::Start);

            if (strKeyWord == NULL)
            {
                qDebug() << "strKeyWord == NULL";
                continue;
            }
            if (strKeyWordType == "0")
            {
                bool bHaveError = false;
                while (!playbackLogFile[intPlaybackNo].atEnd())
                {
                    strLogLine = playbackLogFile[intPlaybackNo].readLine().trimmed();
                    if (strLogLine.contains(strKeyWord))
                    {
                        strResult += strLogLine + "\n";
                        bHaveError = true;
                        bLogNoError[intPlaybackNo] &= false;
                    }
                }
                if (bHaveError)
                    strResult = strResult + "---ERROR: " + strKeyWordMsg + "\n" + "\n";
            }
            if (strKeyWordType == "1")
            {
                QString readValue1;
                QString readValue2;
                bool bHaveError = false;
                while (!playbackLogFile[intPlaybackNo].atEnd())
                {
                    strLogLine = playbackLogFile[intPlaybackNo].readLine().trimmed();
                    if (strLogLine.contains(strKeyWord))
                    {
                        readValue1 = strLogLine.section(strKeyWord, 1, 1, QString::SectionSkipEmpty).trimmed();
                        readValue2 = readValue1.section(',', 0, 0, QString::SectionSkipEmpty).trimmed();
                        if (readValue2.toInt() > intKeyWordValue)
                        {
                            strResult += strLogLine + "\n";
                            bHaveError = true;
                            bLogNoError[intPlaybackNo] &= false;
                        }
                    }
                }
                if (bHaveError)
                    strResult = strResult + "---ERROR: " + strKeyWordMsg + "\n" + "\n";
            }
            if (strKeyWordType == "2")
            {
                int cnt = 0;
                QString temLine;
                while (!playbackLogFile[intPlaybackNo].atEnd())
                {
                    strLogLine = playbackLogFile[intPlaybackNo].readLine().trimmed();
                    if (strLogLine.contains(strKeyWord))
                    {
                        cnt++;
                        bLogNoError[intPlaybackNo] &= false;
                        if (temLine == NULL )temLine = strLogLine;
                    }
                }
                if (cnt > intKeyWordValue)
                    strResult += temLine + "\n---ERROR: " + strKeyWordMsg + "\n" + "\n";
            }
        }
        intPlaybackNo++;
    }
    bool noErr = true;
    for (int i=0; i<intPlayTime; i++) noErr &= bLogNoError[i];
    if (noErr)
        ui->outResultBrowser->setText(tr("小弟认为正常，可以关闭mantis"));
    else
        ui->outResultBrowser->setText(strResult);
    QApplication::restoreOverrideCursor();
    ui->outDiagramButton->setVisible(true);
    ui->outDiagramlabel->setVisible(true);
    ui->outDiagramSpinBox->setVisible(true);

    return true;
}

long long MainWindow::logTimeConvertMs(const QString logTimeStr)
{
    // TODO: 先不弄日期的划分
    int h, m, s, ms;

    h = m = s = ms = 0;
    h = logTimeStr.section(QRegExp("[:.]"), 0, 0).trimmed().toInt();
    m = logTimeStr.section(QRegExp("[:.]"), 1, 1).trimmed().toInt();
    s = logTimeStr.section(QRegExp("[:.]"), 2, 2).trimmed().toInt();
    ms = logTimeStr.section(QRegExp("[:.]"), 3, 3, QString::SectionSkipEmpty).trimmed().toInt();
//    qDebug() << "h:" << h << "m:" << m << "s:" << s << "ms:" << ms;
    return h*60*60*1000 + m*60*1000 + s*1000 + ms;

}

int MainWindow::getLogPtsData(int data[][2], const int ptsType)
{
    int playNo = ui->outDiagramSpinBox->value();
    QString line;
    QString ptsStr;
    QString tmp;
    int dataRaw = 0;
    long long timeMs = 0;
    long long preTimeMs = 0;

    if (!playbackLogFile[playNo].isOpen())
    {
        QMessageBox::warning(this, tr("警告"), tr("没有第%1次播放的log\n总共只有%2次播放！").arg(playNo).arg(intPlayTime));
        return 0;
    }

    playbackLogFile[playNo].seek(0);
    while (!playbackLogFile[playNo].atEnd())
    {
        line = playbackLogFile[playNo].readLine().trimmed();
        if (line == NULL)
            continue;
        if (!line.contains(ptsTypeTable[ptsType].searchWord))
            continue;

        /* get pts */
        if (ptsTypeTable[ptsType].searchWord2 == NULL)
            ptsStr = line.section(ptsTypeTable[ptsType].searchWord, 1, 1).trimmed();
        else
            ptsStr = line.section(ptsTypeTable[ptsType].searchWord2, 1, 1).trimmed();
        data[dataRaw][0] = ptsStr.section(",", 0, 0).trimmed().toInt();

        /* get logtime */
        tmp = line.section(" ", 1, 1).trimmed(); // get 15:33:12.863
        timeMs = logTimeConvertMs(tmp);
//                qDebug() << "time:" << tmp;
        data[dataRaw][1] = (preTimeMs != 0)?(timeMs - preTimeMs):0;
        preTimeMs = timeMs;
//        qDebug() << "pts:" << data[dataRaw][0] << "time:" << timeMs << "gaptime:" << data[dataRaw][1];
        if (dataRaw++ >= DATA_RAW_MAX)
        {
            QMessageBox::warning(this, tr("警告"), tr("pts个数超过了数组最大行数:%1，只能部分显示").arg(DATA_RAW_MAX));
            dataRaw--;
            break;
        } // TODO :check this CRASHED  DATA_RAW_MAX
          // TODO :将pts gap时间差太多的log存到缓存并点击显示；
    }
    return dataRaw;
}

QChart *MainWindow::createLineChart()
{
    int dataRaw;
    QChart *chart = new QChart();
    int ck_keepData[DATA_RAW_MAX][2]; // {pts, time_gap}
    int axisXType = axisX->itemData(axisX->currentIndex()).toInt();
    int axisYtype[3] = {axisY->itemData(axisY->currentIndex()).toInt(),
                        axisY2->itemData(axisY2->currentIndex()).toInt(),
                        axisY3->itemData(axisY3->currentIndex()).toInt()};

    for (int j=0; j<3; j++)
    {
        QLineSeries *series = new QLineSeries(chart);
        memset(ck_keepData, 0, sizeof(ck_keepData));
        dataRaw = getLogPtsData(ck_keepData, axisYtype[j]==AXISY_SHOW_PTS?AXISY_IN_PTS:axisYtype[j]);
    //    qDebug() << "dataraw:" << dataRaw;
        for (int i=0; i< dataRaw; i++)
        {
    //        qDebug() << "ck_keep:" << ck_keepData[i][0] << "gaptime:" << ck_keepData[i][1];
            if (axisYtype[j]==AXISY_SHOW_PTS)
                series->append(i, ck_keepData[i][0]);
            else
                series->append(axisXType==AXISX_USE_TIME?i:ck_keepData[i][0], ck_keepData[i][1]);
        }
        series->setName(ptsTypeTable[axisYtype[j]].name);
    //    series->setColor(Qt::red);
    //    series->setPointLabelsFormat("hehe");]
    //    series->setPointLabelsColor(Qt::red);
    //    series->setPen();
//        series->setPointsVisible(true);
        chart->addSeries(series);
    }

    chart->setTitle("Pts Diagram");
    chart->createDefaultAxes();
    chart->setAnimationOptions(QChart::SeriesAnimations);

    return chart;
}

void MainWindow::freshChartsUI()
{
    if (ptsChartView != NULL)
    {
        ptsChartView->close();
        baseLayout->destroyed(ptsChartView);
    }
    ptsChartView = new QChartView(createLineChart());
    ptsChartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    baseLayout->addWidget(ptsChartView);
//    baseLayout->setHorizontalSpacing(100);
    chartsWin->setLayout(baseLayout);
    chartsWin->resize(900, 600);
    chartsWin->show();
    ptsChartView->setRenderHint(QPainter::Antialiasing, true);
}

void MainWindow::showDiagram()
{
#if 1
    freshChartsUI();

#else
    ptsChartView = new QChartView(createLineChart());
    ptsChartView->resize(900, 600);
    ptsChartView->show();

#endif
}

void MainWindow::on_actionABOUT_triggered()
{
    verDlg->show();
//    if (verDlg->exec() == QDialog::Accepted)
//        return;
}

void MainWindow::on_fileOpenButton_clicked()
{
    openLogFile();
}

void MainWindow::on_startButton_clicked()
{
    analyzeLogFile();
}

void MainWindow::on_outResultButton_clicked()
{
    rltWin->show();
    rltWin->setWindowTitle(tr("结果显示"));
    rltWin->setBrowserText(strResult);
//    rltWin->resize(1000, 500);
    rltWin->setGeometry(100, 100, 1000, 500);
}

void MainWindow::on_inDataButton_clicked()
{
    rltWin->show();
    rltWin->setWindowTitle(tr("结果显示"));
    rltWin->setBrowserText(ui->inDataBrowser->toPlainText());
    rltWin->setGeometry(100, 100, 1000, 500);
}

void MainWindow::on_outDiagramButton_clicked()
{
    showDiagram();
}
