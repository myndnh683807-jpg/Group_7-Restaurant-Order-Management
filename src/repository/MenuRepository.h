// src/repository/MenuRepository.h
// Repository layer — truy vấn CSDL cho các thực thể liên quan đến Menu.
#pragma once

#include "../model/Category.h"

#include <optional>
#include <vector>

namespace restaurant {
namespace repository {

/// @brief Repository thao tác CRUD lên bảng Category.
///
/// Mọi truy vấn đều dùng prepared statements (bind) để chống SQL Injection.
class MenuRepository {
public:
    MenuRepository()  = default;
    ~MenuRepository() = default;

    /// Thêm một danh mục mới vào bảng Category.
    /// @param category Đối tượng Category (id sẽ bị bỏ qua, do DB auto-increment).
    /// @return ID của bản ghi vừa được tạo.
    /// @throws mysqlx::Error nếu truy vấn thất bại.
    int addCategory(const model::Category& category);

    /// Lấy toàn bộ danh mục từ bảng Category.
    /// @return Vector chứa tất cả Category, rỗng nếu bảng chưa có dữ liệu.
    std::vector<model::Category> getAllCategories();

    /// Tìm danh mục theo ID.
    /// @return Category nếu tìm thấy, std::nullopt nếu không.
    std::optional<model::Category> getCategoryById(int categoryId);
};

} // namespace repository
} // namespace restaurant