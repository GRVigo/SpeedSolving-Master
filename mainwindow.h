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

#include "cube_definitions.h"

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

private:
    Ui::MainWindow *ui;

    LangMap DefaultLang, CurrentLang;

    std::vector<QString> History;
    uint SolveId;

    void AddToHistory(const QString&);

    void SaveXMLDefaultLanguage() const;
    bool LoadXMLDefaultLanguage();
    bool LoadXMLCurrentLanguage(const QString &);

    void UpdateFixedTexts(const LangMap&);
    bool CheckXMLLanguageFile(const QString&);

    void GetSearchSpins(std::vector<grcube3::Spn>&);

    void sLBL(const std::string&);
    void sCFOP(const std::string&);
    void sRoux(const std::string&);
    void sPetrus(const std::string&);
    void sZZ(const std::string&);
    void sCEOR(const std::string&);
    void sMehta(const std::string&);

};
#endif // MAINWINDOW_H
