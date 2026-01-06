#include "MainWindow.h"
#include "../Logger/Logger.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    this->setWindowTitle("Eon Engine Editor");
    this->resize(1280, 800);

    // 1. Central Game Widget
    gameWidget = new GameWidget(this);
    setCentralWidget(gameWidget);

    // 2. Hierarchy Panel (Left)
    hierarchyPanel = new HierarchyPanel(gameWidget->GetGame(), this);
    addDockWidget(Qt::LeftDockWidgetArea, hierarchyPanel);

    // 3. Inspector Panel (Right)
    inspectorPanel = new InspectorPanel(gameWidget->GetGame(), this);
    addDockWidget(Qt::RightDockWidgetArea, inspectorPanel);

    // 4. Script Editor Panel (Bottom)
    scriptEditorPanel = new ScriptEditorPanel(gameWidget->GetGame(), this);
    addDockWidget(Qt::BottomDockWidgetArea, scriptEditorPanel);

    // 5. Asset Browser Panel (Bottom, tabbed with Script Editor or separate)
    assetBrowserPanel = new AssetBrowserPanel(gameWidget->GetGame(), this);
    // Adicionando ao fundo também, pode criar uma tab com o Script Editor
    addDockWidget(Qt::BottomDockWidgetArea, assetBrowserPanel);

    // Se quiser "tabify" (juntar em abas) os painéis inferiores:
    tabifyDockWidget(scriptEditorPanel, assetBrowserPanel);
    // Garante que o asset browser fique visível por padrão se preferir
    assetBrowserPanel->raise();

    // 6. Connect Signals
    connect(hierarchyPanel, &HierarchyPanel::EntitySelected,
            inspectorPanel, &InspectorPanel::OnEntitySelected);

    // 7. Layout Adjustment
    QList<QDockWidget*> hDocks;
    hDocks << hierarchyPanel << inspectorPanel;
    QList<int> hSizes;
    hSizes << 200 << 380;
    resizeDocks(hDocks, hSizes, Qt::Horizontal);

    resizeDocks({scriptEditorPanel, assetBrowserPanel}, {200, 200}, Qt::Vertical);

    Logger::Log("\033[36m[Qt] MainWindow initialized with Asset Browser.\033[0m");
}

MainWindow::~MainWindow() {
}