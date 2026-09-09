// src/controller/MenuController.cpp
// Triển khai logic nghiệp vụ cho module Menu (Category).
#include "MenuController.h"

#include <iostream>
#include <stdexcept>

namespace restaurant {
namespace controller {

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
MenuController::MenuController()
    : repository_() {}

// ---------------------------------------------------------------------------
// Thêm danh mục
// ---------------------------------------------------------------------------
int MenuController::createCategory(const std::string& name,
                                   const std::optional<std::string>& description) {
    // --- Validate đầu vào ---
    if (name.empty()) {
        std::cerr << "[MenuController] Loi: Ten danh muc khong duoc de trong.\n";
        return -1;
    }

    if (name.size() > 50) {
        std::cerr << "[MenuController] Loi: Ten danh muc vuot qua 50 ky tu.\n";
        return -1;
    }

    try {
        // Tạo model từ dữ liệu đã validate.
        model::Category category(name, description);

        // Gọi Repository thực thi INSERT.
        int newId = repository_.addCategory(category);
        return newId;

    } catch (const std::exception& e) {
        std::cerr << "[MenuController] Exception khi tao danh muc: " << e.what() << "\n";
        return -1;
    }
}

// ---------------------------------------------------------------------------
// Lấy toàn bộ danh mục
// ---------------------------------------------------------------------------
std::vector<model::Category> MenuController::listAllCategories() {
    try {
        return repository_.getAllCategories();
    } catch (const std::exception& e) {
        std::cerr << "[MenuController] Exception khi lay danh sach: " << e.what() << "\n";
        return {};  // Trả vector rỗng khi có lỗi.
    }
}

// ---------------------------------------------------------------------------
// Tìm danh mục theo ID
// ---------------------------------------------------------------------------
std::optional<model::Category> MenuController::findCategoryById(int categoryId) {
    if (categoryId <= 0) {
        std::cerr << "[MenuController] Loi: ID khong hop le (" << categoryId << ").\n";
        return std::nullopt;
    }

    try {
        return repository_.getCategoryById(categoryId);
    } catch (const std::exception& e) {
        std::cerr << "[MenuController] Exception khi tim danh muc: " << e.what() << "\n";
        return std::nullopt;
    }
}

} // namespace controller
} // namespace restaurant
