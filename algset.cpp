#include "algset.h"
#include "ui_algset.h"

#include "method.h"

using namespace grcube3;

Algset::Algset(QWidget *parent) : QDialog(parent, Qt::Tool), ui(new Ui::Algset)
{
    ui->setupUi(this);

    CurrentCollection = nullptr;

    setWindowTitle("Algorithm collections editor");

    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_OLL.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_PLL.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_1LLL.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_ZBLL.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_OCLL.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_CMLL.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_COLL.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_EPLL.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_2GLL.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_6CO.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_6CP.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_APDR.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_CDRLL.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_DCAL.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_EOLE.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_JTLE.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_L5EP.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_TDR.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_NCLL.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_NCOLL.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_TNCLL.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_L5E.GetName()));
    ui->comboBox_Collection->addItem(QString::fromStdString(Method::Algset_EODF.GetName()));
}

Algset::~Algset()
{
    delete ui;
    if (CurrentCollection != nullptr) delete CurrentCollection;
}

void Algset::on_pushButton_Close_clicked()
{
    close();
}

void Algset::on_comboBox_Collection_currentIndexChanged(int index)
{
    if (CurrentCollection != nullptr) delete CurrentCollection;
    CurrentCollection = nullptr;

    ui->listWidget_Cases->clear();
    ui->tableWidget_Algorithms->clear();

    switch (index)
    {
    case  0: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_OLL; break;
    case  1: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_PLL; break;
    case  2: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_1LLL; break;
    case  3: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_ZBLL; break;
    case  4: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_OCLL; break;
    case  5: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_CMLL; break;
    case  6: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_COLL; break;
    case  7: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_EPLL; break;
    case  8: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_2GLL; break;
    case  9: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_6CO; break;
    case 10: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_6CP; break;
    case 11: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_APDR; break;
    case 12: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_CDRLL; break;
    case 13: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_DCAL; break;
    case 14: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_EOLE; break;
    case 15: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_JTLE; break;
    case 16: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_L5EP; break;
    case 17: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_TDR; break;
    case 18: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_NCLL; break;
    case 19: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_NCOLL; break;
    case 20: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_TNCLL; break;
    case 21: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_L5E; break;
    case 22: CurrentCollection = new Collection(); *CurrentCollection = Method::Algset_EODF; break;
    default: ui->lineEdit_Description->setText("Please, select a collection!"); return;
    }

    ui->lineEdit_Description->setText(QString::fromStdString(CurrentCollection->GetDescription()));

    for (uint n = 0u; n < CurrentCollection->GetCasesNumber(); n++)
        ui->listWidget_Cases->addItem(QString::fromStdString(CurrentCollection->GetCaseName(n)));
}


void Algset::on_listWidget_Cases_currentRowChanged(int currentRow)
{
    if (CurrentCollection == nullptr) return;

    uint nAlgs = CurrentCollection->GetAlgorithmsNumber(currentRow);

    ui->tableWidget_Algorithms->clear();
    ui->tableWidget_Algorithms->setRowCount(nAlgs);
    ui->tableWidget_Algorithms->setColumnCount(1);

    for (uint row = 0u; row < nAlgs; row++)
    {
        QTableWidgetItem *it = new QTableWidgetItem(QString::fromStdString(CurrentCollection->GetAlgorithm(currentRow, row).ToString()));
        ui->tableWidget_Algorithms->setItem(row, 0, it);
    }

}


void Algset::on_tableWidget_Algorithms_cellChanged(int row, int column)
{
    // CurrentCollection[]
}

