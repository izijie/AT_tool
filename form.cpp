#include "form.h"
#include "ui_form.h"
#include <QMessageBox>
#include <qtooltip.h>
form::form(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::form)
{
    setWindowTitle("导出与分享");
    ui->setupUi(this);
    ui->listWidget->setMouseTracking(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    connect(ui->clear,&QPushButton::clicked,this,&form::on_clear_clicked);
    connect(ui->read,&QPushButton::clicked,this,&form::on_read_clicked);
    connect(ui->make,&QPushButton::clicked,this,&form::on_make_clicked);
    connect(ui->remove,&QPushButton::clicked,this,&form::on_remove_clicked);
    connect(ui->listWidget,&QListWidget::itemClicked,this,&form::item_click);
    connect(ui->save,&QPushButton::clicked,this,&form::on_save_clicked);
    connect(ui->listWidget,&QListWidget::itemEntered,this,&form::mouse_tip);

}

form::~form()
{
    delete ui;
}


void form::item_click(QListWidgetItem *item)
{
    this->item = item;
}

void form::listwidget(QMap<QString,QString> map)
{
    ui->listWidget->clear();
    ui->plainTextEdit->clear();
    this->map = map;
    QMap<QString,QString>::const_iterator iterator = map.constBegin();
    while (iterator != map.constEnd()) {
        ui->listWidget->addItem(iterator.key());
        iterator++;
    }
}



void form::on_make_clicked()
{

    QString string;
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        if(ui->listWidget->item(i)==nullptr)continue;
        string.append(ui->listWidget->item(i)->text()+'@'+map.value(ui->listWidget->item(i)->text())+";\n");
    }
    string.insert(0,"{").insert(string.count()-1,'}');
    ui->plainTextEdit->clear();
    ui->plainTextEdit->insertPlainText(string);
}


void form::on_read_clicked()
{
    int num = 0;
    QString step0 = ui->plainTextEdit->toPlainText();
    if((!step0.split('{').last().contains('}'))||step0.split('{').count()==1){
        QMessageBox::information(this,"警告","格式错误");
    }
    QString string = step0.split('{').at(1).split('}').at(0);

    QStringList stringlist = string.split(';');
    for (int i = 0; i < stringlist.count()-1; ++i) {
        if(stringlist[i].isEmpty()||stringlist[i].isNull()){num++;continue;};
        QStringList list = stringlist[i].split('@');
        if(list.count()==1||list.first().isEmpty()||list.last().isNull()){num++;continue;};
        map.insert(list.first().remove('\n').remove(' '),list.last());
    }
    ui->listWidget->clear();
    QMap<QString,QString>::const_iterator iterator = map.constBegin();
    while (iterator != map.constEnd()) {
        ui->listWidget->addItem(iterator.key());
        iterator++;
    }
    if(num != 0){

        QMessageBox::information(this,"警告",QString::asprintf("此次导入有%d处没有被识别到",num));

    }

}


void form::on_clear_clicked()
{
    ui->plainTextEdit->clear();
}


void form::on_remove_clicked()
{
    if(this->item == nullptr)return;
    map.remove(ui->listWidget->currentItem()->text());
    delete ui->listWidget->currentItem();
    this->item = nullptr;
}


void form::on_save_clicked()
{
    emit list_map(this->map);
}

void form::mouse_tip(QListWidgetItem *item)
{
    QToolTip::showText(QCursor::pos(),map.value(item->text()));

}

