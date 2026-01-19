//
//  window.cpp
//  main
//
//  Created by Daniel Cho on 1/8/26.
//

// todo: delete 









//void VulkanWindow::initWindow() {
//        
//    graphics->setInstance(vulkanInstance()->vkInstance());
//    
//    graphics->setSurface(QVulkanInstance::surfaceForWindow(this));
//    
//    graphics->pickPhysicalDevice();
//    graphics->createLogicalDevice();
//    graphics->createCommandPool();
//    graphics->createCommandBuffers();
//    graphics->createSyncObjects();
//
//    graphics->createTextureSampler();
//    
//    graphics->loadQuad();
//    
//    graphics->createVertexBuffer();
//    graphics->createIndexBuffer();
//    graphics->createUniformBuffers();
//        
//    graphics->createSwapChain();
//    graphics->createSwapChainImageViews();
//    graphics->createSwapChainRenderPass();
//    graphics->createSwapChainDescriptorSetLayout();
//    graphics->createSwapChainGraphicsPipeline(resolveBundlePath("vert.spv"),
//                                              resolveBundlePath("frag.spv"));
//    graphics->createSwapChainColorResources();
//    graphics->createSwapChainDepthResources();
//    graphics->createSwapChainFramebuffers();
//    graphics->createSwapChainDescriptorPool();
//    
//}
//
//void VulkanWindow::delayedInitWindow() {
//    
//    // note: needs at least one image target
//    
//    graphics->createSwapChainDescriptorSets();
//    
//}
//
//void VulkanWindow::requestRender() {
//    renderRequested = true;
//}
//
//void VulkanWindow::render() {
//    
//    if (graphics->getSwapChainExtent().width == 0
//        || graphics->getSwapChainExtent().height == 0) {
//        return; // skip render
//    }
//    
//    update(); 
//    
//    if (!renderRequested) return;
//    
//    if (resized) {
//        releaseSwapChain();
//        graphics->recreateSwapChain();
//        recreateSwapChain();
//        resized = false;
//    }
//    
//    graphics->waitForFences();
//    if (graphics->aquireNextImage()) {
//        releaseSwapChain();
//        graphics->recreateSwapChain();
//        recreateSwapChain();
//    } else {
//        graphics->beginCommandBuffer();
//    }
//    
//    recordCommandBuffer();
//    
//    if (graphics->submitFrame()) {
//        releaseSwapChain();
//        graphics->recreateSwapChain();
//        recreateSwapChain();
//    }
//    
//    renderRequested = false;
//    
//}
//
//void VulkanWindow::cleanupWindow() {
//    
//    releaseSwapChain();
//    
//    graphics->cleanupVulkan();
//    
//}
//
//void VulkanWindow::exposeEvent(QExposeEvent *event) {
//    
//    if (isExposed()) { // first update request
//         requestUpdate(); // note: generally synced to refresh rate
//    }
//    
//}
//
//bool VulkanWindow::event(QEvent* event) {
//    
//    // todo: exposed and non-zero size
//    
//    if (event->type() == QEvent::UpdateRequest) {
//        render();
//        requestUpdate(); // note: continue render updates
//        return true;
//        
//    } else if (event->type() == QEvent::PlatformSurface) {
//        auto *surfaceEvent = static_cast<QPlatformSurfaceEvent *>(event);
//
//        switch (surfaceEvent->surfaceEventType()) { // on start
//        case QPlatformSurfaceEvent::SurfaceCreated:
//            initWindow();
//            init();
//            delayedInitWindow();
//            break;
//
//        case QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed: // on close
//            cleanup();
//            cleanupWindow();
//            break;
//
//        default:
//            break;
//        }
//    }
//
//    return QWindow::event(event);
//    
//}
//
//void VulkanWindow::resizeEvent(QResizeEvent *event) {
//    
//    resized = true;
//    
//    QWindow::resizeEvent(event);
//    
//}
