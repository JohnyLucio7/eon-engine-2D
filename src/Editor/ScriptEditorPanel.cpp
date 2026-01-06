#include "ScriptEditorPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFont>
#include "../Logger/Logger.h"

ScriptEditorPanel::ScriptEditorPanel(Game* game, QWidget* parent)
    : QDockWidget("Script Editor", parent), game(game) {
    
    QWidget* container = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(5, 5, 5, 5);

    // Toolbar (Buttons)
    QHBoxLayout* toolbarLayout = new QHBoxLayout();
    btnSave = new QPushButton("Save");
    btnReload = new QPushButton("Reload Scripts (Hot)");
    
    // Style the reload button to stand out
    btnReload->setStyleSheet("background-color: #d32f2f; color: white; font-weight: bold;");

    toolbarLayout->addWidget(btnSave);
    toolbarLayout->addWidget(btnReload);
    toolbarLayout->addStretch();

    // Text Editor
    textEdit = new QPlainTextEdit();
    // Use a monospaced font for code
    QFont font("Monospace");
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(10);
    textEdit->setFont(font);

    layout->addLayout(toolbarLayout);
    layout->addWidget(textEdit);
    container->setLayout(layout);
    setWidget(container);

    // Hardcoded path for Phase 4 as per current engine structure
    currentFilePath = "./assets/scripts/Level2.lua";
    LoadScriptFile();

    connect(btnSave, &QPushButton::clicked, this, &ScriptEditorPanel::SaveScript);
    connect(btnReload, &QPushButton::clicked, this, &ScriptEditorPanel::ReloadScript);

    Logger::Log("\033[36m[Qt] Script Editor Panel initialized.\033[0m");
}

ScriptEditorPanel::~ScriptEditorPanel() {
}

void ScriptEditorPanel::LoadScriptFile() {
    QFile file(currentFilePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        textEdit->setPlainText(in.readAll());
        file.close();
        Logger::Log("\033[36m[Qt] Loaded script: " + currentFilePath.toStdString() + "\033[0m");
    } else {
        Logger::Err("[Qt] Failed to open script file: " + currentFilePath.toStdString());
        textEdit->setPlainText("-- Error loading file: " + currentFilePath);
    }
}

void ScriptEditorPanel::SaveScript() {
    QFile file(currentFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << textEdit->toPlainText();
        file.close();
        Logger::Log("\033[32m[Qt] Script saved successfully.\033[0m");
    } else {
        QMessageBox::critical(this, "Error", "Could not save script file!");
        Logger::Err("[Qt] Failed to save script file.");
    }
}

void ScriptEditorPanel::ReloadScript() {
    // Auto-save before reloading
    SaveScript();
    
    if (game) {
        game->Reload();
    }
}