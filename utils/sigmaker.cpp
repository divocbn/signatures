//
// Created by Divo on 01.05.2026.
//

#include "sigmaker.h"

#include "spdlog/spdlog.h"

#include <sstream>
#include <mem/pattern.h>

void SigMaker::Scan(const uint8_t* data, const size_t size)
{
    m_Bytes.clear();
    m_Mask.clear();

    size_t offset = 0;

    while (offset < size)
    {
        ZydisDecodedInstruction inst;
        ZydisDecodedOperand ops[ZYDIS_MAX_OPERAND_COUNT];

        if (!ZYAN_SUCCESS(ZydisDecoderDecodeFull(&m_Decoder, data + offset, size - offset, &inst, ops)))
            break;

        Process(inst, ops, data + offset);
        offset += inst.length;
    }
}

std::string SigMaker::GetPattern() const
{
    std::ostringstream oss;

    for (size_t i = 0; i < m_Bytes.size(); i++)
    {
        if (m_Mask[i])
        {
            oss << "? ";
        }
        else
        {
            oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(m_Bytes[i]) << " ";
        }
    }

    return oss.str();
}

// NOTE(module): kinda mimics basically https://github.com/senator715/IDA-Fusion
//               very... VERY hacky, but just for test purposes, gonna rework that later
std::string SigMaker::GetUniquePattern(const PE& pe, size_t maxSize)
{
    const auto& base = m_Bytes;
    if (base.empty())
        return {};

    const auto* data = pe.GetRawData();
    const size_t size = pe.GetSize();

    const mem::region fullRange(data, size);

    std::string currentBestPattern;

    const size_t limit = std::min({maxSize, base.size()});

    for (size_t len = 1; len <= limit; ++len)
    {
        m_Bytes.clear();
        m_Mask.clear();

        Scan(base.data(), len);

        const std::string pattern = GetPattern();
        currentBestPattern = pattern;

        const mem::pattern pat(pattern.data());

        size_t matchCount = 0;

        mem::region searchRange(data, size);
        while (true)
        {
            mem::pointer hit = mem::scan(pat, searchRange);
            if (!hit)
                break;

            ++matchCount;

            const auto* hitAddr = hit.as<const uint8_t*>();
            const auto offset = static_cast<size_t>(hitAddr - data);

            if (offset + 1 >= size)
                break;

            searchRange = mem::region(hitAddr + 1, size - (offset + 1));
        }

        if (matchCount == 1)
            return pattern;
    }

    return currentBestPattern;
}

void SigMaker::Process(const ZydisDecodedInstruction& inst, const ZydisDecodedOperand* ops, const uint8_t* data)
{
    const size_t base = m_Bytes.size();

    for (uint8_t i = 0; i < inst.length; i++)
    {
        m_Bytes.push_back(data[i]);
        m_Mask.push_back(false);
    }

    for (uint8_t i = 0; i < inst.operand_count; i++)
    {
        const auto& op = ops[i];

        if (op.type == ZYDIS_OPERAND_TYPE_IMMEDIATE)
            Mask(inst, op, base);

        // NOTE(module): assume disp is always at end, not always true but works most times ig
        if (op.type == ZYDIS_OPERAND_TYPE_MEMORY && op.mem.disp.size)
            Mask(inst, op, base);
    }

    // NOTE(module): also here we just mask last 4 bytes for rel, kinda hacky
    if (inst.attributes & ZYDIS_ATTRIB_IS_RELATIVE)
        MaskRel(inst, base);
}

void SigMaker::Mask(const ZydisDecodedInstruction& inst, const ZydisDecodedOperand& op, const size_t base)
{
    const size_t size = op.type == ZYDIS_OPERAND_TYPE_IMMEDIATE ? op.size / 8 : op.mem.disp.size / 8;

    // NOTE(module): assumes imm/disp always at end of instruction >> not always true (?)
    for (size_t i = 0; i < size && i < inst.length; i++)
        m_Mask[base + inst.length - size + i] = true;
}

void SigMaker::MaskRel(const ZydisDecodedInstruction& inst, size_t base)
{
    if (inst.length >= 4)
        for (size_t i = inst.length - 4; i < inst.length; i++)
            m_Mask[base + i] = true;
}