//
//  main_window.cpp
//  engine
//
//  Created by Daniel Cho on 9/30/25.
//

// creates the main window

#include "paint/main_window.h"

#include <stdexcept>
#include <iostream>
#include <chrono>

#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow() {
    createDockPanels();
    createMenus();
    createToolbar();
}
    
void MainWindow::createMenus() {
    
    menuBar()->setNativeMenuBar(true);
    
    QMenu* fileMenu = menuBar()->addMenu("&File");
    
    fileMenu->addAction("&Open", this, &MainWindow::test);
    fileMenu->addAction("&Save", this, &MainWindow::test);
    
    QMenu* windowMenu = menuBar()->addMenu("&Window");
        
    windowMenu->addAction(colorsDock->toggleViewAction());
    windowMenu->addAction(layersDock->toggleViewAction());
    windowMenu->addAction(brushesDock->toggleViewAction());
    
    // note: example action code
    
    //    QAction* brushesAction = new QAction("&Brushes", this);
    //    brushesAction->setCheckable(true);
    //    brushesAction->setChecked(false);
    //
    //    connect(brushesAction, &QAction::toggled,
    //            this, &MainWindow::test);    // or a dedicated slot
    //
    //    windowMenu->addAction(brushesAction);
    
}

void MainWindow::test() {
    qDebug() << "[main window] test";
}

void MainWindow::createToolbar() {
    
//    QToolBar* toolbar = addToolBar("Tools");
//    toolbar->addAction("Brush", this, &MainWindow::test);
//    toolbar->addAction("Eraser", this, &MainWindow::test);
    
}

void MainWindow::createDockPanels() {
    
    // colors panel
    
    QWidget* colorWidget = new QWidget(this);
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
    
    colorsDock = new QDockWidget("Colors", this);
    colorsDock->setWidget(colorWidget);
    colorsDock->setMinimumWidth(360);
    addDockWidget(Qt::RightDockWidgetArea, colorsDock);
    
    // layer panel
    
    layersPanel = new LayersPanel(this);
    layersDock = new QDockWidget("Layers", this);
    layersDock->setWidget(layersPanel);
    layersDock->setMinimumWidth(360);
    addDockWidget(Qt::RightDockWidgetArea, layersDock);
    
    // brushes panel
    
    brushesPanel = new BrushesPanel(this);
    brushesDock = new QDockWidget("Brushes", this);
    brushesDock->setWidget(brushesPanel);
    brushesDock->setFloating(true);
    brushesDock->hide(); // note: start hiddeen
        
}
