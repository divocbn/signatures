//
// Created by Divo on 29.04.26.
//

#include "app.h"

#include "depot.h"
#include "gamebuild.h"
#include "spdlog/spdlog.h"
#include "lyra/lyra.hpp"

#include "../utils/sigmaker.h"

#include <filesystem>
void App::Run(int32_t argc, char* argv[])
{
    spdlog::info(">> running signatures v{}.{}.{}!",
        SIGNATURES_VERSION_MAJOR, SIGNATURES_VERSION_MINOR, SIGNATURES_VERSION_PATCH);

    auto cli = lyra::cli()
        | lyra::opt(m_Build, "number")["-b"]["--build"]
            .required()
            ("Build number (e.g. 3258)")

        | lyra::opt(m_OffsetStr, "hex")["-o"]["--offset"]
            .required()
            ("Target offset (hex, e.g. 0x140123456)")

        | lyra::opt(m_Download)["-d"]["--download"]
            ("Download all builds");

    if (const auto result = cli.parse({argc, argv}); !result)
    {
        spdlog::error("!! failed to parse arguments!");
        std::cout << cli << std::endl;
        return;
    }

    // NOTE(module): you can change that here if u want to bring your own dump build depot
    Depot depot("https://cdn.m0dule.dev/signatures/gta5");

    if (m_Download)
    {
        spdlog::info(">> fetching manifest...");

        const auto builds = depot.GetList();

        if (builds.empty())
        {
            spdlog::error("!! manifest empty or unreachable");
            return;
        }

        spdlog::info(">> downloading ALL builds (count: {})", builds.size());

        for (const auto& b : builds)
        {
            spdlog::info(">> downloading build {}", b);

            if (!depot.Fetch(b, "./builds"))
            {
                spdlog::error("!! failed to download build {}", b);
                continue;
            }

            spdlog::info("++ downloaded build {}", b);
        }

        spdlog::info("++ all available builds processed");
        return;
    }

    try
    {
        m_Offset = std::stoull(m_OffsetStr, nullptr, 16);
    }
    catch (...)
    {
        spdlog::error("!! invalid hex offset: {}", m_OffsetStr);
        return;
    }

    const std::filesystem::path buildPath =
        "./builds/" + std::to_string(m_Build) + ".exe";

    if (!std::filesystem::exists(buildPath))
    {
        spdlog::warn("!! build {} does not exist locally", m_Build);
        spdlog::warn(">> do you want to download it? (y/n)");

        std::string input;
        std::cin >> input;

        if (input != "y" && input != "Y")
        {
            spdlog::info(">> aborted by user");
            return;
        }

        spdlog::info(">> downloading build {}", m_Build);

        if (!depot.Fetch(m_Build, "./builds"))
        {
            spdlog::error("!! failed to download build {}", m_Build);
            return;
        }

        spdlog::info("++ download complete");
    }

    const GameBuild game(buildPath.string());
    if (!game)
    {
        spdlog::info("!! failed to parse game build, please check again.");
        return;
    }

    spdlog::info(">> converting offset to signature for gamebuild {}", game.GetBuildName());

    const auto bytes = game.GetBytesAtOffset(m_Offset);

    SigMaker sig;
    sig.Scan(bytes.data(), bytes.size());

    std::string pattern = sig.GetPattern();

    spdlog::info("++ pattern: {}", pattern);
}