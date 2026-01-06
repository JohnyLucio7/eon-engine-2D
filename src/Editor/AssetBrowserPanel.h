#ifndef ASSETBROWSERPANEL_H
#define ASSETBROWSERPANEL_H

#include <QDockWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include "../Game/Game.h"

class AssetBrowserPanel : public QDockWidget {
    Q_OBJECT

public:
    explicit AssetBrowserPanel(Game* game, QWidget* parent = nullptr);
    ~AssetBrowserPanel();

private:
    Game* game;
    QTreeView* treeView;
    QFileSystemModel* fileModel;
};

#endif // ASSETBROWSERPANEL_H