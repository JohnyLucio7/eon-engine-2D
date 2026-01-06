#include "MainWindow.h"
#include "../Logger/Logger.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    this->setWindowTitle("Eon Engine Editor");
    this->resize(1280, 720);

    // 1. Central Game Widget
    gameWidget = new GameWidget(this);
    setCentralWidget(gameWidget);

    // 2. Hierarchy Panel (Left)
    hierarchyPanel = new HierarchyPanel(gameWidget->GetGame(), this);
    addDockWidget(Qt::LeftDockWidgetArea, hierarchyPanel);

    // 3. Inspector Panel (Right)
    inspectorPanel = new InspectorPanel(gameWidget->GetGame(), this);
    addDockWidget(Qt::RightDockWidgetArea, inspectorPanel);

    // 4. Connect Signals (Hierarchy Click -> Update Inspector)
    connect(hierarchyPanel, &HierarchyPanel::EntitySelected,
            inspectorPanel, &InspectorPanel::OnEntitySelected);

    // 5. Ajuste Fino de Layout (UX)
    // Força o Inspector a abrir com uma largura maior (380px) e o Hierarchy com 200px.
    // O Qt tentará respeitar esses valores iniciais sem travar o redimensionamento manual depois.
    QList<QDockWidget*> docks;
    docks << hierarchyPanel << inspectorPanel;

    QList<int> dockSizes;
    dockSizes << 200 << 250; // Largura em pixels: [Hierarchy, Inspector]

    resizeDocks(docks, dockSizes, Qt::Horizontal);

    Logger::Log("\033[36m[Qt] MainWindow initialized with all panels connected.\033[0m");
}

MainWindow::~MainWindow() {
}