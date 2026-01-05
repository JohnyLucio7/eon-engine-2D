#include "MainWindow.h"
#include "../Logger/Logger.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    this->setWindowTitle("Eon Engine Editor");
    this->resize(1280, 720);

    gameWidget = new GameWidget(this);
    setCentralWidget(gameWidget);

    Logger::Log("\033[36m[Qt] MainWindow initialized with GameWidget embedded.\033[0m");
}

MainWindow::~MainWindow() {
}