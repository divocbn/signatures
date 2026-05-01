//
// Created by Divo on 01.05.2026.
//

#include "pe.h"

#include <Windows.h>

PE::PE(std::string filePath)
    : m_FilePath(std::move(filePath))
{
    std::ifstream file(m_FilePath, std::ios::binary | std::ios::ate);
    if (!file)
    {
        m_IsValid = false;
        return;
    }

    const auto fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    m_Data.resize(fileSize);
    file.read(reinterpret_cast<char*>(m_Data.data()), fileSize);

    m_IsValid = ParsePe();
}

std::vector<uint8_t> PE::GetBytesAtRva(const uint32_t rva, const uint32_t size) const
{
    const auto raw = RvaToRaw(rva);
    if (!raw.has_value())
        return {};

    const auto offset = raw->first;

    if (offset + size > m_Data.size())
        return {};

    return std::vector(m_Data.begin() + offset, m_Data.begin() + offset + size);
}

bool PE::ParsePe()
{
    if (m_Data.size() < sizeof(IMAGE_DOS_HEADER))
        return false;

    const auto* dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(m_Data.data());

    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        return false;

    const auto* ntHeaders = reinterpret_cast<const IMAGE_NT_HEADERS*>(m_Data.data() + dosHeader->e_lfanew);

    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
        return false;

    const auto* sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);

    const auto sectionCount = ntHeaders->FileHeader.NumberOfSections;

    m_Sections.clear();
    m_Sections.reserve(sectionCount);

    for (uint16_t i = 0; i < sectionCount; ++i)
    {
        const auto& section = sectionHeader[i];

        SectionInfo Info{
            .m_VirtualAddress = section.VirtualAddress,
            .m_VirtualSize    = section.Misc.VirtualSize,
            .m_RawOffset      = section.PointerToRawData,
            .m_RawSize        = section.SizeOfRawData
        };

        m_Sections.push_back(Info);
    }

    return true;
}

std::optional<std::pair<uint32_t, uint32_t>> PE::RvaToRaw(const uint32_t rva) const
{
    for (const auto& [virtualAddress, virtualSize, rawOffset, rawSize] : m_Sections)
    {
        const auto start = virtualAddress;
        const auto end = start + virtualSize;

        if (rva >= start && rva < end)
        {
            const auto offset = rawOffset + (rva - start);
            return std::make_pair(offset, rawSize);
        }
    }

    return std::nullopt;
}
