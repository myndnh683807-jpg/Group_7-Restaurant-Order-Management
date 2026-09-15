#include "Validator.h"

#include <charconv>
#include <cctype>
#include <cmath>
#include <iostream>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string_view>

namespace restaurant::utils {
namespace {

// ========================================
// DOC MOT DONG NHAP
// ========================================

std::string readLine(const std::string& prompt) {
    std::cout << prompt << std::flush;

    std::string value;

    if (!std::getline(std::cin, value)) {
        throw std::runtime_error(
            "Input stream closed"
        );
    }

    return value;
}


// ========================================
// DEM KY TU UTF-8
// ========================================

bool countUtf8(
    std::string_view text,
    std::size_t& count
) {
    count = 0;

    for (std::size_t i = 0; i < text.size();) {
        const unsigned char first =
            static_cast<unsigned char>(text[i]);

        char32_t cp = 0;
        std::size_t bytes = 0;

        if (first < 0x80) {
            cp = first;
            bytes = 1;
        }
        else if (first >= 0xC2 && first <= 0xDF) {
            cp = first & 0x1F;
            bytes = 2;
        }
        else if (first >= 0xE0 && first <= 0xEF) {
            cp = first & 0x0F;
            bytes = 3;
        }
        else if (first >= 0xF0 && first <= 0xF4) {
            cp = first & 0x07;
            bytes = 4;
        }
        else {
            return false;
        }

        if (i + bytes > text.size()) {
            return false;
        }

        for (std::size_t j = 1; j < bytes; ++j) {
            const auto b =
                static_cast<unsigned char>(text[i + j]);

            if ((b & 0xC0) != 0x80) {
                return false;
            }

            cp = (cp << 6) | (b & 0x3F);
        }

        if ((bytes == 2 && cp < 0x80) ||
            (bytes == 3 && cp < 0x800) ||
            (bytes == 4 && cp < 0x10000) ||
            (cp >= 0xD800 && cp <= 0xDFFF) ||
            cp > 0x10FFFF) {

            return false;
        }

        ++count;
        i += bytes;
    }

    return true;
}


// ========================================
// KIEM TRA NAM NHUAN
// ========================================

bool leapYear(int year) {
    return year % 400 == 0 ||
           (year % 4 == 0 && year % 100 != 0);
}

} // namespace


// ========================================
// XOA KHOANG TRANG DAU CUOI
// ========================================

std::string Validator::trim(
    const std::string& text
) {
    // Chi xoa ASCII whitespace,
    // khong lam hong ky tu tieng Viet UTF-8.
    const auto first =
        text.find_first_not_of(" \t\r\n\f\v");

    if (first == std::string::npos) {
        return {};
    }

    const auto last =
        text.find_last_not_of(" \t\r\n\f\v");

    return text.substr(
        first,
        last - first + 1
    );
}


// ========================================
// KIEM TRA SO NGUYEN
// ========================================

bool Validator::isInteger(
    const std::string& text,
    int min,
    int max
) {
    if (min > max) {
        return false;
    }

    const std::string value = trim(text);

    if (value.empty()) {
        return false;
    }

    int number = 0;

    const auto result = std::from_chars(
        value.data(),
        value.data() + value.size(),
        number,
        10
    );

    return result.ec == std::errc{} &&
           result.ptr == value.data() + value.size() &&
           number >= min &&
           number <= max;
}


// ========================================
// KIEM TRA SO THUC
// ========================================

bool Validator::isDecimal(
    const std::string& text,
    double min,
    double max
) {
    if (!std::isfinite(min) ||
        !std::isfinite(max) ||
        min > max) {

        return false;
    }

    const std::string value = trim(text);

    if (value.empty()) {
        return false;
    }

    std::istringstream stream(value);
    stream.imbue(std::locale::classic());

    double number = 0.0;

    stream >> std::noskipws >> number;

    if (stream.fail() || !std::isfinite(number)) {
        return false;
    }

    char extra;

    if (stream >> extra) {
        return false;
    }

    return stream.eof() &&
           number >= min &&
           number <= max;
}


// ========================================
// KIEM TRA DO DAI CHUOI
// ========================================

bool Validator::isValidLength(
    const std::string& text,
    std::size_t min,
    std::size_t max
) {
    if (min > max) {
        return false;
    }

    const std::string value = trim(text);

    if (value.empty()) {
        return false;
    }

    std::size_t count = 0;

    return countUtf8(value, count) &&
           count >= min &&
           count <= max;
}


// ========================================
// KIEM TRA NGAY THANG
// ========================================

bool Validator::isValidDate(
    const std::string& text
) {
    if (text.size() != 10 ||
        text[4] != '-' ||
        text[7] != '-') {

        return false;
    }

    for (std::size_t i = 0; i < text.size(); ++i) {
        if (i == 4 || i == 7) {
            continue;
        }

        if (text[i] < '0' || text[i] > '9') {
            return false;
        }
    }

    const int year =
        std::stoi(text.substr(0, 4));

    const int month =
        std::stoi(text.substr(5, 2));

    const int day =
        std::stoi(text.substr(8, 2));

    if (year < 1 ||
        month < 1 ||
        month > 12 ||
        day < 1) {

        return false;
    }

    static constexpr int days[] = {
        0, 31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    int maxDay = days[month];

    if (month == 2 && leapYear(year)) {
        maxDay = 29;
    }

    return day <= maxDay;
}


// ========================================
// NHAP SO NGUYEN
// ========================================

int Validator::readInt(
    const std::string& prompt,
    int min,
    int max
) {
    if (min > max) {
        throw std::invalid_argument(
            "Invalid integer range"
        );
    }

    while (true) {
        const std::string value =
            trim(readLine(prompt));

        if (isInteger(value, min, max)) {
            int number = 0;

            std::from_chars(
                value.data(),
                value.data() + value.size(),
                number,
                10
            );

            return number;
        }

        std::cout
            << "Invalid integer. Allowed range: "
            << min << ".." << max << "\n";
    }
}


// ========================================
// NHAP SO THUC
// ========================================

double Validator::readDouble(
    const std::string& prompt,
    double min,
    double max
) {
    if (!std::isfinite(min) ||
        !std::isfinite(max) ||
        min > max) {

        throw std::invalid_argument(
            "Invalid decimal range"
        );
    }

    while (true) {
        const std::string value =
            trim(readLine(prompt));

        if (isDecimal(value, min, max)) {
            std::istringstream stream(value);
            stream.imbue(std::locale::classic());

            double number = 0.0;
            stream >> number;

            return number;
        }

        std::cout
            << "Invalid number. Allowed range: "
            << min << ".." << max << "\n";
    }
}


// ========================================
// NHAP CHUOI
// ========================================

std::string Validator::readString(
    const std::string& prompt,
    std::size_t min,
    std::size_t max
) {
    if (min > max) {
        throw std::invalid_argument(
            "Invalid string length range"
        );
    }

    while (true) {
        const std::string value =
            trim(readLine(prompt));

        if (isValidLength(value, min, max)) {
            return value;
        }

        std::cout
            << "Invalid text. Required length: "
            << min << ".." << max
            << " characters.\n";
    }
}


// ========================================
// NHAP NGAY THANG
// ========================================

std::string Validator::readDate(
    const std::string& prompt
) {
    while (true) {
        const std::string value =
            trim(readLine(prompt));

        if (isValidDate(value)) {
            return value;
        }

        std::cout
            << "Invalid date. Use yyyy-mm-dd "
            << "(example: 2026-09-10).\n";
    }
}


// ========================================
// NHAP LUA CHON
// ========================================

std::string Validator::readChoice(
    const std::string& prompt,
    const std::vector<std::string>& allowed
) {
    if (allowed.empty()) {
        throw std::invalid_argument(
            "Allowed choices cannot be empty"
        );
    }

    while (true) {
        const std::string value =
            trim(readLine(prompt));

        for (const auto& choice : allowed) {
            if (value == choice) {
                return value;
            }
        }

        std::cout << "Invalid choice. Allowed:";

        for (const auto& choice : allowed) {
            std::cout << " [" << choice << "]";
        }

        std::cout << '\n';
    }
}

} // namespace restaurant::utils