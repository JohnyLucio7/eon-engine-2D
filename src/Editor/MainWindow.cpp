#include "MainWindow.h"
#include "../Logger/Logger.h"
#include <QStyle>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    this->setWindowTitle("Eon Engine Editor");
    this->resize(1280, 800);

    gameWidget = new GameWidget(this);
    setCentralWidget(gameWidget);

    hierarchyPanel = new HierarchyPanel(gameWidget->GetGame(), this);
    addDockWidget(Qt::LeftDockWidgetArea, hierarchyPanel);

    inspectorPanel = new InspectorPanel(gameWidget->GetGame(), this);
    addDockWidget(Qt::RightDockWidgetArea, inspectorPanel);

    scriptEditorPanel = new ScriptEditorPanel(gameWidget->GetGame(), this);
    addDockWidget(Qt::BottomDockWidgetArea, scriptEditorPanel);

    assetBrowserPanel = new AssetBrowserPanel(gameWidget->GetGame(), this);
    addDockWidget(Qt::BottomDockWidgetArea, assetBrowserPanel);
    tabifyDockWidget(scriptEditorPanel, assetBrowserPanel);
    assetBrowserPanel->raise();

    toolbar = addToolBar("Game Controls");
    toolbar->setMovable(false);

    playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), "Play", this);
    playAction->setShortcut(QKeySequence(Qt::Key_F5));
    connect(playAction, &QAction::triggered, this, &MainWindow::OnPlayClicked);
    toolbar->addAction(playAction);

    stopAction = new QAction(style()->standardIcon(QStyle::SP_MediaStop), "Stop", this);
    stopAction->setShortcut(QKeySequence(Qt::ShiftModifier + Qt::Key_F5));
    stopAction->setEnabled(false);
    connect(stopAction, &QAction::triggered, this, &MainWindow::OnStopClicked);
    toolbar->addAction(stopAction);

    connect(hierarchyPanel, &HierarchyPanel::EntitySelected,
            inspectorPanel, &InspectorPanel::OnEntitySelected);

    QList<QDockWidget*> hDocks;
    hDocks << hierarchyPanel << inspectorPanel;
    QList<int> hSizes;
    hSizes << 200 << 380;
    resizeDocks(hDocks, hSizes, Qt::Horizontal);

    resizeDocks({scriptEditorPanel, assetBrowserPanel}, {200, 200}, Qt::Vertical);

    Logger::Log("\033[36m[Qt] MainWindow initialized with Play/Stop Controls.\033[0m");
}

MainWindow::~MainWindow() {
}

void MainWindow::OnPlayClicked() {
    if (gameWidget && gameWidget->GetGame()) {
        gameWidget->GetGame()->SetGameMode(GameMode::Play);
        playAction->setEnabled(false);
        stopAction->setEnabled(true);
        Logger::Log("[Editor] Play Mode Activated.");
    }
}

void MainWindow::OnStopClicked() {
    if (gameWidget && gameWidget->GetGame()) {
        gameWidget->GetGame()->SetGameMode(GameMode::Edit);
        playAction->setEnabled(true);
        stopAction->setEnabled(false);
        Logger::Log("[Editor] Stop Mode Activated (State Restored).");
    }
}