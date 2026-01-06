#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <memory>
#include <QDragEnterEvent>
#include <QDropEvent>
#include "../Game/Game.h"

class GameWidget : public QWidget {
    Q_OBJECT

public:
    explicit GameWidget(QWidget* parent = nullptr);
    ~GameWidget();

    QPaintEngine* paintEngine() const override;

    // Accessor to allow other Editor panels to reach the Game Instance
    Game* GetGame() const;

protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    // Eventos de Drag & Drop
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void GameLoop();

private:
    std::unique_ptr<Game> game;
    QTimer* gameLoopTimer;

    // Método auxiliar para criar entidade a partir do arquivo
    void CreateEntityFromAsset(const QString& filePath, int x, int y);
};

#endif