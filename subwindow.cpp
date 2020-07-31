#include "subwindow.h"
#include "ui_subwindow.h"

#include <QtGlobal>

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariantList>
#include <QSqlTableModel>
#include <QSqlRecord>

#include <QDir>
#include <QFileDialog>
#include <QTextCodec>
subWindow::subWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::subWindow)
{
    ui->setupUi(this);

}

subWindow::~subWindow()
{
    delete ui;
}

/*********
 数据库初始化
 *********/
void subWindow::modelInit()
{
    model->setTable("LOAD_TABLE");
    //添加模型
    ui->tableView->setModel(model);
    //显示数据
    model->select();
    //将header修改为其他（修改的是UI里面的Tabel不影响Database）
    model->setHeaderData(0, Qt::Horizontal, "序号");
    model->setHeaderData(1, Qt::Horizontal, "设备名称");
    model->setHeaderData(2, Qt::Horizontal, "数量");
    model->setHeaderData(3, Qt::Horizontal, "最大轴功率");
    model->setHeaderData(4, Qt::Horizontal, "额定功率");
    model->setHeaderData(5, Qt::Horizontal, "效率");
    model->setHeaderData(6, Qt::Horizontal, "功率因数");
    model->setHeaderData(7, Qt::Horizontal, "电动机利用系数");
    model->setHeaderData(8, Qt::Horizontal, "机械负荷系数");
    model->setHeaderData(9, Qt::Horizontal, "电动机负荷系数");
    model->setHeaderData(10, Qt::Horizontal, "同时使用系数");
    model->setHeaderData(11, Qt::Horizontal, "有功功率");
    model->setHeaderData(12, Qt::Horizontal, "无功功率");
}

void subWindow::on_pushButton_clicked()
{
    //获取空记录
    QSqlRecord record = model->record();
    //获取行号
    int row = model->rowCount() ;
    model->insertRecord(row, record);
}

void subWindow::on_pushButton_2_clicked()
{

    //获取选中的模型
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    //取出模型中的索引
    QModelIndexList list =  selectionModel->selectedRows();
    for (int i = 0; i < list.size(); i++) {
        model->removeRow(list.at(i).row());
    }
}

void subWindow::on_pushButton_3_clicked()
{

    model->submitAll();
}

void subWindow::on_pushButton_4_clicked()
{

    model->revertAll();
    model->submitAll();
}

void subWindow::on_pushButton_5_clicked()
{

    QString name = ui->lineEdit_looking->text();
    QString str;
    if (name == NULL){
        modelInit();
    }
    else{

        str = QString("name = '%1'").arg(name);
        model->setFilter(str);
        model->select();
    }
}
