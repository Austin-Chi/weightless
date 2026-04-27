//
//  glfw_adapter.mm
//  Metal_Engine_cpp
//
//  Created by 錡亭勳 on 4/22/26.
//

#import "glfw_adapter.h"

NS::Window* get_ns_window(GLFWwindow* glfwWindow, CA::MetalLayer* layer)
{
    CALayer* obj_layer = (__bridge CALayer*) layer;
    NSWindow* obj_window = glfwGetCocoaWindow(glfwWindow);
    
    obj_window.contentView.layer = obj_layer;
    obj_window.contentView.wantsLayer = YES;
    
    return (__bridge NS::Window*)obj_window;
}
