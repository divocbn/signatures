//
// Created by Divo on 01.05.2026.
//

#ifndef SIGNATURES_SIGMAKER_H
#define SIGNATURES_SIGMAKER_H

#include <Zydis/Zydis.h>
#include <vector>
#include <string>
#include <iomanip>

class SigMaker
{
public:
    SigMaker() {
        ZydisDecoderInit(
            &m_Decoder,
            ZYDIS_MACHINE_MODE_LONG_64,
            ZYDIS_STACK_WIDTH_64
        );
    }

    void Scan(const uint8_t* data, size_t size);

    [[nodiscard]]
    std::string GetPattern() const;

private:
    void Process(const ZydisDecodedInstruction& inst,
                 const ZydisDecodedOperand* ops,
                 const uint8_t* data);

    void Mask(const ZydisDecodedInstruction& inst,
              const ZydisDecodedOperand& op, size_t base);

    void MaskRel(const ZydisDecodedInstruction& inst, size_t base);

    ZydisDecoder m_Decoder{};
    std::vector<uint8_t> m_Bytes;
    std::vector<uint8_t> m_Mask;
};

#endif // SIGNATURES_SIGMAKER_H
