//
// Created by Divo on 29.04.26.
//

#ifndef SIGNATURES_GAMEBUILD_H
#define SIGNATURES_GAMEBUILD_H

#include "../utils/pe.h"

#include <string>

class GameBuild
{
public:
    explicit GameBuild(std::string filePath);

    [[nodiscard]]
    std::vector<uint8_t> GetBytesAtOffset(uint32_t offset, uint32_t size = 64) const;

    [[nodiscard]]
    uint32_t GetBuildFromFilePath() const;

    [[nodiscard]]
    std::string GetBuildName() const;

    explicit operator bool() const
    {
        return m_IsValid;
    }
private:
    std::string m_FilePath;
    uint32_t m_BuildNumber = 0;

    PE m_Pe;

    bool m_IsValid = false;
};

#endif //SIGNATURES_GAMEBUILD_H
