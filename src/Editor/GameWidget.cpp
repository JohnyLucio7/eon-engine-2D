#include "GameWidget.h"
#include <QShowEvent>
#include <QResizeEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QDir>
#include "../Logger/Logger.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"

GameWidget::GameWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // Habilita o Widget para aceitar Drops (arquivos do Asset Browser)
    setAcceptDrops(true);

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

// === Implementação Drag & Drop ===

void GameWidget::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void GameWidget::dropEvent(QDropEvent* event) {
    const QMimeData* mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        if (!urlList.isEmpty()) {
            QString filePath = urlList.first().toLocalFile();
            int mouseX = event->pos().x();
            int mouseY = event->pos().y();

            Logger::Log("\033[33m[Qt] Asset dropped: " + filePath.toStdString() + "\033[0m");
            CreateEntityFromAsset(filePath, mouseX, mouseY);
            event->acceptProposedAction();
        }
    }
}

void GameWidget::CreateEntityFromAsset(const QString& filePath, int mouseX, int mouseY) {
    if (!game) return;

    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix().toLower();
    QString fileName = fileInfo.fileName(); // Ex: tank.png

    if (extension == "png" || extension == "jpg" || extension == "jpeg") {

        // 1. Converter posição da tela para posição do mundo
        SDL_Rect camera = game->GetCamera();
        int worldX = camera.x + mouseX;
        int worldY = camera.y + mouseY;

        std::string assetId = fileName.toStdString();

        // 2. Carregar textura no AssetStore (mesmo que já exista, o AddTexture lida ou sobrescreve)
        // Precisamos do Renderer para criar a textura
        game->GetAssetStore()->AddTexture(game->GetRenderer(), assetId, filePath.toStdString());

        // 3. Obter dimensões da textura carregada para configurar o Sprite corretamente
        SDL_Texture* texture = game->GetAssetStore()->GetTexture(assetId);
        int texWidth = 32;
        int texHeight = 32;
        if (texture) {
            SDL_QueryTexture(texture, NULL, NULL, &texWidth, &texHeight);
        } else {
             Logger::Err("[Qt] Failed to retrieve texture after loading: " + assetId);
        }

        // 4. Criar Entidade e Componentes
        Registry* registry = game->GetRegistry();
        Entity newEntity = registry->CreateEntity();

        newEntity.AddComponent<TransformComponent>(
            glm::vec2(worldX, worldY),
            glm::vec2(1.0, 1.0),
            0.0
        );

        newEntity.AddComponent<SpriteComponent>(
            assetId,
            texWidth,
            texHeight,
            2 // Z-index padrão
        );

        // Opcional: Adicionar BoxCollider baseado no tamanho da imagem
        newEntity.AddComponent<BoxColliderComponent>(
            texWidth,
            texHeight
        );

        Logger::Log("\033[32m[Qt] Created Entity " + std::to_string(newEntity.GetId()) +
                    " from asset " + assetId + " at (" + std::to_string(worldX) + "," + std::to_string(worldY) + ")\033[0m");

    } else {
         Logger::Err("[Qt] Unsupported file type for drop: " + extension.toStdString());
    }
}