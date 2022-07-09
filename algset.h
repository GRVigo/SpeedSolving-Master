#ifndef ALGSET_H
#define ALGSET_H

#include <QDialog>

#include "collection.h"

namespace Ui {
class Algset;
}

class Algset : public QDialog
{
    Q_OBJECT

public:
    explicit Algset(QWidget *parent = nullptr);
    ~Algset();

private slots:

    void on_pushButton_Close_clicked();

    void on_comboBox_Collection_currentIndexChanged(int index);

    void on_listWidget_Cases_currentRowChanged(int currentRow);

    void on_tableWidget_Algorithms_cellChanged(int row, int column);

private:
    Ui::Algset *ui;
    grcube3::Collection* CurrentCollection;
};

#endif // ALGSET_H
