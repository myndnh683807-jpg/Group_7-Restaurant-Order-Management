// src/controller/OrderApiController.h
// API Controller — Xử lý tạo và quản lý đơn hàng (Order & OrderItem).
#pragma once

#include "../model/Order.h"
#include "../model/OrderItem.h"
#include "../repository/OrderRepository.h"
#include "../repository/TableRepository.h"

#include <crow.h>
#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace restaurant {
namespace controller {

/// @brief API Controller xử lý HTTP request cho module Order (UI-01 Đặt món).
class OrderApiController {
public:
    OrderApiController() = default;
    ~OrderApiController() = default;

    static void registerRoutes(crow::SimpleApp& app);

private:
    static nlohmann::json orderWithItemsToJson(
        const model::Order& order, 
        const std::vector<model::OrderItem>& items
    );
};

} // namespace controller
} // namespace restaurant
