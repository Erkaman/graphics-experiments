#include "cocoa_util.hpp"

#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL

#include <GLFW/glfw3native.h>

void FocusWindow(GLFWwindow* window) {

    NSWindow* nsWindow = glfwGetCocoaWindow(window);

    [nsWindow makeKeyAndOrderFront:nil];

//    bla;
}
