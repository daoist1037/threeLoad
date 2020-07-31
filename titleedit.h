#ifndef TITLEEDIT_H
#define TITLEEDIT_H

#include <QWidget>

namespace Ui {
class titleEdit;
}

class titleEdit : public QWidget
{
    Q_OBJECT

public:
    explicit titleEdit(QWidget *parent = nullptr);
    ~titleEdit();
    QString title;

private slots:
    void on_pushButton_clicked();

private:
    Ui::titleEdit *ui;
};

#endif // TITLEEDIT_H
