//
//  application.cpp
//  engine
//
//  Created by Daniel Cho on 9/30/25.
//

// creates the main window

#include "paint/application.h"

#include <stdexcept>
#include <iostream>
#include <chrono>

MainWindow::MainWindow() {
    createMenus();
    createToolbar();
    createDockPanels();
}
    
void MainWindow::createMenus() {
    
    QMenu* fileMenu = menuBar()->addMenu("&File");
//    fileMenu->addAction("&Open", this, &MainWindow::openFile);
//    fileMenu->addAction("&Save", this, &MainWindow::saveFile);
    
}

void MainWindow::createToolbar() {
    
    QToolBar* toolbar = addToolBar("Tools");
    toolbar->addAction("Brush");
    toolbar->addAction("Eraser");
    
}

void MainWindow::createDockPanels() {
    
    QDockWidget* colorDock = new QDockWidget("Colors", this);
    colorDock->setMinimumWidth(360);
    addDockWidget(Qt::RightDockWidgetArea, colorDock);
    
    QDockWidget* layersDock = new QDockWidget("Layers", this);
    layersDock->setMinimumWidth(360);
    addDockWidget(Qt::RightDockWidgetArea, layersDock);
    
}
