#include "HierarchyPanel.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include "../Logger/Logger.h"

HierarchyPanel::HierarchyPanel(Game* game, QWidget* parent) 
    : QDockWidget("Hierarchy", parent), game(game) {
    
    // Create the container widget for the dock
    QWidget* container = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);

    // Create the Tree Widget
    treeWidget = new QTreeWidget();
    treeWidget->setColumnCount(3);
    treeWidget->setHeaderLabels(QStringList() << "ID" << "Tag" << "Group");
    treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    
    layout->addWidget(treeWidget);
    container->setLayout(layout);
    setWidget(container);

    // Timer to refresh the hierarchy periodically (e.g., 10 FPS for UI)
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &HierarchyPanel::UpdateHierarchy);
    refreshTimer->start(100);

    Logger::Log("\033[36m[Qt] Hierarchy Panel initialized.\033[0m");
}

HierarchyPanel::~HierarchyPanel() {
}

void HierarchyPanel::UpdateHierarchy() {
    if (!game || !game->GetRegistry()) return;

    // TODO: In the future, implement a diff algorithm to avoid clearing every frame
    // For now, clear and rebuild is robust enough for small entity counts
    treeWidget->clear();

    const auto& entities = game->GetRegistry()->GetEntities();

    for (const auto& entity : entities) {
        QTreeWidgetItem* item = new QTreeWidgetItem(treeWidget);
        
        // Column 0: ID
        item->setText(0, QString::number(entity.GetId()));

        // Column 1: Tag
        std::string tag = game->GetRegistry()->GetEntityTag(entity);
        item->setText(1, QString::fromStdString(tag));

        // Column 2: Group
        std::string group = game->GetRegistry()->GetEntityGroup(entity);
        item->setText(2, QString::fromStdString(group));
    }
}