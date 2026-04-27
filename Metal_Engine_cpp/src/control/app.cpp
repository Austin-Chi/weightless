//
//  app_delegate.cpp
//  Metal_Engine_cpp
//
//  Created by 錡亭勳 on 4/19/26.
//
#include "app.h"
#include "../backend/glfw_adapter.h"
#include "../backend/mtlm.h"

App::App()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindow = glfwCreateWindow(800, 600, "Heavy", NULL, NULL);
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
    device = MTL::CreateSystemDefaultDevice();
    
    metalLayer = CA::MetalLayer::layer()->retain();
    metalLayer->setDevice(device);
    metalLayer->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);
    metalLayer->setFramebufferOnly(false);
    
    window = get_ns_window(glfwWindow, metalLayer)->retain();
    
    renderer = new Renderer(device, metalLayer);

    camera = new Camera();
    camera->setPosition({-10.0f, 0.0f, 5.0f});
    camera->setAngles(0.0f, 0.0f);
}

App::~App()
{
    window->release();
    delete renderer;
    delete camera;
    glfwTerminate();
}

void App::run()
{
    double cursorX, cursorY;
    float dx = 0.0f, dy = 0.0f;
    while(!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();

        simd::float3 movement = {0.0f, 0.0f, 0.0f};
        
        if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS) {
            movement[0] += 0.1f;
        }

        if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS) {
            movement[0] -= 0.1f;
        }

        if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS) {
            movement[1] -= 0.1f;
        }

        if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS) {
            movement[1] += 0.1f;
        }

        if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
        }

        camera->walk(movement);

        glfwGetCursorPos(glfwWindow, &cursorX, &cursorY);
        dx = -10.0f * static_cast<float>(cursorX / 400.0 - 1.0);
        dy = -10.0f * static_cast<float>(cursorY / 300.0 - 1.0);
        glfwSetCursorPos(glfwWindow, 400.0, 300.0);

        camera->spin(dy, dx);
        renderer->update(camera->getViewTransform());
    }
}