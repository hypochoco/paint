//
//  main_window.h
//  engine
//
//  Created by Daniel Cho on 9/30/25.
//

#pragma once

#include <QMainWindow>

#include <QMenuBar>          // Menu bar
#include <QMenu>             // Individual menus
#include <QToolBar>          // Toolbars
#include <QDockWidget>       // Dockable panels (layers, colors, etc.)
#include <QAction>           // Menu/toolbar actions

//#include <QFileDialog>       // File open/save dialogs
//#include <QColorDialog>      // Color picker dialog
//#include <QMessageBox>       // Alert/confirmation dialogs

#include "paint/panels/layers_panel.h"
#include "paint/panels/brushes_panel.h"
#include "paint/panels/colors_panel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow();
    
    ColorsPanel* colorsPanel;
    QDockWidget* colorsDock;
    
    LayersPanel* layersPanel;
    QDockWidget* layersDock;
    
    BrushesPanel* brushesPanel;
    QDockWidget* brushesDock;

private:
    void createMenus();
    void createToolbar();
    void createDockPanels();
    
    void test();
    
};
