#ifndef HIERARCHYPANEL_H
#define HIERARCHYPANEL_H

#include <QDockWidget>
#include <QTreeWidget>
#include <QTimer>
#include <map>
#include "../Game/Game.h"

class HierarchyPanel : public QDockWidget {
    Q_OBJECT

public:
    explicit HierarchyPanel(Game* game, QWidget* parent = nullptr);
    ~HierarchyPanel();

    signals:
        void EntitySelected(int entityId);

private slots:
    void UpdateHierarchy();
    void OnItemClicked(QTreeWidgetItem* item, int column);

private:
    Game* game;
    QTreeWidget* treeWidget;
    QTimer* refreshTimer;

    // Mapeia ID da Entidade -> Item Visual na Árvore
    // Isso permite atualizar itens existentes sem destruir a árvore inteira
    std::map<int, QTreeWidgetItem*> activeTreeItems;
};

#endif // HIERARCHYPANEL_H