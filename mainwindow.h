/*  This file is part of "SpeedSolving Master"

    Copyright (C) 2022 German Ramos Rodriguez

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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

#include "algset.h"

#include "searchthread.h"

#include "cube_definitions.h"

using namespace grcube3;

using LangMap = QMap<QString, QString>;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void printLBL_Solves(const QString, const QString, const QString); // Scramble, Cross layer, Solve
    void printLBL_NoSolves();
    void printLBL_Cross();
    void printLBL_FL();
    void printLBL_SL();
    void printLBL_LL();

    void printMethod_Solves(const QString, const QString); // Scramble, Solves
    void printMethod_Best(const QString); // Best solve
    void printMethod_Time(const QString);
    void printMethod_NoSolves();

    void printCFOP_Cross();
    void printCFOP_F2L();
    void printCFOP_LL();

    void printRoux_FB();
    void printRoux_SB();
    void printRoux_CMLL();
    void printRoux_COLL();
    void printRoux_L6E();

    void printPetrus_FB();
    void printPetrus_EB();
    void printPetrus_EO();
    void printPetrus_F2L();
    void printPetrus_LL();

    void printZZ_EOX();
    void printZZ_F2L();
    void printZZ_LL();

    void printCEOR_Lines();
    void printCEOR_CP();
    void printCEOR_CPLines();
    void printCEOR_pEO();
    void printCEOR_EOBF();
    void printCEOR_F2L();
    void printCEOR_LL();

    void printMehta_FB();
    void printMehta_3QB();
    void printMehta_EOLE();
    void printMehta_6CO();
    void printMehta_6CP();
    void printMehta_L5EP();
    void printMehta_APDR();
    void printMehta_PLL();
    void printMehta_DCAL();
    void printMehta_CDRLL();
    void printMehta_JTLE();
    void printMehta_TDR();
    void printMehta_ZBLL();

    void printNautilus_FB();
    void printNautilus_SB();
    void printNautilus_dFR();
    void printNautilus_NCLL();
    void printNautilus_NCOLL();
    void printNautilus_TNCLL();
    void printNautilus_L5E();
    void printNautilus_EODF();
    void printNautilus_F2L();
    void printNautilus_LL();

    void printLEOR_FB();
    void printLEOR_EOStripe();
    void printLEOR_FLPair();
    void printLEOR_EODF();
    void printLEOR_SB();
    void printLEOR_LL();

    void printCache();

private slots:
    void on_lineEdit_Scramble_textChanged(const QString &);

    void on_pushButton_RandomScramble_clicked();

    void on_pushButton_CopyScramble_clicked();

    void on_pushButton_PasteScramble_clicked();

    void on_pushButton_ClearScramble_clicked();

    void on_pushButton_StartSearch_clicked();

    void on_pushButton_CopyReport_clicked();

    void on_pushButton_ClearReport_clicked();

    void on_pushButton_SaveReport_clicked();

    void on_comboBox_Language_currentIndexChanged(const QString &arg1);

    void on_spinBox_Cores_valueChanged(int cores);

    void on_pushButton_Credits_clicked();

    void on_pushButton_License_clicked();

    void on_comboBox_History_currentIndexChanged(int index);

    void on_comboBox_History_activated(int index);

    void on_pushButton_ClearHistory_clicked();

    void on_checkBox_Cache_stateChanged(int arg1);

    void on_pushButton_ZoomPlus_clicked();

    void on_pushButton_ZoomMinus_clicked();

    void on_comboBox_Method_currentIndexChanged(const QString &arg1);

    void on_comboBox_Orientation_currentIndexChanged(int index);

    void on_pushButton_Help_clicked();

    void on_pushButton_Debug_clicked();

    void on_pushButton_Skip_clicked();

    void on_pushButton_Collections_clicked();

private:
    Ui::MainWindow *ui;
    Algset* aw; // Algset Window

    RunSearch *SearchThread;

    LangMap DefaultLang, CurrentLang;

    std::vector<QString> History;
    uint SolveId;

    QString HtmlReport;

    void DisableWhileSearching();
    void EnableAfterSearch();

    void AddToHistory(const QString&);

    void SaveXMLDefaultLanguage() const;
    bool LoadXMLDefaultLanguage();
    bool LoadXMLCurrentLanguage(const QString&);

    void UpdateFixedTexts(const LangMap&);
    bool CheckXMLLanguageFile(const QString&);

    void sLBL(const std::string&);
    void sCFOP(const std::string&);
    void sRoux(const std::string&);
    void sPetrus(const std::string&);
    void sZZ(const std::string&);
    void sCEOR(const std::string&);
    void sMehta(const std::string&);
    void sNautilus(const std::string&);
    void sLEOR(const std::string&);
};
#endif // MAINWINDOW_H
