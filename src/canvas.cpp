//
//  canvas.cpp
//  main
//
//  Created by Daniel Cho on 12/25/25.
//

// creates the vulkan canvas

#include "paint/canvas.h"

#include <iostream>

void VulkanWindow::init() {
    
    graphics->setInstance(vulkanInstance()->vkInstance());
    
    graphics->setSurface(QVulkanInstance::surfaceForWindow(this));
    
    graphics->pickPhysicalDevice();
    graphics->createLogicalDevice();
    graphics->createCommandPool();
    graphics->createCommandBuffers();
    graphics->createSyncObjects();

    graphics->createTextureSampler();
    
    graphics->loadQuad();
    
    graphics->createVertexBuffer();
    graphics->createIndexBuffer();
    graphics->createUniformBuffers();
        
    graphics->createTexture(1024,
                            1024,
                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                            | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                            | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                            | VK_IMAGE_USAGE_SAMPLED_BIT,
                            1);

    graphics->createSwapChain();
    graphics->createSwapChainImageViews();
    graphics->createSwapChainRenderPass();
    graphics->createSwapChainDescriptorSetLayout();
    graphics->createSwapChainGraphicsPipeline(resolveBundlePath("vert.spv"),
                                              resolveBundlePath("frag.spv"));
    graphics->createSwapChainColorResources();
    graphics->createSwapChainDepthResources();
    graphics->createSwapChainFramebuffers();
    graphics->createSwapChainDescriptorPool();
    graphics->createSwapChainDescriptorSets();
    
    // todo: tiled canvas creation
    // todo: brush + layer pipelines
    
    // canvas draw job
    
    graphics->addDrawJob(0, 0, 1, { glm::mat4 { 1.0f } });
            
}

void VulkanWindow::releaseSwapChain() {
    
}

void VulkanWindow::recreateSwapChain() {
    
    // graphics recreate swapchain
    
    graphics->recreateSwapChain(); // note: waits for device idle
    
    // default camera + positions
    
    float depth = 3.f;
    float cameraX = 0.f;
    float cameraY = 0.f;
    
    glm::mat4 view = glm::lookAt(glm::vec3(cameraX, cameraY, depth), // camera pos
                                 glm::vec3(cameraX, cameraY, 0.0f), // look at
                                 glm::vec3(0.0f, 1.0f, 0.0f)); // up
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), // fovy
                                      graphics->getSwapChainExtent().width
                                      / (float) graphics->getSwapChainExtent().height, // aspect
                                      0.1f, // near
                                      10.0f); // far
    proj[1][1] *= -1; // strange projection fix

    for (int i = 0; i< graphics->MAX_FRAMES_IN_FLIGHT; i++) {
        graphics->updateGlobalUBO(i, view, proj);
        graphics->copyInstanceToBuffer(i);
    }

}

void VulkanWindow::recordCommandBuffer() {
    
}

void VulkanWindow::render() {
    
    // todo: fix resizing delay
    
    if (!dirty) {
        return; // wait for next frame
    }
        
    if (graphics->getSwapChainExtent().width == 0 || graphics->getSwapChainExtent().height == 0) {
        return; // wait for next frame
    }
    
    if (graphics->framebufferResized) {
        graphics->framebufferResized = false;
        recreateSwapChain();
    }

    graphics->startFrame();
    if (graphics->framebufferResized) return;
    recordCommandBuffer();
    graphics->submitFrame();
    if (graphics->framebufferResized) return;
    
    dirty = false;
    
}

void VulkanWindow::cleanup() {
    
    // todo: application specific cleanup
    
    graphics->cleanupVulkan();
    
}

void VulkanWindow::exposeEvent(QExposeEvent *event) {
    
    if (isExposed()) { // first update request
        requestUpdate(); // note: generally synced to refresh rate
    }
    
}

bool VulkanWindow::event(QEvent* event) {
    
    // todo: exposed and non-zero size
    
    if (event->type() == QEvent::UpdateRequest) {
        render();
        requestUpdate(); // continue render updates
        return true;
        
    } else if (event->type() == QEvent::PlatformSurface) {
        auto *surfaceEvent = static_cast<QPlatformSurfaceEvent *>(event);

        switch (surfaceEvent->surfaceEventType()) { // on start
        case QPlatformSurfaceEvent::SurfaceCreated:
            init();
            break;

        case QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed: // on close
            cleanup();
            break;

        default:
            break;
        }
    }

    return QWindow::event(event);
    
}

void VulkanWindow::resizeEvent(QResizeEvent *event) {

    dirty = true;
    graphics->framebufferResized = true;
    QWindow::resizeEvent(event);
        
}

void VulkanWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        qDebug() << "Left click at" << event->position();
    }
}
