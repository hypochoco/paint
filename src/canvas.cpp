//
//  canvas.cpp
//  main
//
//  Created by Daniel Cho on 12/25/25.
//

#include "paint/canvas.h"

//void Canvas::exposeEvent(QExposeEvent *event) {
//    qDebug() << "[canvas] exposed event";
//    QWindow::exposeEvent(event);
//    emit exposed(isExposed());
//}

bool Canvas::event(QEvent* event) {
    
    if (event->type() == QEvent::UpdateRequest) {
        return true;
    }

    if (event->type() == QEvent::PlatformSurface) {
        auto *surfaceEvent = static_cast<QPlatformSurfaceEvent *>(event);

        switch (surfaceEvent->surfaceEventType()) { // on start
            case QPlatformSurfaceEvent::SurfaceCreated:
                qDebug() << "[canvas] surface created event";
                emit surfaceCreated(this);
                return true;
                break;
                
            case QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed: // on close
                qDebug() << "[canvas] surface about to be destroyed event";
                emit surfaceAboutToBeDestroyed();
                return true;
                break;
                
            default:
                break;
        }
    }

    return QWindow::event(event);

}

void Canvas::resizeEvent(QResizeEvent *event) {
    qDebug() << "[canvas] resize event";
    QWindow::resizeEvent(event);
    emit resized(event->size().width(), event->size().height());
}

void Canvas::mousePressEvent(QMouseEvent* event) {
    QWindow::mousePressEvent(event);
    if (event->button() ==  Qt::LeftButton) {
        qDebug() << "[canvas] left mouse press event";
        leftMouseDown = true;
    }
    emit leftButtonPressed(event->pos().x(), event->pos().y());
}

void Canvas::mouseReleaseEvent(QMouseEvent* event) {
    QWindow::mouseReleaseEvent(event);
    if (event->button() ==  Qt::LeftButton) {
        qDebug() << "[canvas] left mouse release event";
        leftMouseDown = false;
    }
    emit leftButtonReleased(event->pos().x(), event->pos().y());
}

void Canvas::mouseMoveEvent(QMouseEvent *event) {
    QWindow::mouseMoveEvent(event);
    if (leftMouseDown) {
        emit mouseMoved(event->pos().x(), event->pos().y());
    }
}

void Canvas::onRequestUpdate() {
    qDebug() << "[canvas] update requested";
    requestUpdate();
        
//#ifdef Q_OS_MACOS
//    QCoreApplication::processEvents();
//    QTest.qWait(1)
//#endif
}































//void VulkanWindow::init() {
//    
//    // general graphics
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
//    // canvas -> swapchain
//    
//    aspect = canvasWidth / (float) canvasHeight;
//    maxTileRow = std::ceil(canvasHeight * 0.5f / TILE_HEIGHT); // note: half the canvas
//    maxTileCol = std::ceil(canvasWidth * 0.5f/ TILE_WIDTH);
//    
//    // note: ceil is for handling odd case
//    
//    xscissorEdge = std::ceil(0.5f * (canvasWidth - TILE_WIDTH * std::floor(canvasWidth / TILE_WIDTH)));
//    xscissorEdge = xscissorEdge == 0 ? TILE_WIDTH : xscissorEdge;
//    yscissorEdge = std::ceil(0.5f * (canvasHeight - TILE_HEIGHT * std::floor(canvasHeight / TILE_HEIGHT)));
//    yscissorEdge = yscissorEdge == .0f ? TILE_HEIGHT : yscissorEdge;
//            
//    graphics->createTexture(canvasWidth,
//                            canvasHeight,
//                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
//                            | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
//                            | VK_IMAGE_USAGE_TRANSFER_DST_BIT
//                            | VK_IMAGE_USAGE_SAMPLED_BIT,
//                            1);
//        
//    graphics->addDrawJob(0, 0, 1, std::vector<glm::mat4> { glm::scale(glm::mat4 { 1.0f },
//                                                                      glm::vec3 { aspect, 1.0f, 1.0f }) });
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
//    graphics->createSwapChainDescriptorSets();
//    
//    // brush -> layer
//    
//    graphics->createRenderPass(brushRenderPass,
//                               VK_ATTACHMENT_LOAD_OP_LOAD);
//    
//    graphics->createDescriptorSetLayout(brushDescriptorSetLayout);
//    graphics->createDescriptorPool(brushDescriptorPool);
//        
//    auto brushVertShaderCode = Graphics::readFile(resolveBundlePath("brush_vert.spv"));
//    auto brushFragShaderCode = Graphics::readFile(resolveBundlePath("brush_frag.spv"));
//    
//    VkShaderModule brushVertShaderModule = graphics->createShaderModule(brushVertShaderCode);
//    VkShaderModule brushFragShaderModule = graphics->createShaderModule(brushFragShaderCode);
//    
//    VkPipelineShaderStageCreateInfo brushVertShaderStageInfo{};
//    brushVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    brushVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//    brushVertShaderStageInfo.module = brushVertShaderModule;
//    brushVertShaderStageInfo.pName = "main";
//    
//    VkPipelineShaderStageCreateInfo brushFragShaderStageInfo{};
//    brushFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    brushFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//    brushFragShaderStageInfo.module = brushFragShaderModule;
//    brushFragShaderStageInfo.pName = "main";
//    
//    VkPipelineShaderStageCreateInfo brushShaderStages[] = { brushVertShaderStageInfo, brushFragShaderStageInfo };
//    
//    VkPushConstantRange pushConstantRange{};
//    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//    pushConstantRange.offset = 0;
//    pushConstantRange.size = sizeof(BrushPushConstant); // pos(vec2) + size(vec2) => 4 floats
//    
//    graphics->createPipeline(brushPipeline,
//                             brushDescriptorSetLayout,
//                             brushPipelineLayout,
//                             brushRenderPass,
//                             brushShaderStages,
//                             pushConstantRange);
//    
//    graphics->destroyShaderModule(brushVertShaderModule);
//    graphics->destroyShaderModule(brushFragShaderModule);
//    
//    // layer -> canvas
//        
//    graphics->createRenderPass(layerRenderPass,
//                               VK_ATTACHMENT_LOAD_OP_LOAD);
//    
//    graphics->createFramebuffer(layerFrameBuffer,
//                                layerRenderPass,
//                                graphics->textureImageViews[0], // canvas image
//                                (int) canvasWidth,
//                                (int) canvasHeight);
//    
//    graphics->createDescriptorSetLayout(layerDescriptorSetLayout);
//    graphics->createDescriptorPool(layerDescriptorPool);
//    
//    auto layerVertShaderCode = Graphics::readFile(resolveBundlePath("layer_vert.spv"));
//    auto layerFragShaderCode = Graphics::readFile(resolveBundlePath("layer_frag.spv"));
//    
//    VkShaderModule layerVertShaderModule = graphics->createShaderModule(layerVertShaderCode);
//    VkShaderModule layerFragShaderModule = graphics->createShaderModule(layerFragShaderCode);
//    
//    VkPipelineShaderStageCreateInfo layerVertShaderStageInfo{};
//    layerVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    layerVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
//    layerVertShaderStageInfo.module = layerVertShaderModule;
//    layerVertShaderStageInfo.pName = "main";
//    
//    VkPipelineShaderStageCreateInfo layerFragShaderStageInfo{};
//    layerFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//    layerFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//    layerFragShaderStageInfo.module = layerFragShaderModule;
//    layerFragShaderStageInfo.pName = "main";
//    
//    VkPipelineShaderStageCreateInfo layerShaderStages[] = { layerVertShaderStageInfo, layerFragShaderStageInfo };
//    
//    graphics->createPipeline(layerPipeline,
//                             layerDescriptorSetLayout,
//                             layerPipelineLayout,
//                             layerRenderPass,
//                             layerShaderStages);
//    
//    graphics->destroyShaderModule(layerVertShaderModule);
//    graphics->destroyShaderModule(layerFragShaderModule);
//    
//    // default camera position
//    
//    camera->z = 3.0f; // note: update called in recreate swapchain
//    
//    // default background layer
//    
//    graphics->createTexture(canvasWidth,
//                            canvasHeight,
//                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
//                            | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
//                            | VK_IMAGE_USAGE_TRANSFER_DST_BIT
//                            | VK_IMAGE_USAGE_SAMPLED_BIT,
//                            1);
//    
//    // todo: function to update descriptor set
//    // todo: what about from layers to canvas ?
//
//    graphics->createDescriptorSets(graphics->textureImageViews[1], // default layer
//                                   layerDescriptorSets,
//                                   layerDescriptorSetLayout,
//                                   layerDescriptorPool);
//    
//    // temp: default texture brush
//        
//    graphics->loadTexture(resolveBundlePath("brush.png"),
//                          VK_IMAGE_USAGE_TRANSFER_SRC_BIT
//                          | VK_IMAGE_USAGE_TRANSFER_DST_BIT
//                          | VK_IMAGE_USAGE_SAMPLED_BIT,
//                          1);
//    
//    graphics->createDescriptorSets(graphics->textureImageViews[2], // default brush index
//                                   brushDescriptorSets,
//                                   brushDescriptorSetLayout,
//                                   brushDescriptorPool);
//    
//    graphics->createFramebuffer(brushFrameBuffer,
//                                brushRenderPass,
//                                graphics->textureImageViews[1],
//                                (int) canvasWidth,
//                                (int) canvasHeight);
//
//}
//
//void VulkanWindow::releaseSwapChain() {
//    
//}
//
//void VulkanWindow::recreateSwapChain() {
//    
//    // graphics recreate swapchain
//    
//    graphics->recreateSwapChain(); // note: waits for device idle
//    
//    // todo: move camera updates to render graph
//    
//    // update global ubo with camera + instance ssbo's
//    
//    camera->update(
//       graphics->getSwapChainExtent().width / (float) graphics->getSwapChainExtent().height // aspect
//    );
//    
//    for (int i = 0; i< graphics->MAX_FRAMES_IN_FLIGHT; i++) {
//        graphics->updateGlobalUBO(i, camera->view, camera->proj);
//        graphics->copyInstanceToBuffer(i);
//    }
//
//}
//
//void VulkanWindow::setDirtyFlag(const DirtyFlag& dirtyFlag) {
//    switch (dirtyFlag) {
//        case DIRTY_SWAPCHAIN:
//            dirtySwapChain = true;
//            break;
//        case DIRTY_CAMERA:
//            dirtyCamera = true;
//            break;
//        default:
//            qWarning("Given dirty flag not implemented");
//            break;
//    }
//}
//
//std::array<int, 2> VulkanWindow::tileCollision(float xworld, float yworld) {
//    
//    // convert to pixel space, then tile space
//    
//    static float xscale = canvasHeight * 0.5f / TILE_WIDTH;
//    static float yscale = canvasHeight * 0.5f / TILE_HEIGHT;
//    
//    int col = static_cast<int>(std::floor(xworld * xscale));
//    int row = static_cast<int>(std::floor(yworld * yscale));
//    return { col, row };
//    
//}
//
//std::array<float, 2> VulkanWindow::screenToWorldSpace(double xpos, double ypos) {
//                
//    float ndcX = (xpos / width()) * 2.0f - 1.0f;
//    float ndcY = 1.0f - (ypos / height()) * 2.0f;
//
//    float aspect = (float) width() / height(); // window aspect
//    float tanHalfFovy = 0.4142f; // hard coded for 45 deg
//    
//    return {
//        ndcX * camera->z * tanHalfFovy * aspect + camera->x,
//        ndcY * camera->z * tanHalfFovy + camera->y
//    };
//
//}
//
//void VulkanWindow::bufferBrush(double xpos, // screen pos
//                               double ypos, // screen pos
//                               float brushSize) { // todo: brush orientation
//    
//    // todo: pixel level out of bounds here 
//        
//    auto [ xworld, yworld ] = screenToWorldSpace(xpos, ypos);
//    auto [ ci, cj ] = tileCollision(xworld, yworld);
//        
//    if (ci < - (int) maxTileCol
//        || cj < - (int) maxTileRow
//        || ci >= (int) maxTileCol
//        || cj >= (int) maxTileRow) return; // canvas bounds
//
//    auto [ bli, blj ] = tileCollision(xworld - brushSize, yworld - brushSize); // bottom left
//    auto [ tri, trj ] = tileCollision(xworld + brushSize, yworld + brushSize); // top right
//    
//    for (int i = bli; i <= tri; i++) {
//        for (int j = blj; j <= trj; j++) {
//            brushBufferXPos.push_back(xworld / aspect); // model space
//            brushBufferYPos.push_back(yworld);
//            brushBufferXBrushSize.push_back(brushSize / aspect); // model space
//            brushBufferYBrushSize.push_back(brushSize);
//            brushBufferXTile.push_back(i); // pixel space
//            brushBufferYTile.push_back(j);
//        }
//    }
//    
//}
//
//void VulkanWindow::recordBrushCommandBuffer(BrushRenderNode* brushRenderNode) {
//    
//    // todo: takes in a brush job
//    
//    VkCommandBuffer& commandBuffer = graphics->commandBuffers[graphics->currentFrame];
//        
//    graphics->recordBeginRenderPass(commandBuffer,
//                                    brushRenderPass,
//                                    brushFrameBuffer,
//                                    canvasWidth,
//                                    canvasHeight,
//                                    brushPipeline);
//    
//    graphics->recordBindDescriptorSet(commandBuffer,
//                                      brushPipelineLayout,
//                                      brushDescriptorSets);
//        
//    for (int i = 0; i < brushRenderNode->brushBufferXPos.size(); i++) {
//        
//        float xpos = brushRenderNode->brushBufferXPos[i];
//        float ypos = brushRenderNode->brushBufferYPos[i];
//        int xtile = brushRenderNode->brushBufferXTile[i];
//        int ytile = brushRenderNode->brushBufferYTile[i];
//        float xbs = brushRenderNode->brushBufferXBrushSize[i];
//        float ybs = brushRenderNode->brushBufferYBrushSize[i];
//                                        
//        graphics->recordSetScissor(commandBuffer,
//                                   std::clamp(xtile * (float) TILE_WIDTH + canvasWidth * 0.5f,
//                                              .0f, (float) canvasWidth),
//                                   std::clamp(ytile * (float) TILE_HEIGHT + canvasHeight * 0.5f,
//                                              .0f, (float) canvasHeight),
//                                   (xtile == - maxTileCol || xtile == maxTileCol - 1 ? xscissorEdge : TILE_WIDTH),
//                                   (ytile == - maxTileRow || ytile == maxTileRow - 1 ? yscissorEdge : TILE_HEIGHT));
//
//        BrushPushConstant pc { { xpos, ypos }, { xbs, ybs } };
//                
//        graphics->recordPushConstant(commandBuffer,
//                                     brushPipelineLayout,
//                                     sizeof(pc),
//                                     &pc);
//        
//        graphics->recordDraw(commandBuffer);
//
//    }
//    
//    graphics->recordEndRenderPass(commandBuffer);
//    
//}
//
//void VulkanWindow::recordLayerCommandBuffer(LayerRenderNode* layerRenderNode) {
//        
//    // todo: visible layers to canvas
//    
//    VkCommandBuffer& commandBuffer = graphics->commandBuffers[graphics->currentFrame];
//        
//    graphics->recordBeginRenderPass(commandBuffer,
//                                    layerRenderPass,
//                                    layerFrameBuffer,
//                                    canvasWidth,
//                                    canvasHeight,
//                                    layerPipeline);
//    
//    graphics->recordBindDescriptorSet(commandBuffer,
//                                      layerPipelineLayout,
//                                      layerDescriptorSets);
//        
//    for (int i = 0; i < layerRenderNode->xTile.size(); i++) {
//        
//        int xtile = layerRenderNode->xTile[i]; // dirty tiles
//        int ytile = layerRenderNode->yTile[i];
//        
//        graphics->recordSetScissor(commandBuffer,
//                                   std::clamp(xtile * (float) TILE_WIDTH + canvasWidth * 0.5f,
//                                              .0f, (float) canvasWidth),
//                                   std::clamp(ytile * (float) TILE_HEIGHT + canvasHeight * 0.5f,
//                                              .0f, (float) canvasHeight),
//                                   (xtile == - maxTileCol || xtile == maxTileCol - 1 ? xscissorEdge : TILE_WIDTH),
//                                   (ytile == - maxTileRow || ytile == maxTileRow - 1 ? yscissorEdge : TILE_HEIGHT));
//        
//        graphics->recordClearAttachment(commandBuffer,
//                                        std::clamp(xtile * (float) TILE_WIDTH + canvasWidth * 0.5f,
//                                                   .0f, (float) canvasWidth),
//                                        std::clamp(ytile * (float) TILE_HEIGHT + canvasHeight * 0.5f,
//                                                   .0f, (float) canvasHeight),
//                                        (xtile == - maxTileCol || xtile == maxTileCol - 1 ? xscissorEdge : TILE_WIDTH),
//                                        (ytile == - maxTileRow || ytile == maxTileRow - 1 ? yscissorEdge : TILE_HEIGHT));
//
//        // todo: for each dirty tile above (render all the layers even though only one dirty)
//        
//        graphics->recordDraw(commandBuffer);
//        
//    }
//    
//    graphics->recordEndRenderPass(commandBuffer);
//    
//}
//
//void VulkanWindow::recordCommandBuffer() {
//    
//    if (!graphics->submitFrame()) {
//        dirtySwapChain = true;
//    }
//    dirtySwapChainRecreated = false; // reset flag after swapchain submit
//    graphics->advanceFrame();
//        
//}
//
//void VulkanWindow::processNode(std::unique_ptr<RenderNode>& renderNode) {
//    
//    if (renderNode == nullptr) return;
//    
//    for (std::unique_ptr<RenderNode>& child : renderNode->children) {
//        processNode(child);
//    }
//    
//    renderNode->execute(this);
//    
//}
//
//void VulkanWindow::processFrameGraph(std::unique_ptr<RenderNode>& frameGraph) {
//    
//    // global binds
//    
//    VkCommandBuffer& commandBuffer = graphics->commandBuffers[graphics->currentFrame];
//    
//    graphics->recordSetViewport(commandBuffer,
//                                0.0f,
//                                0.0f,
//                                canvasWidth,
//                                canvasHeight);
//
//    // traverse frame graph
//    
//    processNode(frameGraph);
//    
//}
//
//std::unique_ptr<RenderNode> VulkanWindow::buildFrameGraph() {
//    
//    // note: after swapchain recreate
//        // try not to skip frame, otherwise some lag
//    // note: keep this simple enough for now
//    // note: assumes success
//    // note: get a clean system everywhere else, handle complex logic here
//    
//    // todo: dirty camera handling
//    
//    if (brushBufferXPos.empty() // no render pass needed
//        || dirtySwapChainRecreated) return nullptr;
//    
//    std::unique_ptr<RenderNode> root = std::make_unique<RenderNode>(); // indicates swapchain render
//    
//    if (!brushBufferXPos.empty()) {
//        
//        // brush
//        
//        std::unique_ptr<BrushRenderNode> brushRenderNode = std::make_unique<BrushRenderNode>();
//        
//        brushRenderNode->brushBufferXPos = brushBufferXPos;
//        brushRenderNode->brushBufferYPos = brushBufferYPos;
//        brushRenderNode->brushBufferXTile = brushBufferXTile;
//        brushRenderNode->brushBufferXBrushSize = brushBufferXBrushSize;
//        brushRenderNode->brushBufferYBrushSize = brushBufferYBrushSize;
//        
//        brushBufferXPos.clear();
//        brushBufferYPos.clear();
//        brushBufferXTile.clear();
//        brushBufferYTile.clear();
//        brushBufferXBrushSize.clear();
//        brushBufferYBrushSize.clear();
//
//        // dirty tiles
//        
//        std::unique_ptr<LayerRenderNode> layerRenderNode = std::make_unique<LayerRenderNode>();
//        
//        std::set<std::pair<int, int>> uniquePairs;
//
//        for (size_t i = 0; i < brushRenderNode->brushBufferXTile.size(); ++i) {
//            uniquePairs.emplace(brushRenderNode->brushBufferXTile[i], brushRenderNode->brushBufferYPos[i]);
//        }
//        
//        for (const auto& [x, y] : uniquePairs) {
//            layerRenderNode->xTile.push_back(x);
//            layerRenderNode->yTile.push_back(y);
//        }
//        
//        // build root
//    
//        layerRenderNode->addChild(std::unique_ptr<RenderNode>(std::move(brushRenderNode)));
//        root->addChild(std::unique_ptr<RenderNode>(std::move(layerRenderNode)));
//        
//    }
//            
//    return root;
//    
//}
//
//void VulkanWindow::render() {
//            
//    if (graphics->getSwapChainExtent().width == 0 || graphics->getSwapChainExtent().height == 0) {
//        return; // skip frame
//    }
//    
//    
//    
//    // just cave and recreate the swapchain in multiple places if needed ...
//    
//    
//    
//    
//    
//    
//    
//    
//    // todo: make this completely bullet proof ... just having good structure here is really important ...
//    // figure out when to do any of this ...
//    
//    
//    
//    
//    
//        
//    if (dirtySwapChain) { // note: redraw if recreated
//        recreateSwapChain();
//        dirtySwapChain = false;
//        dirtySwapChainRecreated = true;
//    }
//        
//    if(!graphics->preFrame()) {
//        dirtySwapChain = true;
//        // graphics->advanceFrame();
//        return;
//    }
//        
////    std::unique_ptr<RenderNode> frameGraph = buildFrameGraph();
////    if (frameGraph == nullptr) return; // no updates
//    
//    graphics->startFrame();
//    
////    processFrameGraph(frameGraph); // note: calls submit frame + advance frame
//
//    recordCommandBuffer();
//            
//}
//
//void VulkanWindow::cleanup() {
//    
//    graphics->deviceWaitIdle();
//    
//    graphics->destroyPipeline(brushPipeline);
//    graphics->destroyPipelineLayout(brushPipelineLayout);
//    graphics->destroyRenderPass(brushRenderPass);
//    graphics->destroyFrameBuffer(brushFrameBuffer);
//    graphics->destroyDescriptorPool(brushDescriptorPool);
//    graphics->destroyDescriptorSetLayout(brushDescriptorSetLayout);
//    
//    graphics->destroyPipeline(layerPipeline);
//    graphics->destroyPipelineLayout(layerPipelineLayout);
//    graphics->destroyRenderPass(layerRenderPass);
//    graphics->destroyFrameBuffer(layerFrameBuffer);
//    graphics->destroyDescriptorPool(layerDescriptorPool);
//    graphics->destroyDescriptorSetLayout(layerDescriptorSetLayout);
//
//    graphics->cleanupVulkan();
//    
//}
//
//void VulkanWindow::exposeEvent(QExposeEvent *event) {
//    
//    if (isExposed()) { // first update request
//        requestUpdate(); // note: generally synced to refresh rate
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
//        requestUpdate(); // continue render updates
//        return true;
//        
//    } else if (event->type() == QEvent::PlatformSurface) {
//        auto *surfaceEvent = static_cast<QPlatformSurfaceEvent *>(event);
//
//        switch (surfaceEvent->surfaceEventType()) { // on start
//        case QPlatformSurfaceEvent::SurfaceCreated:
//            init();
//            break;
//
//        case QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed: // on close
//            cleanup();
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
//    setDirtyFlag(DIRTY_SWAPCHAIN);
//    QWindow::resizeEvent(event);
//}
//
//void VulkanWindow::mousePressEvent(QMouseEvent *event) {
//    if (event->button() == Qt::LeftButton) {
//        // todo: brush down and brush
//        qDebug() << "Left click at" << event->position();
//        bufferBrush(event->position().x(), event->position().y(), brushSize);
//    }
//}
