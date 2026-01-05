#ifndef INSPECTORPANEL_H
#define INSPECTORPANEL_H

#include <QDockWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include "../Game/Game.h"

class InspectorPanel : public QDockWidget {
    Q_OBJECT

public:
    explicit InspectorPanel(Game* game, QWidget* parent = nullptr);
    ~InspectorPanel();

public slots:
    // Slot chamado quando uma entidade é selecionada na hierarquia
    void OnEntitySelected(int entityId);

private:
    // Métodos auxiliares de "Reflexão Manual" para desenhar cada componente
    void DrawTransformComponent(Entity entity, QVBoxLayout* layout);
    void DrawRigidBodyComponent(Entity entity, QVBoxLayout* layout);
    void DrawSpriteComponent(Entity entity, QVBoxLayout* layout);
    void DrawBoxColliderComponent(Entity entity, QVBoxLayout* layout);
    // Adicione mais métodos conforme novos componentes forem criados

    Game* game;
    QWidget* contentWidget;
    QVBoxLayout* contentLayout;
};

#endif // INSPECTORPANEL_H