// src/controller/MenuController.h
// Controller layer — điều phối logic giữa View và Repository.
#pragma once

#include "../model/Category.h"
#include "../repository/MenuRepository.h"

#include <optional>
#include <string>
#include <vector>

namespace restaurant {
namespace controller {

/// @brief Controller xử lý nghiệp vụ cho module Menu (Category).
///
/// Lớp này đóng vai trò trung gian:
///   View ←→ MenuController ←→ MenuRepository ←→ Database
///
/// Controller chịu trách nhiệm:
///   - Validate dữ liệu đầu vào từ View.
///   - Gọi Repository thực thi truy vấn.
///   - Trả kết quả có cấu trúc cho View.
class MenuController {
public:
    MenuController();
    ~MenuController() = default;

    /// Thêm danh mục mới.
    /// @param name        Tên danh mục (bắt buộc, không rỗng).
    /// @param description Mô tả (tuỳ chọn).
    /// @return ID của danh mục vừa tạo, hoặc -1 nếu thất bại.
    int createCategory(const std::string& name,
                       const std::optional<std::string>& description = std::nullopt);

    /// Lấy toàn bộ danh mục.
    std::vector<model::Category> listAllCategories();

    /// Tìm danh mục theo ID.
    /// @return Category nếu tồn tại, std::nullopt nếu không.
    std::optional<model::Category> findCategoryById(int categoryId);

private:
    repository::MenuRepository repository_;
};

} // namespace controller
} // namespace restaurant
