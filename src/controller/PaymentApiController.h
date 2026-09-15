// src/controller/PaymentApiController.h
// API Controller — Xử lý nghiệp vụ thanh toán, xuất hóa đơn (POS - UC07, UC08, UC09).
#pragma once

#include "../model/Invoice.h"
#include "../repository/InvoiceRepository.h"
#include "../repository/OrderRepository.h"
#include "../repository/TableRepository.h"

#include <crow.h>
#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace restaurant {
namespace controller {

/// @brief API Controller phục vụ module Thu ngân / POS (UI-04 Payment & Checkout).
class PaymentApiController {
public:
    PaymentApiController() = default;
    ~PaymentApiController() = default;

    static void registerRoutes(crow::SimpleApp& app);
};

} // namespace controller
} // namespace restaurant
