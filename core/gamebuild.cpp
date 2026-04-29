//
// Created by Divo on 29.04.26.
//

#include "gamebuild.h"

#include "LIEF/PE/Parser.hpp"

GameBuild::GameBuild(std::string filePath) :
    m_FilePath(std::move(filePath))
{
    m_PeBinary = LIEF::PE::Parser::parse("gta5.exe");
}

std::vector<uint8_t> GameBuild::GetBytesAtOffset(const uint32_t offset, const uint32_t size) const {
    // const auto rva = m_PeBinary->offset_to_virtual_address(offset)
    //     .value<uint64_t>();

    const auto contentSpan = m_PeBinary->get_content_from_virtual_address(offset, size);
    return {contentSpan.begin(), contentSpan.end()};
}
