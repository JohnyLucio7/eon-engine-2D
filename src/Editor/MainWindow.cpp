#include "MainWindow.h"
#include "../Logger/Logger.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    this->setWindowTitle("Eon Engine Editor");
    this->resize(1280, 800); // Aumentado um pouco a altura para caber o editor de código

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

    // 5. Connect Signals
    connect(hierarchyPanel, &HierarchyPanel::EntitySelected,
            inspectorPanel, &InspectorPanel::OnEntitySelected);

    // 6. Layout Adjustment
    QList<QDockWidget*> hDocks;
    hDocks << hierarchyPanel << inspectorPanel;
    QList<int> hSizes;
    hSizes << 200 << 380;
    resizeDocks(hDocks, hSizes, Qt::Horizontal);

    // Define uma altura inicial razoável para o editor de script
    resizeDocks({scriptEditorPanel}, {200}, Qt::Vertical);

    Logger::Log("\033[36m[Qt] MainWindow initialized with Script Editor.\033[0m");
}

MainWindow::~MainWindow() {
}