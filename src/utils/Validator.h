#pragma once

#include <cstddef>
#include <limits>
#include <string>
#include <vector>

namespace restaurant::utils {

class Validator final {
public:
    Validator() = delete;

    // Xoa khoang trang dau va cuoi
    static std::string trim(
        const std::string& text
    );

    // Kiem tra so nguyen
    static bool isInteger(
        const std::string& text,
        int min = std::numeric_limits<int>::min(),
        int max = std::numeric_limits<int>::max()
    );

    // Kiem tra so thuc
    static bool isDecimal(
        const std::string& text,
        double min = -std::numeric_limits<double>::max(),
        double max = std::numeric_limits<double>::max()
    );

    // Kiem tra do dai chuoi UTF-8
    static bool isValidLength(
        const std::string& text,
        std::size_t min,
        std::size_t max
    );

    // Kiem tra ngay yyyy-mm-dd
    static bool isValidDate(
        const std::string& text
    );

    // Cac ham nhap se yeu cau nhap lai neu sai
    static int readInt(
        const std::string& prompt,
        int min,
        int max
    );

    static double readDouble(
        const std::string& prompt,
        double min,
        double max
    );

    static std::string readString(
        const std::string& prompt,
        std::size_t min,
        std::size_t max
    );

    static std::string readDate(
        const std::string& prompt
    );

    static std::string readChoice(
        const std::string& prompt,
        const std::vector<std::string>& allowed
    );
};

} // namespace restaurant::utils