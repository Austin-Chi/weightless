//
//  app_delegate.h
//  Metal_Engine_cpp
//
//  Created by 錡亭勳 on 4/19/26.
//
#pragma once
#include "../config.h"
#include "../view/renderer.h"
#include "../model/camera.h"

class App
{
public:
    App();
    ~App();
    void run();

private:
    GLFWwindow* glfwWindow;
    MTL::Device* device;
    CA::MetalLayer* metalLayer;
    NS::Window* window;
    Renderer* renderer;
    Camera* camera;
};
