#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "first_app.hpp"

void ErrorCallback(int, const char* err_str)
{
    std::cout << "GLFW Error: " << err_str << std::endl;
}

int main() {
    // Register error callback first
    glfwSetErrorCallback(ErrorCallback);

    lve::FirstApp app{};

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
