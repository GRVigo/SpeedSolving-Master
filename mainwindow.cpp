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

#include "tinyxml2.h" // For xml language files

#include "method.h"
#include "deep_search.h"
#include "deep_eval.h"

using namespace grcube3;
using namespace tinyxml2;

DeepEval Evaluation(Algorithm(""), 5u); // Scrambles evaluation

  /*****************************************************************************************************************/
 /*** MAIN WINDOW *************************************************************************************************/
/*****************************************************************************************************************/

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    aw = nullptr;

    SearchThread = new RunSearch;

    QObject::connect(SearchThread, SIGNAL(finishLBL_Solves(const QString, const QString, const QString)),
                     this, SLOT(printLBL_Solves(const QString, const QString, const QString)));
    QObject::connect(SearchThread, SIGNAL(finishLBL_NoSolves()), this, SLOT(printLBL_NoSolves()));
    QObject::connect(SearchThread, SIGNAL(msgLBLStartCross()), this, SLOT(printLBL_Cross()));
    QObject::connect(SearchThread, SIGNAL(msgLBLStartFL()), this, SLOT(printLBL_FL()));
    QObject::connect(SearchThread, SIGNAL(msgLBLStartSL()), this, SLOT(printLBL_SL()));
    QObject::connect(SearchThread, SIGNAL(msgLBLStartLL()), this, SLOT(printLBL_LL()));

    QObject::connect(SearchThread, SIGNAL(finishMethod_Solves(const QString, const QString)),
                     this, SLOT(printMethod_Solves(const QString, const QString)));
    QObject::connect(SearchThread, SIGNAL(finishMethod_Best(const QString)), this, SLOT(printMethod_Best(const QString)));
    QObject::connect(SearchThread, SIGNAL(finishMethod_Time(const QString)), this, SLOT(printMethod_Time(const QString)));
    QObject::connect(SearchThread, SIGNAL(finishMethod_NoSolves()), this, SLOT(printMethod_NoSolves()));

    QObject::connect(SearchThread, SIGNAL(msgCFOPStartCross()), this, SLOT(printCFOP_Cross()));
    QObject::connect(SearchThread, SIGNAL(msgCFOPStartF2L()), this, SLOT(printCFOP_F2L()));
    QObject::connect(SearchThread, SIGNAL(msgCFOPStartLL()), this, SLOT(printCFOP_LL()));

    QObject::connect(SearchThread, SIGNAL(msgRouxStartFB()), this, SLOT(printRoux_FB()));
    QObject::connect(SearchThread, SIGNAL(msgRouxStartSB()), this, SLOT(printRoux_SB()));
    QObject::connect(SearchThread, SIGNAL(msgRouxStartCMLL()), this, SLOT(printRoux_CMLL()));
    QObject::connect(SearchThread, SIGNAL(msgRouxStartCOLL()), this, SLOT(printRoux_COLL()));
    QObject::connect(SearchThread, SIGNAL(msgRouxStartL6E()), this, SLOT(printRoux_L6E()));

    QObject::connect(SearchThread, SIGNAL(msgPetrusStartFB()), this, SLOT(printPetrus_FB()));
    QObject::connect(SearchThread, SIGNAL(msgPetrusStartEB()), this, SLOT(printPetrus_EB()));
    QObject::connect(SearchThread, SIGNAL(msgPetrusStartEO()), this, SLOT(printPetrus_EO()));
    QObject::connect(SearchThread, SIGNAL(msgPetrusStartF2L()), this, SLOT(printPetrus_F2L()));
    QObject::connect(SearchThread, SIGNAL(msgPetrusStartLL()), this, SLOT(printPetrus_LL()));

    QObject::connect(SearchThread, SIGNAL(msgZZStartEOX()), this, SLOT(printZZ_EOX()));
    QObject::connect(SearchThread, SIGNAL(msgZZStartF2L()), this, SLOT(printZZ_F2L()));
    QObject::connect(SearchThread, SIGNAL(msgZZStartLL()), this, SLOT(printZZ_LL()));

    QObject::connect(SearchThread, SIGNAL(msgCEORStartLines()), this, SLOT(printCEOR_Lines()));
    QObject::connect(SearchThread, SIGNAL(msgCEORStartCP()), this, SLOT(printCEOR_CP()));
    QObject::connect(SearchThread, SIGNAL(msgCEORStartCPLines()), this, SLOT(printCEOR_CPLines()));
    QObject::connect(SearchThread, SIGNAL(msgCEORStartpEO()), this, SLOT(printCEOR_pEO()));
    QObject::connect(SearchThread, SIGNAL(msgCEORStartEOBF()), this, SLOT(printCEOR_EOBF()));
    QObject::connect(SearchThread, SIGNAL(msgCEORStartF2L()), this, SLOT(printCEOR_F2L()));
    QObject::connect(SearchThread, SIGNAL(msgCEORStartLL()), this, SLOT(printCEOR_LL()));

    QObject::connect(SearchThread, SIGNAL(msgMehtaStartFB()), this, SLOT(printMehta_FB()));
    QObject::connect(SearchThread, SIGNAL(msgMehtaStart3QB()), this, SLOT(printMehta_3QB()));
    QObject::connect(SearchThread, SIGNAL(msgMehtaStartEOLE()), this, SLOT(printMehta_EOLE()));
    QObject::connect(SearchThread, SIGNAL(msgMehtaStart6CO()), this, SLOT(printMehta_6CO()));
    QObject::connect(SearchThread, SIGNAL(msgMehtaStart6CP()), this, SLOT(printMehta_6CP()));
    QObject::connect(SearchThread, SIGNAL(msgMehtaStartL5EP()), this, SLOT(printMehta_L5EP()));
    QObject::connect(SearchThread, SIGNAL(msgMehtaStartAPDR()), this, SLOT(printMehta_APDR()));
    QObject::connect(SearchThread, SIGNAL(msgMehtaStartPLL()), this, SLOT(printMehta_PLL()));
    QObject::connect(SearchThread, SIGNAL(msgMehtaStartDCAL()), this, SLOT(printMehta_DCAL()));
    QObject::connect(SearchThread, SIGNAL(msgMehtaStartCDRLL()), this, SLOT(printMehta_CDRLL()));
    QObject::connect(SearchThread, SIGNAL(msgMehtaStartJTLE()), this, SLOT(printMehta_JTLE()));
    QObject::connect(SearchThread, SIGNAL(msgMehtaStartTDR()), this, SLOT(printMehta_TDR()));
    QObject::connect(SearchThread, SIGNAL(msgMehtaStartZBLL()), this, SLOT(printMehta_ZBLL()));

    QObject::connect(SearchThread, SIGNAL(msgNautilusStartFB()), this, SLOT(printNautilus_FB()));
    QObject::connect(SearchThread, SIGNAL(msgNautilusStartSB()), this, SLOT(printNautilus_SB()));
    QObject::connect(SearchThread, SIGNAL(msgNautilusStartdFR()), this, SLOT(printNautilus_dFR()));
    QObject::connect(SearchThread, SIGNAL(msgNautilusStartNCLL()), this, SLOT(printNautilus_NCLL()));
    QObject::connect(SearchThread, SIGNAL(msgNautilusStartNCOLL()), this, SLOT(printNautilus_NCOLL()));
    QObject::connect(SearchThread, SIGNAL(msgNautilusStartTNCLL()), this, SLOT(printNautilus_TNCLL()));
    QObject::connect(SearchThread, SIGNAL(msgNautilusStartL5E()), this, SLOT(printNautilus_L5E()));
    QObject::connect(SearchThread, SIGNAL(msgNautilusStartEODF()), this, SLOT(printNautilus_EODF()));
    QObject::connect(SearchThread, SIGNAL(msgNautilusStartF2L()), this, SLOT(printNautilus_F2L()));
    QObject::connect(SearchThread, SIGNAL(msgNautilusStartLL()), this, SLOT(printNautilus_LL()));

    QObject::connect(SearchThread, SIGNAL(msgLEORStartFB()), this, SLOT(printLEOR_FB()));
    QObject::connect(SearchThread, SIGNAL(msgLEORStartEOStripe()), this, SLOT(printLEOR_EOStripe()));
    QObject::connect(SearchThread, SIGNAL(msgLEORStartFLPair()), this, SLOT(printLEOR_FLPair()));
    QObject::connect(SearchThread, SIGNAL(msgLEORStartEODF()), this, SLOT(printLEOR_EODF()));
    QObject::connect(SearchThread, SIGNAL(msgLEORStartSB()), this, SLOT(printLEOR_SB()));
    QObject::connect(SearchThread, SIGNAL(msgLEORStartLL()), this, SLOT(printLEOR_LL()));

    QObject::connect(SearchThread, SIGNAL(msgCache()), this, SLOT(printCache()));

    SaveXMLDefaultLanguage(); // Save default language texts

    LoadXMLDefaultLanguage(); // Load default language texts for use in undefined texts

    // Update cores (threads)
    ui->spinBox_Cores->setMinimum(0);
    ui->spinBox_Cores->setMaximum(DeepSearch::GetSystemCores());
    ui->spinBox_Cores->setValue(0);

    // Read language files
    QDir directory = QDir::currentPath();
    QStringList langfiles = directory.entryList(QStringList() << "*.xml" << "*.XML", QDir::Files);
    int Index = 0;
    foreach(QString filename, langfiles) if (CheckXMLLanguageFile(filename))
    {
        ui->comboBox_Language->addItem(filename);
        if (filename.contains("english")) ui->comboBox_Language->setCurrentIndex(Index);
        Index++;
    }

    // No scramble text in status bar
    MainWindow::on_lineEdit_Scramble_textChanged(ui->lineEdit_Scramble->text());

    on_pushButton_Credits_clicked(); // Show credits at start
    ui->comboBox_Method->setCurrentIndex(1); // CFOP as default method

    SolveId = 0u; // Index for solves history

    // Hide unused controls
    ui->pushButton_Debug->hide();
    ui->pushButton_Collections->hide();
    // ui->line_Cores->hide();
    // ui->label_Language->hide();
    // ui->comboBox_Language->hide();

    ui->pushButton_Skip->setDisabled(true); // Skip button disabled at start
}

MainWindow::~MainWindow()
{
    SearchThread->Skip();
    if (aw != nullptr) delete aw;
    delete ui;
    delete SearchThread;
}

void MainWindow::DisableWhileSearching()
{
    ui->pushButton_Credits->setDisabled(true);
    ui->pushButton_License->setDisabled(true);
    ui->pushButton_Help->setDisabled(true);
    ui->checkBox_Cache->setDisabled(true);
    ui->spinBox_Cores->setDisabled(true);
	ui->comboBox_Language->setDisabled(true);
    ui->lineEdit_Scramble->setDisabled(true);
    ui->spinBox_ScrambleLength->setDisabled(true);
    ui->pushButton_RandomScramble->setDisabled(true);
    ui->pushButton_PasteScramble->setDisabled(true);
    ui->pushButton_ClearScramble->setDisabled(true);
    ui->comboBox_Method->setDisabled(true);
    ui->comboBox_Variant->setDisabled(true);
    ui->comboBox_Option->setDisabled(true);
    ui->comboBox_Speed->setDisabled(true);
    ui->comboBox_Orientation->setDisabled(true);
    ui->comboBox_Amount->setDisabled(true);
    ui->comboBox_Metric->setDisabled(true);
    ui->checkBox_Regrips->setDisabled(true);
    ui->checkBox_Cancellations->setDisabled(true);
    ui->pushButton_ZoomPlus->setDisabled(true);
    ui->pushButton_ZoomMinus->setDisabled(true);
    ui->pushButton_CopyReport->setDisabled(true);
    ui->pushButton_SaveReport->setDisabled(true);
    ui->pushButton_ClearReport->setDisabled(true);
    ui->comboBox_History->setDisabled(true);
    ui->pushButton_ClearHistory->setDisabled(true);
}

void MainWindow::EnableAfterSearch()
{
    ui->pushButton_Credits->setEnabled(true);
    ui->pushButton_License->setEnabled(true);
    ui->pushButton_Help->setEnabled(true);
    ui->checkBox_Cache->setEnabled(true);
    ui->spinBox_Cores->setEnabled(true);
	ui->comboBox_Language->setEnabled(true);
    ui->lineEdit_Scramble->setEnabled(true);
    ui->spinBox_ScrambleLength->setEnabled(true);
    ui->pushButton_RandomScramble->setEnabled(true);
    ui->pushButton_PasteScramble->setEnabled(true);
    ui->pushButton_ClearScramble->setEnabled(true);
    ui->comboBox_Method->setEnabled(true);
    ui->comboBox_Variant->setEnabled(true);
    ui->comboBox_Option->setEnabled(true);
    ui->comboBox_Speed->setEnabled(true);
    ui->comboBox_Orientation->setEnabled(true);
    ui->comboBox_Amount->setEnabled(true);
    ui->comboBox_Metric->setEnabled(true);
    ui->checkBox_Regrips->setEnabled(true);
    ui->checkBox_Cancellations->setEnabled(true);
    ui->pushButton_ZoomPlus->setEnabled(true);
    ui->pushButton_ZoomMinus->setEnabled(true);
    ui->pushButton_CopyReport->setEnabled(true);
    ui->pushButton_SaveReport->setEnabled(true);
    ui->pushButton_ClearReport->setEnabled(true);
    ui->comboBox_History->setEnabled(true);
    ui->pushButton_ClearHistory->setEnabled(true);
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
    summary.append(QString::fromStdString(std::to_string(Method::Algset_OLL.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["LoadPLL"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_PLL.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["Load1LLL"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_1LLL.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["LoadZBLL"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_ZBLL.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["LoadOCLL"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_OCLL.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["LoadCMLL"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_CMLL.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["LoadCOLL"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_COLL.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["LoadEPLL"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_EPLL.GetCasesNumber())));

    summary.append("\n\t");
    summary.append(CurrentLang["LoadEOLE"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_EOLE.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["Load6CO"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_6CO.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["Load6CP"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_6CP.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["LoadAPDR"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_APDR.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["LoadL5EP"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_L5EP.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["LoadCDRLL"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_CDRLL.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["LoadJTLE"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_JTLE.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["LoadTDR"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_TDR.GetCasesNumber())));

    summary.append("\n\t");
    summary.append(CurrentLang["LoadNCLL"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_NCLL.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["LoadNCOLL"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_NCOLL.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["LoadTNCLL"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_TNCLL.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["LoadL5E"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_L5E.GetCasesNumber())));
    summary.append("\n\t");
    summary.append(CurrentLang["LoadEODF"]);
    summary.append(QString::fromStdString(std::to_string(Method::Algset_EODF.GetCasesNumber())));

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
    pElement->SetAttribute("ObjectName", ui->pushButton_Skip->objectName().toStdString().c_str());
    pElement->SetText(ui->pushButton_Skip->text().toStdString().c_str());
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
    pElement->SetText("SpeedSolving Master V1.0 by GRVigo");
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "Cache");
    pElement->SetText("[Evaluating from cache]");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "CacheOn");
    pElement->SetText("Cache enabled, please note that slow searchs cause high memory usage");
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
	pElement->SetText("All avaliable threads will be used in the search");
	pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SingleCoreWillBeUsedInTheSearch");
	pElement->SetText("Single thread will be used in the search");
	pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "CoresWillBeUsedInTheSearch");
    pElement->SetText("Number of threads that will be used in the search: ");
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchStarted");
    pElement->SetText("Search in progress...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "Skip");
    pElement->SetText("[Skip]");
    pRoot->InsertEndChild(pElement);

	pElement = lang_xml.NewElement("Message");
	pElement->SetAttribute("Id", "SearchFinished");
	pElement->SetText("Search finished, enjoy!");
	pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchCancelled");
    pElement->SetText("Search cancelled!");
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

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "OLL+PLL");
    pElement->SetText("OLL+PLL");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "1LLL");
    pElement->SetText("1LLL");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "EO+ZBLL");
    pElement->SetText("EO+ZBLL");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "CMLL");
    pElement->SetText("CMLL");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "COLL");
    pElement->SetText("COLL");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "L6EStepByStep");
    pElement->SetText("L6E step by step");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "OneLookL6E");
    pElement->SetText("One-look L6E");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "ZBLL");
    pElement->SetText("ZBLL");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "OCLL+PLL");
    pElement->SetText("OCLL+PLL");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "COLL+EPLL");
    pElement->SetText("COLL+EPLL");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "YruRU");
    pElement->SetText("YruRU");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "CPSkip");
    pElement->SetText("CP-Skip");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoCPSkip");
    pElement->SetText("No CP-Skip");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "TDR");
    pElement->SetText("TDR");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "6CP");
    pElement->SetText("6CP");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "APDR");
    pElement->SetText("APDR");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "CDRLL");
    pElement->SetText("CDRLL");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "JTLE");
    pElement->SetText("JTLE");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "L5E-NCLL");
    pElement->SetText("L5E-NCLL");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "L5E-NCOLL");
    pElement->SetText("L5E-NCOLL");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "L5E-NCLL/TNCLL");
    pElement->SetText("L5E-NCLL/TNCLL");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LSLL-ZBLL");
    pElement->SetText("LSLL-ZBLL");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LSLL-OCLL/PLL");
    pElement->SetText("LSLL-OCLL/PLL");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LSLL-COLL/EPLL");
    pElement->SetText("LSLL-COLL/EPLL");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LEOR-A");
    pElement->SetText("LEOR-A");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LEOR-B");
    pElement->SetText("LEOR-B");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SpeedFast");
    pElement->SetText("Fast");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SpeedMedium");
    pElement->SetText("Medium");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SpeedSlow");
    pElement->SetText("Slow");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SpeedVerySlow");
    pElement->SetText("Very slow");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "OrientAll");
    pElement->SetText("All");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "OrientUD");
    pElement->SetText("U & D layers up");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "OrientFB");
    pElement->SetText("F & B layers up");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "OrientRL");
    pElement->SetText("R & L layers up");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "OrientU");
    pElement->SetText("U layer up");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "OrientD");
    pElement->SetText("D layer up");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "OrientF");
    pElement->SetText("F layer up");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "OrientB");
    pElement->SetText("B layer up");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "OrientR");
    pElement->SetText("R layer up");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "OrientL");
    pElement->SetText("L layer up");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "AmountMinimum");
    pElement->SetText("Minimum");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "AmountLow");
    pElement->SetText("Low");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "AmountMedium");
    pElement->SetText("Medium");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "AmountHigh");
    pElement->SetText("High");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "AmountHuge");
    pElement->SetText("Huge");
    pRoot->InsertEndChild(pElement);

    // LBL

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LBLSolve");
    pElement->SetText("LBL solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingLBLCross");
    pElement->SetText("Searching first layer cross...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingLBLFLCorners");
    pElement->SetText("Searching first layer corners...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingLBLSL");
    pElement->SetText("Searching second layer edges...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingLBLLL");
    pElement->SetText("Searching last layer...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LBLNoSolve");
    pElement->SetText("No LBL solves found. This shouldn't happend.");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LBLHeader");
    pElement->SetText("Layer-by-Layer solve with cross in layer ");
    pRoot->InsertEndChild(pElement);

    // Common to all methods

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NoSolves");
    pElement->SetText("No solves found. Try a slower speed, more orientations or increase the amount of solves.");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "MethodHeader");
    pElement->SetText("All found solves for ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "ScrambleHeader");
    pElement->SetText("Scramble: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "TimeHeader");
    pElement->SetText("Search times / Threads");
    pRoot->InsertEndChild(pElement);

    // CFOP

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "CFOPSolve");
    pElement->SetText("CFOP solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "CFOPBest");
    pElement->SetText("Best CFOP solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingCFOPCross");
    pElement->SetText("Searching CFOP cross...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingCFOPF2L");
    pElement->SetText("Searching first two layers...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingCFOPLL");
    pElement->SetText("Searching last layer...");
    pRoot->InsertEndChild(pElement);

    // Roux

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "RouxSolve");
    pElement->SetText("Roux solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "RouxBest");
    pElement->SetText("Best Roux solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingRouxFB");
    pElement->SetText("Searching Roux first blocks...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingRouxSB");
    pElement->SetText("Searching second blocks...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingRouxCOLL");
    pElement->SetText("Searching COLL...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingRouxCMLL");
    pElement->SetText("Searching CMLL...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingRouxL6E");
    pElement->SetText("Searching last six edges...");
    pRoot->InsertEndChild(pElement);

    // Petrus

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "PetrusSolve");
    pElement->SetText("Petrus solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "PetrusBest");
    pElement->SetText("Best Petrus solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingPetrusBlocks");
    pElement->SetText("Searching Petrus blocks...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingPetrusEB");
    pElement->SetText("Searching expanded blocks...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingPetrusEO");
    pElement->SetText("Searching edges orientations...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingPetrusF2L");
    pElement->SetText("Searching F2L...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingPetrusLL");
    pElement->SetText("Searching last layers...");
    pRoot->InsertEndChild(pElement);

    // ZZ

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "ZZSolve");
    pElement->SetText("ZZ solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "ZZBest");
    pElement->SetText("Best ZZ solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingZZEOX");
    pElement->SetText("Searching EO X...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingZZF2L");
    pElement->SetText("Searching F2L...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingZZLL");
    pElement->SetText("Searching last layers...");
    pRoot->InsertEndChild(pElement);

    // CEOR

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "CEORSolve");
    pElement->SetText("CEOR solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "CEORBest");
    pElement->SetText("Best CEOR solve");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingYruRULines");
    pElement->SetText("Searching YruRU lines...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingYruRUCP");
    pElement->SetText("Searching YruRU CP...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingYruRUCPLines");
    pElement->SetText("Searching YruRU lines + CP...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingpYruRUpEOExtension");
    pElement->SetText("Searching pEO-extension...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingYruRUEOBF");
    pElement->SetText("Searching EO BF...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingYruRUF2L");
    pElement->SetText("Searching F2L...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingYruRULL");
    pElement->SetText("Searching last layers...");
    pRoot->InsertEndChild(pElement);

    // Mehta

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "MehtaSolve");
    pElement->SetText("Mehta solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "MehtaBest");
    pElement->SetText("Best Mehta solve");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingMehtaFB");
    pElement->SetText("Searching Mehta first blocks...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingMehta3QB");
    pElement->SetText("Searching 3QB...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingMehtaEOLE");
    pElement->SetText("Searching EOLE...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingMehta6CO");
    pElement->SetText("Searching 6CO...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingMehta6CP");
    pElement->SetText("Searching 6CP...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingMehtaL5EP");
    pElement->SetText("Searching L5EP...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingMehtaAPDR");
    pElement->SetText("Searching APDR...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingMehtaPLL");
    pElement->SetText("Searching PLL...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingMehtaDCAL");
    pElement->SetText("Searching DCAL...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingMehtaCDRLL");
    pElement->SetText("Searching CDRLL...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingMehtaJTLE");
    pElement->SetText("Searching JTLE...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingMehtaTDR");
    pElement->SetText("Searching TDR...");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingMehtaZBLL");
    pElement->SetText("Searching ZBLL...");
    pRoot->InsertEndChild(pElement);

    // Nautilus

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NautilusSolve");
    pElement->SetText("Nautilus solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "NautilusBest");
    pElement->SetText("Best Nautilus solve");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingNautilusFB");
    pElement->SetText("Searching Nautilus first blocks...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingNautilusSB");
    pElement->SetText("Searching Nautilus second blocks...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingNautilusdFR");
    pElement->SetText("Searching Nautilus dFR pairs...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingNautilusNCLL");
    pElement->SetText("Searching NCLL...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingNautilusNCOLL");
    pElement->SetText("Searching NOCLL...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingNautilusTNCLL");
    pElement->SetText("Searching NCLL/TNCLL...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingNautilusL5E");
    pElement->SetText("Searching last five edges...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingNautilusEODF");
    pElement->SetText("Searching Nautilus EODF...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingNautilusF2L");
    pElement->SetText("Searching Nautilus F2L...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingNautilusLL");
    pElement->SetText("Searching last layers...");
    pRoot->InsertEndChild(pElement);

    // LEOR

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LEORSolve");
    pElement->SetText("LEOR solve");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LEORBest");
    pElement->SetText("Best LEOR solve");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingLEORFB");
    pElement->SetText("Searching LEOR first blocks...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingLEOREOStripe");
    pElement->SetText("Searching EO stripe...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingLEORFLPair");
    pElement->SetText("Searching FL pair...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingLEOREODF");
    pElement->SetText("Searching EO DF...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingLEORSB");
    pElement->SetText("Searching LEOR second blocks...");
    pRoot->InsertEndChild(pElement);
	
	pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "SearchingLEORLL");
    pElement->SetText("Searching last layers...");
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
    pElement->SetText("OLL cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadPLL");
    pElement->SetText("PLL cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "Load1LLL");
    pElement->SetText("1LLL cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadZBLL");
    pElement->SetText("ZBLL cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadOCLL");
    pElement->SetText("OCLL cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadCMLL");
    pElement->SetText("CMLL cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadCOLL");
    pElement->SetText("COLL cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadEPLL");
    pElement->SetText("EPLL cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadEOLE");
    pElement->SetText("EOLE cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "Load6CO");
    pElement->SetText("6CO cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "Load6CP");
    pElement->SetText("6CP cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadAPDR");
    pElement->SetText("APDR cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadL5EP");
    pElement->SetText("L5EP cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadCDRLL");
    pElement->SetText("CDRLL cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadJTLE");
    pElement->SetText("JTLE cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadTDR");
    pElement->SetText("TDR cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadNCLL");
    pElement->SetText("NCLL cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadNCOLL");
    pElement->SetText("NCOLL cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadTNCLL");
    pElement->SetText("TNCLL cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadL5E");
    pElement->SetText("L5E cases loaded: ");
    pRoot->InsertEndChild(pElement);

    pElement = lang_xml.NewElement("Message");
    pElement->SetAttribute("Id", "LoadEODF");
    pElement->SetText("EODF cases loaded: ");
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
            if (Lang.empty() || lang.contains("default")) return false; // No language
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
        on_comboBox_Method_currentIndexChanged(ui->comboBox_Method->currentText());

        ui->comboBox_Speed->clear();
        ui->comboBox_Speed->addItem(CurrentLang["SpeedFast"]);
        ui->comboBox_Speed->addItem(CurrentLang["SpeedMedium"]);
        ui->comboBox_Speed->addItem(CurrentLang["SpeedSlow"]);
        ui->comboBox_Speed->addItem(CurrentLang["SpeedVerySlow"]);

        ui->comboBox_Orientation->clear();
        ui->comboBox_Orientation->addItem(CurrentLang["OrientAll"]);
        ui->comboBox_Orientation->addItem(CurrentLang["OrientUD"]);
        ui->comboBox_Orientation->addItem(CurrentLang["OrientFB"]);
        ui->comboBox_Orientation->addItem(CurrentLang["OrientRL"]);
        ui->comboBox_Orientation->addItem(CurrentLang["OrientU"]);
        ui->comboBox_Orientation->addItem(CurrentLang["OrientD"]);
        ui->comboBox_Orientation->addItem(CurrentLang["OrientF"]);
        ui->comboBox_Orientation->addItem(CurrentLang["OrientB"]);
        ui->comboBox_Orientation->addItem(CurrentLang["OrientR"]);
        ui->comboBox_Orientation->addItem(CurrentLang["OrientL"]);

        ui->comboBox_Amount->clear();
        ui->comboBox_Amount->addItem(CurrentLang["AmountMinimum"]);
        ui->comboBox_Amount->addItem(CurrentLang["AmountLow"]);
        ui->comboBox_Amount->addItem(CurrentLang["AmountMedium"]);
        ui->comboBox_Amount->addItem(CurrentLang["AmountHigh"]);
        ui->comboBox_Amount->addItem(CurrentLang["AmountHuge"]);

        ui->statusBar->showMessage(CurrentLang["ApplicationLanguageChanged"]);
    }
    else
    {
        UpdateFixedTexts(DefaultLang);
        on_comboBox_Method_currentIndexChanged(ui->comboBox_Method->currentText());
        ui->statusBar->showMessage(CurrentLang["DefaultLanguageApplied"]);
    }

    on_pushButton_Credits_clicked();
}

  /*****************************************************************************************************************/
 /*** SCRAMBLE FUNCTIONS ******************************************************************************************/
/*****************************************************************************************************************/

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
        Algorithm B = A.GetSimplified();
        B = B.GetWithoutTurns();
        while (B.Shrink());

        if (B.GetSize() < 12u) // Scramble too short to evaluate
        {
            ui->label_Eval->setText(CurrentLang["NoEval"]);
        }
        else
        {
            // Read the allowed orientations
            std::vector<Spn> SearchSpins;
            RunSearch::GetSearchSpins(SearchSpins, ui->comboBox_Orientation->currentIndex());

            // Get the scramble evaluation
            Evaluation.SetNewScramble(B);
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
    if (cores == 0) ui->statusBar->showMessage(CurrentLang["AllAvaliable"]);
    else if (cores == 1) ui->statusBar->showMessage(CurrentLang["SingleCoreWillBeUsedInTheSearch"]);
    else ui->statusBar->showMessage(CurrentLang["CoresWillBeUsedInTheSearch"] + QString::number(cores));
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
        ui->comboBox_Variant->addItem(CurrentLang["OLL+PLL"]);
        ui->comboBox_Variant->addItem(CurrentLang["1LLL"]);
        ui->comboBox_Variant->addItem(CurrentLang["EO+ZBLL"]);
        ui->comboBox_Option->addItem(CurrentLang["None"]); break;
    case 2: // Roux
        ui->comboBox_Variant->addItem(CurrentLang["CMLL"]);
        ui->comboBox_Variant->addItem(CurrentLang["COLL"]);
        ui->comboBox_Option->addItem(CurrentLang["L6EStepByStep"]);
        ui->comboBox_Option->addItem(CurrentLang["OneLookL6E"]); break;
    case 3: // Petrus
        ui->comboBox_Variant->addItem(CurrentLang["ZBLL"]);
        ui->comboBox_Variant->addItem(CurrentLang["OCLL+PLL"]);
        ui->comboBox_Variant->addItem(CurrentLang["COLL+EPLL"]);
        ui->comboBox_Option->addItem(CurrentLang["None"]); break;
    case 4: // ZZ
        ui->comboBox_Variant->addItem(CurrentLang["ZBLL"]);
        ui->comboBox_Variant->addItem(CurrentLang["OCLL+PLL"]);
        ui->comboBox_Variant->addItem(CurrentLang["COLL+EPLL"]);
        ui->comboBox_Option->addItem(CurrentLang["None"]); break;
    case 5: // CEOR
        ui->comboBox_Variant->addItem(CurrentLang["YruRU"]);
        ui->comboBox_Option->addItem(CurrentLang["CPSkip"]);
        ui->comboBox_Option->addItem(CurrentLang["NoCPSkip"]); break;
    case 6: // Mehta
        ui->comboBox_Variant->addItem(CurrentLang["TDR"]);
        ui->comboBox_Variant->addItem(CurrentLang["6CP"]);
        ui->comboBox_Variant->addItem(CurrentLang["APDR"]);
        ui->comboBox_Variant->addItem(CurrentLang["CDRLL"]);
        ui->comboBox_Variant->addItem(CurrentLang["JTLE"]);
        ui->comboBox_Option->addItem(CurrentLang["None"]); break;
    case 7: // Nautilus
        ui->comboBox_Variant->addItem(CurrentLang["L5E-NCLL"]);
        ui->comboBox_Variant->addItem(CurrentLang["L5E-NCOLL"]);
        ui->comboBox_Variant->addItem(CurrentLang["L5E-NCLL/TNCLL"]);
        ui->comboBox_Variant->addItem(CurrentLang["LSLL-ZBLL"]);
        ui->comboBox_Variant->addItem(CurrentLang["LSLL-OCLL/PLL"]);
        ui->comboBox_Variant->addItem(CurrentLang["LSLL-COLL/EPLL"]);
        ui->comboBox_Option->addItem(CurrentLang["None"]); break;
    case 8: // LEOR
        ui->comboBox_Variant->addItem(CurrentLang["LEOR-A"]);
        ui->comboBox_Variant->addItem(CurrentLang["LEOR-B"]);
        ui->comboBox_Option->addItem(CurrentLang["ZBLL"]);
        ui->comboBox_Option->addItem(CurrentLang["OCLL+PLL"]);
        ui->comboBox_Option->addItem(CurrentLang["COLL+EPLL"]); break;
    default: // LBL
        ui->comboBox_Variant->addItem(CurrentLang["None"]);
        ui->comboBox_Option->addItem(CurrentLang["None"]); break;
    }
}

void MainWindow::on_pushButton_StartSearch_clicked()
{
    // Clear output and disable controls during search
    ui->textBrowser_report->clear();
    DisableWhileSearching();

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
    case 7: sNautilus(Scramble.ToString()); break;
    case 8: sLEOR(Scramble.ToString()); break;

    default:
        EnableAfterSearch();
        ui->statusBar->showMessage(CurrentLang["SearchCancelled"]);
        break;
    }
}

void MainWindow::on_pushButton_Skip_clicked()
{
    ui->textBrowser_report->append(CurrentLang["Skip"]);
    SearchThread->Skip();
}

void MainWindow::printCache()
{
    ui->textBrowser_report->append(CurrentLang["Cache"]);
}

void MainWindow::printMethod_NoSolves()
{
    ui->textBrowser_report->setText(CurrentLang["NoSolves"]);

    EnableAfterSearch(); // Enable controls after search
    ui->pushButton_StartSearch->setEnabled(true);
    ui->pushButton_Skip->setDisabled(true);
    ui->statusBar->showMessage(CurrentLang["SearchFinished"]);
}

void MainWindow::printMethod_Solves(const QString Scramble, const QString Result)
{
    QString Method = ui->comboBox_Method->currentText(),
            MethodText = ui->label_Method->text().toLower(),
            Variant = ui->comboBox_Variant->currentText(),
            VariantText = ui->label_Variant->text().toLower(),
            Option = ui->comboBox_Option->currentText(),
            OptionText = ui->label_Option->text().toLower();

    HtmlReport = "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n";
    HtmlReport += "<html><head><meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\">";
    HtmlReport +=  "<title>Solve</title></head>\n";
    HtmlReport += "<body><h1>" + CurrentLang["ScrambleHeader"] + "[" + Scramble + "]" + "</h1>";
    HtmlReport += "<h2 style=\"color: rgb(0, 0, 127);\">" + CurrentLang["MethodHeader"] + MethodText + " " + Method;
    HtmlReport += ", " + VariantText + " " + Variant;
    if (ui->comboBox_Option->count() > 1) HtmlReport += ", " + OptionText + " " + Option;
    HtmlReport += "</h2>";

    QStringList Lines = Result.split('\n');
    for (auto& Line : Lines)
    {
        HtmlReport += "<span>" + Line + "</span>";
        if (Line != Lines.last()) HtmlReport += "<br>";
    }
}

void MainWindow::printMethod_Best(const QString Result)
{
    HtmlReport += "<h2 style=\"color: rgb(0, 0, 127);\">";
    switch (ui->comboBox_Method->currentIndex())
    {
    case 1: HtmlReport += CurrentLang["CFOPBest"]; break;
    case 2: HtmlReport += CurrentLang["RouxBest"]; break;
    case 3: HtmlReport += CurrentLang["PetrusBest"]; break;
    case 4: HtmlReport += CurrentLang["ZZBest"]; break;
    case 5: HtmlReport += CurrentLang["CEORBest"]; break;
    case 6: HtmlReport += CurrentLang["MehtaBest"]; break;
    case 7: HtmlReport += CurrentLang["NautilusBest"]; break;
    case 8: HtmlReport += CurrentLang["LEORBest"]; break;
    default: break;
    }
    HtmlReport += "</h2>";

    QStringList Lines = Result.split('\n');
    for (auto& Line : Lines)
    {
        QStringList Parts = Line.split("//");
        if (Parts.size() == 2) HtmlReport += "<code style=\"color: rgb(127, 0, 0);\">" + Parts[0] + "</code><span> // " + Parts[1] + "</span><br>";
        else if (Parts.size() == 1) HtmlReport += "<span>" + Parts[0] + "</span><br>";
    }
}

void MainWindow::printMethod_Time(const QString Result)
{
    HtmlReport += "<h3 style=\"color: rgb(0, 0, 127);\">" + CurrentLang["TimeHeader"] + "</h3>";
    QStringList Lines = Result.split('\n');
    for (auto& Line : Lines) HtmlReport += "<span>" + Line + "</span><br>";

    HtmlReport += "</body>\n";

    ui->textBrowser_report->setHtml(HtmlReport);

    EnableAfterSearch(); // Enable controls after search
    ui->pushButton_StartSearch->setEnabled(true);
    ui->pushButton_Skip->setDisabled(true);
    ui->statusBar->showMessage(CurrentLang["SearchFinished"]);

    switch (ui->comboBox_Method->currentIndex())
    {
    case 1: AddToHistory(CurrentLang["CFOPSolve"]); break;
    case 2: AddToHistory(CurrentLang["RouxSolve"]); break;
    case 3: AddToHistory(CurrentLang["PetrusSolve"]); break;
    case 4: AddToHistory(CurrentLang["ZZSolve"]); break;
    case 5: AddToHistory(CurrentLang["CEORSolve"]); break;
    case 6: AddToHistory(CurrentLang["MehtaSolve"]); break;
    case 7: AddToHistory(CurrentLang["NautilusSolve"]); break;
    case 8: AddToHistory(CurrentLang["LEORSolve"]); break;

    default: break;
    }
}

  /*****************************************************************************************************************/
 /*** LBL *********************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sLBL(const std::string& Scramble)
{
    ui->pushButton_StartSearch->setDisabled(true); // Start button disabled while searching
    ui->pushButton_Skip->setDisabled(true); // LBL search is fast, skip is not necessary

    SearchThread->doStartSearchLBL(QString::fromStdString(Scramble),
                                   ui->comboBox_Orientation->currentIndex(),
                                   ui->spinBox_Cores->value(),
                                   ui->comboBox_Metric->currentIndex());
}

void MainWindow::printLBL_Solves(const QString Scramble, const QString CrossLayer, const QString Result)
{
    HtmlReport = "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n";
    HtmlReport += "<html><head><meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\">";
    HtmlReport +=  "<title>Layer by layer</title></head>\n";
    HtmlReport += "<body><h1>" + CurrentLang["ScrambleHeader"] + "[" + Scramble + "]" + "</h1>";
    HtmlReport += "<h2 style=\"color: rgb(0, 0, 127);\">" + CurrentLang["LBLHeader"] + CrossLayer + "</h2>";

    QStringList Lines = Result.split('\n');
    for (auto& Line : Lines)
    {
        QStringList Parts = Line.split("//");
        if (Parts.size() == 2) HtmlReport += "<code style=\"color: rgb(127, 0, 0);\">" + Parts[0] + "</code><span> // " + Parts[1] + "</span><br>";
        else if (Parts.size() == 1) HtmlReport += "<span>" + Parts[0] + "</span><br>";
    }

    HtmlReport += "</body>\n";

    ui->textBrowser_report->setHtml(HtmlReport);

    EnableAfterSearch(); // Enable controls after search
    ui->pushButton_StartSearch->setEnabled(true);
    ui->statusBar->showMessage(CurrentLang["SearchFinished"]);

    AddToHistory(CurrentLang["LBLSolve"]);
}

void MainWindow::printLBL_NoSolves()
{
    EnableAfterSearch(); // Enable controls after search
    ui->pushButton_StartSearch->setEnabled(true);
    ui->pushButton_Skip->setDisabled(true);
    ui->statusBar->showMessage(CurrentLang["SearchFinished"]);

    ui->textBrowser_report->setText(CurrentLang["LBLNoSolve"]);
}

void MainWindow::printLBL_Cross()
{
    ui->textBrowser_report->setText(CurrentLang["SearchingLBLCross"]);
}

void MainWindow::printLBL_FL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingLBLFLCorners"]);
}

void MainWindow::printLBL_SL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingLBLSL"]);
}

void MainWindow::printLBL_LL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingLBLLL"]);
}

  /*****************************************************************************************************************/
 /*** CFOP ********************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sCFOP(const std::string& Scramble)
{
    ui->pushButton_StartSearch->setDisabled(true);
    ui->pushButton_Skip->setEnabled(true);

    SearchThread->doStartSearchCFOP(QString::fromStdString(Scramble),
                                    ui->spinBox_Cores->value(),
                                    ui->checkBox_Cache->isChecked(),
                                    ui->comboBox_Variant->currentIndex(),
                                    ui->comboBox_Option->currentIndex(),
                                    ui->comboBox_Speed->currentIndex(),
                                    ui->comboBox_Orientation->currentIndex(),
                                    ui->comboBox_Amount->currentIndex(),
                                    ui->comboBox_Metric->currentIndex(),
                                    ui->checkBox_Regrips->isChecked(),
                                    ui->checkBox_Cancellations->isChecked());
}

void MainWindow::printCFOP_Cross()
{
    ui->textBrowser_report->setText(CurrentLang["SearchingCFOPCross"]);
}

void MainWindow::printCFOP_F2L()
{
    ui->textBrowser_report->append(CurrentLang["SearchingCFOPF2L"]);
}

void MainWindow::printCFOP_LL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingCFOPLL"]);
}

  /*****************************************************************************************************************/
 /*** Roux ********************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sRoux(const std::string& Scramble)
{
    ui->pushButton_StartSearch->setDisabled(true);
    ui->pushButton_Skip->setEnabled(true);

    SearchThread->doStartSearchRoux(QString::fromStdString(Scramble),
                                    ui->spinBox_Cores->value(),
                                    ui->checkBox_Cache->isChecked(),
                                    ui->comboBox_Variant->currentIndex(),
									ui->comboBox_Option->currentIndex(),
                                    ui->comboBox_Speed->currentIndex(),
                                    ui->comboBox_Orientation->currentIndex(),
                                    ui->comboBox_Amount->currentIndex(),
                                    ui->comboBox_Metric->currentIndex(),
                                    ui->checkBox_Regrips->isChecked(),
                                    ui->checkBox_Cancellations->isChecked());
}

void MainWindow::printRoux_FB()
{
    ui->textBrowser_report->setText(CurrentLang["SearchingRouxFB"]); 
}

void MainWindow::printRoux_SB()
{
    ui->textBrowser_report->append(CurrentLang["SearchingRouxSB"]);
}

void MainWindow::printRoux_CMLL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingRouxCMLL"]);
}

void MainWindow::printRoux_COLL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingRouxCOLL"]);
}

void MainWindow::printRoux_L6E()
{
    ui->textBrowser_report->append(CurrentLang["SearchingRouxL6E"]);
}

  /*****************************************************************************************************************/
 /*** Petrus ******************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sPetrus(const std::string& Scramble)
{
    ui->pushButton_StartSearch->setDisabled(true);
    ui->pushButton_Skip->setEnabled(true);

    SearchThread->doStartSearchPetrus(QString::fromStdString(Scramble),
                                      ui->spinBox_Cores->value(),
                                      ui->checkBox_Cache->isChecked(),
                                      ui->comboBox_Variant->currentIndex(),
                                      ui->comboBox_Option->currentIndex(),
                                      ui->comboBox_Speed->currentIndex(),
                                      ui->comboBox_Orientation->currentIndex(),
                                      ui->comboBox_Amount->currentIndex(),
                                      ui->comboBox_Metric->currentIndex(),
                                      ui->checkBox_Regrips->isChecked(),
                                      ui->checkBox_Cancellations->isChecked());
}

void MainWindow::printPetrus_FB()
{
    ui->textBrowser_report->setText(CurrentLang["SearchingPetrusBlocks"]);
}

void MainWindow::printPetrus_EB()
{
    ui->textBrowser_report->append(CurrentLang["SearchingPetrusEB"]);
}

void MainWindow::printPetrus_EO()
{
    ui->textBrowser_report->append(CurrentLang["SearchingPetrusEO"]);
}

void MainWindow::printPetrus_F2L()
{
    ui->textBrowser_report->append(CurrentLang["SearchingPetrusF2L"]);
}

void MainWindow::printPetrus_LL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingPetrusLL"]);
}

  /*****************************************************************************************************************/
 /*** ZZ **********************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sZZ(const std::string& Scramble)
{
    ui->pushButton_StartSearch->setDisabled(true);
    ui->pushButton_Skip->setEnabled(true);

    SearchThread->doStartSearchZZ(QString::fromStdString(Scramble),
                                      ui->spinBox_Cores->value(),
                                      ui->checkBox_Cache->isChecked(),
                                      ui->comboBox_Variant->currentIndex(),
                                      ui->comboBox_Option->currentIndex(),
                                      ui->comboBox_Speed->currentIndex(),
                                      ui->comboBox_Orientation->currentIndex(),
                                      ui->comboBox_Amount->currentIndex(),
                                      ui->comboBox_Metric->currentIndex(),
                                      ui->checkBox_Regrips->isChecked(),
                                      ui->checkBox_Cancellations->isChecked());
}

void MainWindow::printZZ_EOX()
{
    ui->textBrowser_report->setText(CurrentLang["SearchingZZEOX"]);
}

void MainWindow::printZZ_F2L()
{
    ui->textBrowser_report->append(CurrentLang["SearchingZZF2L"]);
}

void MainWindow::printZZ_LL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingZZLL"]);
}

  /*****************************************************************************************************************/
 /*** CEOR ********************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sCEOR(const std::string& Scramble)
{
    ui->pushButton_StartSearch->setDisabled(true);
    ui->pushButton_Skip->setEnabled(true);

    SearchThread->doStartSearchCEOR(QString::fromStdString(Scramble),
                                    ui->spinBox_Cores->value(),
                                    ui->checkBox_Cache->isChecked(),
                                    ui->comboBox_Variant->currentIndex(),
                                    ui->comboBox_Option->currentIndex(),
                                    ui->comboBox_Speed->currentIndex(),
                                    ui->comboBox_Orientation->currentIndex(),
                                    ui->comboBox_Amount->currentIndex(),
                                    ui->comboBox_Metric->currentIndex(),
                                    ui->checkBox_Regrips->isChecked(),
                                    ui->checkBox_Cancellations->isChecked());
}

void MainWindow::printCEOR_Lines()
{
    ui->textBrowser_report->setText(CurrentLang["SearchingYruRULines"]);
}

void MainWindow::printCEOR_CP()
{
    ui->textBrowser_report->append(CurrentLang["SearchingYruRUCP"]);
}

void MainWindow::printCEOR_CPLines()
{
    ui->textBrowser_report->setText(CurrentLang["SearchingYruRUCPLines"]);
}

void MainWindow::printCEOR_pEO()
{
    ui->textBrowser_report->append(CurrentLang["SearchingpYruRUpEOExtension"]);
}

void MainWindow::printCEOR_EOBF()
{
    ui->textBrowser_report->append(CurrentLang["SearchingYruRUEOBF"]);
}

void MainWindow::printCEOR_F2L()
{
    ui->textBrowser_report->append(CurrentLang["SearchingYruRUF2L"]);
}

void MainWindow::printCEOR_LL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingYruRULL"]);
}

  /*****************************************************************************************************************/
 /*** Mehta *******************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sMehta(const std::string& Scramble)
{
    ui->pushButton_StartSearch->setDisabled(true);
    ui->pushButton_Skip->setEnabled(true);

    SearchThread->doStartSearchMehta(QString::fromStdString(Scramble),
                                     ui->spinBox_Cores->value(),
                                     ui->checkBox_Cache->isChecked(),
                                     ui->comboBox_Variant->currentIndex(),
                                     ui->comboBox_Option->currentIndex(),
                                     ui->comboBox_Speed->currentIndex(),
                                     ui->comboBox_Orientation->currentIndex(),
                                     ui->comboBox_Amount->currentIndex(),
                                     ui->comboBox_Metric->currentIndex(),
                                     ui->checkBox_Regrips->isChecked(),
                                     ui->checkBox_Cancellations->isChecked());
}

void MainWindow::printMehta_FB()
{
    ui->textBrowser_report->setText(CurrentLang["SearchingMehtaFB"]);
}

void MainWindow::printMehta_3QB()
{
    ui->textBrowser_report->append(CurrentLang["SearchingMehta3QB"]);
}

void MainWindow::printMehta_EOLE()
{
    ui->textBrowser_report->append(CurrentLang["SearchingMehtaEOLE"]);
}

void MainWindow::printMehta_6CO()
{
    ui->textBrowser_report->append(CurrentLang["SearchingMehta6CO"]);
}

void MainWindow::printMehta_6CP()
{
    ui->textBrowser_report->append(CurrentLang["SearchingMehta6CP"]);
}

void MainWindow::printMehta_L5EP()
{
    ui->textBrowser_report->append(CurrentLang["SearchingMehtaL5EP"]);
}

void MainWindow::printMehta_APDR()
{
    ui->textBrowser_report->append(CurrentLang["SearchingMehtaAPDR"]);
}

void MainWindow::printMehta_PLL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingMehtaPLL"]);
}

void MainWindow::printMehta_DCAL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingMehtaDCAL"]);
}

void MainWindow::printMehta_CDRLL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingMehtaCDRLL"]);
}

void MainWindow::printMehta_JTLE()
{
    ui->textBrowser_report->append(CurrentLang["SearchingMehtaJTLE"]);
}

void MainWindow::printMehta_TDR()
{
    ui->textBrowser_report->append(CurrentLang["SearchingMehtaTDR"]);
}

void MainWindow::printMehta_ZBLL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingMehtaZBLL"]);
}

  /*****************************************************************************************************************/
 /*** Nautilus ****************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sNautilus(const std::string& Scramble)
{
  ui->pushButton_StartSearch->setDisabled(true);
  ui->pushButton_Skip->setEnabled(true);

  SearchThread->doStartSearchNautilus(QString::fromStdString(Scramble),
                                      ui->spinBox_Cores->value(),
                                      ui->checkBox_Cache->isChecked(),
                                      ui->comboBox_Variant->currentIndex(),
                                      ui->comboBox_Option->currentIndex(),
                                      ui->comboBox_Speed->currentIndex(),
                                      ui->comboBox_Orientation->currentIndex(),
                                      ui->comboBox_Amount->currentIndex(),
                                      ui->comboBox_Metric->currentIndex(),
                                      ui->checkBox_Regrips->isChecked(),
                                      ui->checkBox_Cancellations->isChecked());
}

void MainWindow::printNautilus_FB()
{
    ui->textBrowser_report->setText(CurrentLang["SearchingNautilusFB"]);
}

void MainWindow::printNautilus_SB()
{
    ui->textBrowser_report->append(CurrentLang["SearchingNautilusSB"]);
}

void MainWindow::printNautilus_dFR()
{
    ui->textBrowser_report->append(CurrentLang["SearchingNautilusdFR"]);
}

void MainWindow::printNautilus_NCLL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingNautilusNCLL"]);
}

void MainWindow::printNautilus_NCOLL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingNautilusNCOLL"]);
}

void MainWindow::printNautilus_TNCLL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingNautilusTNCLL"]);
}

void MainWindow::printNautilus_L5E()
{
    ui->textBrowser_report->append(CurrentLang["SearchingNautilusL5E"]);
}

void MainWindow::printNautilus_EODF()
{
    ui->textBrowser_report->append(CurrentLang["SearchingNautilusEODF"]);
}

void MainWindow::printNautilus_F2L()
{
    ui->textBrowser_report->append(CurrentLang["SearchingNautilusF2L"]);
}

void MainWindow::printNautilus_LL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingNautilusLL"]);
}

  /*****************************************************************************************************************/
 /*** LEOR ********************************************************************************************************/
/*****************************************************************************************************************/

void MainWindow::sLEOR(const std::string& Scramble)
{
  ui->pushButton_StartSearch->setDisabled(true);
  ui->pushButton_Skip->setEnabled(true);

  SearchThread->doStartSearchLEOR(QString::fromStdString(Scramble),
                                  ui->spinBox_Cores->value(),
                                  ui->checkBox_Cache->isChecked(),
                                  ui->comboBox_Variant->currentIndex(),
                                  ui->comboBox_Option->currentIndex(),
                                  ui->comboBox_Speed->currentIndex(),
                                  ui->comboBox_Orientation->currentIndex(),
                                  ui->comboBox_Amount->currentIndex(),
                                  ui->comboBox_Metric->currentIndex(),
                                  ui->checkBox_Regrips->isChecked(),
                                  ui->checkBox_Cancellations->isChecked());
}

void MainWindow::printLEOR_FB()
{
    ui->textBrowser_report->setText(CurrentLang["SearchingLEORFB"]);
}

void MainWindow::printLEOR_EOStripe()
{
    ui->textBrowser_report->append(CurrentLang["SearchingLEOREOStripe"]);
}

void MainWindow::printLEOR_FLPair()
{
    ui->textBrowser_report->append(CurrentLang["SearchingLEORFLPair"]);
}

void MainWindow::printLEOR_EODF()
{
    ui->textBrowser_report->append(CurrentLang["SearchingLEOREODF"]);
}

void MainWindow::printLEOR_SB()
{
    ui->textBrowser_report->append(CurrentLang["SearchingLEORSB"]);
}

void MainWindow::printLEOR_LL()
{
    ui->textBrowser_report->append(CurrentLang["SearchingLEORLL"]);
}

void MainWindow::on_pushButton_Collections_clicked()
{
    if (aw == nullptr) aw = new Algset();
    aw->show();
}

