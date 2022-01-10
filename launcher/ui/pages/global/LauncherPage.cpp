/* Copyright 2013-2021 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "LauncherPage.h"
#include "ui_LauncherPage.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QTextCharFormat>

#include "settings/SettingsObject.h"
#include <FileSystem.h>
#include "Application.h"
#include "BuildConfig.h"
#include "ui/themes/ITheme.h"

#include <QApplication>
#include <QProcess>

// FIXME: possibly move elsewhere
enum InstSortMode
{
    // Sort alphabetically by name.
    Sort_Name,
    // Sort by which instance was launched most recently.
    Sort_LastLaunch
};

LauncherPage::LauncherPage(QWidget *parent) : QWidget(parent), ui(new Ui::LauncherPage)
{
    ui->setupUi(this);
    auto origForeground = ui->fontPreview->palette().color(ui->fontPreview->foregroundRole());
    auto origBackground = ui->fontPreview->palette().color(ui->fontPreview->backgroundRole());
    m_colors.reset(new LogColorCache(origForeground, origBackground));

    ui->sortingModeGroup->setId(ui->sortByNameBtn, Sort_Name);
    ui->sortingModeGroup->setId(ui->sortLastLaunchedBtn, Sort_LastLaunch);

    defaultFormat = new QTextCharFormat(ui->fontPreview->currentCharFormat());

    m_languageModel = APPLICATION->translations();
    loadSettings();

    connect(ui->fontSizeBox, SIGNAL(valueChanged(int)), SLOT(refreshFontPreview()));
    connect(ui->consoleFont, SIGNAL(currentFontChanged(QFont)), SLOT(refreshFontPreview()));

    //move mac data button
    QFile file(QDir::current().absolutePath() + "/dontmovemacdata");
    if (!file.exists())
    {
        ui->migrateDataFolderMacBtn->setVisible(false);
    }
}

LauncherPage::~LauncherPage()
{
    delete ui;
    delete defaultFormat;
}

bool LauncherPage::apply()
{
    applySettings();
    return true;
}

void LauncherPage::on_instDirBrowseBtn_clicked()
{
    QString raw_dir = QFileDialog::getExistingDirectory(this, tr("Instance Folder"), ui->instDirTextBox->text());

    // do not allow current dir - it's dirty. Do not allow dirs that don't exist
    if (!raw_dir.isEmpty() && QDir(raw_dir).exists())
    {
        QString cooked_dir = FS::NormalizePath(raw_dir);
        if (FS::checkProblemticPathJava(QDir(cooked_dir)))
        {
            QMessageBox warning;
            warning.setText(tr("You're trying to specify an instance folder which\'s path "
                               "contains at least one \'!\'. "
                               "Java is known to cause problems if that is the case, your "
                               "instances (probably) won't start!"));
            warning.setInformativeText(
                tr("Do you really want to use this path? "
                   "Selecting \"No\" will close this and not alter your instance path."));
            warning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            int result = warning.exec();
            if (result == QMessageBox::Yes)
            {
                ui->instDirTextBox->setText(cooked_dir);
            }
        }
        else
        {
            ui->instDirTextBox->setText(cooked_dir);
        }
    }
}

void LauncherPage::on_iconsDirBrowseBtn_clicked()
{
    QString raw_dir = QFileDialog::getExistingDirectory(this, tr("Icons Folder"), ui->iconsDirTextBox->text());

    // do not allow current dir - it's dirty. Do not allow dirs that don't exist
    if (!raw_dir.isEmpty() && QDir(raw_dir).exists())
    {
        QString cooked_dir = FS::NormalizePath(raw_dir);
        ui->iconsDirTextBox->setText(cooked_dir);
    }
}
void LauncherPage::on_modsDirBrowseBtn_clicked()
{
    QString raw_dir = QFileDialog::getExistingDirectory(this, tr("Mods Folder"), ui->modsDirTextBox->text());

    // do not allow current dir - it's dirty. Do not allow dirs that don't exist
    if (!raw_dir.isEmpty() && QDir(raw_dir).exists())
    {
        QString cooked_dir = FS::NormalizePath(raw_dir);
        ui->modsDirTextBox->setText(cooked_dir);
    }
}
void LauncherPage::on_migrateDataFolderMacBtn_clicked()
{
    QFile file(QDir::current().absolutePath() + "/dontmovemacdata");
    file.remove();
    QProcess::startDetached(qApp->arguments()[0]);
    qApp->quit();
}


void LauncherPage::applySettings()
{
    auto s = APPLICATION->settings();

    if (ui->resetNotificationsBtn->isChecked())
    {
        s->set("ShownNotifications", QString());
    }

    auto original = s->get("IconTheme").toString();
    //FIXME: make generic
    switch (ui->themeComboBox->currentIndex())
    {
    case 1:
        s->set("IconTheme", "pe_dark");
        break;
    case 2:
        s->set("IconTheme", "pe_light");
        break;
    case 3:
        s->set("IconTheme", "pe_blue");
        break;
    case 4:
        s->set("IconTheme", "pe_colored");
        break;
    case 5:
        s->set("IconTheme", "OSX");
        break;
    case 6:
        s->set("IconTheme", "iOS");
        break;
    case 7:
        s->set("IconTheme", "flat");
        break;
    case 8:
        s->set("IconTheme", "custom");
        break;
    case 0:
    default:
        s->set("IconTheme", "multimc");
        break;
    }

    if(original != s->get("IconTheme"))
    {
        APPLICATION->setIconTheme(s->get("IconTheme").toString());
    }

    auto originalAppTheme = s->get("ApplicationTheme").toString();
    auto newAppTheme = ui->themeComboBoxColors->currentData().toString();
    if(originalAppTheme != newAppTheme)
    {
        s->set("ApplicationTheme", newAppTheme);
        APPLICATION->setApplicationTheme(newAppTheme, false);
    }

    // Console settings
    s->set("ShowConsole", ui->showConsoleCheck->isChecked());
    s->set("AutoCloseConsole", ui->autoCloseConsoleCheck->isChecked());
    s->set("ShowConsoleOnError", ui->showConsoleErrorCheck->isChecked());
    QString consoleFontFamily = ui->consoleFont->currentFont().family();
    s->set("ConsoleFont", consoleFontFamily);
    s->set("ConsoleFontSize", ui->fontSizeBox->value());
    s->set("ConsoleMaxLines", ui->lineLimitSpinBox->value());
    s->set("ConsoleOverflowStop", ui->checkStopLogging->checkState() != Qt::Unchecked);

    // Folders
    // TODO: Offer to move instances to new instance folder.
    s->set("InstanceDir", ui->instDirTextBox->text());
    s->set("CentralModsDir", ui->modsDirTextBox->text());
    s->set("IconsDir", ui->iconsDirTextBox->text());

    auto sortMode = (InstSortMode)ui->sortingModeGroup->checkedId();
    switch (sortMode)
    {
    case Sort_LastLaunch:
        s->set("InstSortMode", "LastLaunch");
        break;
    case Sort_Name:
    default:
        s->set("InstSortMode", "Name");
        break;
    }
}
void LauncherPage::loadSettings()
{
    auto s = APPLICATION->settings();
    //FIXME: make generic
    auto theme = s->get("IconTheme").toString();
    if (theme == "pe_dark")
    {
        ui->themeComboBox->setCurrentIndex(1);
    }
    else if (theme == "pe_light")
    {
        ui->themeComboBox->setCurrentIndex(2);
    }
    else if (theme == "pe_blue")
    {
        ui->themeComboBox->setCurrentIndex(3);
    }
    else if (theme == "pe_colored")
    {
        ui->themeComboBox->setCurrentIndex(4);
    }
    else if (theme == "OSX")
    {
        ui->themeComboBox->setCurrentIndex(5);
    }
    else if (theme == "iOS")
    {
        ui->themeComboBox->setCurrentIndex(6);
    }
    else if (theme == "flat")
    {
        ui->themeComboBox->setCurrentIndex(7);
    }
    else if (theme == "custom")
    {
        ui->themeComboBox->setCurrentIndex(8);
    }
    else
    {
        ui->themeComboBox->setCurrentIndex(0);
    }

    {
        auto currentTheme = s->get("ApplicationTheme").toString();
        auto themes = APPLICATION->getValidApplicationThemes();
        int idx = 0;
        for(auto &theme: themes)
        {
            ui->themeComboBoxColors->addItem(theme->name(), theme->id());
            if(currentTheme == theme->id())
            {
                ui->themeComboBoxColors->setCurrentIndex(idx);
            }
            idx++;
        }
    }

    // Console settings
    ui->showConsoleCheck->setChecked(s->get("ShowConsole").toBool());
    ui->autoCloseConsoleCheck->setChecked(s->get("AutoCloseConsole").toBool());
    ui->showConsoleErrorCheck->setChecked(s->get("ShowConsoleOnError").toBool());
    QString fontFamily = APPLICATION->settings()->get("ConsoleFont").toString();
    QFont consoleFont(fontFamily);
    ui->consoleFont->setCurrentFont(consoleFont);

    bool conversionOk = true;
    int fontSize = APPLICATION->settings()->get("ConsoleFontSize").toInt(&conversionOk);
    if(!conversionOk)
    {
        fontSize = 11;
    }
    ui->fontSizeBox->setValue(fontSize);
    refreshFontPreview();
    ui->lineLimitSpinBox->setValue(s->get("ConsoleMaxLines").toInt());
    ui->checkStopLogging->setChecked(s->get("ConsoleOverflowStop").toBool());

    // Folders
    ui->instDirTextBox->setText(s->get("InstanceDir").toString());
    ui->modsDirTextBox->setText(s->get("CentralModsDir").toString());
    ui->iconsDirTextBox->setText(s->get("IconsDir").toString());

    QString sortMode = s->get("InstSortMode").toString();

    if (sortMode == "LastLaunch")
    {
        ui->sortLastLaunchedBtn->setChecked(true);
    }
    else
    {
        ui->sortByNameBtn->setChecked(true);
    }
}

void LauncherPage::refreshFontPreview()
{
    int fontSize = ui->fontSizeBox->value();
    QString fontFamily = ui->consoleFont->currentFont().family();
    ui->fontPreview->clear();
    defaultFormat->setFont(QFont(fontFamily, fontSize));
    {
        QTextCharFormat format(*defaultFormat);
        format.setForeground(m_colors->getFront(MessageLevel::Error));
        // append a paragraph/line
        auto workCursor = ui->fontPreview->textCursor();
        workCursor.movePosition(QTextCursor::End);
        workCursor.insertText(tr("[Something/ERROR] A spooky error!"), format);
        workCursor.insertBlock();
    }
    {
        QTextCharFormat format(*defaultFormat);
        format.setForeground(m_colors->getFront(MessageLevel::Message));
        // append a paragraph/line
        auto workCursor = ui->fontPreview->textCursor();
        workCursor.movePosition(QTextCursor::End);
        workCursor.insertText(tr("[Test/INFO] A harmless message..."), format);
        workCursor.insertBlock();
    }
    {
        QTextCharFormat format(*defaultFormat);
        format.setForeground(m_colors->getFront(MessageLevel::Warning));
        // append a paragraph/line
        auto workCursor = ui->fontPreview->textCursor();
        workCursor.movePosition(QTextCursor::End);
        workCursor.insertText(tr("[Something/WARN] A not so spooky warning."), format);
        workCursor.insertBlock();
    }
}