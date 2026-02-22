
//
//  main.cpp
//  engine
//
//  Created by Daniel Cho on 9/30/25.
//

#include <iostream>

#include <QObject>
#include <QThread>
#include <QFile>

#include <QApplication>
#include <QVulkanInstance>

#include <engine/graphics/graphics.h>

#include "paint/orchestrator.h"
#include "paint/main_window.h"
#include "paint/render_system/render_system.h"
#include "paint/tool_system/tool_system.h"
#include "paint/canvas/canvas_window.h"
#include "paint/canvas/canvas.h"
#include "paint/panels/brushes_panel.h"

#include "paint/utils.h"

int main(int argc, char *argv[]) {
        
    QApplication app(argc, argv);
    
    // load style sheet 
    
    QFile file(QString::fromStdString(resolveBundlePath("global.qss")));
    
    if (file.open(QFile::ReadOnly | QFile::Text)) { // todo: reenable / fix this
//        app.setStyleSheet(QTextStream(&file).readAll());
    } else {
        qDebug() << "could not find file";
    }

    // vulkan instance
    
    QVulkanInstance* inst = new QVulkanInstance;
    
#ifndef NDEBUG // validation layers in debug
    inst->setLayers(QByteArrayList() << "VK_LAYER_KHRONOS_validation");
#endif
    
#if defined(__APPLE__) && defined(__MACH__) // macOS or iOS required extensions
    inst->setExtensions({
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
    });
    // note: portability flag set by default
#endif

    if (!inst->create()) {
        qFatal("failed to create Vulkan instance: %d", inst->errorCode());
        return 1;
    }
    
    // graphics
    
    Graphics* graphics = new Graphics;
    graphics->setInstance(inst->vkInstance());
    
    // systems
    
    RenderSystem* renderSystem = new RenderSystem(graphics);
    ToolSystem* toolSystem = new ToolSystem;
    
    // canvas
    
    Canvas* canvas = new Canvas(graphics);
    canvas->create(1024, 2050); // init canvas size
    canvas->create(2050, 2050);
    
    // windows
    
    CanvasWindow* canvasWindow = new CanvasWindow(inst);
    QWidget* canvasWindowContainer = QWidget::createWindowContainer(canvasWindow);
    
    MainWindow* mainWindow = new MainWindow();
    mainWindow->setWindowTitle("Paint Application");
        
    // orchestrator

    Orchestrator* orchestrator = new Orchestrator(graphics);
    orchestrator->induct(canvas);
    orchestrator->induct(canvasWindow);
    orchestrator->induct(renderSystem);
    orchestrator->induct(toolSystem);
    orchestrator->induct(mainWindow);
    
    // setup connections
        
    // todo: move into orchestrator
        
    QObject::connect(canvasWindow, &CanvasWindow::resized,
                     renderSystem, &RenderSystem::onResized);
    
    QObject::connect(canvasWindow, &CanvasWindow::exposed,
                     renderSystem, &RenderSystem::onExposed);

    QObject::connect(renderSystem, &RenderSystem::requestUpdate,
                     canvasWindow, &CanvasWindow::onRequestUpdate);
    
    QObject::connect(canvasWindow, &CanvasWindow::leftButtonPressed,
                     toolSystem, &ToolSystem::leftButtonPressed);
 
    QObject::connect(canvasWindow, &CanvasWindow::leftButtonReleased,
                     toolSystem, &ToolSystem::leftButtonReleased);
    
    QObject::connect(canvasWindow, &CanvasWindow::mouseMoved,
                     toolSystem, &ToolSystem::mouseMoved);
    
    
    QObject::connect(toolSystem, &ToolSystem::actionsAvailable,
                     renderSystem, &RenderSystem::onActionsAvailable);
    
    QObject::connect(renderSystem, &RenderSystem::queryActions,
                     toolSystem, &ToolSystem::onQueryActions);
    
    // start application
    
    mainWindow->setCentralWidget(canvasWindowContainer);
    mainWindow->resize(1280, 720);
    mainWindow->show();
        
    orchestrator->start();
    
    return app.exec();
}
