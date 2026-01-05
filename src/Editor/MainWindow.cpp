#include "MainWindow.h"
#include "../Logger/Logger.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    this->setWindowTitle("Eon Engine Editor");
    this->resize(1280, 720);

    gameWidget = new GameWidget(this);
    setCentralWidget(gameWidget);

    // Initialize Hierarchy Panel
    // Note: gameWidget->GetGame() works because the object is created in constructor,
    // even if it's not initialized yet. The panel handles null checks.
    hierarchyPanel = new HierarchyPanel(gameWidget->GetGame(), this);
    addDockWidget(Qt::LeftDockWidgetArea, hierarchyPanel);

    Logger::Log("\033[36m[Qt] MainWindow initialized with GameWidget and HierarchyPanel.\033[0m");
}

MainWindow::~MainWindow() {
}