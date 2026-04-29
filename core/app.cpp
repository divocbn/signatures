//
// Created by Divo on 29.04.26.
//

#include "app.h"

#include "gamebuild.h"
#include "spdlog/spdlog.h"

void App::Run() {
    spdlog::info(">> running app!");

    const GameBuild game("gta5.exe");
    const auto bytes = game.GetBytesAtOffset(0xF3FF88);

    for (const auto byte : bytes) {
        printf("%x ", byte);
    }
}
