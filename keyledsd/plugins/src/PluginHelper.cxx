/* Keyleds -- Gaming keyboard tool
 * Copyright (C) 2017 Julien Hartmann, juli1.hartmann@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "keyledsd/PluginHelper.h"

#include "keyledsd/colors.h"
#include <algorithm>
#include <variant>

namespace keyleds::plugin {

std::optional<detail::variant_wrapper> getConfig(const EffectService & service, const char * key)
{
    const auto & config = service.configuration();
    auto it = std::find_if(config.begin(), config.end(),
                           [key](auto & item) { return item.first == key; });
    if (it != config.end()) { return std::ref(it->second); }
    return std::nullopt;
}

std::optional<std::vector<RGBAColor>>
detail::get_config<std::vector<RGBAColor>>::parse(const EffectService &, const alternative & seq)
{
    std::optional<std::vector<RGBAColor>> result = std::vector<RGBAColor>{};
    std::for_each(seq.begin(), seq.end(),
                  [&result](const auto & str) {
                      auto color = RGBAColor::parse(str);
                      if (color) { result->push_back(*color); }
                  });
    return result;
}

std::optional<KeyDatabase::KeyGroup>
detail::get_config<KeyDatabase::KeyGroup>::parse(const EffectService & service, const alternative & name)
{
    const auto & groups = service.keyGroups();
    auto it = std::find_if(groups.begin(), groups.end(),
                           [&name](auto & group) { return group.name() == name; });
    if (it != groups.end()) {
        return *it;
    }
    return std::nullopt;
}

#ifndef KEYLEDS_IGNORE_COMPATIBILITY
static constexpr auto numeredListDeprecationMessage =
    "numbered lists are deprecated, please check "
    "https://github.com/spectras/keyleds/wiki/Numbered-list-deprecation for help.";

std::optional<std::vector<RGBAColor>>
getColorsCompatibility(EffectService & service, const char * key)
{
    using std::to_string;
    auto result = std::vector<RGBAColor>{};

    std::string currentKey;
    for (unsigned idx = 0; ; ++idx) {
        currentKey = key;
        currentKey.pop_back();
        currentKey += to_string(idx);
        auto value = getConfig(service, currentKey.c_str());
        if (!value || !std::holds_alternative<std::string>(value->get())) { break; }
        auto color = RGBAColor::parse(std::get<std::string>(value->get()));
        if (!color) { break; }
        result.push_back(*color);
    }

    if (result.empty()) { return std::nullopt; }

    service.log(logging::warning::value, numeredListDeprecationMessage);
    return std::move(result);
}
#endif

} // namespace keyleds::plugin
