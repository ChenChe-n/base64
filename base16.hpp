#ifndef BASE16_HPP
#define BASE16_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

namespace base16
{
    /**
     * @brief 编码base16
     * @param data 为待编码的数据
     * @param table 为编码表
     * @return std::vector<uint8_t>
     */
    std::u32string encode(const uint8_t *data, const size_t num, const std::u32string &table = U"0123456789ABCDEF")
    {
        if (data == nullptr || num == 0)
            return {};
        if (table.size() != 16)
            return {};
        std::unordered_set<char32_t> table_set;
        table_set.reserve(16);
        for (const auto i : table)
            table_set.insert(i);
        if (table_set.size() != 16)
            return {};

        std::u32string result;
        result.reserve(num * 2);
        for (size_t i = 0; i < num; i++)
        {
            result.push_back(table[(data[i] >> 4) & 0b1111]);
            result.push_back(table[data[i] & 0b1111]);
        }
        return result;
    }
    /**
     * @brief 解码base16
     * @param data 为待解码的数据
     * @param table 为解码表
     * @return std::vector<uint8_t>
     */
    std::vector<uint8_t> decode(const std::u32string &data, const std::u32string &table = U"0123456789ABCDEF")
    {
        if (data.empty())
            return {};
        if (table.size() != 16)
            return {};
        if ((data.size() & 0b1) == 1)
            return {};
        std::unordered_map<char32_t, uint8_t> table_map;
        table_map.reserve(16);
        for (size_t i = 0; i < table.size(); ++i)
            table_map.insert({table[i], i});
        if (table_map.size() != 16)
            return {};

        std::vector<uint8_t> result;
        result.reserve(data.size() / 2);
        for (size_t i = 0; i < data.size(); i += 2)
        {
            auto it1 = table_map.find(data[i]);
            auto it2 = table_map.find(data[i + 1]);
            if (it1 == table_map.end() || it2 == table_map.end())
                return {};
            result.push_back((it1->second << 4) | it2->second);
        }
        return result;
    }
}

#endif