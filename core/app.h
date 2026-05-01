//
// Created by Divo on 29.04.26.
//

#ifndef SIGNATURES_APP_H
#define SIGNATURES_APP_H
#include <cstdint>
#include <string>

#define SIGNATURES_VERSION_MAJOR 1
#define SIGNATURES_VERSION_MINOR 0
#define SIGNATURES_VERSION_PATCH 0


class App
{
public:
    void Run(int32_t argc, char* argv[]);

private:
    uint32_t m_Build = 0;

    std::string m_OffsetStr;
    uint64_t m_Offset{};

    bool m_Download = false;
};

#endif //SIGNATURES_APP_H
