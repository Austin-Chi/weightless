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
    
    battlefield = new Battlefield(32, 32, 32);
    int* positions = battlefield->getCharacterStartPositions();
    character = new Character();
    character->setPosition({float(positions[0]), float(positions[1]), float(positions[2])});
    renderer = new Renderer(device, metalLayer, battlefield, character);
    camera = new Camera();
    camera->followCharacter(character->getPosition(), character->getUpVector(), battlefield);
}

App::~App()
{
    window->release();
    delete renderer;
    delete camera;
    delete battlefield;
    delete character;
    glfwTerminate();
}

void App::run()
{
    double cursorX = 0.0;
    double cursorY = 0.0;
    float dx = 0.0f;
    float dy = 0.0f;

    while(!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();

        if (glfwGetKey(glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
            character->setMoving();
        }

        if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
        }

        character->update(battlefield);
        camera->followCharacter(character->getPosition(), character->getUpVector(), battlefield);
        glfwGetCursorPos(glfwWindow, &cursorX, &cursorY);
        dx = -10.0f * static_cast<float>(cursorX / 400.0 - 1.0);
        dy = -10.0f * static_cast<float>(cursorY / 300.0 - 1.0);
        glfwSetCursorPos(glfwWindow, 400.0, 300.0);
        camera->spin(dy, dx);
        renderer->update(camera->getViewTransform());
    }
}