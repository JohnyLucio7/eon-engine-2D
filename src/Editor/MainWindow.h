#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include "GameWidget.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "ScriptEditorPanel.h"
#include "AssetBrowserPanel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void OnPlayClicked();
    void OnStopClicked();

private:
    GameWidget* gameWidget;
    HierarchyPanel* hierarchyPanel;
    InspectorPanel* inspectorPanel;
    ScriptEditorPanel* scriptEditorPanel;
    AssetBrowserPanel* assetBrowserPanel;

    QToolBar* toolbar;
    QAction* playAction;
    QAction* stopAction;
};
#endif