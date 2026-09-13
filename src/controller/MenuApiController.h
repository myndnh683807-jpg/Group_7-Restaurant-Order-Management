// src/controller/MenuApiController.h
// API Controller — nhận HTTP Request, trả về JSON Response cho Menu & MenuItem.
#pragma once

#include "../model/Category.h"
#include "../model/MenuItem.h"
#include "../repository/MenuRepository.h"

#include <crow.h>
#include <nlohmann/json.hpp>

#include <optional>
#include <string>
#include <vector>

namespace restaurant {
namespace controller {

/// @brief API Controller xử lý HTTP request cho module Menu (Category & MenuItem).
///
/// Kiến trúc:
///   Frontend (fetch) → HTTP → MenuApiController → Repository/DB
class MenuApiController {
public:
    MenuApiController() = default;
    ~MenuApiController() = default;

    /// Đăng ký tất cả routes của module Menu vào Crow app.
    static void registerRoutes(crow::SimpleApp& app);

private:
    static nlohmann::json categoryToJson(const model::Category& category);
    static nlohmann::json categoriesToJson(const std::vector<model::Category>& categories);
    static nlohmann::json menuItemToJson(const model::MenuItem& item);
    static nlohmann::json menuItemsToJson(const std::vector<model::MenuItem>& items);

    static std::vector<model::MenuItem> fetchAllMenuItems(std::optional<int> categoryId = std::nullopt);
    static std::optional<model::MenuItem> fetchMenuItemById(int itemId);

    static nlohmann::json errorJson(const std::string& message, int code = 400);
};

} // namespace controller
} // namespace restaurant
