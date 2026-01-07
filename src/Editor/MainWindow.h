#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QToolBar>
#include <QAction>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTimer>
#include <QDockWidget>
#include "GameWidget.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "ScriptEditorPanel.h"
#include "AssetBrowserPanel.h"

struct Resolution {
    int width;
    int height;
    QString name;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

private slots:
    void OnPlayClicked();
    void OnStopClicked();
    void OnAspectRatioChanged(int index);
    void OnResolutionChanged(int index);
    void UpdateGameViewLayout();

private:
    void SetupGameTab();
    void PopulateResolutionsForAspect(double aspect);

    QTabWidget* mainTabWidget;
    GameWidget* gameWidget;

    HierarchyPanel* hierarchyPanel;
    InspectorPanel* inspectorPanel;
    ScriptEditorPanel* scriptEditorPanel;
    AssetBrowserPanel* assetBrowserPanel;

    QToolBar* toolbar;
    QAction* playAction;
    QAction* stopAction;

    QWidget* gameTab;
    QWidget* gameViewport;
    QComboBox* aspectRatioComboBox;
    QComboBox* resolutionComboBox;
    QLabel* scaleLabel;

    QList<QPair<QString, double>> aspectRatios;
    QVector<Resolution> resolutions16_9;
    QVector<Resolution> resolutions16_10;
    QVector<Resolution> resolutions4_3;
};