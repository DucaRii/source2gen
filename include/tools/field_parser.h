// Copyright (C) 2023 neverlosecc
// See end of file for extended copyright information.
#pragma once

#include <cstdint>
#include <string>
#include <variant>

enum class fieldtype_t : uint8_t;

namespace field_parser {
    class field_info_t {
    public:
        std::string m_type = ""; // var type
        fieldtype_t m_field_type = static_cast<fieldtype_t>(24); // var type
        std::string m_name = ""; // var name

        // array sizes, for example {13, 37} for multi demensional array "[13][37]"
        std::vector<std::size_t> m_array_sizes = {};

        // template type, either a list of types (for ones with multiple eg. <uint16_t,int16_t>) or just one type
        struct template_info_t {
            std::string type_name = "";
            std::vector<std::variant<template_info_t, std::string>> template_types = {};
            bool is_pointer = false;

            std::string to_string() const {
                auto str = type_name;
                str += '<';
                for (auto i = 0; i < template_types.size(); i++) {
                    if (i > 0)
                        str += ',';

                    auto& template_type = template_types[i];
                    if (std::holds_alternative<field_info_t::template_info_t>(template_type)) {
                        str += std::get<field_info_t::template_info_t>(template_type).to_string();
                    } else if (std::holds_alternative<std::string>(template_type)) {
                        str += std::get<std::string>(template_type);
                    }
                }
                str += '>';

                if (is_pointer)
                    str += '*';

                return str;
            }
        } m_template_info = {};

        std::size_t m_bitfield_size = 0ull; // bitfield size, set to 0 if var isn't a bitfield
    public:
        __forceinline bool is_bitfield() const {
            return static_cast<bool>(m_bitfield_size);
        }

        __forceinline bool is_array() const {
            return !m_array_sizes.empty();
        }

        __forceinline bool is_templated() const {
            return m_type.contains('<') && m_type.contains('>');
        }
    public:
        std::size_t total_array_size() const {
            std::size_t result = 0ull;

            for (auto size : m_array_sizes) {
                if (!result) {
                    result = size;
                    continue;
                }

                result *= size;
            }

            return result;
        }
    public:
        std::string formatted_array_sizes() const {
            std::string result;

            for (std::size_t size : m_array_sizes)
                result += std::format("[{}]", size);

            return result;
        }

        std::string formatted_name() const {
            if (is_bitfield())
                return std::format("{}: {}", m_name, m_bitfield_size);

            if (is_array())
                return std::format("{}{}", m_name, formatted_array_sizes());

            return m_name;
        }
    };

    field_info_t parse(const std::string& type_name, const std::string& name, const std::vector<std::size_t>& array_sizes);
    field_info_t parse(const fieldtype_t& type_name, const std::string& name, const std::size_t& array_sizes = 1);
} // namespace field_parser

// source2gen - Source2 games SDK generator
// Copyright 2023 neverlosecc
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
