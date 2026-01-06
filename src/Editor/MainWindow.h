#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QTabWidget>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
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

private slots:
    void OnPlayClicked();
    void OnStopClicked();
    void OnAspectRatioChanged(int index);
    void OnResolutionChanged(int index);
    void UpdateGameViewLayout();

private:
    // Painéis Principais
    QTabWidget* mainTabWidget;
    QWidget* gameTab;
    QWidget* sceneTab; // Futuro

    // Game View Container e Controles
    QWidget* gameViewContainer;
    QWidget* gameViewport; // Widget cinza de fundo onde o GameWidget flutua
    GameWidget* gameWidget;
    QComboBox* aspectRatioComboBox;
    QComboBox* resolutionComboBox;
    QLabel* scaleLabel;

    // Painéis de Ferramentas
    HierarchyPanel* hierarchyPanel;
    InspectorPanel* inspectorPanel;
    ScriptEditorPanel* scriptEditorPanel;
    AssetBrowserPanel* assetBrowserPanel;

    // Toolbar Principal
    QToolBar* toolbar;
    QAction* playAction;
    QAction* stopAction;

    // Dados de Aspect Ratio e Resolução
    QVector<QPair<QString, double>> aspectRatios;
    QVector<Resolution> resolutions16_9;
    QVector<Resolution> resolutions16_10;
    QVector<Resolution> resolutions4_3;

    void SetupGameTab();
    void PopulateResolutionsForAspect(double aspect);
};
#endif