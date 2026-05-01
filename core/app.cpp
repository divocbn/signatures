//
// Created by Divo on 29.04.26.
//

#include "app.h"

#include "gamebuild.h"
#include "spdlog/spdlog.h"

#include "lyra/lyra.hpp"
#include "../utils/sigmaker.h"

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
        spdlog::error("    - see:");
        std::cout << cli << "\n";
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

    const GameBuild game("gta5.exe");
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
