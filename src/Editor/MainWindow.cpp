#include "MainWindow.h"
#include "../Logger/Logger.h" // Testando o link com a Engine Core

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    this->setWindowTitle("Eon Engine Editor");
    this->resize(1024, 768);
    
    // Teste de integração: Usando o Logger da engine dentro do Qt
    // Cor Ciano para logs do sistema
    Logger::Log("\033[36m[Qt] Editor MainWindow initialized successfully.\033[0m");
    Logger::Log("\033[36m[Qt] LGPLv3 Compliance: Qt libraries loaded dynamically.\033[0m");
}

MainWindow::~MainWindow() {
}