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
#include <QRadioButton>
#include <QThread>

#include "tinyxml2.h" // For xml language files

#include "cfop.h"
#include "roux.h"
#include "lbl.h"
#include "petrus.h"
#include "zz.h"
#include "yruru.h"
#include "mehta.h"

#include "collection.h"

#include "deep_eval.h"

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

DeepEval Evaluation(Algorithm(""), 5u); // Scrambles evaluation

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
    ui->comboBox_Method->setCurrentIndex(1); // CFOP as default method

    SolveId = 0u; // Index for solves history
}

MainWindow::~MainWindow()
{
    delete ui;
}

  /*****************************************************************************************************************/
 /*** CREDIT, LICENSE & HELP FUNCTIONS ****************************************************************************/
/*****************************************************************************************************************/

void MainWindow::on_pushButton_Debug_clicked()
{
    QString summary = CurrentLang["LoadExternal"];
    summary.append("\n\t");
    summary.append(CurrentLang["LoadCancel2"]);
    summary.append(std::to_string(Algorithm::GetCancellation2Size() >> 1).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadCancel3"]);
    summary.append(std::to_string(Algorithm::GetCancellation3Size() >> 1).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadOLL"]);
    summary.append(std::to_string(Collection::GetOLLCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadPLL"]);
    summary.append(std::to_string(Collection::GetPLLCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["Load1LLL"]);
    summary.append(std::to_string(Collection::Get1LLLCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadZBLL"]);
    summary.append(std::to_string(Collection::GetZBLLCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadOCLL"]);
    summary.append(std::to_string(Collection::GetOCLLCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadCMLL"]);
    summary.append(std::to_string(Collection::GetCMLLCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadCOLL"]);
    summary.append(std::to_string(Collection::GetCOLLCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadEPLL"]);
    summary.append(std::to_string(Collection::GetEPLLCases()).c_str());

    summary.append("\n\t");
    summary.append(CurrentLang["LoadEOLE"]);
    summary.append(std::to_string(Collection::GetEOLECases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["Load6CO"]);
    summary.append(std::to_string(Collection::Get6COCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["Load6CP"]);
    summary.append(std::to_string(Collection::Get6CPCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadAPDR"]);
    summary.append(std::to_string(Collection::GetAPDRCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadL5EP"]);
    summary.append(std::to_string(Collection::GetL5EPCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadCDRLL"]);
    summary.append(std::to_string(Collection::GetCDRLLCases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadJTLE"]);
    summary.append(std::to_string(Collection::GetJTLECases()).c_str());
    summary.append("\n\t");
    summary.append(CurrentLang["LoadTDR"]);
    summary.append(std::to_string(Collection::GetTDRCases()).c_str());

    ui->textBrowser_report->setText(summary);
	
	ui->pushButton_CopyReport->setEnabled(true);
    ui->pushButton_SaveReport->setEnabled(true);
    ui->pushButton_ClearReport->setEnabled(true);
}

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

    ui->textBrowser_report->setHtml(credits_text);

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

    ui->textBrowser_report->setText(license_text);
    ui->pushButton_CopyReport->setEnabled(true);
    ui->pushButton_SaveReport->setEnabled(true);
    ui->pushButton_ClearReport->setEnabled(true);
}

void MainWindow::on_pushButton_Help_clicked()
{
    QFile file(CurrentLang["FileHelp"]);
    QString help_text;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        help_text.append(stream.readAll());
    }
    else help_text = CurrentLang["NoHelp"];

    ui->textBrowser_report->setHtml(help_text);
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

    // horizontalLayout_Language

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->pushButton_Credits->objectName().toStdString().c_str());
    pElement->SetText(ui->pushButton_Credits->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->pushButton_License->objectName().toStdString().c_str());
    pElement->SetText(ui->pushButton_License->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->pushButton_Help->objectName().toStdString().c_str());
    pElement->SetText(ui->pushButton_Help->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_Cache->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_Cache->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_Cores->objectName().toStdString().c_str());
    pElement->SetText(ui->label_Cores->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_Language->objectName().toStdString().c_str());
    pElement->SetText(ui->label_Language->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    // LayoutHorizontal_Scramble

	pElement = lang_xml.NewElement("FixedText");
	pElement->SetAttribute("ObjectName", ui->label_ScrambleTitle->objectName().toStdString().c_str());
	pElement->SetText(ui->label_ScrambleTitle->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->pushButton_RandomScramble->objectName().toStdString().c_str());
    pElement->SetText(ui->pushButton_RandomScramble->text().toStdString().c_str());
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

    // horizontalLayout_Method

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_Method->objectName().toStdString().c_str());
    pElement->SetText(ui->label_Method->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_Variant->objectName().toStdString().c_str());
    pElement->SetText(ui->label_Variant->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_Option->objectName().toStdString().c_str());
    pElement->SetText(ui->label_Option->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_Speed->objectName().toStdString().c_str());
    pElement->SetText(ui->label_Speed->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_Orientation->objectName().toStdString().c_str());
    pElement->SetText(ui->label_Orientation->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_Amount->objectName().toStdString().c_str());
    pElement->SetText(ui->label_Amount->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->label_Metric->objectName().toStdString().c_str());
    pElement->SetText(ui->label_Metric->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_Regrips->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_Regrips->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->checkBox_Cancellations->objectName().toStdString().c_str());
    pElement->SetText(ui->checkBox_Cancellations->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->pushButton_StartSearch->objectName().toStdString().c_str());
    pElement->SetText(ui->pushButton_StartSearch->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    // horizontalLayout_Report

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->pushButton_ZoomMinus->objectName().toStdString().c_str());
    pElement->SetText(ui->pushButton_ZoomMinus->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->pushButton_ZoomPlus->objectName().toStdString().c_str());
    pElement->SetText(ui->pushButton_ZoomPlus->text().toStdString().c_str());
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
	pElement->SetAttribute("ObjectName", ui->label_History->objectName().toStdString().c_str());
	pElement->SetText(ui->label_History->text().toStdString().c_str());
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("FixedText");
    pElement->SetAttribute("ObjectName", ui->pushButton_ClearHistory->objectName().toStdString().c_str());
    pElement->SetText(ui->pushButton_ClearHistory->text().toStdString().c_str());
    pRoot->InsertEndChild(pElement);

	// Messages

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "MainWindowTitle");
    pElement->SetText("SpeedSolving Master beta 2");
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "Cache");
    pElement->SetText("[Evaluating from cache]");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "CacheOn");
    pElement->SetText("Cache enabled, note that slow searchs cause high memory usage");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "CacheOff");
    pElement->SetText("Cache disabled");
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
    pElement->SetAttribute("Id", "None");
    pElement->SetText("None");
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
    pElement->SetAttribute("Id", "SearchStarted");
    pElement->SetText("Search in progress...");
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
    pElement->SetAttribute("Id", "NoHelp");
    pElement->SetText("Help file not found!");
    pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SolveRecoveredHistory");
	pElement->SetText("Solve recovered from history");
	pRoot->InsertEndChild(pElement);

    // LBL

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingCross");
    pElement->SetText("Searching cross...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingFL");
    pElement->SetText("Searching first layer corners...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingSL");
    pElement->SetText("Searching second layer edges...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingLL");
    pElement->SetText("Searching last layer...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LBLSolve");
    pElement->SetText("LBL solve");
    pRoot->InsertEndChild(pElement);

    // CFOP

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "CFOPSolve");
    pElement->SetText("CFOP solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingF2L");
    pElement->SetText("Searching F2L...");
    pRoot->InsertEndChild(pElement);

    // Roux

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "RouxSolve");
    pElement->SetText("Roux solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingFB");
    pElement->SetText("Searching first blocks...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoFB");
    pElement->SetText("First blocks not found!");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingSB");
    pElement->SetText("Searching second blocks...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingCOLL");
    pElement->SetText("Searching COLL...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingCMLL");
    pElement->SetText("Searching CMLL...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingL6E");
    pElement->SetText("Searching last six edges...");
    pRoot->InsertEndChild(pElement);

    // Petrus

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "PetrusSolve");
    pElement->SetText("Petrus solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingBlocks");
    pElement->SetText("Searching blocks...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoPetrusBlock");
    pElement->SetText("First blocks not found!");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingEB");
    pElement->SetText("Searching expanded block...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingEO");
    pElement->SetText("Searching edges orientation...");
    pRoot->InsertEndChild(pElement);

    // ZZ

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "ZZSolve");
    pElement->SetText("ZZ solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingEOX");
    pElement->SetText("Searching EO X...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoZZBlock");
    pElement->SetText("No ZZ blocks found!");
    pRoot->InsertEndChild(pElement);

    // CEOR

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "CEORSolve");
    pElement->SetText("CEOR solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingCPLines");
    pElement->SetText("Searching Lines+CP...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingLines");
    pElement->SetText("Searching Lines...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingCP");
    pElement->SetText("Searching CP...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoLines");
    pElement->SetText("No Lines found!");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoCPLines");
    pElement->SetText("No CP Lines found!");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingEOBF");
    pElement->SetText("Searching EOBF...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingpEOExtension");
    pElement->SetText("Searching pEO-extension...");
    pRoot->InsertEndChild(pElement);

    // Mehta

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "MehtaSolve");
    pElement->SetText("Mehta solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "Searching3QB");
    pElement->SetText("Searching 3QB...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingEOLE");
    pElement->SetText("Searching EOLE...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "Searching6CO");
    pElement->SetText("Searching 6CO...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "Searching6CP");
    pElement->SetText("Searching 6CP...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingL5EP");
    pElement->SetText("Searching L5EP...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingAPDR");
    pElement->SetText("Searching APDR...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingPLL");
    pElement->SetText("Searching PLL...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingDCAL");
    pElement->SetText("Searching DCAL...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingCDRLL");
    pElement->SetText("Searching CDRLL...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingJTLE");
    pElement->SetText("Searching JTLE...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingTDR");
    pElement->SetText("Searching TDR...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingZBLL");
    pElement->SetText("Searching ZBLL...");
    pRoot->InsertEndChild(pElement);

    // External files messages

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

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadEOLE");
    pElement->SetText("EOLE algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "Load6CO");
    pElement->SetText("6CO algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "Load6CP");
    pElement->SetText("6CP algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadAPDR");
    pElement->SetText("APDR algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadL5EP");
    pElement->SetText("L5EP algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadCDRLL");
    pElement->SetText("CDRLL algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadJTLE");
    pElement->SetText("JTLE algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadTDR");
    pElement->SetText("TDR algorithms loaded: ");
    pRoot->InsertEndChild(pElement);

    // Evaluation

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoEval");
    pElement->SetText("No evaluation");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "EvalVeryEasy");
    pElement->SetText("Very easy");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "EvalEasy");
    pElement->SetText("Easy");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "EvalMediumEasy");
    pElement->SetText("Medium-Easy");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "EvalMedium");
    pElement->SetText("Medium");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "EvalMediumHard");
    pElement->SetText("Medium-Hard");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "EvalHard");
    pElement->SetText("Hard");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "EvalVeryHard");
    pElement->SetText("Very hard");
    pRoot->InsertEndChild(pElement);

	// Filenames
	
	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "FileCredits");
    pElement->SetText("CREDITS.html");
	pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "FileLicense");
	pElement->SetText("COPYING");
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "FileHelp");
    pElement->SetText("HELP.html");
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

// Change the language
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

void MainWindow::GetSearchSpins(std::vector<Spn>& SearchSpins)
{
    SearchSpins.clear();

    switch (ui->comboBox_Orientation->currentIndex())
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

    default: SearchSpins.push_back(Spn::UF); break;
    }
}

void MainWindow::on_lineEdit_Scramble_textChanged(const QString &scrInput)
{
    if (scrInput.isEmpty()) // No scramble
    {
        ui->pushButton_CopyScramble->setDisabled(true);
        ui->pushButton_ClearScramble->setDisabled(true);
        ui->pushButton_StartSearch->setDisabled(true);

        ui->statusBar->showMessage(CurrentLang["NoScramble"]);
        ui->label_Eval->setText(CurrentLang["NoEval"]);
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

        if (A.GetSize() < 12u) // Scramble too short to evaluate
        {
            ui->label_Eval->setText(CurrentLang["NoEval"]);
        }
        else
        {
            // Read the allowed orientations
            std::vector<Spn> SearchSpins;
            GetSearchSpins(SearchSpins);

            // Get the scramble evaluation
            Evaluation.SetNewScramble(A);
            Evaluation.StandardGroups(SearchSpins);
            Evaluation.Run();
            float Score = Evaluation.GetScore();
            if (Score <= 0.0f) ui->label_Eval->setText(CurrentLang["NoEval"]);
            else if (Score < 6.221367f) ui->label_Eval->setText(CurrentLang["EvalVeryHard"]);
            else if (Score < 6.804842f) ui->label_Eval->setText(CurrentLang["EvalHard"]);
            else if (Score < 7.311247f) ui->label_Eval->setText(CurrentLang["EvalMediumHard"]);
            else if (Score < 8.007008f) ui->label_Eval->setText(CurrentLang["EvalMedium"]);
            else if (Score < 8.523252f) ui->label_Eval->setText(CurrentLang["EvalMediumEasy"]);
            else if (Score < 9.060186f) ui->label_Eval->setText(CurrentLang["EvalEasy"]);
            else ui->label_Eval->setText(CurrentLang["EvalVeryEasy"]);
        }
    }
}

void MainWindow::on_comboBox_Orientation_currentIndexChanged(int index)
{
    on_lineEdit_Scramble_textChanged(ui->lineEdit_Scramble->text());
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
 /*** THREADS & CACHE FUNCTIONS ***********************************************************************************/
/*****************************************************************************************************************/

void MainWindow::on_spinBox_Cores_valueChanged(int cores)
{
    if (cores == 0) ui->statusBar->showMessage(CurrentLang["AllAvaliable"] + CurrentLang["CoresWillBeUsedInTheSearch"]);
    else if (cores == 1) ui->statusBar->showMessage(CurrentLang["SingleCoreWillBeUsedInTheSearch"]);
    else ui->statusBar->showMessage(QString::number(cores) + CurrentLang["CoresWillBeUsedInTheSearch"]);
}

void MainWindow::on_checkBox_Cache_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        ui->statusBar->showMessage(CurrentLang["CacheOn"]);
    }
    else // Cache disabled
    {
        ui->statusBar->showMessage(CurrentLang["CacheOff"]);
    }
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

void MainWindow::on_pushButton_ZoomPlus_clicked()
{
    ui->textBrowser_report->zoomIn();
}

void MainWindow::on_pushButton_ZoomMinus_clicked()
{
    ui->textBrowser_report->zoomOut();
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
    QString Solve = ui->textBrowser_report->toHtml();
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
 /*** METHOD & SEARCH FUNCTIONS ***********************************************************************************/
/*****************************************************************************************************************/

void MainWindow::on_comboBox_Method_currentIndexChanged(const QString &arg1)
{
    ui->comboBox_Variant->clear();
    ui->comboBox_Option->clear();

    switch (ui->comboBox_Method->currentIndex())
    {
    case 1: // CFOP
        ui->comboBox_Variant->addItem("OLL+PLL");
        ui->comboBox_Variant->addItem("1LLL");
        ui->comboBox_Variant->addItem("EO+ZBLL");
        ui->comboBox_Option->addItem(CurrentLang["None"]); break;
    case 2: // Roux
        ui->comboBox_Variant->addItem("CMLL");
        ui->comboBox_Variant->addItem("COLL");
        ui->comboBox_Option->addItem("L6E step by step");
        ui->comboBox_Option->addItem("One-look L6E"); break;
    case 3: // Petrus
        ui->comboBox_Variant->addItem("ZBLL");
        ui->comboBox_Variant->addItem("OCLL+PLL");
        ui->comboBox_Variant->addItem("COLL+EPLL");
        ui->comboBox_Option->addItem(CurrentLang["None"]); break;
    case 4: // ZZ
        ui->comboBox_Variant->addItem("ZBLL");
        ui->comboBox_Variant->addItem("OCLL+PLL");
        ui->comboBox_Variant->addItem("COLL+EPLL");
        ui->comboBox_Option->addItem(CurrentLang["None"]); break;
    case 5: // CEOR
        ui->comboBox_Variant->addItem("YruRU");
        ui->comboBox_Option->addItem("CP-Skip");
        ui->comboBox_Option->addItem("No CP-Skip"); break;
    case 6: // Mehta
        ui->comboBox_Variant->addItem("TDR");
        ui->comboBox_Variant->addItem("6CP");
        ui->comboBox_Variant->addItem("APDR");
        ui->comboBox_Variant->addItem("CDRLL");
        ui->comboBox_Variant->addItem("JTLE");
        ui->comboBox_Option->addItem(CurrentLang["None"]); break;
    default: // LBL
        ui->comboBox_Variant->addItem(CurrentLang["None"]);
        ui->comboBox_Option->addItem(CurrentLang["None"]); break;
    }
}

void MainWindow::on_pushButton_StartSearch_clicked()
{
    // Clear output and disable controls during search
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

    ui->statusBar->showMessage(CurrentLang["SearchStarted"]);

    switch (ui->comboBox_Method->currentIndex())
    {
    case 0: sLBL(Scramble.ToString()); break;
    case 1: sCFOP(Scramble.ToString()); break;
    case 2: sRoux(Scramble.ToString()); break;
    case 3: sPetrus(Scramble.ToString()); break;
    case 4: sZZ(Scramble.ToString()); break;
    case 5: sCEOR(Scramble.ToString()); break;
    case 6: sMehta(Scramble.ToString()); break;
    // case 7: sNautilus(Scramble.ToString()); break;

    default: break;
    }

    // Enable controls after search
    ui->pushButton_CopyReport->setEnabled(true);
    ui->pushButton_ClearReport->setEnabled(true);
    ui->pushButton_SaveReport->setEnabled(true);
    ui->pushButton_StartSearch->setEnabled(true);

    ui->statusBar->showMessage(CurrentLang["SearchFinished"]);
}

  /*****************************************************************************************************************/
 /*** LBL *********************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sLBL(const std::string& Scramble)
{
    // Select the layer for cross
    Lyr SearchLayer;
    switch (ui->comboBox_Orientation->currentIndex())
    {
    case 0: case 1: case 4: SearchLayer = Lyr::D; break;
    case 2: case 6: SearchLayer = Lyr::B; break;
    case 3: case 8: SearchLayer = Lyr::L; break;
    case 5: SearchLayer = Lyr::U; break;
    case 7: SearchLayer = Lyr::F; break;
    case 9: SearchLayer = Lyr::R; break;
    default: SearchLayer = Lyr::U; break;
    }

    // Create search
    LBL* SearchLBL = new LBL(Algorithm(Scramble), ui->spinBox_Cores->value());

    SearchLBL->SetMetric(static_cast<Metrics>(ui->comboBox_Metric->currentIndex()));

    // Start cross search
    ui->textBrowser_report->append(CurrentLang["SearchingCross"]);
    qApp->processEvents();
    if (!SearchLBL->SearchFLCross(SearchLayer))
    {
        ui->textBrowser_report->clear();
        ui->textBrowser_report->append(QString::fromStdString(SearchLBL->GetReport()));
        return;
    }

    // Complete first layer search
    ui->textBrowser_report->append(CurrentLang["SearchingFL"]);
    qApp->processEvents();
    SearchLBL->SearchFLCorners();

    // Start second layer search
    ui->textBrowser_report->append(CurrentLang["SearchingSL"]);
    qApp->processEvents();
    SearchLBL->SearchSLEdges();

    // Start last layer search
    ui->textBrowser_report->append(CurrentLang["SearchingLL"]);
    qApp->processEvents();
    SearchLBL->SearchLLCross1();
    SearchLBL->SearchLLCross2();
    SearchLBL->SearchLLCorners1();
    SearchLBL->SearchLLCorners2();

    ui->textBrowser_report->setText(QString::fromStdString(SearchLBL->GetReport()));
    // ui->textBrowser_report->setHtml(QString::fromStdString(SearchLBL->GetHtmlReport()));

    AddToHistory(CurrentLang["LBLSolve"]);

    delete SearchLBL;
}

  /*****************************************************************************************************************/
 /*** CFOP ********************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sCFOP(const std::string& Scr)
{
    const Algorithm Scramble(Scr);

    // Read the allowed layers for cross
    std::vector<Lyr> CrossLayers;
    switch (ui->comboBox_Orientation->currentIndex())
    {
    case 0: CrossLayers.push_back(Lyr::U); CrossLayers.push_back(Lyr::D);
            CrossLayers.push_back(Lyr::F); CrossLayers.push_back(Lyr::B);
            CrossLayers.push_back(Lyr::R); CrossLayers.push_back(Lyr::L); break;
    case 1: CrossLayers.push_back(Lyr::U); CrossLayers.push_back(Lyr::D); break;
    case 2: CrossLayers.push_back(Lyr::F); CrossLayers.push_back(Lyr::B); break;
    case 3: CrossLayers.push_back(Lyr::R); CrossLayers.push_back(Lyr::L); break;
    case 4: CrossLayers.push_back(Lyr::D); break;
    case 5: CrossLayers.push_back(Lyr::U); break;
    case 6: CrossLayers.push_back(Lyr::B); break;
    case 7: CrossLayers.push_back(Lyr::F); break;
    case 8: CrossLayers.push_back(Lyr::L); break;
    case 9: CrossLayers.push_back(Lyr::R); break;
    default: CrossLayers.push_back(Lyr::U); break;
    }

    uint Depth;
    switch (ui->comboBox_Speed->currentIndex())
    {
    case 0: Depth = 6u; break;
    case 1: Depth = 7u; break;
    case 2: Depth = 8u; break;
    case 3: Depth = 9u; break;
    default: Depth = 6u; break;
    }

    uint Inspections;
    switch (ui->comboBox_Amount->currentIndex())
    {
    case 0: Inspections = 1u; break;
    case 1: Inspections = 4u; break;
    case 2: Inspections = 16u; break;
    case 3: Inspections = 32u; break;
    case 4: Inspections = 64u; break;
    default: Inspections = 1u; break;
    }

    // Create search
    CFOP* SearchCFOP = new CFOP(Algorithm(Scramble), ui->spinBox_Cores->value());

    SearchCFOP->SetCrossLayers(CrossLayers);

    SearchCFOP->SetMetric(static_cast<Metrics>(ui->comboBox_Metric->currentIndex()));

    // Start crosses search
    ui->textBrowser_report->append(CurrentLang["SearchingCross"]);
    qApp->processEvents();

    if (ui->checkBox_Cache->isChecked())
    {
        bool found = false;
        for (const auto& c : Cache) // Search in cache
        {
            if (c.Scramble == Scramble && c.Depth >= Depth && !c.Solves.empty())
            {
                ui->textBrowser_report->append(CurrentLang["Cache"]);
                qApp->processEvents();
                SearchCFOP->EvaluateCrosses(c.Solves, Inspections);
                SearchCFOP->SetTimeCrosses(c.Time);
                SearchCFOP->SetDepthCrosses(c.Depth);
                found = true;
                break;
            }
        }

        if (!found) // Solves not found in cache
        {
            Cache.clear(); // Cache memory usage can be large, clear previous cache
            const auto time_cross_start = std::chrono::system_clock::now();

            DeepSearch BaseSearch(Scramble);
            BaseSearch.SearchBase(Depth, Plc::BEST_SOLVES, ui->spinBox_Cores->value());

            if (BaseSearch.Solves.empty())
            {
                ui->textBrowser_report->clear();
                ui->textBrowser_report->append(QString::fromStdString(SearchCFOP->GetReport(false, false)));
                return;
            }

            SearchCFOP->EvaluateCrosses(BaseSearch.Solves, Inspections);

            const std::chrono::duration<double> cross_elapsed_seconds = std::chrono::system_clock::now() - time_cross_start;
            SearchCFOP->SetTimeCrosses(cross_elapsed_seconds.count());
            SearchCFOP->SetDepthCrosses(Depth);

            // Add current solves into cache
            CacheUnit CU;
            CU.Scramble = Scramble;
            CU.Depth = Depth;
            CU.Time = SearchCFOP->GetTime();
            CU.Solves = BaseSearch.Solves;
            Cache.push_back(CU);
        }
    }
    else // No cache
    {
        if (!SearchCFOP->SearchCrosses(Depth, Inspections))
        {
            ui->textBrowser_report->clear();
            ui->textBrowser_report->append(QString::fromStdString(SearchCFOP->GetReport(false, false)));
            return;
        }
    }

    // Start F2L search
    ui->textBrowser_report->append(CurrentLang["SearchingF2L"]);
    qApp->processEvents();
    SearchCFOP->SearchF2L();

    // Start last layer search
    ui->textBrowser_report->append(CurrentLang["SearchingLL"]);
    qApp->processEvents();
    if (ui->comboBox_Variant->currentIndex() == 1)
    {
        SearchCFOP->Search1LLL();
    }
    else if (ui->comboBox_Variant->currentIndex() == 2)
    {
        SearchCFOP->SearchEOLL();
        SearchCFOP->SearchZBLL();
    }
    else // Default
    {
        SearchCFOP->SearchOLL();
        SearchCFOP->SearchPLL();
    }

    if (ui->checkBox_Regrips->isChecked()) SearchCFOP->SetRegrips();

    ui->textBrowser_report->clear();

    ui->textBrowser_report->append(QString::fromStdString(SearchCFOP->GetReport(ui->checkBox_Cancellations->isChecked(), false)));

    ui->textBrowser_report->append(QString::fromStdString("Best solve for " + SearchCFOP->GetBestReport(ui->checkBox_Cancellations->isChecked())));
    ui->textBrowser_report->append(QString::fromStdString(SearchCFOP->GetTimeReport()));
    AddToHistory(CurrentLang["CFOPSolve"]);

    delete SearchCFOP;
}

  /*****************************************************************************************************************/
 /*** Roux ********************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sRoux(const std::string& Scr)
{
    const Algorithm Scramble(Scr);

    // Read the allowed orientations
    std::vector<Spn> SearchSpins;
    GetSearchSpins(SearchSpins);

    uint Depth1, Depth2;
    switch (ui->comboBox_Speed->currentIndex())
    {
    case 0: Depth1 = 6u; Depth2 = 6u; break;
    case 1: Depth1 = 7u; Depth2 = 6u; break;
    case 2: Depth1 = 8u; Depth2 = 7u; break;
    case 3: Depth1 = 8u; Depth2 = 8u; break;
    default: Depth1 = 6u; Depth2 = 6u; break;
    }

    uint Inspections;
    switch (ui->comboBox_Amount->currentIndex())
    {
    case 0: Inspections = 1u; break;
    case 1: Inspections = 4u; break;
    case 2: Inspections = 16u; break;
    case 3: Inspections = 32u; break;
    case 4: Inspections = 64u; break;
    default: Inspections = 1u; break;
    }

    // Create search
    Roux* SearchRoux = new Roux(Scramble, ui->spinBox_Cores->value());

    SearchRoux->SetSearchSpins(SearchSpins);

    SearchRoux->SetMetric(static_cast<Metrics>(ui->comboBox_Metric->currentIndex()));

    // Start first block search
    ui->textBrowser_report->append(CurrentLang["SearchingFB"]);
    qApp->processEvents();

    if (ui->checkBox_Cache->isChecked())
    {
        bool found = false;
        for (const auto& c : Cache) // Search in cache
        {
            if (c.Scramble == Scramble && c.Depth >= Depth1 && !c.Solves.empty())
            {
                ui->textBrowser_report->append(CurrentLang["Cache"]);
                qApp->processEvents();
                SearchRoux->EvaluateFirstBlock(c.Solves, Inspections);
                SearchRoux->SetTimeFB(c.Time);
                SearchRoux->SetDepthFB(c.Depth);
                found = true;
                break;
            }
        }

        if (!found) // Solves not found in cache
        {
            Cache.clear(); // Cache memory usage can be large, clear previous cache
            const auto time_fb_start = std::chrono::system_clock::now();

            DeepSearch BaseSearch(Scramble);
            BaseSearch.SearchBase(Depth1, Plc::BEST_SOLVES, ui->spinBox_Cores->value());

            if (BaseSearch.Solves.empty())
            {
                ui->textBrowser_report->clear();
                ui->textBrowser_report->append(CurrentLang["NoFB"]);
                return;
            }

            SearchRoux->EvaluateFirstBlock(BaseSearch.Solves, Inspections);

            const std::chrono::duration<double> fb_elapsed_seconds = std::chrono::system_clock::now() - time_fb_start;
            SearchRoux->SetTimeFB(fb_elapsed_seconds.count());
            SearchRoux->SetDepthFB(Depth1);

            // Add current solves into cache
            CacheUnit CU;
            CU.Scramble = Scramble;
            CU.Depth = Depth1;
            CU.Time = SearchRoux->GetTimeFB();
            CU.Solves = BaseSearch.Solves;
            Cache.push_back(CU);
        }
    }
    else // No cache
    {
        if (!SearchRoux->SearchFirstBlock(Depth1, Inspections))
        {
            ui->textBrowser_report->clear();
            ui->textBrowser_report->append(CurrentLang["NoFB"]);
            return;
        }
    }

    // Start second blocks search
    ui->textBrowser_report->append(CurrentLang["SearchingSB"]);
    qApp->processEvents();
    SearchRoux->SearchSecondBlocksFirstSquare(Depth2);
    SearchRoux->SearchSecondBlocksSecondSquare(4u);

    if (ui->comboBox_Variant->currentIndex() == 1)
    {
        // Start COLL search
        ui->textBrowser_report->append(CurrentLang["SearchingCOLL"]);
        qApp->processEvents();
        SearchRoux->SearchCOLL();
    }
    else // if (ui->comboBox_Variant->currentIndex() == 0)
    {
        // Start CMLL search
        ui->textBrowser_report->append(CurrentLang["SearchingCMLL"]);
        qApp->processEvents();
        SearchRoux->SearchCMLL();
    }

    // Start L6E search
    ui->textBrowser_report->append(CurrentLang["SearchingL6E"]);
    qApp->processEvents();
    if (ui->comboBox_Option->currentIndex() == 1)
    {
        SearchRoux->SearchL6E(16u);
    }
    else // if (ui->comboBox_Option->currentIndex() == 0)
    {
        SearchRoux->SearchL6EO(10u);
        SearchRoux->SearchL6E2E(12u);
        SearchRoux->SearchL6E(15u);
    }

    if (ui->checkBox_Regrips->isChecked()) SearchRoux->SetRegrips();

    ui->textBrowser_report->clear();

    ui->textBrowser_report->append(QString::fromStdString(SearchRoux->GetReport(ui->checkBox_Cancellations->isChecked(), false)));
    ui->textBrowser_report->append(QString::fromStdString("Best solve for " + SearchRoux->GetBestReport(ui->checkBox_Cancellations->isChecked())));
    ui->textBrowser_report->append(QString::fromStdString(SearchRoux->GetTimeReport()));

    AddToHistory(CurrentLang["RouxSolve"]);

    delete SearchRoux;
}

  /*****************************************************************************************************************/
 /*** Petrus ******************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sPetrus(const std::string& Scr)
{
    const Algorithm Scramble(Scr);

    // Read the allowed orientations
    std::vector<Spn> SearchSpins;
    GetSearchSpins(SearchSpins);

    uint Depth1, Depth2;
    switch (ui->comboBox_Speed->currentIndex())
    {
    case 0: Depth1 = 6u; Depth2 = 6u; break;
    case 1: Depth1 = 7u; Depth2 = 6u; break;
    case 2: Depth1 = 8u; Depth2 = 7u; break;
    case 3: Depth1 = 8u; Depth2 = 8u; break;
    default: Depth1 = 6u; Depth2 = 6u; break;
    }

    uint Inspections;
    switch (ui->comboBox_Amount->currentIndex())
    {
    case 0: Inspections = 1u; break;
    case 1: Inspections = 4u; break;
    case 2: Inspections = 16u; break;
    case 3: Inspections = 32u; break;
    case 4: Inspections = 64u; break;
    default: Inspections = 1u; break;
    }

    // Create search
    Petrus* SearchPetrus = new Petrus(Scramble, ui->spinBox_Cores->value());

    SearchPetrus->SetSearchSpins(SearchSpins);

    SearchPetrus->SetMetric(static_cast<Metrics>(ui->comboBox_Metric->currentIndex()));

    // Start blocks search
    ui->textBrowser_report->append(CurrentLang["SearchingBlocks"]);
    qApp->processEvents();

    if (ui->checkBox_Cache->isChecked())
    {
        bool found = false;
        for (const auto& c : Cache) // Search in cache
        {
            if (c.Scramble == Scramble && c.Depth >= Depth1 && !c.Solves.empty())
            {
                ui->textBrowser_report->append(CurrentLang["Cache"]);
                qApp->processEvents();
                SearchPetrus->EvaluateBlock(c.Solves, Inspections);
                SearchPetrus->SetTimeBlock(c.Time);
                SearchPetrus->SetDepthBlock(c.Depth);
                found = true;
                break;
            }
        }

        if (!found) // Solves not found in cache
        {
            Cache.clear(); // Cache memory usage can be large, clear previous cache
            const auto time_block_start = std::chrono::system_clock::now();

            DeepSearch BaseSearch(Scramble);
            BaseSearch.SearchBase(Depth1, Plc::BEST_SOLVES, ui->spinBox_Cores->value());

            if (BaseSearch.Solves.empty())
            {
                ui->textBrowser_report->clear();
                ui->textBrowser_report->append(CurrentLang["NoPetrusBlock"]);
                return;
            }

            SearchPetrus->EvaluateBlock(BaseSearch.Solves, Inspections);

            const std::chrono::duration<double> block_elapsed_seconds = std::chrono::system_clock::now() - time_block_start;
            SearchPetrus->SetTimeBlock(block_elapsed_seconds.count());
            SearchPetrus->SetDepthBlock(Depth1);

            // Add current solves into cache
            CacheUnit CU;
            CU.Scramble = Scramble;
            CU.Depth = Depth1;
            CU.Time = SearchPetrus->GetTimeBlock();
            CU.Solves = BaseSearch.Solves;
            Cache.push_back(CU);
        }
    }
    else // No cache
    {
        if (!SearchPetrus->SearchBlock(Depth1, Inspections))
        {
            ui->textBrowser_report->clear();
            ui->textBrowser_report->append(CurrentLang["NoPetrusBlock"]);
            return;
        }
    }

    // Start expanded blocks search
    ui->textBrowser_report->append(CurrentLang["SearchingEB"]);
    qApp->processEvents();
    SearchPetrus->SearchExpandedBlock();

    // Start EO search
    ui->textBrowser_report->append(CurrentLang["SearchingEO"]);
    qApp->processEvents();
    SearchPetrus->SearchEO();

    // Start F2L search
    ui->textBrowser_report->append(CurrentLang["SearchingF2L"]);
    qApp->processEvents();
    SearchPetrus->SearchF2L(Depth2);

    // Start last layer search
    ui->textBrowser_report->append(CurrentLang["SearchingLL"]);
    qApp->processEvents();
    if (ui->comboBox_Variant->currentIndex() == 1)
    {
        SearchPetrus->SearchOCLL(); // Start OCLL search
        SearchPetrus->SearchPLL(); // Start PLL search
    }
    else if (ui->comboBox_Variant->currentIndex() == 2)
    {
        SearchPetrus->SearchCOLL(); // Start COLL search
        SearchPetrus->SearchEPLL(); // Start EPLL search
    }
    else // if (ui->comboBox_Variant->currentIndex() == 0)
    {
        SearchPetrus->SearchZBLL(); // Start ZBLL search
    }

    if (ui->checkBox_Regrips->isChecked()) SearchPetrus->SetRegrips();

    ui->textBrowser_report->clear();

    ui->textBrowser_report->append(QString::fromStdString(SearchPetrus->GetReport(ui->checkBox_Cancellations->isChecked(), false)));
    ui->textBrowser_report->append(QString::fromStdString("Best solve for " + SearchPetrus->GetBestReport(ui->checkBox_Cancellations->isChecked())));
    ui->textBrowser_report->append(QString::fromStdString(SearchPetrus->GetTimeReport()));

    AddToHistory(CurrentLang["PetrusSolve"]);

    delete SearchPetrus;
}

  /*****************************************************************************************************************/
 /*** ZZ **********************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sZZ(const std::string& Scr)
{
    const Algorithm Scramble(Scr);

    // Read the allowed orientations
    std::vector<Spn> SearchSpins;
    GetSearchSpins(SearchSpins);

    uint Depth;
    switch (ui->comboBox_Speed->currentIndex())
    {
    case 0: Depth = 6u; break;
    case 1: Depth = 7u; break;
    case 2: Depth = 8u; break;
    case 3: Depth = 8u; break;
    default: Depth = 6u; break;
    }

    uint Inspections;
    switch (ui->comboBox_Amount->currentIndex())
    {
    case 0: Inspections = 1u; break;
    case 1: Inspections = 4u; break;
    case 2: Inspections = 16u; break;
    case 3: Inspections = 32u; break;
    case 4: Inspections = 64u; break;
    default: Inspections = 1u; break;
    }

    // Create search
    ZZ* SearchZZ = new ZZ(Scramble, ui->spinBox_Cores->value());

    SearchZZ->SetSearchSpins(SearchSpins);

    SearchZZ->SetMetric(static_cast<Metrics>(ui->comboBox_Metric->currentIndex()));

    // Start EOX search
    ui->textBrowser_report->append(CurrentLang["SearchingEOX"]);
    qApp->processEvents();

    if (ui->checkBox_Cache->isChecked())
    {
        bool found = false;
        for (const auto& c : Cache) // Search in cache
        {
          if (c.Scramble == Scramble && c.Depth >= Depth && !c.Solves.empty())
          {
              ui->textBrowser_report->append(CurrentLang["Cache"]);
              qApp->processEvents();
              SearchZZ->EvaluateEOX(c.Solves, Inspections);
              SearchZZ->SetTimeEOX(c.Time);
              SearchZZ->SetDepthEOX(c.Depth);
              found = true;
              break;
          }
        }

        if (!found) // Solves not found in cache
        {
            Cache.clear(); // Cache memory usage can be large, clear previous cache
            const auto time_eoline_start = std::chrono::system_clock::now();

            DeepSearch BaseSearch(Scramble);
            BaseSearch.SearchBase(Depth, Plc::BEST_SOLVES, ui->spinBox_Cores->value());

            if (BaseSearch.Solves.empty())
            {
              ui->textBrowser_report->clear();
              ui->textBrowser_report->append(CurrentLang["NoZZBlock"]);
              return;
            }

            SearchZZ->EvaluateEOX(BaseSearch.Solves, Inspections);

            const std::chrono::duration<double> eoline_elapsed_seconds = std::chrono::system_clock::now() - time_eoline_start;
            SearchZZ->SetTimeEOX(eoline_elapsed_seconds.count());
            SearchZZ->SetDepthEOX(Depth);

            // Add current solves into cache
            CacheUnit CU;
            CU.Scramble = Scramble;
            CU.Depth = Depth;
            CU.Time = SearchZZ->GetTimeEOX();
            CU.Solves = BaseSearch.Solves;
            Cache.push_back(CU);
        }
    }
    else // No cache
    {
        if (!SearchZZ->SearchEOX(Depth, Inspections))
        {
            ui->textBrowser_report->clear();
            ui->textBrowser_report->append(CurrentLang["NoZZBlock"]);
            return;
        }
    }

    // Start F2L search
    ui->textBrowser_report->append(CurrentLang["SearchingF2L"]);
    qApp->processEvents();
    SearchZZ->SearchF2L();

    // Start last layer search
    ui->textBrowser_report->append(CurrentLang["SearchingLL"]);
    qApp->processEvents();
    if (ui->comboBox_Variant->currentIndex() == 1)
    {
      SearchZZ->SearchOCLL(); // Start OCLL search
      SearchZZ->SearchPLL(); // Start PLL search
    }
    else if (ui->comboBox_Variant->currentIndex() == 2)
    {
      SearchZZ->SearchCOLL(); // Start COLL search
      SearchZZ->SearchEPLL(); // Start EPLL search
    }
    else // if (ui->comboBox_Variant->currentIndex() == 0)
    {
      SearchZZ->SearchZBLL(); // Start ZBLL search
    }

    if (ui->checkBox_Regrips->isChecked()) SearchZZ->SetRegrips();

    ui->textBrowser_report->clear();

    ui->textBrowser_report->append(QString::fromStdString(SearchZZ->GetReport(ui->checkBox_Cancellations->isChecked(), false)));
    ui->textBrowser_report->append(QString::fromStdString("Best solve for " + SearchZZ->GetBestReport(ui->checkBox_Cancellations->isChecked())));
    ui->textBrowser_report->append(QString::fromStdString(SearchZZ->GetTimeReport()));

    AddToHistory(CurrentLang["ZZSolve"]);

    delete SearchZZ;
}

  /*****************************************************************************************************************/
 /*** CEOR ********************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sCEOR(const std::string& Scr)
{
    const Algorithm Scramble(Scr);

    // Read the allowed orientations
    std::vector<Spn> SearchSpins;
    GetSearchSpins(SearchSpins);

    uint Depth1, Depth2;
    switch (ui->comboBox_Speed->currentIndex())
    {
    case 0: Depth1 = 6u; Depth2 = 6u; break;
    case 1: Depth1 = 7u; Depth2 = 6u; break;
    case 2: Depth1 = 8u; Depth2 = 7u; break;
    case 3: Depth1 = 8u; Depth2 = 8u; break;
    default: Depth1 = 6u; Depth2 = 6u; break;
    }

    uint Inspections;
    switch (ui->comboBox_Amount->currentIndex())
    {
    case 0: Inspections = 1u; break;
    case 1: Inspections = 4u; break;
    case 2: Inspections = 16u; break;
    case 3: Inspections = 32u; break;
    case 4: Inspections = 64u; break;
    default: Inspections = 1u; break;
    }

    // Create search
    YruRU* SearchYruRU = new YruRU(Scramble, ui->spinBox_Cores->value());

    SearchYruRU->SetSearchSpins(SearchSpins);

    SearchYruRU->SetMetric(static_cast<Metrics>(ui->comboBox_Metric->currentIndex()));

    if (ui->comboBox_Option->currentIndex() == 1)
    {
        // Start Line search
        ui->textBrowser_report->append(CurrentLang["SearchingLines"]);
        qApp->processEvents();

        if (ui->checkBox_Cache->isChecked())
        {
            bool found = false;
            for (const auto& c : Cache) // Search in cache
            {
              if (c.Scramble == Scramble && c.Depth >= 6u && !c.Solves.empty())
              {
                  ui->textBrowser_report->append(CurrentLang["Cache"]);
                  qApp->processEvents();
                  SearchYruRU->EvaluateLines(c.Solves, Inspections);
                  SearchYruRU->SetTimeLines(c.Time);
                  SearchYruRU->SetDepthCPLines(c.Depth);
                  found = true;
                  break;
              }
            }

            if (!found) // Solves not found in cache
            {
                Cache.clear(); // Cache memory usage can be large, clear previous cache
                const auto time_line_start = std::chrono::system_clock::now();

                DeepSearch BaseSearch(Scramble);
                BaseSearch.SearchBase(6u, Plc::BEST_SOLVES, ui->spinBox_Cores->value());

                if (BaseSearch.Solves.empty())
                {
                  ui->textBrowser_report->clear();
                  ui->textBrowser_report->append(CurrentLang["NoLines"]);
                  return;
                }

                SearchYruRU->EvaluateLines(BaseSearch.Solves, Inspections);

                const std::chrono::duration<double> line_elapsed_seconds = std::chrono::system_clock::now() - time_line_start;
                SearchYruRU->SetTimeLines(line_elapsed_seconds.count());
                SearchYruRU->SetDepthCPLines(6u);

                // Add current solves into cache
                CacheUnit CU;
                CU.Scramble = Scramble;
                CU.Depth = 6u;
                CU.Time = SearchYruRU->GetTimeLines();
                CU.Solves = BaseSearch.Solves;
                Cache.push_back(CU);
            }
        }
        else // No cache
        {
            if (!SearchYruRU->SearchLines(6u, Inspections))
            {
                ui->textBrowser_report->clear();
                ui->textBrowser_report->append(CurrentLang["NoLines"]);
                return;
            }
        }

        // Start CP search
        ui->textBrowser_report->append(CurrentLang["SearchingCP"]);
        qApp->processEvents();
        SearchYruRU->SearchCP();
    }
    else // if (ui->comboBox_Option->currentIndex() == 0)
    {
        // Start Line+CP search
        ui->textBrowser_report->append(CurrentLang["SearchingCPLines"]);
        qApp->processEvents();

        if (ui->checkBox_Cache->isChecked())
        {
            bool found = false;
            for (const auto& c : Cache) // Search in cache
            {
                if (c.Scramble == Scramble && c.Depth >= 6u && !c.Solves.empty())
                {
                    ui->textBrowser_report->append(CurrentLang["Cache"]);
                    qApp->processEvents();
                    SearchYruRU->EvaluateCPLines(c.Solves, Inspections);
                    SearchYruRU->SetTimeCPLines(c.Time);
                    SearchYruRU->SetDepthCPLines(c.Depth);
                    found = true;
                    break;
                }
            }

            if (!found) // Solves not found in cache
            {
                Cache.clear(); // Cache memory usage can be large, clear previous cache
                const auto time_cpline_start = std::chrono::system_clock::now();

                DeepSearch BaseSearch(Scramble);
                BaseSearch.SearchBase(6u, Plc::BEST_SOLVES, ui->spinBox_Cores->value());

                if (BaseSearch.Solves.empty())
                {
                  ui->textBrowser_report->clear();
                  ui->textBrowser_report->append(CurrentLang["NoCPLines"]);
                  return;
                }

                SearchYruRU->EvaluateCPLines(BaseSearch.Solves, Inspections);

                const std::chrono::duration<double> cpline_elapsed_seconds = std::chrono::system_clock::now() - time_cpline_start;
                SearchYruRU->SetTimeCPLines(cpline_elapsed_seconds.count());
                SearchYruRU->SetDepthCPLines(6u);

                // Add current solves into cache
                CacheUnit CU;
                CU.Scramble = Scramble;
                CU.Depth = 6u;
                CU.Time = SearchYruRU->GetTimeCPLines();
                CU.Solves = BaseSearch.Solves;
                Cache.push_back(CU);
            }
        }
        else // No cache
        {
            if (!SearchYruRU->SearchCPLines(6u, Inspections))
            {
                ui->textBrowser_report->clear();
                ui->textBrowser_report->append(CurrentLang["NoCPLines"]);
                return;
            }
        }
    }

    // Start pEO-extension search
    ui->textBrowser_report->append(CurrentLang["SearchingpEOExtension"]);
    qApp->processEvents();
    SearchYruRU->SearchpEO(Depth1);

    // Start EOBF search
    ui->textBrowser_report->append(CurrentLang["SearchingEOBF"]);
    qApp->processEvents();
    SearchYruRU->SearchEO(9u);
    SearchYruRU->SearchEOBF(Depth2);

    // Start F2L search
    ui->textBrowser_report->append(CurrentLang["SearchingF2L"]);
    qApp->processEvents();
    SearchYruRU->SearchF2L(13u);

    // Start last layer search
    ui->textBrowser_report->append(CurrentLang["SearchingLL"]);
    qApp->processEvents();
    SearchYruRU->Search2GLL();

    if (ui->checkBox_Regrips->isChecked()) SearchYruRU->SetRegrips();

    ui->textBrowser_report->clear();

    ui->textBrowser_report->append(QString::fromStdString(SearchYruRU->GetReport(ui->checkBox_Cancellations->isChecked(), false)));
    ui->textBrowser_report->append(QString::fromStdString("Best solve for " + SearchYruRU->GetBestReport(ui->checkBox_Cancellations->isChecked())));
    ui->textBrowser_report->append(QString::fromStdString(SearchYruRU->GetTimeReport()));

    AddToHistory(CurrentLang["CEORSolve"]);

    delete SearchYruRU;
}

  /*****************************************************************************************************************/
 /*** Mehta *******************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sMehta(const std::string& Scr)
{
    const Algorithm Scramble(Scr);

    // Read the allowed orientations
    std::vector<Spn> SearchSpins;
    GetSearchSpins(SearchSpins);

    uint Depth1, Depth2;
    switch (ui->comboBox_Speed->currentIndex())
    {
    case 0: Depth1 = 6u; Depth2 = 6u; break;
    case 1: Depth1 = 7u; Depth2 = 6u; break;
    case 2: Depth1 = 8u; Depth2 = 7u; break;
    case 3: Depth1 = 8u; Depth2 = 8u; break;
    default: Depth1 = 6u; Depth2 = 6u; break;
    }

    uint Inspections;
    switch (ui->comboBox_Amount->currentIndex())
    {
    case 0: Inspections = 1u; break;
    case 1: Inspections = 4u; break;
    case 2: Inspections = 16u; break;
    case 3: Inspections = 32u; break;
    case 4: Inspections = 64u; break;
    default: Inspections = 1u; break;
    }

    // Create search
    Mehta* SearchMehta = new Mehta(Scramble, ui->spinBox_Cores->value());

    SearchMehta->SetSearchSpins(SearchSpins);

    SearchMehta->SetMetric(static_cast<Metrics>(ui->comboBox_Metric->currentIndex()));

    // Start first blocks search
    ui->textBrowser_report->append(CurrentLang["SearchingFB"]);
    qApp->processEvents();

    if (ui->checkBox_Cache->isChecked())
    {
        bool found = false;
        for (const auto& c : Cache) // Search in cache
        {
            if (c.Scramble == Scramble && c.Depth >= Depth1 && !c.Solves.empty())
            {
                ui->textBrowser_report->append(CurrentLang["Cache"]);
                qApp->processEvents();
                SearchMehta->EvaluateFB(c.Solves, Inspections);
                SearchMehta->SetTimeFB(c.Time);
                SearchMehta->SetDepthFB(c.Depth);
                found = true;
                break;
            }
        }

        if (!found) // Solves not found in cache
        {
            Cache.clear(); // Cache memory usage can be large, clear previous cache
            const auto time_fb_start = std::chrono::system_clock::now();

            DeepSearch BaseSearch(Scramble);
            BaseSearch.SearchBase(Depth1, Plc::BEST_SOLVES, ui->spinBox_Cores->value());

            if (BaseSearch.Solves.empty())
            {
                ui->textBrowser_report->clear();
                ui->textBrowser_report->append(CurrentLang["NoFB"]);
                return;
            }

            SearchMehta->EvaluateFB(BaseSearch.Solves, Inspections);

            const std::chrono::duration<double> fb_elapsed_seconds = std::chrono::system_clock::now() - time_fb_start;
            SearchMehta->SetTimeFB(fb_elapsed_seconds.count());
            SearchMehta->SetDepthFB(Depth1);

            // Add current solves into cache
            CacheUnit CU;
            CU.Scramble = Scramble;
            CU.Depth = Depth1;
            CU.Time = SearchMehta->GetTimeFB();
            CU.Solves = BaseSearch.Solves;
            Cache.push_back(CU);
        }
    }
    else // No cache
    {
        if (!SearchMehta->SearchFB(Depth1, Inspections))
        {
            ui->textBrowser_report->clear();
            ui->textBrowser_report->append(CurrentLang["NoFB"]);
            return;
        }
    }

    // Start 3QB search
    ui->textBrowser_report->append(CurrentLang["Searching3QB"]);
    qApp->processEvents();
    SearchMehta->Search3QB(Depth2);

    // Start EOLE search
    ui->textBrowser_report->append(CurrentLang["SearchingEOLE"]);
    qApp->processEvents();
    SearchMehta->SearchEOLE();

    if (ui->comboBox_Variant->currentIndex() == 1)
    {
        ui->textBrowser_report->append(CurrentLang["Searching6CO"]);
        qApp->processEvents();
        SearchMehta->Search6CO();

        ui->textBrowser_report->append(CurrentLang["Searching6CP"]);
        qApp->processEvents();
        SearchMehta->Search6CP();

        ui->textBrowser_report->append(CurrentLang["SearchingL5EP"]);
        qApp->processEvents();
        SearchMehta->SearchL5EP();
    }
    else if (ui->comboBox_Variant->currentIndex() == 2)
    {
        ui->textBrowser_report->append(CurrentLang["Searching6CO"]);
        qApp->processEvents();
        SearchMehta->Search6CO();

        ui->textBrowser_report->append(CurrentLang["SearchingAPDR"]);
        qApp->processEvents();
        SearchMehta->SearchAPDR();

        ui->textBrowser_report->append(CurrentLang["SearchingPLL"]);
        qApp->processEvents();
        SearchMehta->SearchPLL();
    }
    else if (ui->comboBox_Variant->currentIndex() == 3)
    {
        ui->textBrowser_report->append(CurrentLang["SearchingDCAL"]);
        qApp->processEvents();
        SearchMehta->SearchDCAL();

        ui->textBrowser_report->append(CurrentLang["SearchingCDRLL"]);
        qApp->processEvents();
        SearchMehta->SearchCDRLL();

        ui->textBrowser_report->append(CurrentLang["SearchingL5EP"]);
        qApp->processEvents();
        SearchMehta->SearchL5EP();
    }
    else if (ui->comboBox_Variant->currentIndex() == 4)
    {
        ui->textBrowser_report->append(CurrentLang["SearchingDCAL"]);
        qApp->processEvents();
        SearchMehta->SearchDCAL();

        ui->textBrowser_report->append(CurrentLang["SearchingJTLE"]);
        qApp->processEvents();
        SearchMehta->SearchJTLE();

        ui->textBrowser_report->append(CurrentLang["SearchingPLL"]);
        qApp->processEvents();
        SearchMehta->SearchPLL();
    }
    else // Mehta-TDR
    {
        ui->textBrowser_report->append(CurrentLang["SearchingTDR"]);
        qApp->processEvents();
        SearchMehta->SearchTDR();

        ui->textBrowser_report->append(CurrentLang["SearchingZBLL"]);
        qApp->processEvents();
        SearchMehta->SearchZBLL();
    }

    if (ui->checkBox_Regrips->isChecked()) SearchMehta->SetRegrips();

    ui->textBrowser_report->clear();

    ui->textBrowser_report->append(QString::fromStdString(SearchMehta->GetReport(ui->checkBox_Cancellations->isChecked(), false)));
    ui->textBrowser_report->append(QString::fromStdString("Best solve for " + SearchMehta->GetBestReport(ui->checkBox_Cancellations->isChecked())));
    ui->textBrowser_report->append(QString::fromStdString(SearchMehta->GetTimeReport()));

    AddToHistory(CurrentLang["MehtaSolve"]);

    delete SearchMehta;
}



