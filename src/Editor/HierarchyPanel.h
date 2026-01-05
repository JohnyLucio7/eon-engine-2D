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

private slots:
    void UpdateHierarchy();

private:
    Game* game;
    QTreeWidget* treeWidget;
    QTimer* refreshTimer;
};

#endif // HIERARCHYPANEL_H