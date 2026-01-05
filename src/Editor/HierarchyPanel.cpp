#include "HierarchyPanel.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QCheckBox>
#include "../Logger/Logger.h"
#include <set>

HierarchyPanel::HierarchyPanel(Game* game, QWidget* parent)
    : QDockWidget("Hierarchy", parent), game(game) {

    QWidget* container = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);

    // Tree Widget Configuration
    treeWidget = new QTreeWidget();
    treeWidget->setColumnCount(3);
    treeWidget->setHeaderLabels(QStringList() << "ID" << "Tag" << "Group");
    treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Configurações visuais para melhorar a UX
    treeWidget->setAlternatingRowColors(true);
    treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(treeWidget, &QTreeWidget::itemClicked, this, &HierarchyPanel::OnItemClicked);

    layout->addWidget(treeWidget);
    container->setLayout(layout);
    setWidget(container);

    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &HierarchyPanel::UpdateHierarchy);
    // Aumentei a frequência para 60ms para parecer mais "real-time" sem pesar,
    // já que agora não destruímos a árvore toda vez.
    refreshTimer->start(60);

    Logger::Log("\033[36m[Qt] Hierarchy Panel initialized (Differential Update Mode).\033[0m");
}

HierarchyPanel::~HierarchyPanel() {
}

void HierarchyPanel::OnItemClicked(QTreeWidgetItem* item, int column) {
    if (!item) return;
    QString idString = item->text(0);
    int entityId = idString.toInt();
    emit EntitySelected(entityId);
}

void HierarchyPanel::UpdateHierarchy() {
    if (!game || !game->GetRegistry()) return;

    const auto& entities = game->GetRegistry()->GetEntities();

    // Conjunto para rastrear quais IDs foram processados neste frame.
    // O que sobrar no mapa 'activeTreeItems' mas não estiver aqui, deve ser deletado.
    std::set<int> processedIds;

    for (const auto& entity : entities) {
        int entityId = entity.GetId();

        std::string tag = game->GetRegistry()->GetEntityTag(entity);
        std::string group = game->GetRegistry()->GetEntityGroup(entity);

        // --- FILTRO DE RELEVÂNCIA ---
        // Se a entidade não tem Tag E não tem Grupo, assumimos que é um tile de cenário
        // ou objeto interno irrelevante para edição no momento.
        // Isso resolve o problema de "poluição visual" (ID 1200+).
        if (tag.empty() && group.empty()) {
            // Se existia antes (ex: perdeu a tag), precisamos remover da visualização
            if (activeTreeItems.find(entityId) != activeTreeItems.end()) {
                delete activeTreeItems[entityId];
                activeTreeItems.erase(entityId);
            }
            continue;
        }

        processedIds.insert(entityId);

        // --- ATUALIZAÇÃO DIFERENCIAL ---

        QTreeWidgetItem* item = nullptr;

        // Verifica se o item já existe na árvore
        if (activeTreeItems.find(entityId) != activeTreeItems.end()) {
            // Item existe: Atualizamos apenas os dados
            item = activeTreeItems[entityId];
        } else {
            // Item novo: Criamos e adicionamos
            item = new QTreeWidgetItem(treeWidget);
            item->setText(0, QString::number(entityId));
            activeTreeItems[entityId] = item;
        }

        // Atualiza colunas (apenas se mudou, para evitar repaint desnecessário,
        // mas setar texto direto é barato no Qt)
        if (item->text(1) != QString::fromStdString(tag)) {
            item->setText(1, QString::fromStdString(tag));
        }
        if (item->text(2) != QString::fromStdString(group)) {
            item->setText(2, QString::fromStdString(group));
        }
    }

    // --- REMOÇÃO DE ENTIDADES MORTAS ---
    // Verifica itens que estão na árvore visual, mas não existem mais no ECS (ou foram filtrados)
    auto it = activeTreeItems.begin();
    while (it != activeTreeItems.end()) {
        int id = it->first;
        if (processedIds.find(id) == processedIds.end()) {
            // Entidade morreu: Remove da UI
            delete it->second; // O Qt remove do widget automaticamente ao deletar o item
            it = activeTreeItems.erase(it);
        } else {
            ++it;
        }
    }
}