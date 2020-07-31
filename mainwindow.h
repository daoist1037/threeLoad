#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "subwindow.h"
#include "ui_subwindow.h"
#include <QMainWindow>
#include <QtGlobal>

#include <QSqlRecord>
#include <qdebug.h>

#include <QSqlDatabase>
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariantList>

#include <QSqlTableModel>
#include <QAxObject>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QSqlTableModel *model;
    QSqlTableModel *model_total;
    subWindow subwindow;
    QAxObject * excel ;
    QAxObject * workbooks;
    QAxObject * workbook;
    QAxObject * worksheet;

    ~MainWindow();
    bool saveTextByIODevice(const QString &aFileName);
    int excelWrite(int writeRow, int writeColumn, int currentRows);
    void totalWrite(QString maplStr, QString str, int currentRows, int currentTotal);

    void modelInit();

private slots:
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();


    void on_pushButton_active_power_clicked();


    void on_pushButton_add_clicked();
    void on_pushButton_admin_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_delete_clicked();
    void on_pushButton_looking_clicked();
    void on_pushButton_csv_clicked();

    void on_pushButton_9_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
