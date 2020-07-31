#ifndef SUBWINDOW_H
#define SUBWINDOW_

#include <QWidget>

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

namespace Ui {
class subWindow;
}

class subWindow : public QWidget
{
    Q_OBJECT

public:
    Ui::subWindow *ui;
    QSqlTableModel *model;
    explicit subWindow(QWidget *parent = nullptr);
    ~subWindow();
    void modelInit();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:

};

#endif // SUBWINDOW_H
