#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Informações de conformidade LGPL (conforme solicitado na tarefa)
    QCoreApplication::setOrganizationName("EonEngine");
    QCoreApplication::setApplicationName("Eon Editor");

    MainWindow window;
    window.show();

    return app.exec();
}