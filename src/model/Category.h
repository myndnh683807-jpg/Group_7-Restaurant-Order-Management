// src/model/Category.h
// Model ánh xạ 1-1 với bảng Category trong MySQL.
#pragma once

#include <optional>
#include <string>

namespace restaurant {
namespace model {

/// @brief Model class cho bảng Category.
///
/// Bảng SQL:
///   CREATE TABLE Category (
///       category_id   INT AUTO_INCREMENT PRIMARY KEY,
///       category_name VARCHAR(50) NOT NULL,
///       description   TEXT
///   );
class Category {
public:
    /// Constructor mặc định (dùng khi đọc từ DB).
    Category() = default;

    /// Constructor tạo mới (chưa có id, sẽ do DB auto-generate).
    Category(std::string name, std::optional<std::string> description = std::nullopt);

    /// Constructor đầy đủ (đọc từ DB, đã có id).
    Category(int id, std::string name, std::optional<std::string> description = std::nullopt);

    // --- Getters ---
    int                                 getId()          const noexcept;
    const std::string&                  getName()        const noexcept;
    const std::optional<std::string>&   getDescription() const noexcept;

    // --- Setters ---
    void setId(int id);
    void setName(const std::string& name);
    void setDescription(const std::optional<std::string>& description);

private:
    int                          id_{0};
    std::string                  name_;
    std::optional<std::string>   description_;
};

} // namespace model
} // namespace restaurant
