// src/controller/KitchenApiController.h
// API Controller — Xử lý nghiệp vụ Màn hình Bếp (KDS - Kitchen Display System - UC10, UC11).
#pragma once

#include "../model/OrderItem.h"
#include "../repository/OrderRepository.h"

#include <crow.h>
#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace restaurant {
namespace controller {

/// @brief API Controller phục vụ module Bếp (UI-03 Kitchen).
class KitchenApiController {
public:
    KitchenApiController() = default;
    ~KitchenApiController() = default;

    static void registerRoutes(crow::SimpleApp& app);
};

} // namespace controller
} // namespace restaurant
