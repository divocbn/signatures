//
// Created by Divo on 01.05.2026.
//

#ifndef SIGNATURES_PE_H
#define SIGNATURES_PE_H

#include <fstream>
#include <optional>
#include <vector>
#include <string>


class PE
{
public:
    explicit PE(std::string filePath);

    [[nodiscard]]
    bool IsValid() const noexcept
    {
        return m_IsValid;
    }

    [[nodiscard]]
    std::vector<uint8_t> GetBytesAtRva(uint32_t rva, uint32_t size) const;

private:
    struct SectionInfo
    {
        uint32_t m_VirtualAddress;
        uint32_t m_VirtualSize;
        uint32_t m_RawOffset;
        uint32_t m_RawSize;
    };

    bool ParsePe();

    [[nodiscard]]
    std::optional<std::pair<uint32_t, uint32_t>> RvaToRaw(uint32_t rva) const;

    std::string m_FilePath;
    std::vector<uint8_t> m_Data;
    std::vector<SectionInfo> m_Sections;

    bool m_IsValid = false;
};

#endif //SIGNATURES_PE_H
