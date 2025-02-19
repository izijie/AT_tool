#ifndef FORM_H
#define FORM_H

#include <QDialog>
#include <QMap>
#include<QListWidgetItem>
namespace Ui {
class form;
}

class form : public QDialog
{
    Q_OBJECT

public:
    explicit form(QWidget *parent = nullptr);
    void listwidget(QMap<QString,QString> map);
    ~form();
private slots:

    void item_click(QListWidgetItem *item);

    void on_make_clicked();

    void on_read_clicked();

    void on_clear_clicked();

    void on_remove_clicked();

    void on_save_clicked();

signals:
    void list_map(QMap<QString,QString> map);

private:
    QMap<QString,QString> map;
    QListWidgetItem *item = nullptr;
    Ui::form *ui;
    void mouse_tip(QListWidgetItem *item);
};

#endif // FORM_H
