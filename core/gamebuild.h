//
// Created by Divo on 29.04.26.
//

#ifndef SIGNATURES_GAMEBUILD_H
#define SIGNATURES_GAMEBUILD_H

#include <memory>
#include <string>

#include "LIEF/MachO/type_traits.hpp"
#include "LIEF/PE/Binary.hpp"

class GameBuild
{
public:
    explicit GameBuild(std::string filePath);

    [[nodiscard]]
    std::vector<uint8_t> GetBytesAtOffset(uint32_t offset, uint32_t size = 64) const;

    explicit operator bool() const {
        return m_IsValid;
    }
private:
    std::string m_FilePath;
    uint32_t m_BuildNumber = 0;

    std::unique_ptr<LIEF::PE::Binary> m_PeBinary;

    bool m_IsValid = false;
};

#endif //SIGNATURES_GAMEBUILD_H
