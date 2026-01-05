#include "GameWidget.h"
#include <QShowEvent>
#include <QResizeEvent>
#include "../Logger/Logger.h"

GameWidget::GameWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_OpaquePaintEvent);

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    game = std::make_unique<Game>();
    gameLoopTimer = new QTimer(this);
}

GameWidget::~GameWidget() {
    if (game) {
        game->Destroy();
    }
}

QPaintEngine* GameWidget::paintEngine() const {
    return nullptr;
}

void GameWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);

    static bool isInitialized = false;
    if (isInitialized) return;

    void* nativeWindowHandle = (void*)this->winId();

    Logger::Log("\033[36m[Qt] GameWidget obtaining Native Window Handle...\033[0m");

    game->AttachToWindow(nativeWindowHandle, width(), height());
    game->Initialize();
    game->Setup();

    connect(gameLoopTimer, &QTimer::timeout, this, &GameWidget::GameLoop);
    gameLoopTimer->start(16);

    isInitialized = true;
}

void GameWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    Logger::Log("\033[36m[Qt] GameWidget resized.\033[0m");
}

void GameWidget::GameLoop() {
    game->ProcessInput();
    game->Update();
    game->Render();
}

Game* GameWidget::GetGame() const {
    return game.get();
}