#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QToolTip>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("AT_tool");

    // 发送、接收计数清零
    sendNum = 0;
    recvNum = 0;
    // 状态栏
    QStatusBar *sBar = statusBar();
    // 状态栏的收、发计数标签
    lblSendNum = new QLabel(this);
    lblRecvNum = new QLabel(this);
    // 设置标签最小大小
    lblSendNum->setMinimumSize(100, 20);
    lblRecvNum->setMinimumSize(100, 20);

    // 状态栏显示计数值
    setNumOnLabel(lblSendNum, "S: ", sendNum);
    setNumOnLabel(lblRecvNum, "R: ", recvNum);
    // 从右往左依次添加
    sBar->addPermanentWidget(lblSendNum);
    sBar->addPermanentWidget(lblRecvNum);
    myhighter = new Myhighter(ui->txtRec->document());

    // 定时发送-定时器
    timSend = new QTimer;
    timSend->setInterval(1000);// 设置默认定时时长1000ms
    connect(timSend, &QTimer::timeout, this, [=](){
        on_btnSend_clicked();
    });

    // 串口
    mySerialPort = new QSerialPort(this);
    connect(mySerialPort, SIGNAL(readyRead()), this, SLOT(serialPortRead_Slot()));
    //ui
    connect(ui->listWidget,&QListWidget::itemClicked,this,&MainWindow::item_clicked);
    connect(ui->write,&QPushButton::clicked,this,&MainWindow::on_write_clicked);
    connect(ui->insert,&QPushButton::clicked,this,&MainWindow::on_insert_clicked);
    connect(ui->listWidget,&QListWidget::itemDoubleClicked,this,&MainWindow::on_double_clicked);
    connect(ui->pushButton,&QPushButton::clicked,this,&MainWindow::on_pushButton_clicked);
    connect(ui->listWidget,&QListWidget::itemEntered,this,&MainWindow::mouse_tip);
    connect(ui->share,&QPushButton::clicked,this,&MainWindow::on_share_clicked);
    ui->listWidget->setMouseTracking(true);
    //file worker
    dictionary_worker();
    dialog_form = new form(this);

}

MainWindow::~MainWindow()
{
    save_dictionary();
    delete ui;
}

void MainWindow::shre_window_close(QMap<QString, QString> map)
{
    this->map = map;
    ui->listWidget->clear();
    QMap<QString,QString>::const_iterator iterator = map.constBegin();
    while (iterator != map.constEnd()) {
        ui->listWidget->addItem(iterator.key());
        iterator++;
    }
}


void MainWindow::item_clicked(QListWidgetItem *item)
{
    QString string = item->text();
    ui->textEdit->clear();
    ui->textEdit->insertPlainText(map.value(string));
    ui->lineEdit->setText(string);
    this->item = item;

}


// 串口接收显示，槽函数
void MainWindow::serialPortRead_Slot()
{
    /*QString recBuf;
    recBuf = QString(mySerialPort->readAll());*/

    QByteArray recBuf;
    recBuf = mySerialPort->readAll();

    // 接收字节计数
    recvNum += recBuf.size();
    // 状态栏显示计数值
    setNumOnLabel(lblRecvNum, "R: ", recvNum);

    // 判断是否为16进制接收，将以后接收的数据全部转换为16进制显示（先前接收的部分在多选框槽函数中进行转换。最好多选框和接收区组成一个自定义控件，方便以后调用）
    if(ui->chkRec->checkState() == false){

        // 在当前位置插入文本，不会发生换行。如果没有移动光标到文件结尾，会导致文件超出当前界面显示范围，界面也不会向下滚动。
        ui->txtRec->insertPlainText(recBuf);
    }else{
        // 16进制显示，并转换为大写
        QString str1 = recBuf.toHex().toUpper();//.data();
        // 添加空格
        QString str2;
        for(int i = 0; i<str1.length (); i+=2)
        {
            str2 += str1.mid (i,2);
            str2 += " ";
        }
        ui->txtRec->insertPlainText(str2);
        //ui->txtRec->insertPlainText(recBuf.toHex());
    }

    // 移动光标到文本结尾
    ui->txtRec->moveCursor(QTextCursor::End);
}

// 打开/关闭串口 槽函数
void MainWindow::on_btnSwitch_clicked()
{

    QSerialPort::BaudRate baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::StopBits stopBits;
    QSerialPort::Parity   checkBits;

    // 获取串口波特率
    // 有没有直接字符串转换为 int的方法？？？
    //baudRate = ui->cmbBaudRate->currentText().toInt();
    if(ui->cmbBaudRate->currentText() == "9600"){
        baudRate = QSerialPort::Baud9600;
    }else if(ui->cmbBaudRate->currentText() == "38400"){
        baudRate = QSerialPort::Baud38400;
    }else if(ui->cmbBaudRate->currentText() == "115200"){
        baudRate = QSerialPort::Baud115200;
    }else{

    }

    // 获取串口数据位
    if(ui->cmbData->currentText() == "5"){
        dataBits = QSerialPort::Data5;
    }else if(ui->cmbData->currentText() == "6"){
        dataBits = QSerialPort::Data6;
    }else if(ui->cmbData->currentText() == "7"){
        dataBits = QSerialPort::Data7;
    }else if(ui->cmbData->currentText() == "8"){
        dataBits = QSerialPort::Data8;
    }else{

    }

    // 获取串口停止位
    if(ui->cmbStop->currentText() == "1"){
        stopBits = QSerialPort::OneStop;
    }else if(ui->cmbStop->currentText() == "1.5"){
        stopBits = QSerialPort::OneAndHalfStop;
    }else if(ui->cmbStop->currentText() == "2"){
        stopBits = QSerialPort::TwoStop;
    }else{

    }

    // 获取串口奇偶校验位
    if(ui->cmbCheck->currentText() == "无"){
        checkBits = QSerialPort::NoParity;
    }else if(ui->cmbCheck->currentText() == "奇校验"){
        checkBits = QSerialPort::OddParity;
    }else if(ui->cmbCheck->currentText() == "偶校验"){
        checkBits = QSerialPort::EvenParity;
    }else{

    }

    // 想想用 substr strchr怎么从带有信息的字符串中提前串口号字符串
    // 初始化串口属性，设置 端口号、波特率、数据位、停止位、奇偶校验位数
    mySerialPort->setBaudRate(baudRate);
    mySerialPort->setDataBits(dataBits);
    mySerialPort->setStopBits(stopBits);
    mySerialPort->setParity(checkBits);
    //mySerialPort->setPortName(ui->cmbSerialPort->currentText());// 不匹配带有串口设备信息的文本
    // 匹配带有串口设备信息的文本
    QString spTxt = ui->cmbSerialPort->currentText();
    spTxt = spTxt.section(':', 0, 0);//spTxt.mid(0, spTxt.indexOf(":"));
    //qDebug() << spTxt;
    mySerialPort->setPortName(spTxt);

    // 根据初始化好的串口属性，打开串口
    // 如果打开成功，反转打开按钮显示和功能。打开失败，无变化，并且弹出错误对话框。
    if(ui->btnSwitch->text() == "打开串口"){
        if(mySerialPort->open(QIODevice::ReadWrite) == true){
            //QMessageBox::
            ui->btnSwitch->setText("关闭串口");
            // 让端口号下拉框不可选，避免误操作（选择功能不可用，控件背景为灰色）
            ui->cmbSerialPort->setEnabled(false);
            ui->cmbBaudRate->setEnabled(false);
            ui->cmbStop->setEnabled(false);
            ui->cmbData->setEnabled(false);
            ui->cmbCheck->setEnabled(false);
        }else{
            QMessageBox::critical(this, "错误提示", "串口打开失败！！！\r\n该串口可能被占用\r\n请选择正确的串口");
        }
    }else{
        mySerialPort->close();
        ui->btnSwitch->setText("打开串口");
        // 端口号下拉框恢复可选，避免误操作
        ui->cmbSerialPort->setEnabled(true);
        ui->cmbBaudRate->setEnabled(true);
        ui->cmbStop->setEnabled(true);
        ui->cmbData->setEnabled(true);
        ui->cmbCheck->setEnabled(true);
    }

}

// 发送按键槽函数
// 如果勾选16进制发送，按照asc2的16进制发送
void MainWindow::on_btnSend_clicked()
{
    QByteArray sendData;
    // 判断是否为16进制发送，将发送区全部的asc2转换为16进制字符串显示，发送的时候转换为16进制发送
    if(ui->chkSend->checkState() == Qt::Checked){
        // 字符串形式发送
        sendData = QByteArray::fromHex(ui->txtSend->text().toUtf8()).data();
    }
    if(ui->form_check->checkState() == Qt::Checked){
        sendData = ui->txtSend->text().append("\r\n").toUtf8();
    }
    else{
        // 16进制发送
        sendData = ui->txtSend->text().toUtf8();
    }

    // 如发送成功，会返回发送的字节长度。失败，返回-1。
    int a = mySerialPort->write(sendData);
    // 发送字节计数并显示
    if(a > 0)
    {
        // 发送字节计数
        sendNum += a;
        // 状态栏显示计数值
        setNumOnLabel(lblSendNum, "S: ", sendNum);
    }

}

// 状态栏标签显示计数值
void MainWindow::setNumOnLabel(QLabel *lbl, QString strS, long num)
{
    // 标签显示
    QString strN;
    strN.sprintf("%ld", num);
    QString str = strS + strN;
    lbl->setText(str);
}

void MainWindow::on_btnClearRec_clicked()
{
    ui->txtRec->clear();
    // 清除发送、接收字节计数
    sendNum = 0;
    recvNum = 0;
    // 状态栏显示计数值
    setNumOnLabel(lblSendNum, "S: ", sendNum);
    setNumOnLabel(lblRecvNum, "R: ", recvNum);
}

void MainWindow::on_btnClearSend_clicked()
{
    ui->txtSend->clear();
    // 清除发送字节计数
    sendNum = 0;
    // 状态栏显示计数值
    setNumOnLabel(lblSendNum, "S: ", sendNum);
}

// 先前接收的部分在多选框状态转换槽函数中进行转换。（最好多选框和接收区组成一个自定义控件，方便以后调用）
void MainWindow::on_chkRec_stateChanged(int arg1)
{
    // 获取文本字符串
    QString txtBuf = ui->txtRec->toPlainText();

    // 获取多选框状态，未选为0，选中为2
    // 为0时，多选框未被勾选，接收区先前接收的16进制数据转换为asc2字符串格式
    if(arg1 == 0){

        QByteArray str1 = QByteArray::fromHex(txtBuf.toUtf8());
        // 文本控件清屏，显示新文本
        ui->txtRec->clear();
        ui->txtRec->insertPlainText(str1);
        // 移动光标到文本结尾
        ui->txtRec->moveCursor(QTextCursor::End);

    }else{// 不为0时，多选框被勾选，接收区先前接收asc2字符串转换为16进制显示

        QByteArray str1 = txtBuf.toUtf8().toHex().toUpper();
        // 添加空格
        QByteArray str2;
        for(int i = 0; i<str1.length (); i+=2)
        {
            str2 += str1.mid (i,2);
            str2 += " ";
        }
        // 文本控件清屏，显示新文本
        ui->txtRec->clear();
        ui->txtRec->insertPlainText(str2);
        // 移动光标到文本结尾
        ui->txtRec->moveCursor(QTextCursor::End);

    }
}

// 先前发送区的部分在多选框状态转换槽函数中进行转换。（最好多选框和发送区组成一个自定义控件，方便以后调用）
void MainWindow::on_chkSend_stateChanged(int arg1)
{
    // 获取文本字符串
    QString txtBuf = ui->txtSend->text();

    // 获取多选框状态，未选为0，选中为2
    // 为0时，多选框未被勾选，将先前的发送区的16进制字符串转换为asc2字符串
    if(arg1 == 0){

        QByteArray str1 = QByteArray::fromHex(txtBuf.toUtf8());
        // 文本控件清屏，显示新文本
        ui->txtSend->clear();
        ui->txtSend->insert(str1);

    }else{// 多选框被勾选，将先前的发送区的asc2字符串转换为16进制字符串

        QByteArray str1 = txtBuf.toUtf8().toHex().toUpper();
        // 添加空格
        QByteArray str2;
        for(int i = 0; i<str1.length (); i+=2)
        {
            str2 += str1.mid (i,2);
            str2 += " ";
        }
        // 文本控件清屏，显示新文本
        ui->txtSend->clear();
        ui->txtSend->insert(str2);

    }
}

// 定时发送开关 选择复选框
void MainWindow::on_chkTimSend_stateChanged(int arg1)
{
    // 获取复选框状态，未选为0，选中为2
    if(arg1 == 0){
        timSend->stop();
        // 时间输入框恢复可选
        ui->txtSendMs->setEnabled(true);
    }else{
        // 对输入的值做限幅，小于10ms会弹出对话框提示
        if(ui->txtSendMs->text().toInt() >= 10){
            timSend->start(ui->txtSendMs->text().toInt());// 设置定时时长，重新计数
            // 让时间输入框不可选，避免误操作（输入功能不可用，控件背景为灰色）
            ui->txtSendMs->setEnabled(false);
        }else{
            ui->chkTimSend->setCheckState(Qt::Unchecked);
            QMessageBox::critical(this, "错误提示", "定时发送的最小间隔为 10ms\r\n请确保输入的值 >=10");
        }
    }
}

void MainWindow::on_insert_clicked()
{
    if(ui->lineEdit->text().isEmpty())return;
    QString string = ui->lineEdit->text();
    map.insert(string,ui->textEdit->toPlainText());
    ui->listWidget->addItem(string);
    ui->lineEdit->clear();
    ui->textEdit->clear();
}


void MainWindow::on_write_clicked()
{
    if(this->item == nullptr)return;
    map.remove(this->item->text());
    QString string = ui->lineEdit->text();
    map.insert(string,ui->textEdit->toPlainText());
    this->item->setText(string);
    ui->listWidget->editItem(this->item);
    this->item = nullptr;
    ui->lineEdit->clear();
    ui->textEdit->clear();
}
void MainWindow::on_double_clicked(QListWidgetItem *item)
{
    QString string = item->text();
   //  string.append("\r\n");
    ui->txtSend->clear();
    ui->txtSend->insert(string);
}


void MainWindow::on_pushButton_clicked()
{
    if(this->item == nullptr)return;
    map.remove(this->item->text());
    delete ui->listWidget->currentItem();
    this->item = nullptr;
    ui->lineEdit->clear();
    ui->textEdit->clear();

}

void MainWindow::mouse_tip(QListWidgetItem *item)
{
    QToolTip::showText(QCursor::pos(),map.value(item->text()));
}

void MainWindow::file_open_fail()
{
    QFile file("./dictionary.txt");
    ui->txtRec->insertPlainText("file open fail,software use defalut dictionary");
    if(file.open(QIODevice::WriteOnly|QIODevice::Text)){
        QTextStream out(&file);
        out<<this->txt;
        file.close();
    }

}



void MainWindow::dictionary_worker()
{
    QFile file("./dictionary.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        file.close();
        file_open_fail();
        file.open(QIODevice::ReadOnly|QIODevice::Text);
    }
    QTextStream in(&file);
    QString step0 = in.readAll();
    if((!step0.split('{').last().contains('}'))||step0.split('{').count()==1){
        QMessageBox::critical(new QWidget(),"意外","dictionary.txt文件读取意外,软件使用默认dictionary.txt\n如果软件仍无法修复此问题，联系开发人员\n错误于：{和}错误");
        file.close();
        file_open_fail();
        file.open(QIODevice::ReadOnly|QIODevice::Text);
        QTextStream in(&file);
        step0 = in.readAll();
    }
    QString string = step0.split('{').at(1).split('}').at(0);

        QStringList stringlist = string.split(';');
        QMap<QString,QString> map;
        for (int i = 0; i < stringlist.count()-1; ++i) {
            if(stringlist[i].isEmpty()||stringlist[i].isNull())continue;
            QStringList list = stringlist[i].split('@');
            if(list.count()==1||list.first().isEmpty()||list.last().isNull())continue;
            map.insert(list.first().remove('\n').remove(' '),list.last());
        }
        file.close();
        QMap<QString,QString>::const_iterator iterator = map.constBegin();
        while (iterator != map.constEnd()) {
            ui->listWidget->addItem(iterator.key());
            qDebug()<<iterator.key()<<':'<<iterator.value();
            iterator++;
        }
        this->map = map;



}

void MainWindow::save_dictionary()
{
    QFile file("./dictionary.txt");
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text)){
        QMessageBox::critical(this,"错误","DICTIONARY保存失败");
    }
    QTextStream out(&file);
    QString string;
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        if(ui->listWidget->item(i)==nullptr)continue;
        string.append(ui->listWidget->item(i)->text()+'@'+map.value(ui->listWidget->item(i)->text())+";\n");
    }
    string.insert(0,"这是AT_TOOL的配置文件,可使用AT_tool配置,当然也按照规范修改此文件，如果出现错误，软件会用默认配置自动覆盖掉此文件\n *******************\n{")
        .insert(string.count()-1,'}');
    out<<string;
    file.close();
}

void MainWindow::on_share_clicked()
{
    dialog_form->show();
    dialog_form->setEnabled(true);
    connect(this,&MainWindow::map_list,dialog_form,&form::listwidget);
    connect(dialog_form,&form::list_map,this,&MainWindow::shre_window_close);
    emit map_list(this->map);
}

