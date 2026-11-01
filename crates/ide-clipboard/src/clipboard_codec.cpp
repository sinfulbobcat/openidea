#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

namespace {

constexpr std::size_t header_size = 20;

void append_utf8(std::vector<std::uint8_t>& output, std::uint32_t codepoint) {
    if (codepoint <= 0x7f) {
        output.push_back(static_cast<std::uint8_t>(codepoint));
    } else if (codepoint <= 0x7ff) {
        output.push_back(static_cast<std::uint8_t>(0xc0 | (codepoint >> 6)));
        output.push_back(static_cast<std::uint8_t>(0x80 | (codepoint & 0x3f)));
    } else if (codepoint <= 0xffff) {
        output.push_back(static_cast<std::uint8_t>(0xe0 | (codepoint >> 12)));
        output.push_back(static_cast<std::uint8_t>(0x80 | ((codepoint >> 6) & 0x3f)));
        output.push_back(static_cast<std::uint8_t>(0x80 | (codepoint & 0x3f)));
    } else {
        output.push_back(static_cast<std::uint8_t>(0xf0 | (codepoint >> 18)));
        output.push_back(static_cast<std::uint8_t>(0x80 | ((codepoint >> 12) & 0x3f)));
        output.push_back(static_cast<std::uint8_t>(0x80 | ((codepoint >> 6) & 0x3f)));
        output.push_back(static_cast<std::uint8_t>(0x80 | (codepoint & 0x3f)));
    }
}

void append_utf16_lossy(std::vector<std::uint8_t>& output, const std::vector<std::uint16_t>& units) {
    for (std::size_t index = 0; index < units.size(); ++index) {
        std::uint32_t codepoint = units[index];
        if (codepoint >= 0xd800 && codepoint <= 0xdbff) {
            if (index + 1 < units.size() && units[index + 1] >= 0xdc00 && units[index + 1] <= 0xdfff) {
                codepoint = 0x10000 + ((codepoint - 0xd800) << 10) + (units[++index] - 0xdc00);
            } else {
                codepoint = 0xfffd;
            }
        } else if (codepoint >= 0xdc00 && codepoint <= 0xdfff) {
            codepoint = 0xfffd;
        }
        append_utf8(output, codepoint);
    }
}

std::vector<std::uint8_t> encode(const char* const* paths, const std::size_t* lengths, std::size_t count) {
    std::vector<std::uint16_t> units;
    for (std::size_t path_index = 0; path_index < count; ++path_index) {
        const auto* path = reinterpret_cast<const std::uint8_t*>(paths[path_index]);
        const auto length = lengths[path_index];
        if (length == 0) continue;
        for (std::size_t index = 0; index < length;) {
            const auto first = path[index];
            std::size_t count_bytes = first >= 0xf0 ? 4 : first >= 0xe0 ? 3 : first >= 0xc0 ? 2 : 1;
            std::uint32_t codepoint = count_bytes == 4 ? first & 0x07 : count_bytes == 3 ? first & 0x0f : count_bytes == 2 ? first & 0x1f : first;
            for (std::size_t byte = 1; byte < count_bytes && index + byte < length; ++byte) {
                codepoint = (codepoint << 6) | (path[index + byte] & 0x3f);
            }
            index += count_bytes;
            if (codepoint <= 0xffff) {
                units.push_back(static_cast<std::uint16_t>(codepoint));
            } else {
                codepoint -= 0x10000;
                units.push_back(static_cast<std::uint16_t>(0xd800 | (codepoint >> 10)));
                units.push_back(static_cast<std::uint16_t>(0xdc00 | (codepoint & 0x3ff)));
            }
        }
        units.push_back(0);
    }
    units.push_back(0);

    std::vector<std::uint8_t> output;
    output.reserve(header_size + units.size() * 2);
    output.insert(output.end(), {static_cast<std::uint8_t>(header_size), 0, 0, 0});
    output.insert(output.end(), 8, 0);
    output.insert(output.end(), {0, 0, 0, 0, 1, 0, 0, 0});
    for (const auto unit : units) {
        output.push_back(static_cast<std::uint8_t>(unit));
        output.push_back(static_cast<std::uint8_t>(unit >> 8));
    }
    return output;
}

std::vector<std::uint8_t> decode(const std::uint8_t* bytes, std::size_t length) {
    if (length < header_size) return {};
    const std::uint32_t offset = static_cast<std::uint32_t>(bytes[0]) |
        (static_cast<std::uint32_t>(bytes[1]) << 8) |
        (static_cast<std::uint32_t>(bytes[2]) << 16) |
        (static_cast<std::uint32_t>(bytes[3]) << 24);
    const bool wide = bytes[16] != 0 || bytes[17] != 0 || bytes[18] != 0 || bytes[19] != 0;
    if (offset >= length) return {};

    std::vector<std::uint8_t> output;
    if (wide) {
        std::vector<std::uint16_t> current;
        for (std::size_t index = offset; index + 1 < length; index += 2) {
            const auto unit = static_cast<std::uint16_t>(bytes[index] | (static_cast<std::uint16_t>(bytes[index + 1]) << 8));
            if (unit != 0) {
                current.push_back(unit);
            } else if (current.empty()) {
                break;
            } else {
                append_utf16_lossy(output, current);
                output.push_back(0);
                current.clear();
            }
        }
    } else {
        for (std::size_t start = offset; start < length;) {
            auto end = start;
            while (end < length && bytes[end] != 0) ++end;
            if (end == start) break;
            output.insert(output.end(), bytes + start, bytes + end);
            output.push_back(0);
            start = end + 1;
        }
    }
    return output;
}

} // namespace

extern "C" std::size_t ide_clipboard_encode(const char* const* paths, const std::size_t* lengths, std::size_t count, std::uint8_t* output, std::size_t capacity) noexcept {
    try {
        const auto encoded = encode(paths, lengths, count);
        if (output == nullptr || capacity < encoded.size()) return encoded.size();
        std::memcpy(output, encoded.data(), encoded.size());
        return encoded.size();
    } catch (...) {
        return 0;
    }
}

extern "C" std::size_t ide_clipboard_decode(const std::uint8_t* bytes, std::size_t length, std::uint8_t* output, std::size_t capacity) noexcept {
    try {
        const auto decoded = decode(bytes, length);
        if (output == nullptr || capacity < decoded.size()) return decoded.size();
        std::memcpy(output, decoded.data(), decoded.size());
        return decoded.size();
    } catch (...) {
        return 0;
    }
}