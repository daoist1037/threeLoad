#include "titleedit.h"
#include "ui_titleedit.h"

titleEdit::titleEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::titleEdit)
{
    ui->setupUi(this);
}

titleEdit::~titleEdit()
{
    delete ui;
}

void titleEdit::on_pushButton_clicked()
{
    title = ui->lineEdit->text();
    QWidget::close();
}
