/*  This file is part of "Speed Solving Master"

    Copyright (C) 2021 German Ramos Rodriguez

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    German Ramos Rodriguez
    Vigo, Spain
    grvigo@hotmail.com
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QClipboard>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
// #include <QThread>

#include "tinyxml2.h" // For xml language files

#include "cfop.h"
#include "roux.h"
#include "lbl.h"
#include "petrus.h"
#include "zz.h"

using namespace grcube3;
using namespace tinyxml2;

// Information to store about a solve search for the cache
struct CacheUnit
{
    Algorithm Scramble;
    uint Depth;
    double Time;
    std::vector<Algorithm> Solves;

    void Reset() { Scramble.Clear(); Depth = 0u; Time = 0.0; Solves.clear(); }
};

std::vector<CacheUnit> Cache; // Solves cache

#define _DEBUG_ false

  /*****************************************************************************************************************/
 /*** MAIN WINDOW *************************************************************************************************/
/*****************************************************************************************************************/

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    SaveXMLDefaultLanguage(); // Save default language texts

    LoadXMLDefaultLanguage(); // Load default language texts for use in undefined texts

    // Update cores (threads)
    ui->spinBox_Cores->setMinimum(0);
    ui->spinBox_Cores->setMaximum(DeepSearch::GetSystemCores());
    ui->spinBox_Cores->setValue(0);

    // Read language files
    QDir directory = QDir::currentPath();
    QStringList langfiles = directory.entryList(QStringList() << "*.xml" << "*.XML", QDir::Files);
    foreach(QString filename, langfiles) if (CheckXMLLanguageFile(filename)) ui->comboBox_Language->addItem(filename);

    // No scramble text in status bar
    MainWindow::on_lineEdit_Scramble_textChanged(ui->lineEdit_Scramble->text());

    on_pushButton_Credits_clicked(); // Show credits at start

    SolveId = 0u; // Index for solves history
}

MainWindow::~MainWindow()
{
    delete ui;
}

  /*****************************************************************************************************************/
 /*** CREDIT & LICENSE FUNCTIONS **********************************************************************************/
/*****************************************************************************************************************/

void MainWindow::on_pushButton_Credits_clicked()
{
    QFile file(CurrentLang["FileCredits"]);
    QString credits_text;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        credits_text.append(stream.readAll());
    }
    else credits_text = CurrentLang["NoCredits"];

    QString summary = CurrentLang["LoadExternal"];
    summary.append("\n\t");
    summary.append(CurrentLang["LoadCancel2"]);
    summary.append(std::to_string(Algorithm::GetCancellation2Size() >> 1).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadCancel3"]);
    summary.append(std::to_string(Algorithm::GetCancellation3Size() >> 1).c_str());

    summary.append("\n\t");
    summary.append(CurrentLang["LoadOLL"]);
    summary.append(std::to_string(Cube::GetOLLCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadPLL"]);
    summary.append(std::to_string(Cube::GetPLLCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["Load1LLL"]);
    summary.append(std::to_string(Cube::Get1LLLCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadZBLL"]);
    summary.append(std::to_string(Cube::GetZBLLCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadOCLL"]);
    summary.append(std::to_string(Cube::GetOCLLCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadCMLL"]);
    summary.append(std::to_string(Cube::GetCMLLCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadCOLL"]);
    summary.append(std::to_string(Cube::GetCOLLCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadEPLL"]);
    summary.append(std::to_string(Cube::GetEPLLCases()).c_str());

    // ui->textBrowser_report->clear();
    ui->textBrowser_report->setText(credits_text);
    ui->textBrowser_report->append(summary);
    ui->pushButton_CopyReport->setEnabled(true);
    ui->pushButton_SaveReport->setEnabled(true);
    ui->pushButton_ClearReport->setEnabled(true);
}

void MainWindow::on_pushButton_License_clicked()
{
    QFile file(CurrentLang["FileLicense"]);
    QString license_text;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        license_text.append(stream.readAll());
    }
    else license_text = CurrentLang["NoLicense"];

    ui->textBrowser_report->clear();
    ui->textBrowser_report->setText(license_text);
    ui->pushButton_CopyReport->setEnabled(true);
    ui->pushButton_SaveReport->setEnabled(true);
    ui->pushButton_ClearReport->setEnabled(true);
}

  /*****************************************************************************************************************/
 /*** LANGUAGE FUNCTIONS ******************************************************************************************/
/*****************************************************************************************************************/

// Read default texts from the application and save it to a default xml language file
void MainWindow::SaveXMLDefaultLanguage() const
{
    XMLDocument lang_xml;
	
	XMLNode* pRoot = lang_xml.NewElement("Translation"); // Root node
	XMLElement* pElementRoot = pRoot->ToElement();
	pElementRoot->SetAttribute("Language", "default");
	lang_xml.InsertFirstChild(pRoot);

	XMLElement* pElement;

	// Fixed texts for labels, push buttons, check boxes and radio buttons

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->label_ScrambleTitle->objectName().toStdString().c_str());
	pElement->SetText(ui->label_ScrambleTitle->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->pushButton_CopyScramble->objectName().toStdString().c_str());
	pElement->SetText(ui->pushButton_CopyScramble->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->pushButton_PasteScramble->objectName().toStdString().c_str());
	pElement->SetText(ui->pushButton_PasteScramble->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->pushButton_ClearScramble->objectName().toStdString().c_str());
	pElement->SetText(ui->pushButton_ClearScramble->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->pushButton_RandomScramble->objectName().toStdString().c_str());
	pElement->SetText(ui->pushButton_RandomScramble->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->pushButton_CopyReport->objectName().toStdString().c_str());
	pElement->SetText(ui->pushButton_CopyReport->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->pushButton_ClearReport->objectName().toStdString().c_str());
	pElement->SetText(ui->pushButton_ClearReport->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->pushButton_SaveReport->objectName().toStdString().c_str());
	pElement->SetText(ui->pushButton_SaveReport->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->label_Cores->objectName().toStdString().c_str());
	pElement->SetText(ui->label_Cores->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->pushButton_StartSearch->objectName().toStdString().c_str());
	pElement->SetText(ui->pushButton_StartSearch->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->label_Language->objectName().toStdString().c_str());
	pElement->SetText(ui->label_Language->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->pushButton_Credits->objectName().toStdString().c_str());
	pElement->SetText(ui->pushButton_Credits->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->pushButton_License->objectName().toStdString().c_str());
	pElement->SetText(ui->pushButton_License->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_Cache->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_Cache->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->label_History->objectName().toStdString().c_str());
	pElement->SetText(ui->label_History->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->pushButton_ClearHistory->objectName().toStdString().c_str());
    pElement->SetText(ui->pushButton_ClearHistory->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_Metric->objectName().toStdString().c_str());
    pElement->SetText(ui->label_Metric->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

	// CFOP

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->label_CFOP_Crosses->objectName().toStdString().c_str());
	pElement->SetText(ui->label_CFOP_Crosses->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->label_CFOP_CrossDepth->objectName().toStdString().c_str());
	pElement->SetText(ui->label_CFOP_CrossDepth->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_CFOP_Inspections->objectName().toStdString().c_str());
    pElement->SetText(ui->label_CFOP_Inspections->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->radioButton_CFOP_1LLL->objectName().toStdString().c_str());
	pElement->SetText(ui->radioButton_CFOP_1LLL->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->radioButton_CFOP_2LLL->objectName().toStdString().c_str());
	pElement->SetText(ui->radioButton_CFOP_2LLL->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->radioButton_CFOP_EO->objectName().toStdString().c_str());
    pElement->SetText(ui->radioButton_CFOP_EO->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_CFOP_BestSolve->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_CFOP_BestSolve->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_CFOP_Cancellations->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_CFOP_Cancellations->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_CFOP_Regrips->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_CFOP_Regrips->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->pushButton_CFOP_Help->objectName().toStdString().c_str());
    pElement->SetText(ui->pushButton_CFOP_Help->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

	// Roux

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_Roux_Orientation->objectName().toStdString().c_str());
    pElement->SetText(ui->label_Roux_Orientation->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_Roux_Inspections->objectName().toStdString().c_str());
    pElement->SetText(ui->label_Roux_Inspections->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->label_Roux_FBDepth->objectName().toStdString().c_str());
	pElement->SetText(ui->label_Roux_FBDepth->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->label_Roux_SBDepth->objectName().toStdString().c_str());
	pElement->SetText(ui->label_Roux_SBDepth->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->radioButton_Roux_FullL6E->objectName().toStdString().c_str());
	pElement->SetText(ui->radioButton_Roux_FullL6E->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->radioButton_Roux_StepsL6E->objectName().toStdString().c_str());
	pElement->SetText(ui->radioButton_Roux_StepsL6E->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->radioButton_Roux_CMLL->objectName().toStdString().c_str());
	pElement->SetText(ui->radioButton_Roux_CMLL->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->radioButton_Roux_COLL->objectName().toStdString().c_str());
	pElement->SetText(ui->radioButton_Roux_COLL->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_Roux_BestSolve->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_Roux_BestSolve->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_Roux_Cancellations->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_Roux_Cancellations->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_Roux_Regrips->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_Roux_Regrips->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->pushButton_Roux_Help->objectName().toStdString().c_str());
    pElement->SetText(ui->pushButton_Roux_Help->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);
	
	// Petrus
	
    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_Petrus_Orientation->objectName().toStdString().c_str());
    pElement->SetText(ui->label_Petrus_Orientation->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_Petrus_Inspections->objectName().toStdString().c_str());
    pElement->SetText(ui->label_Petrus_Inspections->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->radioButton_Petrus_ZBLL->objectName().toStdString().c_str());
	pElement->SetText(ui->radioButton_Petrus_ZBLL->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->radioButton_Petrus_OCLL_PLL->objectName().toStdString().c_str());
	pElement->SetText(ui->radioButton_Petrus_OCLL_PLL->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->radioButton_Petrus_COLL_EPLL->objectName().toStdString().c_str());
	pElement->SetText(ui->radioButton_Petrus_COLL_EPLL->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->label_Petrus_BlockDepth->objectName().toStdString().c_str());
	pElement->SetText(ui->label_Petrus_BlockDepth->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_Petrus_F2LDepth->objectName().toStdString().c_str());
    pElement->SetText(ui->label_Petrus_F2LDepth->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_Petrus_BestSolve->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_Petrus_BestSolve->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_Petrus_Cancellations->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_Petrus_Cancellations->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_Petrus_Regrips->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_Petrus_Regrips->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->pushButton_Petrus_Help->objectName().toStdString().c_str());
    pElement->SetText(ui->pushButton_Petrus_Help->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    // ZZ

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_ZZ_Orientation->objectName().toStdString().c_str());
    pElement->SetText(ui->label_ZZ_Orientation->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_ZZ_Inspections->objectName().toStdString().c_str());
    pElement->SetText(ui->label_ZZ_Inspections->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_ZZ_EODepth->objectName().toStdString().c_str());
    pElement->SetText(ui->label_ZZ_EODepth->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->radioButton_ZZ_ZBLL->objectName().toStdString().c_str());
    pElement->SetText(ui->radioButton_ZZ_ZBLL->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->radioButton_ZZ_COLL_EPLL->objectName().toStdString().c_str());
    pElement->SetText(ui->radioButton_ZZ_COLL_EPLL->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->radioButton_ZZ_OCLL_PLL->objectName().toStdString().c_str());
    pElement->SetText(ui->radioButton_ZZ_OCLL_PLL->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_ZZ_BestSolve->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_ZZ_BestSolve->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_ZZ_Cancellations->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_ZZ_Cancellations->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_ZZ_Regrips->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_ZZ_Regrips->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->pushButton_ZZ_Help->objectName().toStdString().c_str());
    pElement->SetText(ui->pushButton_ZZ_Help->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

	// LBL

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->label_LBL_Cross->objectName().toStdString().c_str());
	pElement->SetText(ui->label_LBL_Cross->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->radioButton_LBL_U->objectName().toStdString().c_str());
	pElement->SetText(ui->radioButton_LBL_U->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->radioButton_LBL_D->objectName().toStdString().c_str());
	pElement->SetText(ui->radioButton_LBL_D->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->radioButton_LBL_F->objectName().toStdString().c_str());
	pElement->SetText(ui->radioButton_LBL_F->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->radioButton_LBL_B->objectName().toStdString().c_str());
	pElement->SetText(ui->radioButton_LBL_B->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->radioButton_LBL_R->objectName().toStdString().c_str());
	pElement->SetText(ui->radioButton_LBL_R->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->radioButton_LBL_L->objectName().toStdString().c_str());
	pElement->SetText(ui->radioButton_LBL_L->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->pushButton_LBL_Help->objectName().toStdString().c_str());
	pElement->SetText(ui->pushButton_LBL_Help->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

	// Messages

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "MainWindowTitle");
	pElement->SetText("SpeedSolving Master beta");
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "Cache");
    pElement->SetText("[Evaluating from cache]");
    pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "NoScramble");
	pElement->SetText("No scramble!");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "ScrambleSimplified");
	pElement->SetText("Scramble simplified: ");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "NewRandomScrambleCreated");
	pElement->SetText("New random scramble created");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "ScrambleCopiedToClipboard");
	pElement->SetText("Scramble copied to clipboard");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "ScramblePastedFromClipboard");
	pElement->SetText("Scramble pasted from clipboard");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "ScrambleCleared");
	pElement->SetText("Scramble cleared");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "AllAvaliable");
	pElement->SetText("All avaliable");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "CoresWillBeUsedInTheSearch");
	pElement->SetText(" threads will be used in the search");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SingleCoreWillBeUsedInTheSearch");
	pElement->SetText("Single thread will be used in the search");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "EstimatedSearchTime");
	pElement->SetText("Estimated search time: ");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SecondsUnit");
	pElement->SetText(" s");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SearchingCross");
	pElement->SetText("Searching cross...");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SearchFinishedCrossNotFound");
	pElement->SetText("Search finished, cross not found!");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SearchingF2L");
	pElement->SetText("Searching F2L...");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SearchingOLL");
	pElement->SetText("Searching OLL...");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SearchingPLL");
	pElement->SetText("Searching PLL...");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "Searching1LLL");
	pElement->SetText("Searching 1LLL...");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SearchFinished");
	pElement->SetText("Search finished, enjoy!");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "ReportCopiedToTheClipboard");
	pElement->SetText("Report copied to the clipboard");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "ReportCleared");
	pElement->SetText("Report cleared");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SaveReport");
	pElement->SetText("Save report");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "TextAllFiles");
	pElement->SetText("Text (*.txt);;All Files (*)");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "ReportSaved");
	pElement->SetText("Report saved");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "Error");
	pElement->SetText("Error");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "UnableToOpenFile");
	pElement->SetText("Unable to open file");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "ReportNotSaved");
	pElement->SetText("Report not saved!");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "ApplicationLanguageChanged");
	pElement->SetText("Application language changed");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "DefaultLanguageApplied");
	pElement->SetText("Application language change error, default language applied!");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "NoCredits");
	pElement->SetText("Credits file not found!");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "NoLicense");
	pElement->SetText("License file not found!");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "TwoLookLastLayer");
	pElement->SetText("Two-Look Last Layer (OLL + PLL) selected");
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "EOZBLL");
    pElement->SetText("EO + ZBLL selected");
    pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "OneLookLastLayer");
    pElement->SetText("One-Look Last Layer (1LLL) selected");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "NoMethod");
	pElement->SetText("Method not implemented yet!");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SearchingFB");
	pElement->SetText("Searching first blocks...");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "NoFB");
	pElement->SetText("No first blocks found");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SearchingSB");
	pElement->SetText("Searching second blocks...");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SearchingCMLL");
	pElement->SetText("Searching CMLL...");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SearchingCOLL");
	pElement->SetText("Searching COLL...");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SearchingL6E");
	pElement->SetText("Searching last six edges...");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SearchingFL");
	pElement->SetText("Searching first layer...");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SearchingSL");
	pElement->SetText("Searching second layer...");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SearchingLL");
	pElement->SetText("Searching last layer...");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "NoRouxFB1");
	pElement->SetText("No Roux first blocks found for scramble (");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "NoRouxFB2");
	pElement->SetText(") in ");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "NoRouxFB3");
	pElement->SetText(" movements");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "CFOPNoCross");
	pElement->SetText("CFOP no cross");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "CFOPSolve");
	pElement->SetText("CFOP solve");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "RouxNoBlock");
	pElement->SetText("Roux no block");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "RouxSolve");
	pElement->SetText("Roux solve");
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingBlocks");
    pElement->SetText("Searching blocks...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoBlocks");
    pElement->SetText("No blocks found");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingEB");
    pElement->SetText("Expanding blocks...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoPetrusB1");
    pElement->SetText("No Petrus blocks found for scramble (");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoPetrusB2");
    pElement->SetText(") in ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoPetrusB3");
    pElement->SetText(" movements");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "PetrusNoBlock");
    pElement->SetText("Petrus no block");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "PetrusSolve");
    pElement->SetText("Petrus solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingZBLL");
    pElement->SetText("Searching ZBLL...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingOCLL");
    pElement->SetText("Searching OCLL...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingEPLL");
    pElement->SetText("Searching EPLL...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingEO");
    pElement->SetText("Searching EO...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingEOX");
    pElement->SetText("Searching EOX...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoEOX");
    pElement->SetText("No EOX solved!");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoZZB1");
    pElement->SetText("No ZZ EOLine found for scramble (");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoZZB2");
    pElement->SetText(") in ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoZZB3");
    pElement->SetText(" movements");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "ZZSolve");
    pElement->SetText("ZZ solve");
    pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "LBLNoCross");
	pElement->SetText("LBL no cross");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "LBLSolve");
	pElement->SetText("LBL solve");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SolveRecoveredHistory");
	pElement->SetText("Solve recovered from history");
	pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "NoHelp");
    pElement->SetText("\n<Help file not found>\n");
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadExternal");
    pElement->SetText("External files data: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadCancel2");
    pElement->SetText("2-Movs cancellations loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadCancel3");
    pElement->SetText("3-Movs cancellations loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadOLL");
    pElement->SetText("OLL algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadPLL");
    pElement->SetText("PLL algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "Load1LLL");
    pElement->SetText("1LLL algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadZBLL");
    pElement->SetText("ZBLL algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadOCLL");
    pElement->SetText("OCLL algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadCMLL");
    pElement->SetText("CMLL algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadCOLL");
    pElement->SetText("COLL algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadEPLL");
    pElement->SetText("EPLL algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

	// Filenames
	
	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "FileCredits");
	pElement->SetText("CREDITS");
	pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "FileLicense");
	pElement->SetText("COPYING");
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "FileHelp");
    pElement->SetText("help_en.txt");
    pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "FileCFOPHelp");
	pElement->SetText("cfop_en.txt");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "FileRouxHelp");
	pElement->SetText("roux_en.txt");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "FileLBLHelp");
	pElement->SetText("lbl_en.txt");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "FilePetrusHelp");
	pElement->SetText("petrus_en.txt");
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "FileZZHelp");
    pElement->SetText("zz_en.txt");
    pRoot->InsertEndChild(pElement);

	lang_xml.SaveFile("default.xml");
}

// Load the default xml language file texts
bool MainWindow::LoadXMLDefaultLanguage()
{
    DefaultLang.clear();

    XMLDocument lang_xml;

    if (lang_xml.LoadFile("default.xml") == XML_SUCCESS) // Read existing file
    {
        XMLNode* pRoot = lang_xml.FirstChild();

        if (pRoot != nullptr)
        {
            XMLElement*  pElementRoot = pRoot->ToElement();
            const char* s_Language;
            s_Language = pElementRoot->Attribute("Language");
            std::string Lang = s_Language;
            if (Lang.compare("default") != 0) return false; // No default language

            const char *objectName, *objectText, *Id, *Message;
            XMLElement* pElement = pRoot->FirstChildElement("FixedText");
            while (pElement != nullptr)
            {
                objectText = pElement->GetText();
                objectName = pElement->Attribute("ObjectName");

                DefaultLang.insert(objectName, objectText);

                pElement = pElement->NextSiblingElement("FixedText");
            }

            pElement = pRoot->FirstChildElement("Message");
            while (pElement != nullptr)
            {
                Message = pElement->GetText();
                Id = pElement->Attribute("Id");

                DefaultLang.insert(Id, Message);

                pElement = pElement->NextSiblingElement("Message");
            }
        }
        return true;
    }
    return false;
}

// Load the xml language file for the application current language
bool MainWindow::LoadXMLCurrentLanguage(const QString &lang)
{
    CurrentLang.clear();

    XMLDocument lang_xml;

    if (lang_xml.LoadFile(lang.toStdString().c_str()) == XML_SUCCESS) // Read existing file
    {
        XMLNode* pRoot = lang_xml.FirstChild();

        if (pRoot != nullptr)
        {
            XMLElement*  pElementRoot = pRoot->ToElement();
            const char* s_Language;
            s_Language = pElementRoot->Attribute("Language");
            std::string Lang = s_Language;
            if (Lang.empty()) return false; // No language

            const char *objectName, *objectText, *Id, *Message;
            XMLElement* pElement = pRoot->FirstChildElement("FixedText");
            while (pElement != nullptr)
            {
                objectText = pElement->GetText();
                objectName = pElement->Attribute("ObjectName");

                CurrentLang.insert(objectName, objectText);

                pElement = pElement->NextSiblingElement("FixedText");
            }

            pElement = pRoot->FirstChildElement("Message");
            while (pElement != nullptr)
            {
                Message = pElement->GetText();
                Id = pElement->Attribute("Id");

                CurrentLang.insert(Id, Message);

                pElement = pElement->NextSiblingElement("Message");
            }
        }

        // Complete the language data with the default texts
        LangMap::const_iterator i = DefaultLang.constBegin();
        while (i != DefaultLang.constEnd())
        {
            if (!CurrentLang.contains(i.key())) CurrentLang.insert(i.key(), i.value());
            ++i;
        }

        return true;
    }
    return false;
}

// Updates the fixed texts in the application
void MainWindow::UpdateFixedTexts(const LangMap& Lang)
{
    if (Lang.contains("MainWindowTitle")) setWindowTitle(Lang["MainWindowTitle"]);
    else setWindowTitle(DefaultLang["MainWindowTitle"]);

    QWidget *pWin = QApplication::activeWindow();

    LangMap::const_iterator i = Lang.constBegin();
    while (i != Lang.constEnd())
    {
        if (QWidget* widget = pWin->findChild<QWidget*>(i.key()))
        { // In this application, fixed texts are only in push buttons, labels, check boxes and radio buttons
            if(QPushButton *pb = qobject_cast<QPushButton*>(widget)) pb->setText(i.value());
            else if(QLabel *lb = qobject_cast<QLabel*>(widget)) lb->setText(i.value());
            else if(QCheckBox *cb = qobject_cast<QCheckBox*>(widget)) cb->setText(i.value());
            else if(QRadioButton *rb = qobject_cast<QRadioButton*>(widget)) rb->setText(i.value());
        }
        ++i;
    }
}

// Check the xml language file for the application current language
bool MainWindow::CheckXMLLanguageFile(const QString& lang)
{
    XMLDocument lang_xml;

    if (lang_xml.LoadFile(lang.toStdString().c_str()) == XML_SUCCESS) // Read existing file
    {
        XMLNode* pRoot = lang_xml.FirstChild();

        if (pRoot != nullptr)
        {
            XMLElement*  pElementRoot = pRoot->ToElement();
            const char* s_Language = pElementRoot->Attribute("Language");
            if (s_Language == nullptr) return false;
            std::string Lang = s_Language;
            if (Lang.empty()) return false; // No language
            return true;
        }
    }
    return false;
}

void MainWindow::on_comboBox_Language_currentIndexChanged(const QString &langfile)
{
    if (LoadXMLCurrentLanguage(langfile))
    {
        UpdateFixedTexts(CurrentLang);
        ui->statusBar->showMessage(CurrentLang["ApplicationLanguageChanged"]);
    }
    else
    {
        UpdateFixedTexts(DefaultLang);
        ui->statusBar->showMessage(CurrentLang["DefaultLanguageApplied"]);
    }
}

  /*****************************************************************************************************************/
 /*** SCRAMBLE FUNCTIONS ******************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::on_lineEdit_Scramble_textChanged(const QString &scrInput)
{
    if (scrInput.isEmpty())
    {
        ui->pushButton_CopyScramble->setDisabled(true);
        ui->pushButton_ClearScramble->setDisabled(true);
        ui->pushButton_StartSearch->setDisabled(true);

        ui->statusBar->showMessage(CurrentLang["NoScramble"]);
    }
    else
    {
        ui->pushButton_CopyScramble->setEnabled(true);
        ui->pushButton_ClearScramble->setEnabled(true);
        ui->pushButton_StartSearch->setEnabled(true);

        Algorithm A(ui->lineEdit_Scramble->text().toStdString().c_str());
        A = A.GetSimplified();
        A = A.GetWithoutTurns();
        while (A.Shrink());
        ui->statusBar->showMessage(CurrentLang["ScrambleSimplified"] + QString::fromStdString(A.ToString()));
    }
}

void MainWindow::on_pushButton_RandomScramble_clicked()
{
    Algorithm A(ui->spinBox_ScrambleLength->value());
    ui->lineEdit_Scramble->setText(QString::fromStdString(A.ToString()));
    ui->statusBar->showMessage(CurrentLang["NewRandomScrambleCreated"]);
}

void MainWindow::on_pushButton_CopyScramble_clicked()
{
    QApplication::clipboard()->setText(ui->lineEdit_Scramble->text());
    ui->statusBar->showMessage(CurrentLang["ScrambleCopiedToClipboard"]);
}

void MainWindow::on_pushButton_PasteScramble_clicked()
{
    ui->lineEdit_Scramble->setText(QApplication::clipboard()->text());
    ui->statusBar->showMessage(CurrentLang["ScramblePastedFromClipboard"]);
}

void MainWindow::on_pushButton_ClearScramble_clicked()
{
    ui->lineEdit_Scramble->clear();
    ui->statusBar->showMessage(CurrentLang["ScrambleCleared"]);
}

  /*****************************************************************************************************************/
 /*** THREADS, ESTIMATED TIME & CACHE FUNCTIONS **************************************************************************/
/*****************************************************************************************************************/

void MainWindow::on_spinBox_Cores_valueChanged(int cores)
{
    if (cores == 0) ui->statusBar->showMessage(CurrentLang["AllAvaliable"] + CurrentLang["CoresWillBeUsedInTheSearch"]);
    else if (cores == 1) ui->statusBar->showMessage(CurrentLang["SingleCoreWillBeUsedInTheSearch"]);
    else ui->statusBar->showMessage(QString::number(cores) + CurrentLang["CoresWillBeUsedInTheSearch"]);

    UpdateRefTime(cores);
}

// Calculates the reference time for estimations
double MainWindow::UpdateRefTime(const int cores)
{
    // J perm's "hardest scramble"
    Algorithm RefA("B F U F D R' F D L B2 U' B2 D B' R' F2 L2 R2 U'");
    CFOP RefSearch(RefA, cores);
    RefSearch.SearchCrosses(5u);
    return RefSearch.GetTime();
}

double MainWindow::GetCFOPEstimatedTime(const uint depth, const uint insp)
{
    double RefTime = UpdateRefTime(ui->spinBox_Cores->value());
    double EstimatedTime = RefTime;
    for (uint n = 5u; n < depth; n++) EstimatedTime *= 16.0; // Cross estimated search time
    EstimatedTime += RefTime * 150.0 * insp; // F2L estimated search time
	return EstimatedTime;
}

double MainWindow::GetRouxEstimatedTime(const uint depth1, const uint depth2, const uint insp)
{
	double EstimatedTimeFB = UpdateRefTime(ui->spinBox_Cores->value());
    double EstimatedTimeSB = EstimatedTimeFB * insp;
    double EstimatedTimeL6E = EstimatedTimeFB * 75.0 * insp; // L6E estimated search time
    for (uint n = 5u; n < depth1; n++) EstimatedTimeFB *= 15.0; // FB estimated search time
    for (uint n = 5u; n < depth2; n++) EstimatedTimeSB *= 15.0; // SB estimated search time
	
	return EstimatedTimeFB + EstimatedTimeSB + EstimatedTimeL6E;
}	

double MainWindow::GetPetrusEstimatedTime(const uint depth1, const uint depth2, const uint insp)
{
    double EstimatedTimeBlock = UpdateRefTime(ui->spinBox_Cores->value());
    double EstimatedTimeExpBlock = EstimatedTimeBlock * 75.0 * insp; // Expanded block estimated search time
    double EstimatedTimeEO = EstimatedTimeBlock * 75.0 * insp; // EO estimated search time
    double EstimatedTimeF2L = EstimatedTimeBlock * insp; // F2L estimated search time
    for (uint n = 5; n < depth1; n++) EstimatedTimeBlock *= 15.0; // Block estimated search time
    for (uint n = 5; n < depth2; n++) EstimatedTimeF2L *= 15.0; // F2L estimated search time

    return EstimatedTimeBlock + EstimatedTimeExpBlock + EstimatedTimeEO + EstimatedTimeF2L;
}

double MainWindow::GetZZEstimatedTime(const uint depth, const uint insp)
{
    double RefTime = UpdateRefTime(ui->spinBox_Cores->value());
    double EstimatedTime = RefTime;
    for (uint n = 5u; n < depth; n++) EstimatedTime *= 16.0; // EOX estimated search time
    EstimatedTime += RefTime * 30.0 * insp; // F2L estimated search time
    return EstimatedTime;
}

double MainWindow::GetLBLEstimatedTime()
{
    return UpdateRefTime(ui->spinBox_Cores->value()) * 200.0;
}

void MainWindow::on_checkBox_Cache_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        // With cache enabled, limit depth to 8 (memory reasons)
        ui->spinBox_CFOP_Cross->setMaximum(8);
        ui->spinBox_Roux_FB->setMaximum(8);
        ui->spinBox_Petrus_BlockDepth->setMaximum(8);
        ui->spinBox_ZZ_EODepth->setMaximum(8);

        // Match first search depths
        switch (ui->tabWidgetMethods->currentIndex())
        {
        case 0: // CFOP
        case 4: // LBL
            ui->spinBox_Roux_FB->setValue(ui->spinBox_CFOP_Cross->value());
            ui->spinBox_Petrus_BlockDepth->setValue(ui->spinBox_CFOP_Cross->value());
            ui->spinBox_ZZ_EODepth->setValue(ui->spinBox_CFOP_Cross->value());
            break;
        case 1: // Roux
            ui->spinBox_CFOP_Cross->setValue(ui->spinBox_Roux_FB->value());
            ui->spinBox_Petrus_BlockDepth->setValue(ui->spinBox_Roux_FB->value());
            ui->spinBox_ZZ_EODepth->setValue(ui->spinBox_Roux_FB->value());
            break;
        case 2: // Petrus
            ui->spinBox_CFOP_Cross->setValue(ui->spinBox_Petrus_BlockDepth->value());
            ui->spinBox_Roux_FB->setValue(ui->spinBox_Petrus_BlockDepth->value());
            ui->spinBox_ZZ_EODepth->setValue(ui->spinBox_Petrus_BlockDepth->value());
            break;
        case 3: // ZZ
            ui->spinBox_CFOP_Cross->setValue(ui->spinBox_ZZ_EODepth->value());
            ui->spinBox_Roux_FB->setValue(ui->spinBox_ZZ_EODepth->value());
            ui->spinBox_Petrus_BlockDepth->setValue(ui->spinBox_ZZ_EODepth->value());
            break;
        default: break;
        }
    }
    else // Cache disabled
    {
        ui->spinBox_CFOP_Cross->setMaximum(9);
        ui->spinBox_Roux_FB->setMaximum(9);
        ui->spinBox_Petrus_BlockDepth->setMaximum(9);
        ui->spinBox_ZZ_EODepth->setMaximum(9);
    }
}

  /*****************************************************************************************************************/
 /*** HELP FUNCTIONS **********************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::on_pushButton_CFOP_Help_clicked()
{
    QString help_text;

    QFile file(CurrentLang["FileCFOPHelp"]);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        help_text.append(stream.readAll());
    }
    else help_text.append(CurrentLang["NoHelp"]);

    QFile file_help(CurrentLang["FileHelp"]);
    if (file_help.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file_help);
        help_text.append(stream.readAll());
    }
    else help_text.append(CurrentLang["NoHelp"]);

    ui->textBrowser_report->clear();
    ui->textBrowser_report->setText(help_text);
    ui->pushButton_CopyReport->setEnabled(true);
    ui->pushButton_SaveReport->setEnabled(true);
    ui->pushButton_ClearReport->setEnabled(true);
}

void MainWindow::on_pushButton_Roux_Help_clicked()
{
    QString help_text;

    QFile file(CurrentLang["FileRouxHelp"]);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        help_text.append(stream.readAll());
    }
    else help_text.append(CurrentLang["NoHelp"]);

    QFile file_help(CurrentLang["FileHelp"]);
    if (file_help.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file_help);
        help_text.append(stream.readAll());
    }
    else help_text.append(CurrentLang["NoHelp"]);

    ui->textBrowser_report->clear();
    ui->textBrowser_report->setText(help_text);
    ui->pushButton_CopyReport->setEnabled(true);
    ui->pushButton_SaveReport->setEnabled(true);
    ui->pushButton_ClearReport->setEnabled(true);
}

void MainWindow::on_pushButton_Petrus_Help_clicked()
{
    QString help_text;

    QFile file(CurrentLang["FilePetrusHelp"]);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        help_text.append(stream.readAll());
    }
    else help_text.append(CurrentLang["NoHelp"]);

    QFile file_help(CurrentLang["FileHelp"]);
    if (file_help.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file_help);
        help_text.append(stream.readAll());
    }
    else help_text.append(CurrentLang["NoHelp"]);

    ui->textBrowser_report->clear();
    ui->textBrowser_report->setText(help_text);
    ui->pushButton_CopyReport->setEnabled(true);
    ui->pushButton_SaveReport->setEnabled(true);
    ui->pushButton_ClearReport->setEnabled(true);
}

void MainWindow::on_pushButton_ZZ_Help_clicked()
{
    QString help_text;

    QFile file(CurrentLang["FileZZHelp"]);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        help_text.append(stream.readAll());
    }
    else help_text.append(CurrentLang["NoHelp"]);

    QFile file_help(CurrentLang["FileHelp"]);
    if (file_help.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file_help);
        help_text.append(stream.readAll());
    }
    else help_text.append(CurrentLang["NoHelp"]);

    ui->textBrowser_report->clear();
    ui->textBrowser_report->setText(help_text);
    ui->pushButton_CopyReport->setEnabled(true);
    ui->pushButton_SaveReport->setEnabled(true);
    ui->pushButton_ClearReport->setEnabled(true);
}

void MainWindow::on_pushButton_LBL_Help_clicked()
{
    QString help_text;

    QFile file(CurrentLang["FileLBLHelp"]);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        help_text.append(stream.readAll());
    }
    else help_text.append(CurrentLang["NoHelp"]);

    QFile file_help(CurrentLang["FileHelp"]);
    if (file_help.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file_help);
        help_text.append(stream.readAll());
    }
    else help_text.append(CurrentLang["NoHelp"]);

    ui->textBrowser_report->clear();
    ui->textBrowser_report->setText(help_text);
    ui->pushButton_CopyReport->setEnabled(true);
    ui->pushButton_SaveReport->setEnabled(true);
    ui->pushButton_ClearReport->setEnabled(true);
}

  /*****************************************************************************************************************/
 /*** REPORT FUNCTIONS ********************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::on_pushButton_CopyReport_clicked()
{
    QApplication::clipboard()->setText(ui->textBrowser_report->toPlainText());
    ui->statusBar->showMessage(CurrentLang["ReportCopiedToTheClipboard"]);
}

void MainWindow::on_pushButton_ClearReport_clicked()
{
    // Clear output
    ui->textBrowser_report->clear();
    ui->pushButton_CopyReport->setDisabled(true);
    ui->pushButton_ClearReport->setDisabled(true);
    ui->pushButton_SaveReport->setDisabled(true);
    ui->statusBar->showMessage(CurrentLang["ReportCleared"]);
}

void MainWindow::on_pushButton_SaveReport_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr(CurrentLang["SaveReport"].toStdString().c_str()), "", tr(CurrentLang["TextAllFiles"].toStdString().c_str()));

    if (fileName.isEmpty()) return;
    else
    {
        QFile file(fileName);
        if (file.open(QIODevice::ReadWrite))
        {
            QTextStream stream(&file);
            stream << ui->textBrowser_report->toPlainText();
            file.flush();
            file.close();
            ui->statusBar->showMessage(CurrentLang["ReportSaved"]);
        }
        else
        {
            QMessageBox::information(this, tr(CurrentLang["Error"].toStdString().c_str()), tr(CurrentLang["UnableToOpenFile"].toStdString().c_str()));
            ui->statusBar->showMessage(CurrentLang["ReportNotSaved"]);
            return;
        }
    }
}

  /*****************************************************************************************************************/
 /*** HISTORY FUNCTIONS *******************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::on_comboBox_History_currentIndexChanged(int index)
{
    if (ui->comboBox_History->count() == 0) return;

    ui->textBrowser_report->clear();
    ui->textBrowser_report->append(History[index]);
    ui->statusBar->showMessage(CurrentLang["SolveRecoveredHistory"]);

    ui->pushButton_CopyReport->setEnabled(true);
    ui->pushButton_SaveReport->setEnabled(true);
    ui->pushButton_ClearReport->setEnabled(true);
}

void MainWindow::on_comboBox_History_activated(int index)
{
    on_comboBox_History_currentIndexChanged(index);
}

void MainWindow::AddToHistory(const QString& HistoryId)
{
    SolveId++;
    QString Solve = ui->textBrowser_report->toPlainText();
    if (Solve.isEmpty() || HistoryId.isEmpty()) return;
    History.push_back(Solve);
    QString SId = HistoryId + " (#" + QString::number(SolveId) + ")";
    ui->comboBox_History->addItem(SId);
}

void MainWindow::on_pushButton_ClearHistory_clicked()
{
    ui->comboBox_History->clear();
    History.clear();
    SolveId = 0u;
}

  /*****************************************************************************************************************/
 /*** START SEARCH FUNCTION ***************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::on_pushButton_StartSearch_clicked()
{
    // Clear output
    ui->textBrowser_report->clear();
    ui->pushButton_CopyReport->setDisabled(true);
    ui->pushButton_ClearReport->setDisabled(true);
    ui->pushButton_SaveReport->setDisabled(true);
    ui->pushButton_StartSearch->setDisabled(true);

    // Read scramble
    Algorithm Scramble(ui->lineEdit_Scramble->text().toStdString().c_str());
    Scramble = Scramble.GetSimplified();
    Scramble = Scramble.GetWithoutTurns();
    while (Scramble.Shrink());

      /*****************************************************************************************************************/
     /*** CFOP ********************************************************************************************************/
    /*****************************************************************************************************************/

    if (ui->tabWidgetMethods->currentIndex() == 0)
    {
        // Show estimated search time
        ui->statusBar->showMessage(CurrentLang["EstimatedSearchTime"] +
                                   QString::number(static_cast<int>(GetCFOPEstimatedTime(ui->spinBox_CFOP_Cross->value(),
                                                                                         ui->spinBox_CFOP_Inspections->value()))) +
                                   CurrentLang["SecondsUnit"]);

        // Read the allowed layers for cross
        std::vector<Lyr> CrossLayers;
        switch (ui->comboBox_CFOP_Orientation->currentIndex())
        {
        case 0: CrossLayers.push_back(Lyr::U); CrossLayers.push_back(Lyr::D);
                CrossLayers.push_back(Lyr::F); CrossLayers.push_back(Lyr::B);
                CrossLayers.push_back(Lyr::R); CrossLayers.push_back(Lyr::L); break;
        case 1: CrossLayers.push_back(Lyr::U); CrossLayers.push_back(Lyr::D); break;
        case 2: CrossLayers.push_back(Lyr::F); CrossLayers.push_back(Lyr::B); break;
        case 3: CrossLayers.push_back(Lyr::R); CrossLayers.push_back(Lyr::L); break;
        case 4: CrossLayers.push_back(Lyr::U); break;
        case 5: CrossLayers.push_back(Lyr::D); break;
        case 6: CrossLayers.push_back(Lyr::F); break;
        case 7: CrossLayers.push_back(Lyr::B); break;
        case 8: CrossLayers.push_back(Lyr::R); break;
        case 9: CrossLayers.push_back(Lyr::L); break;
        default: CrossLayers.push_back(Lyr::D); break;
        }

        // Create search
        CFOP SearchCFOP(Scramble, ui->spinBox_Cores->value());

        SearchCFOP.SetCrossLayers(CrossLayers);

		SearchCFOP.SetMetric(static_cast<Metrics>(ui->comboBox_Metric->currentIndex()));

        // Start cross search
        ui->textBrowser_report->append(CurrentLang["SearchingCross"]);
        qApp->processEvents();

        if (ui->checkBox_Cache->isChecked())
        {
            bool found = false;
            for (const auto& c : Cache) // Search in cache
            {
                if (c.Scramble == Scramble && static_cast<int>(c.Depth) == ui->spinBox_CFOP_Cross->value() && !c.Solves.empty())
                {
                    ui->textBrowser_report->append(CurrentLang["Cache"]);
                    qApp->processEvents();
                    SearchCFOP.EvaluateCrosses(c.Solves, ui->spinBox_CFOP_Inspections->value());
                    SearchCFOP.SetTimeCrosses(c.Time);
                    SearchCFOP.SetDepthCrosses(c.Depth);
                    found = true;
                    break;
                }
            }

            if (!found) // Solves not found in cache
            {
				Cache.clear(); // Cache memory usage can be large, clear previous cache
                const auto time_cross_start = std::chrono::system_clock::now();

                DeepSearch BaseSearch(Scramble);
                BaseSearch.SearchBase(ui->spinBox_CFOP_Cross->value(), ui->spinBox_Cores->value());

                if (BaseSearch.Solves.empty())
                {
                    ui->statusBar->showMessage(CurrentLang["SearchFinishedCrossNotFound"]);
                    ui->textBrowser_report->clear();
                    ui->textBrowser_report->append(QString::fromStdString(SearchCFOP.GetReport(false, false)));
                    AddToHistory(CurrentLang["CFOPNoCross"]);

                    ui->pushButton_CopyReport->setEnabled(true);
                    ui->pushButton_ClearReport->setEnabled(true);
                    ui->pushButton_SaveReport->setEnabled(true);
                    ui->pushButton_StartSearch->setEnabled(true);
                    return;
                }

                SearchCFOP.EvaluateCrosses(BaseSearch.Solves, ui->spinBox_CFOP_Inspections->value());

                const std::chrono::duration<double> cross_elapsed_seconds = std::chrono::system_clock::now() - time_cross_start;
                SearchCFOP.SetTimeCrosses(cross_elapsed_seconds.count());
                SearchCFOP.SetDepthCrosses(ui->spinBox_CFOP_Cross->value());

                // Add current solves into cache
                CacheUnit CU;
                CU.Scramble = Scramble;
                CU.Depth = ui->spinBox_CFOP_Cross->value();
                CU.Time = SearchCFOP.GetTime();
                CU.Solves = BaseSearch.Solves;
                Cache.push_back(CU);
            }
        }
        else // No cache
        {
            if (!SearchCFOP.SearchCrosses(ui->spinBox_CFOP_Cross->value(), ui->spinBox_CFOP_Inspections->value()))
            {
                ui->statusBar->showMessage(CurrentLang["SearchFinishedCrossNotFound"]);
                ui->textBrowser_report->clear();
                ui->textBrowser_report->append(QString::fromStdString(SearchCFOP.GetReport(false, false)));
                AddToHistory(CurrentLang["CFOPNoCross"]);

                ui->pushButton_CopyReport->setEnabled(true);
                ui->pushButton_ClearReport->setEnabled(true);
                ui->pushButton_SaveReport->setEnabled(true);
                ui->pushButton_StartSearch->setEnabled(true);
                return;
            }
        }

        // Start F2L search
        ui->textBrowser_report->append(CurrentLang["SearchingF2L"]);
        qApp->processEvents();

        SearchCFOP.SearchF2L();

        // Start last layer search
        ui->textBrowser_report->append(CurrentLang["SearchingLL"]);
        qApp->processEvents();

        if (ui->radioButton_CFOP_2LLL->isChecked()) // OLL + PLL
        {
            SearchCFOP.SearchOLL();
            SearchCFOP.SearchPLL();
        }
        else if (ui->radioButton_CFOP_EO->isChecked()) // EO + ZBLL
        {
            SearchCFOP.SearchEOLL();
            SearchCFOP.SearchZBLL();
        }
        else SearchCFOP.Search1LLL(); // 1LLL

        if (ui->checkBox_CFOP_Regrips->isChecked()) SearchCFOP.SetRegrips();

        ui->textBrowser_report->clear();

        ui->textBrowser_report->append(QString::fromStdString(SearchCFOP.GetReport(ui->checkBox_CFOP_Cancellations->isChecked(), false)));
        if (ui->checkBox_CFOP_BestSolve->isChecked())
            ui->textBrowser_report->append(QString::fromStdString("Best solve for " + SearchCFOP.GetBestReport(ui->checkBox_CFOP_Cancellations->isChecked())));
        ui->textBrowser_report->append(QString::fromStdString(SearchCFOP.GetTimeReport()));
        AddToHistory(CurrentLang["CFOPSolve"]);

        ui->pushButton_CopyReport->setEnabled(true);
        ui->pushButton_ClearReport->setEnabled(true);
        ui->pushButton_SaveReport->setEnabled(true);
        ui->pushButton_StartSearch->setEnabled(true);

        ui->statusBar->showMessage(CurrentLang["SearchFinished"]);
    }

      /*****************************************************************************************************************/
     /*** Roux ********************************************************************************************************/
    /*****************************************************************************************************************/

    else if (ui->tabWidgetMethods->currentIndex() == 1)
    {
        // Show estimated search time
        ui->statusBar->showMessage(CurrentLang["EstimatedSearchTime"] +
                                   QString::number(static_cast<int>(GetRouxEstimatedTime(ui->spinBox_Roux_FB->value(),
                                                                                         ui->spinBox_Roux_SB->value(),
                                                                                         ui->spinBox_Roux_Inspections->value()))) +
                                   CurrentLang["SecondsUnit"]);

        // Read the allowed layers for cross
        std::vector<Spn> SearchSpins;
        switch (ui->comboBox_Roux_Orientation->currentIndex())
        {
        case 0: for (int s = 0; s < 24; s++) SearchSpins.push_back(static_cast<Spn>(s)); break; // All

        case 1: SearchSpins.push_back(Spn::UF); SearchSpins.push_back(Spn::UB); SearchSpins.push_back(Spn::UR); SearchSpins.push_back(Spn::UL);
                SearchSpins.push_back(Spn::DF); SearchSpins.push_back(Spn::DB); SearchSpins.push_back(Spn::DR); SearchSpins.push_back(Spn::DL);
            break;
        case 2: SearchSpins.push_back(Spn::FU); SearchSpins.push_back(Spn::FD); SearchSpins.push_back(Spn::FR); SearchSpins.push_back(Spn::FL);
                SearchSpins.push_back(Spn::BU); SearchSpins.push_back(Spn::BD); SearchSpins.push_back(Spn::BR); SearchSpins.push_back(Spn::BL);
            break;
        case 3: SearchSpins.push_back(Spn::RU); SearchSpins.push_back(Spn::RD); SearchSpins.push_back(Spn::RF); SearchSpins.push_back(Spn::RB);
                SearchSpins.push_back(Spn::LU); SearchSpins.push_back(Spn::LD); SearchSpins.push_back(Spn::LF); SearchSpins.push_back(Spn::LB);
            break;

        case 4: SearchSpins.push_back(Spn::UF); SearchSpins.push_back(Spn::UB); SearchSpins.push_back(Spn::UR); SearchSpins.push_back(Spn::UL);
            break;
        case 5: SearchSpins.push_back(Spn::DF); SearchSpins.push_back(Spn::DB); SearchSpins.push_back(Spn::DR); SearchSpins.push_back(Spn::DL);
            break;
        case 6: SearchSpins.push_back(Spn::FU); SearchSpins.push_back(Spn::FD); SearchSpins.push_back(Spn::FR); SearchSpins.push_back(Spn::FL);
            break;
        case 7: SearchSpins.push_back(Spn::BU); SearchSpins.push_back(Spn::BD); SearchSpins.push_back(Spn::BR); SearchSpins.push_back(Spn::BL);
            break;
        case 8: SearchSpins.push_back(Spn::RU); SearchSpins.push_back(Spn::RD); SearchSpins.push_back(Spn::RF); SearchSpins.push_back(Spn::RB);
            break;
        case 9: SearchSpins.push_back(Spn::LU); SearchSpins.push_back(Spn::LD); SearchSpins.push_back(Spn::LF); SearchSpins.push_back(Spn::LB);
            break;

        default: SearchSpins.push_back(static_cast<Spn>(ui->comboBox_Roux_Orientation->currentIndex() - 10)); break;
        }

        // Create search
        Roux SearchRoux(Scramble, ui->spinBox_Cores->value());

        SearchRoux.SetSearchSpins(SearchSpins);

        SearchRoux.SetMetric(static_cast<Metrics>(ui->comboBox_Metric->currentIndex()));

        // Start first block search
        ui->textBrowser_report->append(CurrentLang["SearchingFB"]);
        qApp->processEvents();
		
		if (ui->checkBox_Cache->isChecked())
        {
            bool found = false;
            for (const auto& c : Cache) // Search in cache
            {
                if (c.Scramble == Scramble && static_cast<int>(c.Depth) == ui->spinBox_Roux_FB->value() && !c.Solves.empty())
                {
                    ui->textBrowser_report->append(CurrentLang["Cache"]);
                    qApp->processEvents();
                    SearchRoux.EvaluateFirstBlock(c.Solves, ui->spinBox_Roux_Inspections->value());
                    SearchRoux.SetTimeFB(c.Time);
                    SearchRoux.SetDepthFB(c.Depth);
                    found = true;
                    break;
                }
            }

            if (!found) // Solves not found in cache
            {
				Cache.clear(); // Cache memory usage can be large, clear previous cache
                const auto time_fb_start = std::chrono::system_clock::now();

                DeepSearch BaseSearch(Scramble);
                BaseSearch.SearchBase(ui->spinBox_Roux_FB->value(), ui->spinBox_Cores->value());

                if (BaseSearch.Solves.empty())
                {
                    ui->statusBar->showMessage(CurrentLang["NoFB"]);
                    ui->textBrowser_report->clear();
                    ui->textBrowser_report->append(CurrentLang["NoRouxFB1"] + QString::fromStdString(Scramble.ToString()) +
												CurrentLang["NoRouxFB2"] + QString::number(ui->spinBox_Roux_FB->value()) +
												CurrentLang["NoRouxFB3"]);
                    AddToHistory(CurrentLang["RouxNoBlock"]);

                    ui->pushButton_CopyReport->setEnabled(true);
                    ui->pushButton_ClearReport->setEnabled(true);
                    ui->pushButton_SaveReport->setEnabled(true);
                    ui->pushButton_StartSearch->setEnabled(true);
                    return;
                }

                SearchRoux.EvaluateFirstBlock(BaseSearch.Solves, ui->spinBox_Roux_Inspections->value());

                const std::chrono::duration<double> fb_elapsed_seconds = std::chrono::system_clock::now() - time_fb_start;
                SearchRoux.SetTimeFB(fb_elapsed_seconds.count());
                SearchRoux.SetDepthFB(ui->spinBox_Roux_FB->value());

                // Add current solves into cache
                CacheUnit CU;
                CU.Scramble = Scramble;
                CU.Depth = ui->spinBox_Roux_FB->value();
                CU.Time = SearchRoux.GetTimeFB();
                CU.Solves = BaseSearch.Solves;
                Cache.push_back(CU);
            }
        }
        else // No cache
        {
            if (!SearchRoux.SearchFirstBlock(ui->spinBox_Roux_FB->value(), ui->spinBox_Roux_Inspections->value()))
			{
				ui->statusBar->showMessage(CurrentLang["NoFB"]);

                ui->textBrowser_report->clear();
                ui->textBrowser_report->append(CurrentLang["NoRouxFB1"] + QString::fromStdString(Scramble.ToString()) +
											CurrentLang["NoRouxFB2"] + QString::number(ui->spinBox_Roux_FB->value()) +
											CurrentLang["NoRouxFB3"]);
				AddToHistory(CurrentLang["RouxNoBlock"]);
				
				ui->pushButton_CopyReport->setEnabled(true);
				ui->pushButton_ClearReport->setEnabled(true);
				ui->pushButton_SaveReport->setEnabled(true);
				ui->pushButton_StartSearch->setEnabled(true);
				return;
			}
        }

        // Start second block search
        ui->textBrowser_report->append(CurrentLang["SearchingSB"]);
        qApp->processEvents();
        SearchRoux.SearchSecondBlocksFirstSquare(ui->spinBox_Roux_SB->value());
        SearchRoux.SearchSecondBlocksSecondSquare(4u);

        if (ui->radioButton_Roux_COLL->isChecked())
        {
            // Start COLL search
            ui->textBrowser_report->append(CurrentLang["SearchingCOLL"]);
            qApp->processEvents();
            SearchRoux.SearchCOLL();
        }
        else
        {
            // Start CMLL search
            ui->textBrowser_report->append(CurrentLang["SearchingCMLL"]);
            qApp->processEvents();
            SearchRoux.SearchCMLL();
        }

        // Start L6E search
        ui->textBrowser_report->append(CurrentLang["SearchingL6E"]);
        qApp->processEvents();
        if (ui->radioButton_Roux_StepsL6E->isChecked())
        {
            SearchRoux.SearchL6EO(10u);
            SearchRoux.SearchL6E2E(12u);
            SearchRoux.SearchL6E(15u);
        }
        else SearchRoux.SearchL6E(16u);

        if (ui->checkBox_Roux_Regrips->isChecked()) SearchRoux.SetRegrips();

        ui->textBrowser_report->clear();

        ui->textBrowser_report->append(QString::fromStdString(SearchRoux.GetReport(ui->checkBox_Roux_Cancellations->isChecked(), false)));
        if (ui->checkBox_Roux_BestSolve->isChecked())
            ui->textBrowser_report->append(QString::fromStdString("Best solve for " + SearchRoux.GetBestReport(ui->checkBox_Roux_Cancellations->isChecked())));
        ui->textBrowser_report->append(QString::fromStdString(SearchRoux.GetTimeReport()));

        AddToHistory(CurrentLang["RouxSolve"]);

        ui->pushButton_CopyReport->setEnabled(true);
        ui->pushButton_ClearReport->setEnabled(true);
        ui->pushButton_SaveReport->setEnabled(true);
        ui->pushButton_StartSearch->setEnabled(true);

        ui->statusBar->showMessage(CurrentLang["SearchFinished"]);
    }

      /*****************************************************************************************************************/
     /*** Petrus ******************************************************************************************************/
    /*****************************************************************************************************************/

    else if (ui->tabWidgetMethods->currentIndex() == 2)
    {
        // Show estimated search time
        ui->statusBar->showMessage(CurrentLang["EstimatedSearchTime"] +
                                   QString::number(static_cast<int>(GetPetrusEstimatedTime(ui->spinBox_Petrus_BlockDepth->value(),
                                                                                           ui->spinBox_Petrus_F2LDepth->value(),
                                                                                           ui->spinBox_Petrus_Inspections->value()))) +
                                   CurrentLang["SecondsUnit"]);

        // Read the allowed layers for cross
        std::vector<Spn> SearchSpins;
        switch (ui->comboBox_Petrus_Orientation->currentIndex())
        {
        case 0: for (int s = 0; s < 24; s++) SearchSpins.push_back(static_cast<Spn>(s)); break; // All

        case 1: SearchSpins.push_back(Spn::UF); SearchSpins.push_back(Spn::UB); SearchSpins.push_back(Spn::UR); SearchSpins.push_back(Spn::UL);
                SearchSpins.push_back(Spn::DF); SearchSpins.push_back(Spn::DB); SearchSpins.push_back(Spn::DR); SearchSpins.push_back(Spn::DL);
            break;
        case 2: SearchSpins.push_back(Spn::FU); SearchSpins.push_back(Spn::FD); SearchSpins.push_back(Spn::FR); SearchSpins.push_back(Spn::FL);
                SearchSpins.push_back(Spn::BU); SearchSpins.push_back(Spn::BD); SearchSpins.push_back(Spn::BR); SearchSpins.push_back(Spn::BL);
            break;
        case 3: SearchSpins.push_back(Spn::RU); SearchSpins.push_back(Spn::RD); SearchSpins.push_back(Spn::RF); SearchSpins.push_back(Spn::RB);
                SearchSpins.push_back(Spn::LU); SearchSpins.push_back(Spn::LD); SearchSpins.push_back(Spn::LF); SearchSpins.push_back(Spn::LB);
            break;

        case 4: SearchSpins.push_back(Spn::UF); SearchSpins.push_back(Spn::UB); SearchSpins.push_back(Spn::UR); SearchSpins.push_back(Spn::UL);
            break;
        case 5: SearchSpins.push_back(Spn::DF); SearchSpins.push_back(Spn::DB); SearchSpins.push_back(Spn::DR); SearchSpins.push_back(Spn::DL);
            break;
        case 6: SearchSpins.push_back(Spn::FU); SearchSpins.push_back(Spn::FD); SearchSpins.push_back(Spn::FR); SearchSpins.push_back(Spn::FL);
            break;
        case 7: SearchSpins.push_back(Spn::BU); SearchSpins.push_back(Spn::BD); SearchSpins.push_back(Spn::BR); SearchSpins.push_back(Spn::BL);
            break;
        case 8: SearchSpins.push_back(Spn::RU); SearchSpins.push_back(Spn::RD); SearchSpins.push_back(Spn::RF); SearchSpins.push_back(Spn::RB);
            break;
        case 9: SearchSpins.push_back(Spn::LU); SearchSpins.push_back(Spn::LD); SearchSpins.push_back(Spn::LF); SearchSpins.push_back(Spn::LB);
            break;

        default: SearchSpins.push_back(static_cast<Spn>(ui->comboBox_Petrus_Orientation->currentIndex() - 10)); break;
        }

        // Create search
        Petrus SearchPetrus(Scramble, ui->spinBox_Cores->value());

        SearchPetrus.SetSearchSpins(SearchSpins);

        SearchPetrus.SetMetric(static_cast<Metrics>(ui->comboBox_Metric->currentIndex()));

        // Start blocks search
        ui->textBrowser_report->append(CurrentLang["SearchingBlocks"]);
        qApp->processEvents();
		
		if (ui->checkBox_Cache->isChecked())
        {
            bool found = false;
            for (const auto& c : Cache) // Search in cache
            {
                if (c.Scramble == Scramble && static_cast<int>(c.Depth) == ui->spinBox_Petrus_BlockDepth->value() && !c.Solves.empty())
                {
                    ui->textBrowser_report->append(CurrentLang["Cache"]);
                    qApp->processEvents();
                    SearchPetrus.EvaluateBlock(c.Solves, ui->spinBox_Petrus_Inspections->value());
                    SearchPetrus.SetTimeBlock(c.Time);
                    SearchPetrus.SetDepthBlock(c.Depth);
                    found = true;
                    break;
                }
            }

            if (!found) // Solves not found in cache
            {
				Cache.clear(); // Cache memory usage can be large, clear previous cache
                const auto time_block_start = std::chrono::system_clock::now();

                DeepSearch BaseSearch(Scramble);
                BaseSearch.SearchBase(ui->spinBox_Petrus_BlockDepth->value(), ui->spinBox_Cores->value());

                if (BaseSearch.Solves.empty())
                {
                    ui->statusBar->showMessage(CurrentLang["NoBlocks"]);
                    ui->textBrowser_report->clear();
                    ui->textBrowser_report->append(CurrentLang["NoPetrusB1"] + QString::fromStdString(Scramble.ToString()) +
												CurrentLang["NoPetrusB2"] + QString::number(ui->spinBox_Petrus_BlockDepth->value()) +
												CurrentLang["NoPetrusB3"]);
                    AddToHistory(CurrentLang["PetrusNoBlock"]);

                    ui->pushButton_CopyReport->setEnabled(true);
                    ui->pushButton_ClearReport->setEnabled(true);
                    ui->pushButton_SaveReport->setEnabled(true);
                    ui->pushButton_StartSearch->setEnabled(true);
                    return;
                }

                SearchPetrus.EvaluateBlock(BaseSearch.Solves, ui->spinBox_Petrus_Inspections->value());

                const std::chrono::duration<double> block_elapsed_seconds = std::chrono::system_clock::now() - time_block_start;
                SearchPetrus.SetTimeBlock(block_elapsed_seconds.count());
                SearchPetrus.SetDepthBlock(ui->spinBox_Petrus_BlockDepth->value());

                // Add current solves into cache
                CacheUnit CU;
                CU.Scramble = Scramble;
                CU.Depth = ui->spinBox_Petrus_BlockDepth->value();
                CU.Time = SearchPetrus.GetTimeBlock();
                CU.Solves = BaseSearch.Solves;
                Cache.push_back(CU);
            }
        }
        else // No cache
        {
            if (!SearchPetrus.SearchBlock(ui->spinBox_Petrus_BlockDepth->value(), ui->spinBox_Petrus_Inspections->value()))
			{
				ui->statusBar->showMessage(CurrentLang["NoBlocks"]);

                ui->textBrowser_report->clear();
                ui->textBrowser_report->append(CurrentLang["NoPetrusB1"] + QString::fromStdString(Scramble.ToString()) +
											CurrentLang["NoPetrusB2"] + QString::number(ui->spinBox_Petrus_BlockDepth->value()) +
											CurrentLang["NoPetrusB3"]);
				AddToHistory(CurrentLang["PetrusNoBlock"]);
				
				ui->pushButton_CopyReport->setEnabled(true);
				ui->pushButton_ClearReport->setEnabled(true);
				ui->pushButton_SaveReport->setEnabled(true);
				ui->pushButton_StartSearch->setEnabled(true);
				return;
			}
        }

        // Start expanded blocks search
        ui->textBrowser_report->append(CurrentLang["SearchingEB"]);
        qApp->processEvents();
        SearchPetrus.SearchExpandedBlock();

        // Start EO search
        ui->textBrowser_report->append(CurrentLang["SearchingEO"]);
        qApp->processEvents();
        SearchPetrus.SearchEO();

        // Start F2L search
        ui->textBrowser_report->append(CurrentLang["SearchingF2L"]);
        qApp->processEvents();
        SearchPetrus.SearchF2L(ui->spinBox_Petrus_F2LDepth->value());

        if (ui->radioButton_Petrus_ZBLL->isChecked())
        {
            // Start ZBLL search
            ui->textBrowser_report->append(CurrentLang["SearchingZBLL"]);
            qApp->processEvents();
            SearchPetrus.SearchZBLL();
        }
        else if (ui->radioButton_Petrus_OCLL_PLL->isChecked())
        {
            // Start OCLL search
            ui->textBrowser_report->append(CurrentLang["SearchingOCLL"]);
            qApp->processEvents();
            SearchPetrus.SearchOCLL();

            // Start PLL search
            ui->textBrowser_report->append(CurrentLang["SearchingPLL"]);
            qApp->processEvents();
            SearchPetrus.SearchPLL();
        }
        else
        {
            // Start COLL search
            ui->textBrowser_report->append(CurrentLang["SearchingCOLL"]);
            qApp->processEvents();
            SearchPetrus.SearchCOLL();

            // Start EPLL search
            ui->textBrowser_report->append(CurrentLang["SearchingEPLL"]);
            qApp->processEvents();
            SearchPetrus.SearchEPLL();
        }

        if (ui->checkBox_Petrus_Regrips->isChecked()) SearchPetrus.SetRegrips();

        ui->textBrowser_report->clear();

        ui->textBrowser_report->append(QString::fromStdString(SearchPetrus.GetReport(ui->checkBox_Petrus_Cancellations->isChecked(), false)));
        if (ui->checkBox_Petrus_BestSolve->isChecked())
            ui->textBrowser_report->append(QString::fromStdString("Best solve for " + SearchPetrus.GetBestReport(ui->checkBox_Petrus_Cancellations->isChecked())));
        ui->textBrowser_report->append(QString::fromStdString(SearchPetrus.GetTimeReport()));

        AddToHistory(CurrentLang["PetrusSolve"]);

        ui->pushButton_CopyReport->setEnabled(true);
        ui->pushButton_ClearReport->setEnabled(true);
        ui->pushButton_SaveReport->setEnabled(true);
        ui->pushButton_StartSearch->setEnabled(true);

        ui->statusBar->showMessage(CurrentLang["SearchFinished"]);
    }

    /*****************************************************************************************************************/
   /*** ZZ **********************************************************************************************************/
  /*****************************************************************************************************************/

  else if (ui->tabWidgetMethods->currentIndex() == 3)
  {
        // Show estimated search time
        ui->statusBar->showMessage(CurrentLang["EstimatedSearchTime"] +
                                   QString::number(static_cast<int>(GetZZEstimatedTime(ui->spinBox_ZZ_EODepth->value(),
                                                                                           ui->spinBox_ZZ_Inspections->value()))) +
                                   CurrentLang["SecondsUnit"]);

		// Read the allowed layers for cross
		std::vector<Spn> SearchSpins;
		switch (ui->comboBox_ZZ_Orientation->currentIndex())
		{
		case 0: for (int s = 0; s < 24; s++) SearchSpins.push_back(static_cast<Spn>(s)); break; // All

		case 1: SearchSpins.push_back(Spn::UF); SearchSpins.push_back(Spn::UB); SearchSpins.push_back(Spn::UR); SearchSpins.push_back(Spn::UL);
				SearchSpins.push_back(Spn::DF); SearchSpins.push_back(Spn::DB); SearchSpins.push_back(Spn::DR); SearchSpins.push_back(Spn::DL);
			break;
		case 2: SearchSpins.push_back(Spn::FU); SearchSpins.push_back(Spn::FD); SearchSpins.push_back(Spn::FR); SearchSpins.push_back(Spn::FL);
				SearchSpins.push_back(Spn::BU); SearchSpins.push_back(Spn::BD); SearchSpins.push_back(Spn::BR); SearchSpins.push_back(Spn::BL);
			break;
		case 3: SearchSpins.push_back(Spn::RU); SearchSpins.push_back(Spn::RD); SearchSpins.push_back(Spn::RF); SearchSpins.push_back(Spn::RB);
				SearchSpins.push_back(Spn::LU); SearchSpins.push_back(Spn::LD); SearchSpins.push_back(Spn::LF); SearchSpins.push_back(Spn::LB);
			break;

		case 4: SearchSpins.push_back(Spn::UF); SearchSpins.push_back(Spn::UB); SearchSpins.push_back(Spn::UR); SearchSpins.push_back(Spn::UL);
			break;
		case 5: SearchSpins.push_back(Spn::DF); SearchSpins.push_back(Spn::DB); SearchSpins.push_back(Spn::DR); SearchSpins.push_back(Spn::DL);
			break;
		case 6: SearchSpins.push_back(Spn::FU); SearchSpins.push_back(Spn::FD); SearchSpins.push_back(Spn::FR); SearchSpins.push_back(Spn::FL);
			break;
		case 7: SearchSpins.push_back(Spn::BU); SearchSpins.push_back(Spn::BD); SearchSpins.push_back(Spn::BR); SearchSpins.push_back(Spn::BL);
			break;
		case 8: SearchSpins.push_back(Spn::RU); SearchSpins.push_back(Spn::RD); SearchSpins.push_back(Spn::RF); SearchSpins.push_back(Spn::RB);
			break;
		case 9: SearchSpins.push_back(Spn::LU); SearchSpins.push_back(Spn::LD); SearchSpins.push_back(Spn::LF); SearchSpins.push_back(Spn::LB);
			break;

		default: SearchSpins.push_back(static_cast<Spn>(ui->comboBox_ZZ_Orientation->currentIndex() - 10)); break;
		}

		// Create search
		ZZ SearchZZ(Scramble, ui->spinBox_Cores->value());

		SearchZZ.SetSearchSpins(SearchSpins);

        SearchZZ.SetMetric(static_cast<Metrics>(ui->comboBox_Metric->currentIndex()));

		// Start EOX search
        ui->textBrowser_report->append(CurrentLang["SearchingEOX"]);
		qApp->processEvents();

		if (ui->checkBox_Cache->isChecked())
		  {
			  bool found = false;
			  for (const auto& c : Cache) // Search in cache
			  {
                  if (c.Scramble == Scramble && static_cast<int>(c.Depth) == ui->spinBox_ZZ_EODepth->value() && !c.Solves.empty())
				  {
                      ui->textBrowser_report->append(CurrentLang["Cache"]);
                      qApp->processEvents();
                      SearchZZ.EvaluateEOX(c.Solves, ui->spinBox_ZZ_Inspections->value());
                      SearchZZ.SetTimeEOX(c.Time);
                      SearchZZ.SetDepthEOX(c.Depth);
					  found = true;
					  break;
				  }
			  }

			  if (!found) // Solves not found in cache
			  {
				  Cache.clear(); // Cache memory usage can be large, clear previous cache
				  const auto time_eoline_start = std::chrono::system_clock::now();

				  DeepSearch BaseSearch(Scramble);
                  BaseSearch.SearchBase(ui->spinBox_ZZ_EODepth->value(), ui->spinBox_Cores->value());

				  if (BaseSearch.Solves.empty())
				  {
					  ui->statusBar->showMessage(CurrentLang["NoEOX"]);
                      ui->textBrowser_report->clear();
                      ui->textBrowser_report->append(CurrentLang["NoZZB1"] + QString::fromStdString(Scramble.ToString()) +
											CurrentLang["NoZZB2"] + QString::number(ui->spinBox_Petrus_BlockDepth->value()) +
											CurrentLang["NoZZB3"]);
					  AddToHistory(CurrentLang["NoEOX"]);

					  ui->pushButton_CopyReport->setEnabled(true);
					  ui->pushButton_ClearReport->setEnabled(true);
					  ui->pushButton_SaveReport->setEnabled(true);
					  ui->pushButton_StartSearch->setEnabled(true);
					  return;
				  }

                  SearchZZ.EvaluateEOX(BaseSearch.Solves, ui->spinBox_ZZ_Inspections->value());

				  const std::chrono::duration<double> eoline_elapsed_seconds = std::chrono::system_clock::now() - time_eoline_start;
                  SearchZZ.SetTimeEOX(eoline_elapsed_seconds.count());
                  SearchZZ.SetDepthEOX(ui->spinBox_ZZ_EODepth->value());

				  // Add current solves into cache
				  CacheUnit CU;
				  CU.Scramble = Scramble;
                  CU.Depth = ui->spinBox_ZZ_EODepth->value();
                  CU.Time = SearchZZ.GetTimeEOX();
				  CU.Solves = BaseSearch.Solves;
				  Cache.push_back(CU);
			  }
		  }
		  else // No cache
		  {
              if (!SearchZZ.SearchEOX(ui->spinBox_ZZ_EODepth->value(), ui->spinBox_ZZ_Inspections->value()))
			{
				ui->statusBar->showMessage(CurrentLang["NoEOX"]);

                ui->textBrowser_report->clear();
                ui->textBrowser_report->append(CurrentLang["NoZZB1"] + QString::fromStdString(Scramble.ToString()) +
											CurrentLang["NoZZB2"] + QString::number(ui->spinBox_Petrus_BlockDepth->value()) +
											CurrentLang["NoZZB3"]);
				AddToHistory(CurrentLang["NoEOX"]);
				
				ui->pushButton_CopyReport->setEnabled(true);
				ui->pushButton_ClearReport->setEnabled(true);
				ui->pushButton_SaveReport->setEnabled(true);
				ui->pushButton_StartSearch->setEnabled(true);
				return;
			}
		  }

		// Start F2L search
        ui->textBrowser_report->append(CurrentLang["SearchingF2L"]);
		qApp->processEvents();
		SearchZZ.SearchF2L();

      if (ui->radioButton_ZZ_ZBLL->isChecked())
      {
          // Start ZBLL search
          ui->textBrowser_report->append(CurrentLang["SearchingZBLL"]);
          qApp->processEvents();
          SearchZZ.SearchZBLL();
      }
      else if (ui->radioButton_ZZ_OCLL_PLL->isChecked())
      {
          // Start OCLL search
          ui->textBrowser_report->append(CurrentLang["SearchingOCLL"]);
          qApp->processEvents();
          SearchZZ.SearchOCLL();

          // Start PLL search
          ui->textBrowser_report->append(CurrentLang["SearchingPLL"]);
          qApp->processEvents();
          SearchZZ.SearchPLL();
      }
      else
      {
          // Start COLL search
          ui->textBrowser_report->append(CurrentLang["SearchingCOLL"]);
          qApp->processEvents();
          SearchZZ.SearchCOLL();

          // Start EPLL search
          ui->textBrowser_report->append(CurrentLang["SearchingEPLL"]);
          qApp->processEvents();
          SearchZZ.SearchEPLL();
      }

      if (ui->checkBox_ZZ_Regrips->isChecked()) SearchZZ.SetRegrips();

      ui->textBrowser_report->clear();

      ui->textBrowser_report->append(QString::fromStdString(SearchZZ.GetReport(ui->checkBox_ZZ_Cancellations->isChecked(), false)));
      if (ui->checkBox_ZZ_BestSolve->isChecked())
          ui->textBrowser_report->append(QString::fromStdString("Best solve for " + SearchZZ.GetBestReport(ui->checkBox_ZZ_Cancellations->isChecked())));
      ui->textBrowser_report->append(QString::fromStdString(SearchZZ.GetTimeReport()));

      AddToHistory(CurrentLang["ZZSolve"]);

      ui->pushButton_CopyReport->setEnabled(true);
      ui->pushButton_ClearReport->setEnabled(true);
      ui->pushButton_SaveReport->setEnabled(true);
      ui->pushButton_StartSearch->setEnabled(true);

      ui->statusBar->showMessage(CurrentLang["SearchFinished"]);
  }

      /*****************************************************************************************************************/
     /*** LBL *********************************************************************************************************/
    /*****************************************************************************************************************/

    else if (ui->tabWidgetMethods->currentIndex() == 4)
    {
        // Read the allowed layers for cross
        Lyr SearchLayer = Lyr::U;
        if (ui->radioButton_LBL_U->isChecked()) SearchLayer = Lyr::U;
        if (ui->radioButton_LBL_D->isChecked()) SearchLayer = Lyr::D;
        if (ui->radioButton_LBL_F->isChecked()) SearchLayer = Lyr::F;
        if (ui->radioButton_LBL_B->isChecked()) SearchLayer = Lyr::B;
        if (ui->radioButton_LBL_R->isChecked()) SearchLayer = Lyr::R;
        if (ui->radioButton_LBL_L->isChecked()) SearchLayer = Lyr::L;

        // Show estimated search time
        ui->statusBar->showMessage(CurrentLang["EstimatedSearchTime"] +
                                   QString::number(static_cast<int>(GetLBLEstimatedTime())) +
                                   CurrentLang["SecondsUnit"]);

        // Create search
        LBL SearchLBL(Scramble, ui->spinBox_Cores->value());

        SearchLBL.SetMetric(static_cast<Metrics>(ui->comboBox_Metric->currentIndex()));

        // Start cross search
        ui->textBrowser_report->append(CurrentLang["SearchingCross"]);
        qApp->processEvents();

        if (!SearchLBL.SearchFLCross(SearchLayer))
        {
            ui->statusBar->showMessage(CurrentLang["SearchFinishedCrossNotFound"]);
            ui->textBrowser_report->clear();
            ui->textBrowser_report->append(QString::fromStdString(SearchLBL.GetReport()));
            AddToHistory(CurrentLang["LBLNoCross"]);

            ui->pushButton_CopyReport->setEnabled(true);
            ui->pushButton_ClearReport->setEnabled(true);
            ui->pushButton_SaveReport->setEnabled(true);
            ui->pushButton_StartSearch->setEnabled(true);
            return;
        }

        // Start first layer search
        ui->textBrowser_report->append(CurrentLang["SearchingFL"]);
        qApp->processEvents();
        SearchLBL.SearchFLCorners();

        // Start second layer search
        ui->textBrowser_report->append(CurrentLang["SearchingSL"]);
        qApp->processEvents();
        SearchLBL.SearchSLEdges();

        // Start last layer search
        ui->textBrowser_report->append(CurrentLang["SearchingLL"]);
        qApp->processEvents();
        SearchLBL.SearchLLCross1();
        SearchLBL.SearchLLCross2();
        SearchLBL.SearchLLCorners1();
        SearchLBL.SearchLLCorners2();

        ui->textBrowser_report->clear();
        ui->textBrowser_report->append(QString::fromStdString(SearchLBL.GetReport()));
        AddToHistory(CurrentLang["LBLSolve"]);

        ui->pushButton_CopyReport->setEnabled(true);
        ui->pushButton_ClearReport->setEnabled(true);
        ui->pushButton_SaveReport->setEnabled(true);
        ui->pushButton_StartSearch->setEnabled(true);

        ui->statusBar->showMessage(CurrentLang["SearchFinished"]);
    }

      /*****************************************************************************************************************/
     /*** No method ***************************************************************************************************/
    /*****************************************************************************************************************/

    else
    {
        ui->statusBar->showMessage(CurrentLang["NoMethod"]);
        ui->textBrowser_report->clear();
    }
}

void MainWindow::on_pushButton_ZoomPlus_clicked()
{
    ui->textBrowser_report->zoomIn();
}


void MainWindow::on_pushButton_ZoomMinus_clicked()
{
    ui->textBrowser_report->zoomOut();
}

