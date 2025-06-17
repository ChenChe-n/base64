#ifndef BASE64_HPP
#define BASE64_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

namespace base64
{
    /**
     * @brief 编码base64
     * @param data 为待编码的数据
     * @param table 为编码表
     * @return std::vector<uint8_t>
     */
    std::u32string encode(const uint8_t *data, const size_t num, const std::u32string &table = U"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=")
    {
        if (data == nullptr || num == 0)
            return {};
        if (table.size() != 65)
            return {};

        std::unordered_set<char32_t> table_set;
        table_set.reserve(65);
        for (char32_t ch : table)
            table_set.insert(ch);
        if (table_set.size() != 65)
            return {};

        std::u32string result;
        result.reserve(num * 2 + 4);

        size_t i = 0;
        while (i + 2 < num)
        {
            uint32_t value = (static_cast<uint32_t>(data[i]) << 16) |
                             (static_cast<uint32_t>(data[i + 1]) << 8) |
                             (static_cast<uint32_t>(data[i + 2]));

            result.push_back(table[(value >> 18) & 0x3F]);
            result.push_back(table[(value >> 12) & 0x3F]);
            result.push_back(table[(value >> 6) & 0x3F]);
            result.push_back(table[value & 0x3F]);
            i += 3;
        }

        // 处理尾部不足3字节
        if (num % 3 == 1)
        {
            uint32_t value = (static_cast<uint32_t>(data[i]) << 16);

            result.push_back(table[(value >> 18) & 0x3F]);
            result.push_back(table[(value >> 12) & 0x3F]);
            result.push_back(table[64]); // '='
            result.push_back(table[64]); // '='
        }
        else if (num % 3 == 2)
        {
            uint32_t value = (static_cast<uint32_t>(data[i]) << 16) |
                             (static_cast<uint32_t>(data[i + 1]) << 8);

            result.push_back(table[(value >> 18) & 0x3F]);
            result.push_back(table[(value >> 12) & 0x3F]);
            result.push_back(table[(value >> 6) & 0x3F]);
            result.push_back(table[64]); // '='
        }

        return result;
    }

    /**
     * @brief 解码base64
     * @param data 为待解码的数据
     * @param table 为解码表
     * @return std::vector<uint8_t>
     */
    std::vector<uint8_t> decode(const std::u32string &data, const std::u32string &table = U"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=")
    {
        if (data.empty())
            return {};
        if (table.size() != 65)
            return {};
        if ((data.size() & 0b11) != 0)
            return {};
        std::unordered_map<char32_t, uint8_t> table_map;
        table_map.reserve(256);
        for (size_t i = 0; i < table.size(); ++i)
            table_map.insert({table[i], i});
        if (table_map.size() != 65)
            return {};

        size_t padding = 0;
        if (data.size() >= 2 && data[data.size() - 1] == table.back())
            padding++;
        if (data.size() >= 2 && data[data.size() - 2] == table.back())
            padding++;

        std::vector<uint8_t> result;
        result.reserve(data.size() / 4 * 3 - padding);

        size_t full_blocks = (data.size() / 4) - (padding ? 1 : 0);

        for (size_t i = 0; i < full_blocks * 4; i += 4)
        {
            uint32_t value = (table_map[data[i]] << 18) |
                             (table_map[data[i + 1]] << 12) |
                             (table_map[data[i + 2]] << 6) |
                             (table_map[data[i + 3]]);
            result.push_back(value >> 16);
            result.push_back(value >> 8);
            result.push_back(value);
        }

        // 处理最后一个带 padding 的 block
        if (padding)
        {
            size_t i = full_blocks * 4;
            uint32_t value = (table_map[data[i]] << 18) |
                             (table_map[data[i + 1]] << 12);

            if (padding == 1)
            {
                value |= (table_map[data[i + 2]] << 6);
                result.push_back(value >> 16);
                result.push_back((value >> 8) & 0xFF);
            }
            else if (padding == 2)
            {
                result.push_back(value >> 16);
            }
        }

        return result;
    }
}

#endif