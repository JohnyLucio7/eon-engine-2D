#include "HierarchyPanel.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include "../Logger/Logger.h"

HierarchyPanel::HierarchyPanel(Game* game, QWidget* parent) 
    : QDockWidget("Hierarchy", parent), game(game) {
    
    QWidget* container = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);

    treeWidget = new QTreeWidget();
    treeWidget->setColumnCount(3);
    treeWidget->setHeaderLabels(QStringList() << "ID" << "Tag" << "Group");
    treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Conecta o clique no item ao nosso slot de processamento
    connect(treeWidget, &QTreeWidget::itemClicked, this, &HierarchyPanel::OnItemClicked);

    layout->addWidget(treeWidget);
    container->setLayout(layout);
    setWidget(container);

    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &HierarchyPanel::UpdateHierarchy);
    refreshTimer->start(100);

    Logger::Log("\033[36m[Qt] Hierarchy Panel initialized.\033[0m");
}

HierarchyPanel::~HierarchyPanel() {
}

void HierarchyPanel::OnItemClicked(QTreeWidgetItem* item, int column) {
    if (!item) return;

    // A coluna 0 contém o ID como string
    QString idString = item->text(0);
    int entityId = idString.toInt();

    // Emite o sinal para quem estiver escutando (InspectorPanel)
    emit EntitySelected(entityId);

    // Log para debug visual
    // Logger::Log("\033[36m[Qt] Entity Selected: " + std::to_string(entityId) + "\033[0m");
}

void HierarchyPanel::UpdateHierarchy() {
    if (!game || !game->GetRegistry()) return;

    // Salva o ID selecionado atualmente para tentar restaurar a seleção após o refresh
    int selectedId = -1;
    if (!treeWidget->selectedItems().isEmpty()) {
        selectedId = treeWidget->selectedItems().first()->text(0).toInt();
    }

    treeWidget->clear();

    const auto& entities = game->GetRegistry()->GetEntities();

    for (const auto& entity : entities) {
        QTreeWidgetItem* item = new QTreeWidgetItem(treeWidget);

        item->setText(0, QString::number(entity.GetId()));

        std::string tag = game->GetRegistry()->GetEntityTag(entity);
        item->setText(1, QString::fromStdString(tag));

        std::string group = game->GetRegistry()->GetEntityGroup(entity);
        item->setText(2, QString::fromStdString(group));

        // Restaura a seleção se este for o item que estava selecionado
        if (entity.GetId() == selectedId) {
            item->setSelected(true);
        }
    }
}