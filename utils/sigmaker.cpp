//
// Created by Divo on 01.05.2026.
//

#include "sigmaker.h"

#include "spdlog/spdlog.h"

#include <sstream>

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
        {
            spdlog::error("failed to decode instruction: {}", reinterpret_cast<uintptr_t>(data + offset));
            break;
        }

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