#include <iostream>

#include "core/app.h"
#include "spdlog/spdlog.h"

int main(int32_t argc, char* argv[]) {
    spdlog::info(">> starting app!");

    const auto app = std::make_unique<App>();
    app->Run();
}
