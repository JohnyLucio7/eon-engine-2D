#ifndef SCRIPTEDITORPANEL_H
#define SCRIPTEDITORPANEL_H

#include <QDockWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include "../Game/Game.h"

class ScriptEditorPanel : public QDockWidget {
    Q_OBJECT

public:
    explicit ScriptEditorPanel(Game* game, QWidget* parent = nullptr);
    ~ScriptEditorPanel();

private slots:
    void SaveScript();
    void ReloadScript();

private:
    void LoadScriptFile();

    Game* game;
    QPlainTextEdit* textEdit;
    QPushButton* btnSave;
    QPushButton* btnReload;
    QString currentFilePath;
};

#endif // SCRIPTEDITORPANEL_H