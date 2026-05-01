//
// Created by Divo on 01.05.2026.
//

#include "depot.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

Depot::Depot(std::string baseUrl)
    : m_BaseUrl(std::move(baseUrl))
{
    if (!m_BaseUrl.ends_with('/'))
        m_BaseUrl.push_back('/');
}

std::string Depot::Request(const std::string& url) const
{
    std::string_view view = url;

    if (!view.empty() && view.front() == '/')
        view.remove_prefix(1);

    const auto res = cpr::Get(cpr::Url{std::string(m_BaseUrl) + std::string(view)});

    return (res.status_code == 200) ? res.text : std::string{};
}

std::vector<uint32_t> Depot::GetList() const
{
    const auto body = Request("/manifest.json");
    if (body.empty())
        return {};

    auto json = nlohmann::json::parse(body);
    if (json.is_discarded() || !json.contains("builds"))
        return {};

    std::vector<uint32_t> builds;

    for (const auto& b : json["builds"])
    {
        if (b.is_number_unsigned())
            builds.push_back(b.get<uint32_t>());
    }

    return builds;
}

bool Depot::Fetch(const uint32_t build, const std::string& outDir) const
{
    if (const auto builds = GetList(); !std::ranges::contains(builds, build))
        return false;

    std::filesystem::create_directories(outDir);

    const std::string file = std::to_string(build) + ".exe";
    const std::filesystem::path path = std::filesystem::path(outDir) / file;

    std::ofstream out(path, std::ios::binary);
    if (!out)
        return false;

    const std::string url =
        (m_BaseUrl.ends_with('/') ? m_BaseUrl : m_BaseUrl + "/") + file;

    const auto res = cpr::Get(
        cpr::Url{url},
        cpr::WriteCallback([&out](const std::string_view data, intptr_t) noexcept {
            out.write(data.data(), data.size());
            return true;
        })
    );

    return res.status_code == 200;
}