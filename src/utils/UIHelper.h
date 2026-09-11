#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace restaurant::utils {

enum class Align { Left, Center, Right };

struct TableColumn {
    std::string title;
    std::size_t width;
    Align align{Align::Left};
};

class UIHelper final {
public:
    UIHelper() = delete;

    // Xoa man hinh
    static void clearScreen();

    // Ve duong ke
    static void printLine(
        std::size_t width = 72,
        char fill = '-'
    );

    // Can chinh van ban
    static std::string alignText(
        const std::string& text,
        std::size_t width,
        Align align = Align::Left
    );

    static std::string centerText(
        const std::string& text,
        std::size_t width
    );

    // Ve khung
    static void drawBox(
        const std::string& text,
        std::size_t width = 72
    );

    // Ve bang nhieu cot
    static void drawTable(
        const std::vector<TableColumn>& columns,
        const std::vector<std::vector<std::string>>& rows
    );

    // Tam dung, cho nhan Enter
    static void pause(
        const std::string& message =
            "Press Enter to continue..."
    );
};

} // namespace restaurant::utils