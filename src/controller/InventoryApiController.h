// src/controller/InventoryApiController.h
// API Controller — Quản lý kho nguyên liệu (UI-05 Admin - UC14).
#pragma once

#include "../model/Inventory.h"
#include "../repository/InventoryRepository.h"

#include <crow.h>
#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace restaurant {
namespace controller {

/// @brief API Controller phục vụ quản lý kho (UI-05 Admin).
class InventoryApiController {
public:
    InventoryApiController() = default;
    ~InventoryApiController() = default;

    static void registerRoutes(crow::SimpleApp& app);
};

} // namespace controller
} // namespace restaurant
