
//
//  main.cpp
//  engine
//
//  Created by Daniel Cho on 9/30/25.
//

#include <iostream>

#include <QObject>
#include <QThread>

#include <QApplication>
#include <QVulkanInstance>

#include "paint/application.h" // main window
#include "paint/render.h" // render system
#include "paint/tool.h" // tool system
#include "paint/canvas.h" // vulkan canvas

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
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
    
    // systems
    
    RenderSystem* renderSystem = new RenderSystem(inst);
    ToolSystem* toolSystem = new ToolSystem;
    
    // main window
    
    MainWindow* mainWindow = new MainWindow();
    mainWindow->setWindowTitle("Paint Application");
    
    // canvas
    
    Canvas* canvas = new Canvas(inst);
    QWidget* canvasContainer = QWidget::createWindowContainer(canvas);

    // setup / start application
    
    QObject::connect(canvas, &Canvas::surfaceCreated,
                     renderSystem, &RenderSystem::onSurfaceCreated);
    
    QObject::connect(canvas, &Canvas::exposed, // todo
                     renderSystem, &RenderSystem::onExposed);
    
    QObject::connect(canvas, &Canvas::resized,
                     renderSystem, &RenderSystem::onResized);
    
    QObject::connect(canvas, &Canvas::surfaceAboutToBeDestroyed,
                     renderSystem, &RenderSystem::onSurfaceAbobutToBeDestroyed);
        
    QObject::connect(renderSystem, &RenderSystem::requestUpdate,
                     canvas, &Canvas::onRequestUpdate);

    QObject::connect(canvas, &Canvas::leftButtonPressed,
                     toolSystem, &ToolSystem::leftButtonPressed);
 
    QObject::connect(canvas, &Canvas::leftButtonReleased,
                     toolSystem, &ToolSystem::leftButtonReleased);
    
    QObject::connect(canvas, &Canvas::mouseMoved,
                     toolSystem, &ToolSystem::mouseMoved);
    
    QObject::connect(renderSystem, &RenderSystem::queryToolSystem,
                     toolSystem, &ToolSystem::onQuery);
    
    QObject::connect(toolSystem, &ToolSystem::submitActions,
                     renderSystem, &RenderSystem::onActions);
    
    
    mainWindow->setCentralWidget(canvasContainer);
    mainWindow->resize(1280, 720);
    mainWindow->show();
        
    renderSystem->start();
    
    return app.exec();
}
