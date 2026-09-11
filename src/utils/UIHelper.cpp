#include "UIHelper.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string_view>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace restaurant::utils {
namespace {

// Do rong hien thi cua ky tu UTF-8.
// std::string::size() dem byte, khong phai do rong tren terminal.
struct Rune {
    std::size_t bytes;
    std::size_t width;
};

Rune nextRune(std::string_view s, std::size_t pos) {
    const unsigned char first =
        static_cast<unsigned char>(s[pos]);

    char32_t cp = 0;
    std::size_t count = 0;

    if (first < 0x80) {
        cp = first;
        count = 1;
    }
    else if (first >= 0xC2 && first <= 0xDF) {
        cp = first & 0x1F;
        count = 2;
    }
    else if (first >= 0xE0 && first <= 0xEF) {
        cp = first & 0x0F;
        count = 3;
    }
    else if (first >= 0xF0 && first <= 0xF4) {
        cp = first & 0x07;
        count = 4;
    }
    else {
        return {1, 1};
    }

    if (pos + count > s.size()) {
        return {1, 1};
    }

    for (std::size_t i = 1; i < count; ++i) {
        const auto b =
            static_cast<unsigned char>(s[pos + i]);

        if ((b & 0xC0) != 0x80) {
            return {1, 1};
        }

        cp = (cp << 6) | (b & 0x3F);
    }

    if ((count == 2 && cp < 0x80) ||
        (count == 3 && cp < 0x800) ||
        (count == 4 && cp < 0x10000) ||
        (cp >= 0xD800 && cp <= 0xDFFF) ||
        cp > 0x10FFFF) {

        return {1, 1};
    }

    if (cp == 0x200D ||
        (cp >= 0x0300 && cp <= 0x036F) ||
        (cp >= 0x1AB0 && cp <= 0x1AFF) ||
        (cp >= 0x1DC0 && cp <= 0x1DFF) ||
        (cp >= 0x20D0 && cp <= 0x20FF) ||
        (cp >= 0xFE20 && cp <= 0xFE2F) ||
        (cp >= 0xFE00 && cp <= 0xFE0F)) {

        return {count, 0};
    }

    if (cp < 0x20 ||
        (cp >= 0x7F && cp < 0xA0)) {

        return {count, 0};
    }

    if ((cp >= 0x1100 && cp <= 0x115F) ||
        (cp >= 0x2E80 && cp <= 0xA4CF && cp != 0x303F) ||
        (cp >= 0xAC00 && cp <= 0xD7A3) ||
        (cp >= 0xF900 && cp <= 0xFAFF) ||
        (cp >= 0xFE10 && cp <= 0xFE19) ||
        (cp >= 0xFE30 && cp <= 0xFE6F) ||
        (cp >= 0xFF00 && cp <= 0xFF60) ||
        (cp >= 0xFFE0 && cp <= 0xFFE6) ||
        (cp >= 0x1F300 && cp <= 0x1FAFF) ||
        (cp >= 0x20000 && cp <= 0x3FFFD)) {

        return {count, 2};
    }

    return {count, 1};
}

std::size_t displayWidth(std::string_view text) {
    std::size_t width = 0;

    for (std::size_t i = 0; i < text.size();) {
        const Rune r = nextRune(text, i);
        width += r.width;
        i += r.bytes;
    }

    return width;
}

std::string fitText(
    std::string_view text,
    std::size_t width
) {
    if (width == 0) {
        return {};
    }

    if (displayWidth(text) <= width) {
        return std::string(text);
    }

    // Khong cat giua mot ky tu UTF-8
    const std::string suffix =
        width > 3 ? "..." : "";

    const std::size_t limit =
        width - suffix.size();

    std::string result;
    std::size_t used = 0;

    for (std::size_t i = 0; i < text.size();) {
        const Rune r = nextRune(text, i);

        if (used + r.width > limit) {
            break;
        }

        result.append(text.substr(i, r.bytes));
        used += r.width;
        i += r.bytes;
    }

    result += suffix;
    return result;
}

std::string border(
    const std::vector<TableColumn>& columns
) {
    std::string result = "+";

    for (const auto& column : columns) {
        result.append(column.width + 2, '-');
        result += '+';
    }

    return result;
}

void printRow(
    const std::vector<TableColumn>& columns,
    const std::vector<std::string>& cells
) {
    std::cout << '|';

    for (std::size_t i = 0; i < columns.size(); ++i) {
        const std::string value =
            i < cells.size() ? cells[i] : "";

        std::cout << ' '
                  << UIHelper::alignText(
                         value,
                         columns[i].width,
                         columns[i].align
                     )
                  << " |";
    }

    std::cout << '\n';
}

} // namespace


// ========================================
// XOA MAN HINH
// ========================================

void UIHelper::clearScreen() {
#ifdef _WIN32
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;

    if (out != INVALID_HANDLE_VALUE &&
        out != nullptr &&
        GetConsoleMode(out, &mode)) {

        SetConsoleOutputCP(CP_UTF8);

        if (SetConsoleMode(
                out,
                mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING
            )) {

            std::cout << "\033[2J\033[H"
                      << std::flush;
            return;
        }

        CONSOLE_SCREEN_BUFFER_INFO info;

        if (GetConsoleScreenBufferInfo(out, &info)) {
            const DWORD cells =
                static_cast<DWORD>(info.dwSize.X) *
                info.dwSize.Y;

            DWORD written = 0;
            const COORD home{0, 0};

            FillConsoleOutputCharacterA(
                out, ' ', cells, home, &written
            );

            FillConsoleOutputAttribute(
                out,
                info.wAttributes,
                cells,
                home,
                &written
            );

            SetConsoleCursorPosition(out, home);
            return;
        }
    }
#endif

    std::cout << "\033[2J\033[H" << std::flush;
}


// ========================================
// VE DUONG KE
// ========================================

void UIHelper::printLine(
    std::size_t width,
    char fill
) {
    if (width > 1000) {
        throw std::invalid_argument(
            "Line width must be <= 1000"
        );
    }

    std::cout << std::string(width, fill) << '\n';
}


// ========================================
// CAN CHINH VAN BAN
// ========================================

std::string UIHelper::alignText(
    const std::string& text,
    std::size_t width,
    Align align
) {
    if (width > 1000) {
        throw std::invalid_argument(
            "Text width must be <= 1000"
        );
    }

    std::string fitted = fitText(text, width);

    const std::size_t used = displayWidth(fitted);
    const std::size_t padding = width - used;

    std::size_t left = 0;

    if (align == Align::Right) {
        left = padding;
    }
    else if (align == Align::Center) {
        left = padding / 2;
    }

    return std::string(left, ' ') +
           fitted +
           std::string(padding - left, ' ');
}

std::string UIHelper::centerText(
    const std::string& text,
    std::size_t width
) {
    return alignText(text, width, Align::Center);
}


// ========================================
// VE KHUNG ASCII
// ========================================

void UIHelper::drawBox(
    const std::string& text,
    std::size_t width
) {
    if (width < 2 || width > 1000) {
        throw std::invalid_argument(
            "Box width must be between 2 and 1000"
        );
    }

    const std::string edge =
        "+" + std::string(width, '-') + "+";

    std::cout << edge << '\n';

    std::size_t start = 0;

    do {
        const auto end = text.find('\n', start);

        const std::string line = text.substr(
            start,
            end == std::string::npos
                ? end
                : end - start
        );

        std::cout << "|"
                  << centerText(line, width)
                  << "|\n";

        if (end == std::string::npos) {
            break;
        }

        start = end + 1;

    } while (true);

    std::cout << edge << '\n';
}


// ========================================
// VE BANG ASCII
// ========================================

void UIHelper::drawTable(
    const std::vector<TableColumn>& columns,
    const std::vector<std::vector<std::string>>& rows
) {
    if (columns.empty() || columns.size() > 64) {
        throw std::invalid_argument(
            "Table needs 1 to 64 columns"
        );
    }

    for (const auto& column : columns) {
        if (column.width == 0 || column.width > 1000) {
            throw std::invalid_argument(
                "Column width must be between 1 and 1000"
            );
        }
    }

    const std::string edge = border(columns);

    std::cout << edge << '\n';

    std::vector<std::string> headers;

    for (const auto& c : columns) {
        headers.push_back(c.title);
    }

    printRow(columns, headers);
    std::cout << edge << '\n';

    for (const auto& row : rows) {
        printRow(columns, row);
        std::cout << edge << '\n';
    }
}


// ========================================
// TAM DUNG
// ========================================

void UIHelper::pause(
    const std::string& message
) {
    std::cout << message << std::flush;

    std::string ignored;

    if (!std::getline(std::cin, ignored)) {
        throw std::runtime_error(
            "Input stream closed"
        );
    }
}

} // namespace restaurant::utils