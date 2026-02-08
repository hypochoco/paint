//
//  canvas.h
//  main
//
//  Created by Daniel Cho on 12/24/25.
//

#pragma once

#include <QObject>
#include <QDebug>

#include <vector>

#include <engine/graphics/graphics.h>

#include "paint/canvas/canvas_data.h"

class Canvas : public QObject {
    Q_OBJECT
    
    // todo: hold functions for saving textures
        
public:
    Canvas(Graphics* graphics) : graphics(graphics) {
        data = new CanvasData;
    };
    ~Canvas() {
        delete data;
    };
    
    CanvasData* data;
    
    void create(int width, int height) {
        data->width = width;
        data->height = height;
        data->aspect = width / (float) height;
        initializeNew = true;
    }
    
    void load(std::string path) {
        // todo: setup loading
        throw std::runtime_error("not implemented");
    }
    
    void init() {
        if (initializeNew) {
            createLayer(0, "Background", 255); // base white layer
        }
    }
    
public slots:
    void onCleanup() {
        qDebug() << "[canvas] cleanup";
        cleanup();
    }
    void onSync(std::function<void(std::string)> reply) {
        qDebug() << "[canvas] on sync";
        for (Layer& layer : data->layers) {
            reply(layer.name);
        }
        emit dirty();
    }
    void onAddLayer(int index, std::function<void(std::string)> reply) {
        qDebug() << "[canvas] on add layer";
        reply(createLayer(index));
        emit dirty();
    }
    void onRemoveLayer(int index) {
        qDebug() << "[canvas] on remove layer";
        deleteLayer(index);
        emit dirty();
    }
    void onLayersMoved(int from, int to) {
        if (from == to) return;
        Layer layer = data->layers[from];
        data->layers.erase(data->layers.begin() + from);
        data->layers.insert(data->layers.begin() + to, layer);
        emit dirty();
    }

signals:
    void dirty();
            
private:
    Graphics* graphics;
    
    bool initializeNew = false;
    
    std::string createLayer(int index, std::string name = "", int alpha = 0) {
        
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        
        graphics->createTexture(textureImage,
                                textureImageMemory,
                                textureImageView,
                                data->width,
                                data->height,
                                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                                | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                                | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                                | VK_IMAGE_USAGE_SAMPLED_BIT,
                                1,
                                alpha);
        
        Layer layer;
        if (name == "") {
            layer.name = "Layer " + std::to_string(layer.id);
        } else {
            layer.name = name;
        }
        layer.image = textureImage;
        layer.imageMemory = textureImageMemory;
        layer.imageView = textureImageView;
        
        data->layers.insert(data->layers.begin() + index, layer);
        
        return layer.name;
        
    }
    
    void deleteLayer(int index) { // todo
        Layer& layer = data->layers[index];
        
        graphics->destroyVkImageView(layer.imageView);
        graphics->destroyVkImage(layer.image);
        graphics->destroyVkDeviceMemory(layer.imageMemory);
        
        data->layers.erase(data->layers.begin() + index);
    }
    
    void cleanup() {
        for (Layer& layer : data->layers) {
            graphics->destroyVkImageView(layer.imageView);
            graphics->destroyVkImage(layer.image);
            graphics->destroyVkDeviceMemory(layer.imageMemory);
        }
    }
    
};
