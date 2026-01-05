#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GameWidget.h"
#include "HierarchyPanel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    GameWidget* gameWidget;
    HierarchyPanel* hierarchyPanel;
};

#endif // MAINWINDOW_H