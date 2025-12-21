// graphics application

GraphicsApplication::GraphicsApplication() : Application() { // constructor
    // currently empty
}

GraphicsApplication::~GraphicsApplication() { // destructor
    // cleanup here
}

void GraphicsApplication::init() {
    
    graphics.initWindow(); // todo: configurables here ? window size
    graphics.initVulkan();
    
    // model texture
            
    graphics.loadTexture("viking_room.png",
                         modelTextureImage,
                         modelTextureImageMemory,
                         modelTextureImageView,
                         VK_IMAGE_USAGE_TRANSFER_SRC_BIT
                         | VK_IMAGE_USAGE_TRANSFER_DST_BIT
                         | VK_IMAGE_USAGE_SAMPLED_BIT,
                         1);
    
    graphics.transitionImageLayout(modelTextureImage,
                                   VK_FORMAT_R8G8B8A8_SRGB,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                   1);
    
    graphics.textureImages.push_back(modelTextureImage);
    graphics.textureImageMemories.push_back(modelTextureImageMemory);
    graphics.textureImageViews.push_back(modelTextureImageView);
    
    // load canvas quad
    
    auto model = Graphics::loadObj("viking_room.obj");
    graphics.pushModel(model); // todo: which objs to draw, with which materials
    
    // finish rest of vulkan setup
    
    graphics.initRender("vert.spv", "frag.spv"); // vertex buffers + swap chain pipeline

}

void GraphicsApplication::draw() {
    
    uint32_t imageIndex;
    
    graphics.startFrame(imageIndex);
    
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    
    int windowWidth, windowHeight;
    glfwGetWindowSize(graphics.window, &windowWidth, &windowHeight);

    glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 0.0f, 1.0f));
    
    glm::mat4 proj = glm::perspective(glm::radians(45.0f),
                                      windowWidth / (float) windowHeight,
                                      0.1f,
                                      10.0f);
    proj[1][1] *= -1;
    
    graphics.updateGlobalUBO(view, proj);
    
    std::vector<InstanceSSBO> instances(graphics.MAX_ENTITIES);
    instances[0].model = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    graphics.updateInstanceSSBOs(instances);
    
    graphics.submitFrame(imageIndex);

}

void GraphicsApplication::run() {
    
    // actual main loop
    while (!glfwWindowShouldClose(graphics.window)) {
        glfwPollEvents();
        draw();
    }
    vkDeviceWaitIdle(graphics.device);
    
}

void GraphicsApplication::cleanup() {
    
    graphics.cleanup();
    
}
