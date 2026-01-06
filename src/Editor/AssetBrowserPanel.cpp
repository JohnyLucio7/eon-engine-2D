#include "AssetBrowserPanel.h"
#include <QVBoxLayout>
#include <QDir>
#include "../Logger/Logger.h"

AssetBrowserPanel::AssetBrowserPanel(Game* game, QWidget* parent)
    : QDockWidget("Asset Browser", parent), game(game) {

    QWidget* container = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);

    // 1. Configurar o Modelo de Arquivos (Sistema de Arquivos)
    fileModel = new QFileSystemModel(this);
    // Define o diretório raiz como o diretório atual + /assets
    QString rootPath = QDir::currentPath() + "/assets";
    
    // Filtros para mostrar apenas arquivos relevantes (imagens, scripts, sons)
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.lua" << "*.ttf" << "*.wav" << "*.mp3" << "*.map";
    fileModel->setNameFilters(filters);
    fileModel->setNameFilterDisables(false);
    
    fileModel->setRootPath(rootPath);

    // 2. Configurar a TreeView
    treeView = new QTreeView();
    treeView->setModel(fileModel);
    treeView->setRootIndex(fileModel->index(rootPath));
    
    // Habilitar Drag & Drop
    treeView->setDragEnabled(true);
    treeView->setAcceptDrops(false); // Browser é apenas origem, não destino
    treeView->setDropIndicatorShown(true);
    treeView->setDragDropMode(QAbstractItemView::DragOnly);

    // Ocultar colunas irrelevantes (Tamanho, Tipo, Data) para economizar espaço
    treeView->hideColumn(1);
    treeView->hideColumn(2);
    treeView->hideColumn(3);

    // Estilização
    treeView->setAlternatingRowColors(true);
    treeView->setHeaderHidden(false);

    layout->addWidget(treeView);
    container->setLayout(layout);
    setWidget(container);

    Logger::Log("\033[36m[Qt] Asset Browser Panel initialized. Root: " + rootPath.toStdString() + "\033[0m");
}

AssetBrowserPanel::~AssetBrowserPanel() {
}