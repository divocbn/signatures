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
            ("Download builds");

    if (const auto result = cli.parse({argc, argv}); !result)
    {
        spdlog::error("!! failed to parse arguments!");

        // looks weird with spdlog prefix
        std::cout << cli << std::endl;
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

        spdlog::info(">> available builds:");

        for (const auto b : builds)
            spdlog::info("   - {}", b);

        spdlog::info(">> downloading build {}", m_Build);

        if (!std::ranges::contains(builds, m_Build))
        {
            spdlog::error("!! build {} not in manifest", m_Build);
            return;
        }

        if (!depot.Fetch(m_Build, "./builds"))
        {
            spdlog::error("!! failed to download build {}", m_Build);
            return;
        }

        spdlog::info("++ downloaded successfully to ./builds");
    }

    const GameBuild game("./builds/" + std::to_string(m_Build) + ".exe");
    if (!game)
    {
        spdlog::info("!! failed to parse game build, please check again.");
        return;
    }

    spdlog::info(">> converting offset to signature for gamebuild {}", game.GetBuildName());

    const auto bytes = game.GetBytesAtOffset(m_Offset);

    SigMaker sig;

    // NOTE: currently size is hardcoded as 64 bytes, need to think about a way to generate sigs which are unique and short as possible
    sig.Scan(bytes.data(), bytes.size());

    std::string pattern = sig.GetPattern();

    spdlog::info("++ pattern: {}", pattern.c_str());
}
