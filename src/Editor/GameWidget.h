#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <memory>
#include "../Game/Game.h"

class GameWidget : public QWidget {
    Q_OBJECT

public:
    explicit GameWidget(QWidget* parent = nullptr);
    ~GameWidget();

    QPaintEngine* paintEngine() const override;

protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void GameLoop();

private:
    std::unique_ptr<Game> game;
    QTimer* gameLoopTimer;
};

#endif