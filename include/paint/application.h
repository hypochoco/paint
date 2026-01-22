//
//  application.h
//  engine
//
//  Created by Daniel Cho on 9/30/25.
//

#pragma once

#include <QMainWindow>

//#include <QResizeEvent>      // For resizeEvent()
//#include <QMouseEvent>       // For mouse input
//#include <QKeyEvent>         // For keyboard input

#include <QMenuBar>          // Menu bar
#include <QMenu>             // Individual menus
#include <QToolBar>          // Toolbars
#include <QDockWidget>       // Dockable panels (layers, colors, etc.)
#include <QAction>           // Menu/toolbar actions

//#include <QFileDialog>       // File open/save dialogs
//#include <QColorDialog>      // Color picker dialog
//#include <QMessageBox>       // Alert/confirmation dialogs

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow();

private:
    void createMenus();
    void createToolbar();
    void createDockPanels();
    
};
