
//
//  main.cpp
//  engine
//
//  Created by Daniel Cho on 9/30/25.
//

#include <iostream>

#include <QApplication>
#include <QVulkanInstance>

#include "paint/application.h" // main window
#include "paint/canvas.h" // vulkan canvas

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QVulkanInstance inst;
    
#ifndef NDEBUG // validation layers in debug
    inst.setLayers(QByteArrayList() << "VK_LAYER_KHRONOS_validation");
#endif
    
#if defined(__APPLE__) && defined(__MACH__) // macOS or iOS required extensions
    inst.setExtensions({
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
    });
    // note: portability flag set by default
#endif

    if (!inst.create()) {
        qFatal("failed to create Vulkan instance: %d", inst.errorCode());
        return 1;
    }
        
    // todo: graphics system here with the instance
    // GraphicsSystem* graphicsSystem = new GraphicsSystem;
    
    // todo: input system, external widgets (i.e. layers, colors)
    // todo: qt signals and slots
    
    MainWindow* mainWindow = new MainWindow();
    mainWindow->setWindowTitle("Paint Application");
    
    VulkanWindow* canvas = new VulkanWindow(&inst);
    QWidget* canvasContainer = QWidget::createWindowContainer(canvas);
    
    mainWindow->setCentralWidget(canvasContainer); // todo: not central widget
    
    mainWindow->resize(1280, 720);
    mainWindow->show();

    return app.exec();
}
