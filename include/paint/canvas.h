//
//  canvas.h
//  main
//
//  Created by Daniel Cho on 12/24/25.
//

#pragma once

#include <QWindow>
#include <QVulkanInstance>

//#include <QExposeEvent>
#include <QEvent>
#include <QResizeEvent>
#include <QMouseEvent>

class Canvas : public QWindow {
    Q_OBJECT
    
public:
    Canvas(QVulkanInstance* inst) {
        setVulkanInstance(inst);
        setSurfaceType(QSurface::VulkanSurface);
    }
    
//    void exposeEvent(QExposeEvent *event) override;
    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    
signals:
    void surfaceCreated(QWindow* window);
    void exposed(bool isExposed);
    void resized(uint32_t width, uint32_t height);
    void leftButtonPressed(int x, int y);
    void leftButtonReleased(int x, int y);
    void mouseMoved(int x, int y);
    void surfaceAboutToBeDestroyed();
    
private:
    bool leftMouseDown = false;
    
};







































//#include <QVulkanInstance>
//#include <QWindow>
//
//#include <QEvent>
//#include <QResizeEvent>
//#include <QExposeEvent>
//#include <QMouseEvent>
//
//#include <memory>
//
//#include <engine/graphics/graphics.h>
//
//#include "paint/utils.h"


//enum DirtyFlag {
//    DIRTY_SWAPCHAIN,
//    DIRTY_CAMERA
//};
//
//struct Camera {
//    
//    float x, y, z;
//    
//    glm::mat4 view, proj;
//    
//    void update(const float& aspect) {
//        
//        view = glm::lookAt(glm::vec3(x, y, z), // camera pos
//                           glm::vec3(x, y, 0.0f), // look at
//                           glm::vec3(0.0f, 1.0f, 0.0f)); // up
//        proj = glm::perspective(glm::radians(45.0f), // fovy
//                                aspect,
//                                0.1f, // near
//                                10.0f); // far
//        proj[1][1] *= -1; // strange projection fix
//        
//    }
//    
//};
//
//struct BrushPushConstant {
//    
//    float pos[2];
//    float size[2];
//    
//};
//
//class VulkanWindow : public QWindow {
//    Q_OBJECT
//    
//public:
//    
//    VulkanWindow(QVulkanInstance* inst) {
//        setVulkanInstance(inst);
//        setSurfaceType(QSurface::VulkanSurface);
//        
//        graphics = new Graphics();
//        camera = new Camera();
//    }
//    
//    ~VulkanWindow() {
//        delete graphics;
//    }
//    
//    // constants
//    
//    static constexpr uint32_t TILE_WIDTH = 512;
//    static constexpr uint32_t TILE_HEIGHT = 512;
//    
//    // canvas variables
//    
//    uint32_t canvasWidth = 1061; // todo: take as input, odd numbers?
//    uint32_t canvasHeight = 1399;
//    
//    float aspect;
//    uint32_t maxTileRow, maxTileCol;
//    uint32_t xscissorEdge, yscissorEdge;
//            
//    // window variables
//        
//    Graphics* graphics;
//    Camera* camera;
//    
//    // dirty flags
//    
//    bool dirtySwapChain;
//    bool dirtySwapChainRecreated;
//    bool dirtyCamera; // todo: convert into vector
//    std::vector<bool> dirtyTiles;
//    
//    // brush variables
//    
//    float brushSize = 0.5f; // current
//    uint32_t brushTextureIndex; // current
//    std::vector<uint32_t> brushTextureIndices;
//    
//    std::vector<float> brushBufferXPos;
//    std::vector<float> brushBufferYPos;
//    std::vector<int> brushBufferXTile;
//    std::vector<int> brushBufferYTile;
//    std::vector<float> brushBufferXBrushSize;
//    std::vector<float> brushBufferYBrushSize;
//    
//    VkRenderPass brushRenderPass;
//    VkFramebuffer brushFrameBuffer;
//    VkDescriptorSetLayout brushDescriptorSetLayout;
//    VkDescriptorPool brushDescriptorPool;
//    std::vector<VkDescriptorSet> brushDescriptorSets;
//    VkPipeline brushPipeline;
//    VkPipelineLayout brushPipelineLayout;
//    
//    // layer variables
//    
//    uint32_t layerTextureIndex; // current
//    std::vector<uint32_t> layerTextureIndices;
//
//    VkRenderPass layerRenderPass;
//    VkFramebuffer layerFrameBuffer;
//    VkDescriptorSetLayout layerDescriptorSetLayout;
//    VkDescriptorPool layerDescriptorPool;
//    std::vector<VkDescriptorSet> layerDescriptorSets;
//    VkPipeline layerPipeline;
//    VkPipelineLayout layerPipelineLayout;
//        
//    // window functions
//            
//    void init();
//    void releaseSwapChain();
//    void recreateSwapChain();
//    void render();
//    void cleanup();
//    
//    void exposeEvent(QExposeEvent* event) override;
//    bool event(QEvent* event) override;
//    void resizeEvent(QResizeEvent* event) override;
//    
//    void mousePressEvent(QMouseEvent* event) override;
//    
//    // render functions
//    
//    void setDirtyFlag(const DirtyFlag& dirtyFlag);
//    
//    std::unique_ptr<RenderNode> buildFrameGraph();
//    void processNode(std::unique_ptr<RenderNode>& renderNode);
//    void processFrameGraph(std::unique_ptr<RenderNode>& frameGraph);
//    
//    void recordCommandBuffer();
//    
//    // brush functions
//    
//    std::array<int, 2> tileCollision(float xworld, float yworld);
//    std::array<float, 2> screenToWorldSpace(double xpos, double ypos);
//    
//    void bufferBrush(double xpos, // screen pos
//                     double ypos, // screen pos
//                     float brushSize);
//    
//    void recordBrushCommandBuffer(BrushRenderNode* brushRenderNode);
//    
//    // layer functions
//    
//    void recordLayerCommandBuffer(LayerRenderNode* layerRenderNode);
//    
//private:
//    // todo
//    
//};
