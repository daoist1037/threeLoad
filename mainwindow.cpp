#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtGlobal>
#include <QDebug>
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
#include <QAxObject>
#include <QMap>

//工况状态
static int iLoadStatus;

//参数表
static int id;                      //序号
static QString name;                //名称
static int nums;                    //数量
static qreal maxPower;              //最大轴功率
static qreal ratedPower;            //额定功率
static qreal efficiency;            //效率
static qreal powerFactor;           //功率因数
static qreal motorUtilizeFactor;    //电动机利用系数 k1
static qreal mecLoadFactor;         //机械负荷系数 k2
static qreal motorLoadFactor;       //电动机负荷系数 k3
static qreal stimultaneous;         //同时利用系数 k0
static qreal activePower;           //有功功率
static qreal reactivePower;         //无功功率


static QMap<int, QString> map;
static QMap<QString, int> sqlMap;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("stu.db");

    if (!db.open())
    {
        QMessageBox::warning(this, "错误", db.lastError().text());
        return;
    }
    //设置模型，使用students表
    model = new QSqlTableModel(this);

    model_total = new QSqlTableModel(this);
    model_total->setTable("TOTAL");
    //ui->tableView_2->setModel(model_total);
    model_total->select();

    modelInit();


    //设置model编辑模式为手动提交
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //禁止ui界面点击表格修改
    //ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    map.insert(0, "A");
    map.insert(1, "B");
    map.insert(2, "C");
    map.insert(3, "D");
    map.insert(4, "E");
    map.insert(5, "F");
    map.insert(6, "G");
    map.insert(7, "H");
    map.insert(8, "I");
    map.insert(9, "J");
    map.insert(10, "K");
    map.insert(11, "L");
    map.insert(12, "M");
    map.insert(13, "N");
    map.insert(14, "O");
    map.insert(15, "P");
    map.insert(16, "Q");
    map.insert(17, "R");
    map.insert(18, "S");
    map.insert(19, "T");
    map.insert(20, "U");
    map.insert(21, "V");
    map.insert(22, "W");
    map.insert(23, "X");
    map.insert(24, "Y");
    map.insert(25, "Z");
    map.insert(26, "AA");
    map.insert(27, "AB");
    map.insert(28, "AC");
    map.insert(29, "AD");
    map.insert(30, "AE");
    map.insert(31, "AF");
    map.insert(32, "AG");
    map.insert(33, "AH");
    map.insert(34, "AI");
    map.insert(35, "AJ");
    map.insert(36, "AK");
    map.insert(37, "AL");
}

MainWindow::~MainWindow()
{
    delete ui;
}


/*********
 数据库初始化
 *********/
void MainWindow::modelInit()
{
    if (iLoadStatus != 0)
    {
        switch (iLoadStatus) {
        case 1:
            model->setTable("LOAD_FLY");
            break;
        case 2:
            model->setTable("LOAD_ANCHOR");
            break;
        case 3:
            model->setTable("LOAD_PARK");
            break;
        case 4:
            model->setTable("LOAD_LOAD");
            break;
        case 5:
            model->setTable("LOAD_EMERGENCY");
            break;
        }
    }
    else{
        return;
    }
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
    model->setHeaderData(13, Qt::Horizontal, "负荷类别");
}


/*********
 负荷总功率计算 选中按钮
 *********/
void MainWindow::on_radioButton_clicked()
{
    ui->groupBox_95->setEnabled(true);
    ui->pushButton_active_power->setEnabled(true);
    ui->groupBox_1->setEnabled(true);
    ui->groupBox_2->setEnabled(true);
    ui->groupBox_3->setEnabled(true);
}


/*********
 负荷总功率计算（考虑同时系数） 选中按钮
 *********/
void MainWindow::on_radioButton_2_clicked()
{
        ui->groupBox->setEnabled(true);
        ui->pushButton->setEnabled(true);
        ui->groupBox_96->setEnabled(true);
        ui->groupBox_97->setEnabled(true);
        ui->groupBox_98->setEnabled(true);
        ui->groupBox_99->setEnabled(true);
        ui->groupBox_100->setEnabled(true);
}



/*********
 计算功率
 *********/
void MainWindow::on_pushButton_active_power_clicked()
{

    qreal totalActivatePower_1 = 0;    //有功功率之和
    qreal totalReactivatePower_1 = 0;  //无功功率之和
    qreal totalActivatePower_2 = 0;    //有功功率之和
    qreal totalReactivatePower_2 = 0;  //无功功率之和
    qreal totalActivatePower_3 = 0;    //有功功率之和
    qreal totalReactivatePower_3 = 0;  //无功功率之和

    int loadCategory;                //负荷类别
    QString str;

    QModelIndex index;
    qreal totalPower;
    for (int i = 0; i < model->rowCount(); i++)
    {
       //遍历求每一行的有功和无功
            index = model->index(i,0);
            id = model->data(index).toInt();
            index = model->index(i,1);
            name = model->data(index).toString();
            index = model->index(i,2);
            nums = model->data(index).toInt();
            index = model->index(i,3);
            maxPower = model->data(index).toDouble();
            index = model->index(i,4);
            ratedPower = model->data(index).toDouble();
            index = model->index(i,5);
            efficiency = model->data(index).toDouble();
            index = model->index(i,6);
            powerFactor = model->data(index).toDouble();
            index = model->index(i,7);
            motorUtilizeFactor = model->data(index).toDouble();
            index = model->index(i,8);
            mecLoadFactor = model->data(index).toDouble();
            index = model->index(i,9);
            motorLoadFactor = model->data(index).toDouble();
            index = model->index(i,10);
            stimultaneous = model->data(index).toDouble();
            totalPower = (ratedPower / efficiency) * nums * 100;


            activePower = totalPower * stimultaneous * motorUtilizeFactor * mecLoadFactor;
            index = model->index(i,11);
            model->setData(index, activePower);
            if (fabs(powerFactor) < 1e-15){
                ;
            }
            else
            {
                reactivePower = totalPower * tan(acos(powerFactor));
                index = model->index(i,12);
                model->setData(index, reactivePower);
            }

            index = model->index(i,13);
            loadCategory = model->data(index).toInt();
            if (loadCategory == 1)
            {
                totalActivatePower_1 += activePower;
                totalReactivatePower_1 += reactivePower;
            }
            else if (loadCategory == 2)
            {
                totalActivatePower_2 += activePower;
                totalReactivatePower_2 += reactivePower;
            }
            else if (loadCategory == 3)
            {
                totalActivatePower_3 += activePower;
                totalReactivatePower_3 += reactivePower;
            }

    }

    index = model_total->index(iLoadStatus - 1, 1);
    model_total->setData(index, totalActivatePower_1);
    str = QString::number(totalActivatePower_1, 'f', 2);
    ui->lineEdit_2->setText(str);
    str = QString::number(totalReactivatePower_1);
    ui->lineEdit_3->setText(str);

    index = model_total->index(iLoadStatus - 1, 2);
    model_total->setData(index, totalActivatePower_2);
    str = QString::number(totalActivatePower_2);
    ui->lineEdit_4->setText(str);
    str = QString::number(totalReactivatePower_2);
    ui->lineEdit_5->setText(str);

    index = model_total->index(iLoadStatus - 1, 3);
    model_total->setData(index, totalActivatePower_3);
    str = QString::number(totalActivatePower_3);
    ui->lineEdit_6->setText(str);
    str = QString::number(totalReactivatePower_3);
    ui->lineEdit_7->setText(str);

    model->submitAll();
    model_total->submitAll();

    ui->radioButton_2->setEnabled(true);
}

/*********
 一类负荷总功率（同时系数）
 *********/
void MainWindow::on_pushButton_clicked()
{
    QString str;
    qreal k1;
    qreal k2;
    QModelIndex index;
    qreal temp;
    qreal P1;
    qreal Q1;
    qreal P2;
    qreal Q2;
    qreal P;
    qreal Q;

    k1 = ui->lineEdit_8->text().toDouble();
    k2 = ui->lineEdit_9->text().toDouble();

    index = model_total->index(iLoadStatus - 1, 4);
    model_total->setData(index, k1);
    index = model_total->index(iLoadStatus - 1, 5);
    model_total->setData(index, k2);

    //
    temp = ui->lineEdit_2->text().toDouble();
    P1 = temp * k1;
    index = model_total->index(iLoadStatus - 1, 6);
    model_total->setData(index, P1);
    str = QString::number(P1);
    ui->lineEdit_10->setText(str);

    temp = ui->lineEdit_3->text().toDouble();
    Q1 = temp * k2;
    str = QString::number(Q1);
    ui->lineEdit_11->setText(str);

    //
    temp = ui->lineEdit_4->text().toDouble();
    P2 = temp * k1;
    index = model_total->index(iLoadStatus - 1, 7);
    model_total->setData(index, P2);
    str = QString::number(P2);
    ui->lineEdit_12->setText(str);

    temp = ui->lineEdit_5->text().toDouble();
    Q2 = temp * k2;
    str = QString::number(Q2);
    ui->lineEdit_13->setText(str);

    //
    P = P1 + P2;
    index = model_total->index(iLoadStatus - 1, 8);
    model_total->setData(index, P);
    str = QString::number(P);
    ui->lineEdit_14->setText(str);

    Q = Q1 + Q2;
    str = QString::number(Q);
    ui->lineEdit_15->setText(str);

    //
    P = P * 1.05;
    index = model_total->index(iLoadStatus - 1, 9);
    model_total->setData(index, P);
    str = QString::number(P);
    ui->lineEdit_22->setText(str);

    Q = Q * 1.05;
    str = QString::number(Q);
    ui->lineEdit_23->setText(str);

    model_total->submitAll();

    if(P > 112.5)
    {
        index = model_total->index(iLoadStatus - 1, 10);
        model_total->setData(index, "2 x 90");
        index = model_total->index(iLoadStatus - 1, 11);
        model_total->setData(index, "1 x 90");
        index = model_total->index(iLoadStatus - 1, 12);
        model_total->setData(index, P / 180);
    }
    else{
        index = model_total->index(iLoadStatus - 1, 10);
        model_total->setData(index, "1 x 90");
        index = model_total->index(iLoadStatus - 1, 11);
        model_total->setData(index, "2 x 90");
        index = model_total->index(iLoadStatus - 1, 12);
        model_total->setData(index, P / 90);
    }

}


/*********
 数据库增加记录
 *********/
void MainWindow::on_pushButton_add_clicked()
{
    //获取空记录
    QSqlRecord record = model->record();
    //获取行号
    int row = model->rowCount() ;
    model->insertRecord(row, record);
}


/*********
 数据库确认并提交更改
 *********/
void MainWindow::on_pushButton_admin_clicked()
{
    model->submitAll();
//    model_total->submitAll();
}


/*********
 数据库取消更改
 *********/
void MainWindow::on_pushButton_cancel_clicked()
{
    model->revertAll();
    model->submitAll();
}


/*********
 数据库删除记录
 *********/
void MainWindow::on_pushButton_delete_clicked()
{
    //获取选中的模型
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();
    //取出模型中的索引
    QModelIndexList list =  selectionModel->selectedRows();
    for (int i = 0; i < list.size(); i++) {
        model->removeRow(list.at(i).row());
    }
}


/*********
 数据库查找
 *********/
void MainWindow::on_pushButton_looking_clicked()
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


/*********
 负荷表CSV的导出——文件另存对话框
 *********/
void MainWindow::on_pushButton_csv_clicked()
{
    QString curPath = QDir::currentPath() + "/output/";
    QString dlgTitle="另存为一个文件"; //对话框标题
    QString filter="Excel文件(*.xls);;csv文件(*.csv);;文本文件(*.txt);;所有文件(*.*)"; //文件过滤器
    QString aFileName=QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);
    if (aFileName.isEmpty())
            return;
    if(saveTextByIODevice(aFileName))
    {
        QMessageBox::information(NULL, "END", "导出成功");
    }
    else
    {
        QMessageBox::information(NULL, "END", "导出失败");
    }
}

/*********
 负荷表excel的导出——写入excel文件
 *********/
bool MainWindow::saveTextByIODevice(const QString &aFileName)
{
    sqlMap.clear();
    int currentRows = 4;
    int writeRow;
    int writeColumn;

    QString str;
    QString maplStr;
    QString templateFileName = QDir::currentPath() + "./template/template.xls";
    QFile::copy(templateFileName, aFileName);

    excel = NULL;
    workbooks = NULL;
    workbook = NULL;
    excel = new QAxObject(this);
    excel -> setControl("Excel.Application");
    if (!excel)
    {
        QMessageBox:: critical(NULL, "错误信息", "EXCEL对象丢失");
        return false;
    }

    excel -> dynamicCall("SetVisible(bool)", false);
    workbooks = excel -> querySubObject("WorkBooks");
    workbook = workbooks -> querySubObject("Open(const QString&)", aFileName);
    worksheet = workbook -> querySubObject("WorkSheets(int)", 1); // 获取第一个工作sheet

    QAxObject *cell ;
    cell = worksheet->querySubObject("Range(QVariant, QVariant)", "A1");
    cell->setProperty("Value", ui->lineEdit->text());
    delete  cell;

    model->setTable("LOAD_FLY");
    model->select();
    writeColumn = 8;
    currentRows = excelWrite(writeRow,  writeColumn,  currentRows);
    QModelIndex index;
    index = model->index(0, 5);

    model->setTable("LOAD_ANCHOR");
    model->select();
    writeColumn = 14;
    currentRows = excelWrite(writeRow,  writeColumn,  currentRows);

    model->setTable("LOAD_PARK");
    model->select();
    writeColumn = 20;
    currentRows = excelWrite(writeRow,  writeColumn,  currentRows);

    model->setTable("LOAD_LOAD");
    model->select();
    writeColumn = 26;
    currentRows = excelWrite(writeRow,  writeColumn,  currentRows);

    model->setTable("LOAD_EMERGENCY");
    model->select();
    writeColumn = 32;
    currentRows = excelWrite(writeRow,  writeColumn,  currentRows);

    QMap<int, QString> newText;
    newText.insert(1, "I类负荷总功率/kW");
    newText.insert(2, "Ⅱ类负荷总功率/kW");
    newText.insert(3, "Ⅲ类负荷总功率/kW");
    newText.insert(4, "I类负荷考虑总同时系数K0I");
    newText.insert(5, "Ⅱ类负荷考虑总同时系数K0Ⅱ");
    newText.insert(6, "I类负荷考虑总同时系数时总功率/kW");
    newText.insert(7, "Ⅱ类负荷考虑总同时系数时总功率/kW");
    newText.insert(8, "考虑总同时系数后I、Ⅱ类总功率之和/kW");
    newText.insert(9, "考虑网络损失5%后所需总功率/kW");
    newText.insert(10, "运行发电机/(台数×kW)");
    newText.insert(11, "备用发电机/(台数×kW)");
    newText.insert(12, "发电机的负荷率/%");
    for(int i = 1; i <= 12; i++)
    {
        maplStr = "A" + QString::number(currentRows + i);
        cell  = worksheet->querySubObject("Range(QVariant, QVariant)", maplStr);
        cell->setProperty("Value", newText[i]);
        str = "H" + QString::number(currentRows + i);
        cell = worksheet->querySubObject("Range(QVariant, QVariant)", maplStr, str);
        cell->setProperty("MergeCells", true);
        delete  cell;
    }

    totalWrite( "I", "N", currentRows, 0);
    totalWrite( "O", "T", currentRows, 1);
    totalWrite("U", "Z", currentRows, 2);
    totalWrite( "AA", "AF", currentRows, 3);
    totalWrite("AG", "AL", currentRows, 4);

    workbook->dynamicCall("Save()", true);
    //关闭文件
    workbook->dynamicCall("Close(Boolean)", true);
    excel->dynamicCall("Quit()");
    delete excel;
    excel = NULL;
    return true;
}

void MainWindow::totalWrite( QString str_form, QString str_later, int currentRows, int currentTotal)
{
    QString maplStr;
    QString str;
    QString value;
    QModelIndex index;
    QAxObject *cell;
    for(int i = 1; i < model_total->columnCount(); i++)
    {
        currentRows++;
        maplStr = str_form + QString::number(currentRows);
        cell = worksheet->querySubObject("Range(QVariant, QVariant)", maplStr);
        index =  model_total->index(currentTotal,i);
        value = model_total->data(index).toString();
        cell->setProperty("Value", value);

        str = str_later+ QString::number(currentRows);
        cell = worksheet->querySubObject("Range(QVariant, QVariant)", maplStr, str);
        cell->setProperty("MergeCells", true);
        delete  cell;
    }
}

int MainWindow::excelWrite(int writeRow, int writeColumn, int currentRows)
{
    QModelIndex index;
    QString str;
    QString maplStr;
    QAxObject *cell;
    for(int i = 0; i < model->rowCount(); i++)
    {
        index = model->index(i,1);
        str = model->data(index).toString();
        if (sqlMap.contains(str))
        {
            writeRow = sqlMap[str];

            for (int j = 8; j < model->columnCount(); j++)
            {
                str = QString::number(writeRow);
                maplStr = map[writeColumn + j - 8] + str;

                index = model->index(i,j);
                str = model->data(index).toString();
                cell = worksheet->querySubObject("Range(QVariant, QVariant)", maplStr);
                cell->setProperty("Value", str);
                delete  cell;
            }
        }
        else{


            writeRow = currentRows + 1;
            index = model->index(i, 1);
            str = model->data(index).toString();
            sqlMap.insert(str, writeRow);

            str = QString::number(writeRow);
            maplStr = map[0] + str;
            index = model->index(i,0);
            str = QString::number(writeRow - 4);
            cell = worksheet->querySubObject("Range(QVariant, QVariant)", maplStr);
            cell->setProperty("Value", str);
            delete  cell;

            for (int j = 1; j < 8; j++)
            {
                str = QString::number(writeRow);
                maplStr = map[j] + str;

                index = model->index(i,j);
                str = model->data(index).toString();
                cell = worksheet->querySubObject("Range(QVariant, QVariant)", maplStr);
                cell->setProperty("Value", str);
                delete  cell;
            }
            for (int j = 8; j < model->columnCount(); j++)
            {
                str = QString::number(writeRow);
                maplStr = map[writeColumn + j - 8] + str;
                index = model->index(i,j);
                str = model->data(index).toString();
                cell = worksheet->querySubObject("Range(QVariant, QVariant)", maplStr);
                cell->setProperty("Value", str);
                delete  cell;
            }
            currentRows++;
        }
    }
    return  currentRows;
}


/*********
 子窗口编辑表格
 *********/
void MainWindow::on_pushButton_9_clicked()
{
   subwindow.ui->tableView->setModel(model);
   subwindow.model = model;
   subwindow.show();
}

/*********
 运行工况的选择——iLoadStatus的赋值
 *********/
void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    QString str = arg1;
    if (!str.compare("无选择"))
    {
        iLoadStatus = 0;
    }
    else if (!str.compare("航行状态"))
    {
        iLoadStatus = 1;
        modelInit();
    }
    else if (!str.compare("起锚状态"))
    {
        iLoadStatus = 2;
        modelInit();
    }
    else if (!str.compare("停泊状态"))
    {
        iLoadStatus = 3;
        modelInit();
    }
    else if (!str.compare("装卸货状态"))
    {
        iLoadStatus = 4;
        modelInit();
    }
    else if (!str.compare("应急状态"))
    {
        iLoadStatus = 5;
        modelInit();
    }
    ui->radioButton->setChecked(false);
    ui->radioButton_2->setChecked(false);
    ui->radioButton_2->setEnabled(false);
    ui->groupBox_95->setEnabled(false);
    ui->groupBox->setEnabled(false);

    ui->lineEdit_2->setText("");
    ui->lineEdit_3->setText("");
    ui->lineEdit_4->setText("");
    ui->lineEdit_5->setText("");
    ui->lineEdit_6->setText("");
    ui->lineEdit_7->setText("");
    ui->lineEdit_8->setText("");
    ui->lineEdit_9->setText("");
    ui->lineEdit_10->setText("");
    ui->lineEdit_11->setText("");
    ui->lineEdit_12->setText("");
    ui->lineEdit_13->setText("");
    ui->lineEdit_14->setText("");
    ui->lineEdit_15->setText("");
    ui->lineEdit_22->setText("");
    ui->lineEdit_23->setText("");
}
