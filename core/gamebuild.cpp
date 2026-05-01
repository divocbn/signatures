//
// Created by Divo on 29.04.26.
//

#include "gamebuild.h"

#include <filesystem>
#include <regex>

GameBuild::GameBuild(std::string filePath) :
    m_FilePath(std::move(filePath)),
    m_Pe(m_FilePath)
{
    m_IsValid = m_Pe.IsValid();
}

std::vector<uint8_t> GameBuild::GetBytesAtOffset(const uint32_t offset, const uint32_t size) const
{
    return m_Pe.GetBytesAtRva(offset, size);
}

uint32_t GameBuild::GetBuildFromFilePath() const
{
    if (!m_IsValid)
        return 0;

    const std::filesystem::path path(m_FilePath);

    if (!std::filesystem::exists(path))
        return 0;

    // NOTE: that regex basically parses all type of game file name, for example:
    //       - 3258.exe, gta5-3258.exe, gta5_b3258.exe, gta5-b3258-noaslr-dump.exe
    static const std::regex pattern(R"((?:^|[-_])(?:b)?([0-9]{3,5})(?=[^0-9]|$))",
                                    std::regex::icase);

    const std::string filename = path.filename().string();
    if (std::smatch match; std::regex_search(filename, match, pattern))
    {
        return static_cast<uint32_t>(std::stoul(match[1].str()));
    }

    return 0;
}

std::string GameBuild::GetBuildName() const
{
    const auto build = GetBuildFromFilePath();
    if (!build)
        return "Unknown";

    return "b" + std::to_string(build);
}
