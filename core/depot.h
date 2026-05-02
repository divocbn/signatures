//
// Created by Divo on 01.05.2026.
//

#ifndef SIGNATURES_DEPOT_H
#define SIGNATURES_DEPOT_H

#include <string>
#include <vector>
#include <cstdint>

class Depot
{
public:
    explicit Depot(std::string baseUrl);

    [[nodiscard]]
    std::vector<uint32_t> GetList() const;

    [[nodiscard]]
    bool Fetch(uint32_t build, const std::string& outDir = "builds") const;

private:
    [[nodiscard]]
    std::string Request(const std::string& url) const;

    std::string m_BaseUrl;
};

#endif // SIGNATURES_DEPOT_H
