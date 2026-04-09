// main.cpp
#include <graphics/window.h>

int main() {
    Window window(800, 600, u8"BlezeBolt game engine - движок для инди игр");
    
    window.setClearColor(192/255.0f, 192/255.0f, 192/255.0f, 1.0f);
    
    while (!window.shouldClose()) {
        window.clear();
        
        window.pollEvents();
        window.swapBuffers();
    }
    
    return 0;
}