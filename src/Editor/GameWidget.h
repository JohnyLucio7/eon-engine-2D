#pragma once

#include <QWidget>
#include <QTimer>
#include <memory>
#include "../Game/Game.h"

class GameWidget : public QWidget {
    Q_OBJECT

public:
    explicit GameWidget(QWidget* parent = nullptr);
    ~GameWidget();

    Game* GetGame() const;

protected:
    QPaintEngine* paintEngine() const override;
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void GameLoop();

private:
    void CreateEntityFromAsset(const QString& filePath, int mouseX, int mouseY);

    std::unique_ptr<Game> game;
    QTimer* gameLoopTimer;
};