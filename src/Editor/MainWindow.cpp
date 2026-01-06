#include "MainWindow.h"
#include "../Logger/Logger.h"
#include <QStyle>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    this->setWindowTitle("Eon Engine Editor");
    this->resize(1280, 800);

    // Configuração do Tab Widget Central
    mainTabWidget = new QTabWidget(this);
    setCentralWidget(mainTabWidget);

    // Setup do Game Widget e sua Aba
    gameWidget = new GameWidget(this);
    SetupGameTab();

    // Inicialização dos Painéis Acopláveis
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

    // Toolbar Principal
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

    // Conexões de sinais
    connect(hierarchyPanel, &HierarchyPanel::EntitySelected,
            inspectorPanel, &InspectorPanel::OnEntitySelected);

    // Ajuste dos Docks
    QList<QDockWidget*> hDocks;
    hDocks << hierarchyPanel << inspectorPanel;
    QList<int> hSizes;
    hSizes << 250 << 400;
    resizeDocks(hDocks, hSizes, Qt::Horizontal);

    resizeDocks({scriptEditorPanel, assetBrowserPanel}, {250, 250}, Qt::Vertical);

    // Definição das Resoluções e Aspect Ratios
    aspectRatios.append(qMakePair(QString("Free Aspect"), 0.0));
    aspectRatios.append(qMakePair(QString("16:9"), 1.7777));
    aspectRatios.append(qMakePair(QString("16:10"), 1.6));
    aspectRatios.append(qMakePair(QString("4:3"), 1.3333));

    resolutions16_9.append({3840, 2160, "4K UHD"});
    resolutions16_9.append({1920, 1080, "Full HD"});
    resolutions16_9.append({1280, 720, "HD"});

    resolutions16_10.append({2560, 1600, "WQXGA"});
    resolutions16_10.append({1920, 1200, "WUXGA"});
    resolutions16_10.append({1280, 800, "WXGA"});

    resolutions4_3.append({1024, 768, "XGA"});
    resolutions4_3.append({800, 600, "SVGA"});
    resolutions4_3.append({640, 480, "VGA"});

    // Popula Combo de Aspect Ratio
    for(const auto& ar : aspectRatios) {
        aspectRatioComboBox->addItem(ar.first);
    }

    Logger::Log("\033[36m[Qt] MainWindow initialized with Unity-Like Game View.\033[0m");
}

MainWindow::~MainWindow() {
}

void MainWindow::SetupGameTab() {
    gameTab = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(gameTab);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Barra de Ferramentas da Game View
    QWidget* gameToolbar = new QWidget();
    gameToolbar->setFixedHeight(30);
    gameToolbar->setStyleSheet("background-color: #333333;");
    QHBoxLayout* toolbarLayout = new QHBoxLayout(gameToolbar);
    toolbarLayout->setContentsMargins(5, 0, 5, 0);

    QLabel* aspectLabel = new QLabel("Aspect:");
    aspectLabel->setStyleSheet("color: white;");
    aspectRatioComboBox = new QComboBox();
    connect(aspectRatioComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnAspectRatioChanged(int)));

    resolutionComboBox = new QComboBox();
    resolutionComboBox->setEnabled(false); // Inicia desabilitado (Free Aspect)
    connect(resolutionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnResolutionChanged(int)));

    scaleLabel = new QLabel("Scale: 1x");
    scaleLabel->setStyleSheet("color: white; padding-left: 10px;");

    toolbarLayout->addWidget(aspectLabel);
    toolbarLayout->addWidget(aspectRatioComboBox);
    toolbarLayout->addWidget(resolutionComboBox);
    toolbarLayout->addWidget(scaleLabel);
    toolbarLayout->addStretch();

    // Área do Viewport (Fundo Cinza)
    gameViewport = new QWidget();
    gameViewport->setStyleSheet("background-color: #202020;"); // Cor escura estilo Unity

    // O GameWidget será adicionado ao gameViewport, mas não via layout padrão para permitir centralização manual
    // Para simplificar, usamos um Grid Layout centralizado
    QGridLayout* viewportLayout = new QGridLayout(gameViewport);
    viewportLayout->setContentsMargins(0, 0, 0, 0);
    viewportLayout->setAlignment(Qt::AlignCenter);
    viewportLayout->addWidget(gameWidget);

    mainLayout->addWidget(gameToolbar);
    mainLayout->addWidget(gameViewport);

    mainTabWidget->addTab(gameTab, "Game");
}

void MainWindow::OnAspectRatioChanged(int index) {
    double aspect = aspectRatios[index].second;

    resolutionComboBox->clear();

    if (aspect == 0.0) { // Free Aspect
        resolutionComboBox->addItem("Window Size");
        resolutionComboBox->setEnabled(false);
    } else {
        resolutionComboBox->setEnabled(true);
        PopulateResolutionsForAspect(aspect);
    }

    UpdateGameViewLayout();
}

void MainWindow::PopulateResolutionsForAspect(double aspect) {
    const QVector<Resolution>* resList = nullptr;

    if (qAbs(aspect - 1.7777) < 0.01) resList = &resolutions16_9;
    else if (qAbs(aspect - 1.6) < 0.01) resList = &resolutions16_10;
    else if (qAbs(aspect - 1.3333) < 0.01) resList = &resolutions4_3;

    if (resList) {
        for (const auto& res : *resList) {
            QString itemText = QString("%1 (%2x%3)").arg(res.name).arg(res.width).arg(res.height);
            resolutionComboBox->addItem(itemText, QVariant::fromValue(QSize(res.width, res.height)));
        }
        // Seleciona um padrão (ex: Full HD ou primeiro da lista)
        if (resolutionComboBox->count() > 1) resolutionComboBox->setCurrentIndex(1);
        else resolutionComboBox->setCurrentIndex(0);
    }
}

void MainWindow::OnResolutionChanged(int index) {
    UpdateGameViewLayout();
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    UpdateGameViewLayout();
}

void MainWindow::UpdateGameViewLayout() {
    if (!gameWidget || !gameViewport) return;

    double targetAspect = aspectRatios[aspectRatioComboBox->currentIndex()].second;
    QSize availableSize = gameViewport->size();

    if (availableSize.width() == 0 || availableSize.height() == 0) return;

    int newWidth, newHeight;
    double scaleFactor = 1.0;
    int logicalWidth, logicalHeight;

    if (targetAspect == 0.0) {
        // Free Aspect
        newWidth = availableSize.width();
        newHeight = availableSize.height();
        logicalWidth = newWidth;
        logicalHeight = newHeight;
        scaleFactor = 1.0;
    } else {
        // Fixed Aspect
        // Pega a resolução lógica selecionada
        QSize selectedRes = resolutionComboBox->currentData().toSize();
        if (!selectedRes.isValid()) selectedRes = QSize(1920, 1080); // Fallback

        logicalWidth = selectedRes.width();
        logicalHeight = selectedRes.height();

        // Calcula tamanho físico do widget para caber no viewport mantendo aspecto
        double availableAspect = (double)availableSize.width() / availableSize.height();

        if (availableAspect > targetAspect) {
            // Viewport é mais largo que o alvo (Pillarbox)
            newHeight = availableSize.height();
            newWidth = static_cast<int>(newHeight * targetAspect);
        } else {
            // Viewport é mais alto que o alvo (Letterbox)
            newWidth = availableSize.width();
            newHeight = static_cast<int>(newWidth / targetAspect);
        }

        scaleFactor = (double)newWidth / logicalWidth;
    }

    // Aplica as dimensões físicas ao widget Qt (isso redimensiona a janela SDL)
    gameWidget->setFixedSize(newWidth, newHeight);

    // Atualiza a engine com o tamanho físico e a resolução lógica
    if (gameWidget->GetGame()) {
        gameWidget->GetGame()->ResizeWindow(newWidth, newHeight);
        gameWidget->GetGame()->SetRenderLogicalSize(logicalWidth, logicalHeight);
    }

    scaleLabel->setText(QString("Scale: %1x").arg(scaleFactor, 0, 'f', 2));
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