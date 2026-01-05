#ifndef HIERARCHYPANEL_H
#define HIERARCHYPANEL_H

#include <QDockWidget>
#include <QTreeWidget>
#include <QTimer>
#include "../Game/Game.h"

class HierarchyPanel : public QDockWidget {
    Q_OBJECT

public:
    explicit HierarchyPanel(Game* game, QWidget* parent = nullptr);
    ~HierarchyPanel();

    signals:
        // Sinal emitido quando o usuário clica em uma entidade na lista
        void EntitySelected(int entityId);

private slots:
    void UpdateHierarchy();
    // Slot interno para tratar o clique do QTreeWidget
    void OnItemClicked(QTreeWidgetItem* item, int column);

private:
    Game* game;
    QTreeWidget* treeWidget;
    QTimer* refreshTimer;
};

#endif // HIERARCHYPANEL_H