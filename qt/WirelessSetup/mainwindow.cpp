#include "mainwindow.h"
#include "ui_mainwindow.h"

#define Ver "1.0.0"  // 当前版本

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("TallyHo 配置软件"));  // 设定名称
    this->setWindowIcon(QIcon(":/pic/logo.ico")); // 设定Logo

    // 获取本机Ip
    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);
    foreach(QHostAddress address,info.addresses())
    {
         if(address.protocol() == QAbstractSocket::IPv4Protocol)
            ui->SystemLogInfo->insertPlainText("\n本机IP:"+address.toString());
    }
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(onDeviceListItemChanged(int)));

    // Udp初始化
    udpServer = new QUdpSocket(this);
    connect(udpServer, SIGNAL(readyRead()), this, SLOT(readFromudpSocket()));

    // IP输入框配置
    // 正则表达式限制输入
    ui->VmixIpEdit->setValidator(
       new QRegularExpressionValidator
       (
           QRegularExpression("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")
       )
    );
    // 默认提示setPlaceholderText
    ui->VmixIpEdit->setPlaceholderText(QString("192.168.111.111"));
    // 用于占位
//    ui->VmixIpEdit->setInputMask("000.000.000.000; ");

    // 底部状态栏显示
    // 隐藏状态栏下方三角形
    ui->statusbar->setSizeGripEnabled(false);


    QLabel *labCellIndex = new QLabel("   应用版本: "+QString(Ver),this);
    labCellIndex->setMinimumWidth(150);
    // 将初始化的标签添加到底部状态栏上
    ui->statusbar->addWidget(labCellIndex);

    // 进度条
    pro = new QProgressBar(this);
    // 自动计算
    ui->statusbar->addPermanentWidget(pro, 1);
    // 设定大小
    pro->setMinimum(200);
    // 设置进度是否显示
    pro->setTextVisible(true);
    // 设置初始化进度位置
    pro->setValue(0);
    // 进度条文本
    proLabel = new QLabel(this);
    proLabel->setText("等待搜索");
    ui->statusbar->addWidget(labCellIndex);
    ui->statusbar->addPermanentWidget(pro);
    ui->statusbar->addPermanentWidget(proLabel);

    // 跳转网页
    QLabel *label_url = new QLabel(this);
    QLabel *label_about = new QLabel(this);
    // 配置连接
    label_url->setFrameStyle(QFrame::Box | QFrame::Sunken);
    label_url->setText(tr("<a href=\"https://github.com/MakingClub/TallyHo\">访问项目主页</a>"));
    label_url->setOpenExternalLinks(true);

    label_about->setFrameStyle(QFrame::Box | QFrame::Sunken);
    label_about->setText(tr("<a href=\"https://space.bilibili.com/1369737196\">Making Club</a>"));
    label_about->setOpenExternalLinks(true);

    // 将信息增加到底部（永久添加）
    ui->statusbar->addPermanentWidget(label_url);
    ui->statusbar->addPermanentWidget(label_about);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 设备列表的currentRowChanged信号 更改page
void MainWindow::onDeviceListItemChanged(int currentRow)
{
//    ui->SystemLogInfo->insertPlainText("\n改变222!");
    if(currentRow >= 0)
        ui->stackedWidget->setCurrentIndex(currentRow);  // 跳过自带的page
}
// 查找局域网设备
void MainWindow::on_SearchButton_clicked()
{
    proLabel->setText("正在搜索");  // 底部提示
    SearchTimeCount = 0;  // 计时器清零
    DeviceNum = 0;  // 设备计数清零
    SearchTime = new QTimer();
    SearchTime->setInterval(1);
    connect(SearchTime, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    SearchTime->start();  // 搜索定时器
    ui->listWidget->clear();  // 清除列表设备
    for(int i = ui->stackedWidget->count(); i >= 0; i--) {  // 清除设备配置界面
       QWidget* widget = ui->stackedWidget->widget(i);
       ui->stackedWidget->removeWidget(widget);
       widget->deleteLater();
    }

//    SearchBox = new QMessageBox(QMessageBox::Information,QString("提示"),QString("正在搜索"));  // 阻塞  暂时不使用
}
// 局域网广播函数
void MainWindow::udpServerbroadcast(QString Data) {
    QList<QNetworkInterface> networkinterfaces = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface interfaces, networkinterfaces) {
        foreach (QNetworkAddressEntry entry, interfaces.addressEntries()) {
          QHostAddress broadcastAddress = entry.broadcast();
          if (broadcastAddress != QHostAddress::Null
                && entry.ip() != QHostAddress::LocalHost
                && entry.ip().protocol() == QAbstractSocket::IPv4Protocol
              ) {
              udpServer->writeDatagram(Data.toUtf8(), broadcastAddress, 2333);
          }
       }
    }
}
// 搜索定时器回调函数
void MainWindow::onTimeOut()
{
    SearchTimeCount++;
    udpServerbroadcast("vMix");  // 局域网广播
    pro->setValue(SearchTimeCount*50);  // 进度条移动
    if(SearchTimeCount > 3){
//        SearchBox->exec();  // 阻塞  暂时不使用
        SearchTime->stop();
        pro->setValue(0);  // 进度条设为0
        proLabel->setText("搜索完成");
    }
}
// 解析设备回信
void MainWindow::readFromudpSocket()
{
    QHostAddress client_address;//声明一个QHostAddress对象用于保存发送端的信息
    // char buf[100];//声明一个字符数组用于接收发送过来的字符串
    // 数据报不为空
    while( udpServer->hasPendingDatagrams() )
    {
        quint16 recPort = 0;
        QByteArray datagram;
        // datagram 大小为等待处理数据报的大小才能就收数据;
        datagram.resize( udpServer->pendingDatagramSize() );
        // 接收数据报
        udpServer->readDatagram(datagram.data(),datagram.size(), &client_address, &recPort);
        ui->SystemLogInfo->insertPlainText("\r\n 收到数据:"+datagram);
        // json 解析
        QJsonParseError jsonError;
        QJsonDocument doucment = QJsonDocument::fromJson(datagram, &jsonError);  // 转化为 JSON 文档
        if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {  // 解析未发生错误
            QJsonObject object = doucment.object();  // 转化为对象
            QJsonValue  ipvalue, idvalue, powervalue, vipvalue, RSSIvalue;
            if (object.contains("ip")) {  // 检索ip地址
                ipvalue = object.value("ip");
            }

            if (object.contains("id")) {  // 检索id
                idvalue = object.value("id");
            }

            if (object.contains("power")) {  // 检索电量
                powervalue = object.value("power");
            }

            if (object.contains("vip")) {  // 检索vmix ip
                vipvalue = object.value("vip");
            }

            if(object.contains("RSSI")) {  // 检索信号 RSSI
                RSSIvalue = object.value("RSSI");
            }

            // 在列表中查找此设备是否存在
            //检查设备是否已存在，避免重复添加
            QList<QListWidgetItem *> items = ui->listWidget->findItems(ipvalue.toString(), Qt::MatchContains);
            //不存在则添加至设备列表
            if (items.empty())
            {
                DeviceNum ++;  // 设备加1
                // 添加此ip设备
                ui->listWidget->addItem("设备 "+ QString("%1").arg(DeviceNum) +\
                                        "  vMix ID: " + idvalue.toVariant().toString() +\
                                        "\nip: "+ipvalue.toString());
                // 创建界面
                QWidget *widget = new QWidget;
                QVBoxLayout *layout = new QVBoxLayout;
                // 创建信息标签
                QHBoxLayout *infolayout = new QHBoxLayout;  // 创建水平布局
                QLabel *devicelabel = new QLabel("设备 "+ QString("%1").arg(DeviceNum) +\
                                           "    vMix ID: "+idvalue.toVariant().toString() +\
                                           "    ip: "+ipvalue.toString());
                devicelabel->setObjectName("devicelabel");  // 添加对象名称方便查找
                QFont ft;
                ft.setPointSize(20);
                devicelabel->setFont(ft);
                QPushButton *blinkbutton = new QPushButton();
                connect(blinkbutton, SIGNAL(clicked()), this, SLOT(sendBlink()));  // 闪烁设备信号槽连接
                blinkbutton->setText("闪烁设备ID");
                infolayout->addWidget(devicelabel);
                infolayout->addWidget(blinkbutton);
                layout->addLayout(infolayout);  // 将横向添加到竖向布局

                // 创建ID配置
                QHBoxLayout *idlayout = new QHBoxLayout;  // 创建水平布局
                QLabel *vmixidlabel = new QLabel("设置当前设备vMix ID: ");
                QLineEdit *idinput = new QLineEdit();  // id设置框
                idinput->setObjectName("idinput");
                idinput->setPlaceholderText("请输入新的ID  1-20");
                idinput->setValidator(new QIntValidator(1, 20, this));
                idinput->resize(200, 50);
                QPushButton *idsetbutton = new QPushButton();
                idsetbutton->setText("发送ID配置");
                connect(idsetbutton, SIGNAL(clicked()), this, SLOT(sendIdConfig()));  // id按键信号槽连接
                idlayout->addWidget(vmixidlabel);  // 将标签添加到横向
                idlayout->addWidget(idinput);  // 将输入添加到横向
                idlayout->addWidget(idsetbutton);  // 将按键添加到横向
                layout->addLayout(idlayout);  // 将横向添加到竖向布局

                // 创建ip配置
                QHBoxLayout *iplayout = new QHBoxLayout;  // 创建水平布局
                QLabel *vmixiplabel = new QLabel("设置设备vMix服务器ip: ");
                QLineEdit *ipinput = new QLineEdit();  // ip设置框
                ipinput->setObjectName("ipinput");
                ipinput->setValidator(new QIntValidator(1, 20, this));
                ipinput->resize(200, 50);
                ipinput->setValidator(
                   new QRegularExpressionValidator
                   (
                       QRegularExpression("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b")
                   )
                );
                // 默认提示setPlaceholderText
                ipinput->setPlaceholderText(QString("192.168.111.111"));
                // 用于占位
                ipinput->setInputMask("000.000.000.000; ");
                QPushButton *ipsetbutton = new QPushButton();
                ipsetbutton->setText("发送ip配置");
                connect(ipsetbutton, SIGNAL(clicked()), this, SLOT(sendIpConfig()));  // ip按键信号槽连接
                iplayout->addWidget(vmixiplabel);  // 将标签添加到横向
                iplayout->addWidget(ipinput);  // 将输入添加到横向
                iplayout->addWidget(ipsetbutton);  // 将按键添加到横向
                layout->addLayout(iplayout);  // 将横向添加到竖向布局

                widget->setLayout(layout);  // 设定竖向布局
                ui->stackedWidget->addWidget(widget);  // 添加到界面
            }
        }
    }
}

// 自动将调试信息滑到最底
void MainWindow::on_SystemLogInfo_textChanged()
{
    ui->SystemLogInfo->moveCursor(QTextCursor::End);
}

//static bool checkMask(QString mask)
//{
//    QRegularExpression rx2("^(254|252|248|240|224|192|128|0)\\.0\\.0\\.0|255\\.(254|252|248|240|224|192|128|0)\\.0\\.0|255\\.255\\.(254|252|248|240|224|192|128|0)\\.0|255\\.255\\.255\\.(254|252|248|240|224|192|128|0)$");
//     if( !rx2.exactMatch(mask) )
//     {
//           return false;
//     }
//     return true;
// }
// 提取字符串中的ip字符串
QString getIpAddress(const QString & strText)
{
    QString strip;
    QRegularExpression ipmatch("(25[0-5]|2[0-4]\\d|[0-1]\\d{2}|[1-9]?\\d)\\.(25[0-5]|2[0-4]\\d|[0-1]\\d{2}|[1-9]?\\d)\\.(25[0-5]|2[0-4]\\d|[0-1]\\d{2}|[1-9]?\\d)\\.(25[0-5]|2[0-4]\\d|[0-1]\\d{2}|[1-9]?\\d)");
    QRegularExpressionMatch match = ipmatch.match(strText);
    if (match.hasMatch()) {
        QString str1 = match.captured(1);
        QString str2 = match.captured(2);
        QString str3 = match.captured(3);
        QString str4 = match.captured(4);
        strip = str1+"."+str2+"."+str3+"."+str4;
    }

    return strip;
}
// 向局域网所有设备发送vMix服务器地址
// 需要判断是否选择顺序配置
void MainWindow::on_VmixIpButton_clicked()
{
    proLabel->setText("正在配置");
    if(ui->checkBox->isChecked() == true) {  // 复选框被选中 按顺序配置
        uint8_t row = 0;
        while(row < (ui->listWidget->count())) {  // 列表获取
            QHostAddress ip = QHostAddress(getIpAddress(ui->listWidget->item(row)->text()));  // 获取对应行的ip
//            ui->SystemLogInfo->insertPlainText("\n"+ip);
//            ui->SystemLogInfo->insertPlainText("\n"+row);
            // 将数据发送到指定ip
            QString meg = "id"+QString ("%1").arg(row+1);
            udpServer->writeDatagram(meg.toUtf8(), ip, 2333);
            // 将服务器IP发送到指定ip

            row++;
        }
    }else {  // 复选框未选中 向局域网广播ip地址
        QString newiP = ui->VmixIpEdit->text().trimmed();
        udpServerbroadcast(newiP);  // 局域网广播
    }
    proLabel->setText("配置完成");
}
// 像局域网所有设备发送闪烁信息

// 向指定设备发送闪烁信息
void MainWindow::sendBlink() {
    int row = ui->listWidget->currentRow();  // 获取当前选中项的序
    if(row != -1) {  // 选中对象
        // 获取当前要发送的设备ip
        QLabel *label = ui->stackedWidget->widget(row)->findChild<QLabel*>("devicelabel");
        QString TargetIp= getIpAddress(label->text());
        QHostAddress ip = QHostAddress(TargetIp);

        ui->SystemLogInfo->insertPlainText("\r\n 闪烁设备IP为:"+TargetIp);
        udpServer->writeDatagram(QByteArray("Blink"), ip, 2333);
    }else {  // 错误弹窗
        QMessageBox::information(NULL, "错误", "请先选中设备再进行设置", QMessageBox::Yes, QMessageBox::Yes);
    }
}
// 向指定设备发送id信息
void MainWindow::sendIdConfig() {
    int row = ui->listWidget->currentRow();  // 获取当前选中项的序
//    ui->SystemLogInfo->insertPlainText("\n"+QString ("%1").arg (row));
    if(row != -1) {  // 选中对象
        // 获取当前要发送的设备ip
        QLabel *label = ui->stackedWidget->widget(row)->findChild<QLabel*>("devicelabel");
        QString TargetIp= getIpAddress(label->text());
        QHostAddress ip = QHostAddress(TargetIp);
        // 获取设置的id
        QLineEdit *idinput = ui->stackedWidget->widget(row)->findChild<QLineEdit*>("idinput");
        uint8_t id = idinput->text().toInt();
        // 发送id信息
        ui->SystemLogInfo->insertPlainText("\r\n 设置设备IP为:"+TargetIp+" 新序号: " +QString ("%1").arg (id));
        if(id<=0 || id>20)
            QMessageBox::information(NULL, "错误", "设备新ID无效，请检查", QMessageBox::Yes, QMessageBox::Yes);
        else {
            QString meg = "id"+QString ("%1").arg (id);
            udpServer->writeDatagram(meg.toUtf8(), ip, 2333);
        }
    }else {  // 错误弹窗
        QMessageBox::information(NULL, "错误", "请先选中设备再进行设置", QMessageBox::Yes, QMessageBox::Yes);
    }
}
// 向指定设备发送vMix服务器信息
void MainWindow::sendIpConfig() {
    int row = ui->listWidget->currentRow();  // 获取当前选中项的序
//    ui->SystemLogInfo->insertPlainText("\n"+QString ("%1").arg (row));
    if(row != -1) {  // 选中对象
        // 获取当前要发送的设备ip
        QLabel *label = ui->stackedWidget->widget(row)->findChild<QLabel*>("devicelabel");
        QString TargetIp= getIpAddress(label->text());
        QHostAddress ip = QHostAddress(TargetIp);
        // 获取设置的服务器ip
        QLineEdit *ipinput = ui->stackedWidget->widget(row)->findChild<QLineEdit*>("ipinput");
        QString newiP = ipinput->text().trimmed();
        // 发送id信息
        ui->SystemLogInfo->insertPlainText("\r\n 设置设备IP为:"+TargetIp+" 新服务器IP: " +newiP);
        QHostAddress _Address;//借助QHostAddress判断输入的内容的格式是不是IP
        bool _isAddr= _Address.setAddress(newiP.trimmed()); //timmed是去掉空格
        _isAddr = 0;
        if(_isAddr)
            QMessageBox::information(NULL, "错误", "设备新服务器IP无效，请检查", QMessageBox::Yes, QMessageBox::Yes);
        else {
            QString meg = "ip"+newiP;
//            ui->SystemLogInfo->insertPlainText("\r\n 消息格式为" +meg);
            udpServer->writeDatagram(meg.toUtf8(), ip, 2333);
        }
    }else {  // 错误弹窗
        QMessageBox::information(NULL, "错误", "请先选中设备再进行设置", QMessageBox::Yes, QMessageBox::Yes);
    }
}
