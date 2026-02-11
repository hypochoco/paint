//
//  orchestrator.h
//  main
//
//  Created by Daniel Cho on 2/2/26.
//

#pragma once

#include <QObject>

#include <engine/graphics/graphics.h>

#include "paint/canvas/canvas.h"
#include "paint/canvas/canvas_window.h"
#include "paint/render_system/render_system.h"
#include "paint/tool_system/tool_system.h"
#include "paint/main_window.h"

class Orchestrator : public QObject {
    Q_OBJECT
    
    // organize connections
        
public:
    Orchestrator(Graphics* graphics) : graphics(graphics) {}
    
    void induct(Canvas* canvas) {
        this->canvas = canvas;
        connect(this, &Orchestrator::cleanup,
                canvas, &Canvas::onCleanup);
        connect(this, &Orchestrator::sync,
                canvas, &Canvas::onSync);
        connect(this, &Orchestrator::addLayer,
                canvas, &Canvas::onAddLayer);
        connect(this, &Orchestrator::removeLayer,
                canvas, &Canvas::onRemoveLayer);
        connect(this, &Orchestrator::rowsMoved,
                canvas, &Canvas::onLayersMoved);
        connect(canvas, &Canvas::dirty,
                this, &Orchestrator::onLayersDirty);
    }
    void induct(CanvasWindow* canvasWindow) {
        connect(canvasWindow, &CanvasWindow::surfaceCreated,
                this, &Orchestrator::onSurfaceCreated);
        connect(canvasWindow, &CanvasWindow::surfaceAboutToBeDestroyed,
                this, &Orchestrator::onSurfaceAboutToBeDestroyed);
    }
    void induct(RenderSystem* renderSystem) {
        this->renderSystem = renderSystem;
        connect(this, &Orchestrator::surfaceCreated,
                renderSystem, &RenderSystem::onSurfaceCreated);
        connect(this, &Orchestrator::start,
                renderSystem, &RenderSystem::onStart);
        connect(renderSystem, &RenderSystem::ready,
                this, &Orchestrator::onReady);
        connect(this, &Orchestrator::layersDirty,
                renderSystem, &RenderSystem::onLayersDirty);
        connect(this, &Orchestrator::cleanup,
                renderSystem, &RenderSystem::onCleanup);
    }
    void induct(ToolSystem* toolSystem) {
        connect(toolSystem->brushTool, &BrushTool::querySelectedLayer,
                this, &Orchestrator::onQuerySelectedLayer);
    }
    void induct(MainWindow* mainWindow) {
        this->mainWindow = mainWindow;
        connect(mainWindow->layersPanel, &LayersPanel::signalSync,
                this, &Orchestrator::onSync);
        connect(mainWindow->layersPanel, &LayersPanel::signalAddLayer,
                this, &Orchestrator::onAddLayer);
        connect(mainWindow->layersPanel, &LayersPanel::signalRemoveLayer,
                this, &Orchestrator::onRemoveLayer);
        connect(mainWindow->layersPanel, &LayersPanel::signalRowsMoved,
                this, &Orchestrator::onRowsMoved);
        connect(this, &Orchestrator::selectedLayer,
                mainWindow->layersPanel, &LayersPanel::onSelectedLayer);
        connect(mainWindow->layersPanel, &LayersPanel::dirty,
                this, &Orchestrator::onLayersDirty);
    }
        
public slots:
    void onSurfaceCreated(QWindow* window) {
        emit surfaceCreated(window);
    }
    void onReady() { // todo: better way of handling this creation
        if (canvas && renderSystem && mainWindow) {
            canvas->init();
            renderSystem->setCanvasData(canvas->data);
            mainWindow->layersPanel->sync();
        }
    }
    void onSync(std::function<void(std::string)> reply) {
        emit sync(reply);
    }
    void onAddLayer(int index, std::function<void(std::string)> reply) {
        emit addLayer(index, reply);
    }
    void onRemoveLayer(int index) {
        emit removeLayer(index);
    }
    void onRowsMoved(int from, int to) {
        emit rowsMoved(from, to);
    }
    void onLayersDirty() {
        emit layersDirty();
    }
    void onQuerySelectedLayer(std::function<void(int)> reply) {
        emit selectedLayer(reply);
    }
    void onSurfaceAboutToBeDestroyed() {
        // todo: handle multiple canvas windows
        graphics->deviceWaitIdle();
        emit cleanup();
        graphics->cleanupVulkan();
    }
    
signals:
    void surfaceCreated(QWindow* window);
    void start();
    void cleanup();
    
    void sync(std::function<void(std::string)> reply);
    void addLayer(int index, std::function<void(std::string)> reply);
    void removeLayer(int index);
    void rowsMoved(int from, int to);
    
    void layersDirty();
    void selectedLayer(std::function<void(int)> reply);
    
private:
    Graphics* graphics;
    
    Canvas* canvas; // temp
    RenderSystem* renderSystem; // temp
    MainWindow* mainWindow;
    
};
