#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QString>
#include <QTimer>
#include <QPainter>
#include <QListWidgetItem>
#include "myhighter.h"
#include "form.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    void shre_window_close(QMap<QString,QString> map);
private slots:
    void item_clicked(QListWidgetItem *item);

    void on_btnSwitch_clicked();

    void serialPortRead_Slot();

    void on_btnSend_clicked();

    void on_btnClearRec_clicked();

    void on_btnClearSend_clicked();

    void on_chkRec_stateChanged(int arg1);

    void on_chkSend_stateChanged(int arg1);

    void on_chkTimSend_stateChanged(int arg1);

    void on_insert_clicked();

    void on_write_clicked();

    void on_double_clicked(QListWidgetItem *item);

    void on_pushButton_clicked();


    void on_share_clicked();
signals:
    void map_list(QMap<QString,QString> map);

private:

    Ui::MainWindow *ui;
    QMap<QString,QString> map;
    void mouse_tip(QListWidgetItem *item);
    void file_open_fail();
    QSerialPort *mySerialPort;
    Myhighter *myhighter;
    // 发送、接收字节计数
    long sendNum, recvNum;
    QLabel *lblSendNum;
    QLabel *lblRecvNum;
    void dictionary_worker();
    void save_dictionary();
    void setNumOnLabel(QLabel *lbl, QString strS, long num);
    QListWidgetItem *item = nullptr;
    // 定时发送-定时器
    form *dialog_form = nullptr;
    QTimer *timSend;
    QString txt = "这是AT_TOOL的配置文件,可使用AT_tool配置,当然也按照规范修改此文件，如果出现错误，软件会用默认配置自动覆盖掉此文件*******************{AT+CWMODE=1@AT+CWMODE=<mode>[,<auto_connect>]<mode>：模式0: 无 Wi-Fi 模式，并且关闭 Wi-Fi RF1: Station 模式2: SoftAP 模式3: SoftAP+Station 模式 <auto_connect>：切换 ESP32 设备的 Wi-Fi 模式时（例如，从 SoftAP 或无 Wi-Fi 模式切换为 Station 模式或 SoftAP+Station 模式），是否启用自动连接 AP 的功能，默认值：1。参数缺省时，使用默认值，也就是能自动连接。0: 禁用自动连接 AP 的功能1: 启用自动连接 AP 的功能，若之前已经将自动连接 AP 的配置保存到 flash 中，则 ESP32 设备将自动连接 AP;AT+GMR@查看版本信息;AT+CWLAP@查询与 ESP32 Station 连接的 AP 信息;AT+CWJAP_DEF=\"newifi_F8A0\",\"anxinke123\"@ ;AT+RST@重启模块;AT+CLDUNBIND@ ;AT+CIPSTA@查询 ESP32 Station 的 IP 地址;AT+CWLAP@扫描附近的 AP 信息;}";
};
#endif // MAINWINDOW_H
