#include "InspectorPanel.h"
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include "../Logger/Logger.h"

// Inclusão dos componentes para acesso aos dados
#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"

InspectorPanel::InspectorPanel(Game *game, QWidget *parent)
    : QDockWidget("Inspector", parent), game(game) {
    // Configuração da área de rolagem para quando houver muitos componentes
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    contentWidget = new QWidget();
    contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setAlignment(Qt::AlignTop);

    scrollArea->setWidget(contentWidget);
    setWidget(scrollArea);

    Logger::Log("\033[36m[Qt] Inspector Panel initialized.\033[0m");
}

InspectorPanel::~InspectorPanel() {
}

void InspectorPanel::OnEntitySelected(int entityId) {
    // Limpa o painel anterior
    QLayoutItem *item;
    while ((item = contentLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    if (!game || !game->GetRegistry()) return;

    // Busca a entidade pelo ID. Como não temos um GetEntityById direto seguro (além de criar um objeto Entity),
    // assumimos que o ID é válido pois veio da Hierarquia.
    // Nota: Entity é apenas um wrapper leve para um ID e um ponteiro de registro.
    Entity entity(entityId);
    entity.registry = game->GetRegistry();
    // OBS: O método GetRegistry no Game.cpp retorna Registry*, então usamos .get() se fosse smart pointer
    // mas na nossa implementação atual retorna ponteiro bruto, então só atribuímos.
    entity.registry = game->GetRegistry();

    // Título com ID
    QLabel *title = new QLabel("Entity ID: " + QString::number(entityId));
    title->setStyleSheet("font-weight: bold; font-size: 14px; margin-bottom: 10px;");
    contentLayout->addWidget(title);

    // === REFLEXÃO MANUAL: Verifica e desenha cada componente ===

    if (entity.HasComponent<TransformComponent>()) {
        DrawTransformComponent(entity, contentLayout);
    }

    if (entity.HasComponent<RigidbodyComponent>()) {
        DrawRigidBodyComponent(entity, contentLayout);
    }

    if (entity.HasComponent<SpriteComponent>()) {
        DrawSpriteComponent(entity, contentLayout);
    }

    if (entity.HasComponent<BoxColliderComponent>()) {
        DrawBoxColliderComponent(entity, contentLayout);
    }

    // Adicione chamadas para outros componentes aqui
}

// --- Implementação dos Drawers de Componentes ---

void InspectorPanel::DrawTransformComponent(Entity entity, QVBoxLayout *layout) {
    auto &transform = entity.GetComponent<TransformComponent>();

    QGroupBox *group = new QGroupBox("Transform");
    QFormLayout *form = new QFormLayout(group);

    // Position X
    QDoubleSpinBox *posX = new QDoubleSpinBox();
    posX->setRange(-10000, 10000);
    posX->setValue(transform.position.x);
    // Conexão bidirecional: UI -> ECS
    connect(posX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform](double val) {
        transform.position.x = val;
    });
    form->addRow("Position X:", posX);

    // Position Y
    QDoubleSpinBox *posY = new QDoubleSpinBox();
    posY->setRange(-10000, 10000);
    posY->setValue(transform.position.y);
    connect(posY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform](double val) {
        transform.position.y = val;
    });
    form->addRow("Position Y:", posY);

    // Scale X
    QDoubleSpinBox *scaleX = new QDoubleSpinBox();
    scaleX->setRange(0.1, 100);
    scaleX->setSingleStep(0.1);
    scaleX->setValue(transform.scale.x);
    connect(scaleX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform](double val) {
        transform.scale.x = val;
    });
    form->addRow("Scale X:", scaleX);

    // Scale Y
    QDoubleSpinBox *scaleY = new QDoubleSpinBox();
    scaleY->setRange(0.1, 100);
    scaleY->setSingleStep(0.1);
    scaleY->setValue(transform.scale.y);
    connect(scaleY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform](double val) {
        transform.scale.y = val;
    });
    form->addRow("Scale Y:", scaleY);

    // Rotation
    QDoubleSpinBox *rot = new QDoubleSpinBox();
    rot->setRange(0, 360);
    rot->setValue(transform.rotation);
    connect(rot, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&transform](double val) {
        transform.rotation = val;
    });
    form->addRow("Rotation:", rot);

    layout->addWidget(group);
}

void InspectorPanel::DrawRigidBodyComponent(Entity entity, QVBoxLayout *layout) {
    auto &rb = entity.GetComponent<RigidbodyComponent>();

    QGroupBox *group = new QGroupBox("RigidBody");
    QFormLayout *form = new QFormLayout(group);

    QDoubleSpinBox *velX = new QDoubleSpinBox();
    velX->setRange(-5000, 5000);
    velX->setValue(rb.velocity.x);
    connect(velX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&rb](double val) {
        rb.velocity.x = val;
    });
    form->addRow("Velocity X:", velX);

    QDoubleSpinBox *velY = new QDoubleSpinBox();
    velY->setRange(-5000, 5000);
    velY->setValue(rb.velocity.y);
    connect(velY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&rb](double val) {
        rb.velocity.y = val;
    });
    form->addRow("Velocity Y:", velY);

    layout->addWidget(group);
}

void InspectorPanel::DrawSpriteComponent(Entity entity, QVBoxLayout *layout) {
    auto &sprite = entity.GetComponent<SpriteComponent>();

    QGroupBox *group = new QGroupBox("Sprite");
    QFormLayout *form = new QFormLayout(group);

    // Width
    QSpinBox *width = new QSpinBox();
    width->setRange(0, 10000);
    width->setValue(sprite.width);
    connect(width, QOverload<int>::of(&QSpinBox::valueChanged), [&sprite](int val) {
        sprite.width = val;
    });
    form->addRow("Width:", width);

    // Height
    QSpinBox *height = new QSpinBox();
    height->setRange(0, 10000);
    height->setValue(sprite.height);
    connect(height, QOverload<int>::of(&QSpinBox::valueChanged), [&sprite](int val) {
        sprite.height = val;
    });
    form->addRow("Height:", height);

    // Z-Index
    QSpinBox *zIndex = new QSpinBox();
    zIndex->setRange(0, 100);
    zIndex->setValue(sprite.zIndex);
    connect(zIndex, QOverload<int>::of(&QSpinBox::valueChanged), [&sprite](int val) {
        sprite.zIndex = val;
    });
    form->addRow("Z-Index:", zIndex);

    layout->addWidget(group);
}

void InspectorPanel::DrawBoxColliderComponent(Entity entity, QVBoxLayout *layout) {
    auto &collider = entity.GetComponent<BoxColliderComponent>();

    QGroupBox *group = new QGroupBox("BoxCollider");
    QFormLayout *form = new QFormLayout(group);

    QSpinBox *w = new QSpinBox();
    w->setRange(0, 10000);
    w->setValue(collider.width);
    connect(w, QOverload<int>::of(&QSpinBox::valueChanged), [&collider](int val) {
        collider.width = val;
    });
    form->addRow("Width:", w);

    QSpinBox *h = new QSpinBox();
    h->setRange(0, 10000);
    h->setValue(collider.height);
    connect(h, QOverload<int>::of(&QSpinBox::valueChanged), [&collider](int val) {
        collider.height = val;
    });
    form->addRow("Height:", h);

    layout->addWidget(group);
}
