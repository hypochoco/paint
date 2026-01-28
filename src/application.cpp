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

#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

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
    QWidget* colorWidget = new QWidget(colorDock);
    QVBoxLayout* colorLayout = new QVBoxLayout(colorWidget);
    QPushButton* colorButton = new QPushButton(colorWidget);
    colorButton->setFixedSize(30, 30);
    colorButton->setStyleSheet(
        "QPushButton {"
        "background-color: #3498db;"
        "border-radius: 15px;"
        "}"
    );
    colorLayout->addWidget(colorButton, 0, Qt::AlignTop | Qt::AlignLeft);
    colorWidget->setLayout(colorLayout);
    colorDock->setWidget(colorWidget);
    addDockWidget(Qt::RightDockWidgetArea, colorDock);
    
    QDockWidget* layersDock = new QDockWidget("Layers", this);
    layersDock->setMinimumWidth(360);
    addDockWidget(Qt::RightDockWidgetArea, layersDock);
    
}
