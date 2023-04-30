#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QRegularExpressionValidator>
// Network
#include <QtNetwork>
#include <QNetworkInterface>
// JSON
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
// UI
#include <QLabel>
#include <QListWidgetItem>
#include <QPainter>
#include <QMessageBox>
#include <QProgressBar>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void udpServerbroadcast(QString Data);

private slots:
    void onDeviceListItemChanged(int currentRow);
    void readFromudpSocket();
    void on_SearchButton_clicked();

    void on_SystemLogInfo_textChanged();
    void onTimeOut();

    void on_VmixIpButton_clicked();  // IP输入按键
    void sendBlink();  // 发送闪烁信息
    void sendIdConfig();  // 发送id配置
    void sendIpConfig();  // 发送ip配置
private:
    int SearchTimeCount;
    Ui::MainWindow *ui;
    QTimer *SearchTime;
    QMessageBox *SearchBox;
    uint8_t DeviceNum = 0;
    QUdpSocket *udpServer;  // udp
    QProgressBar *pro;
    QLabel *proLabel;

//    QList<QUdpSocket *> udpSocketlist;

};
#endif // MAINWINDOW_H
