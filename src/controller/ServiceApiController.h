// src/controller/ServiceApiController.h
// API Controller — HTTP endpoints cho dịch vụ phục vụ (yêu cầu thanh toán,
// hỗ trợ khách hàng, phục vụ món ăn).
#pragma once

#include "../repository/TableRepository.h"

#include <crow.h>
#include <nlohmann/json.hpp>

#include <mutex>
#include <string>
#include <unordered_set>
#include <vector>

namespace restaurant {
namespace controller {

/// @brief API Controller cho module Service (UC03, UC05, UC06).
///
/// Quản lý trạng thái in-memory:
///   - Danh sách yêu cầu thanh toán (payment requests)
///   - Danh sách yêu cầu hỗ trợ (assistance requests)
///
/// Routes:
///   POST   /api/service/payment/request     — Yêu cầu thanh toán
///   POST   /api/service/payment/cancel      — Hủy yêu cầu thanh toán
///   GET    /api/service/payment/requests     — Danh sách yêu cầu thanh toán
///   POST   /api/service/assistance/request   — Yêu cầu hỗ trợ
///   POST   /api/service/assistance/resolve   — Giải quyết yêu cầu hỗ trợ
///   GET    /api/service/assistance/requests   — Danh sách yêu cầu hỗ trợ
class ServiceApiController {
public:
    ServiceApiController() = default;
    ~ServiceApiController() = default;

    static void registerRoutes(crow::SimpleApp& app);

private:
    // Thread-safe in-memory state (shared across all routes)
    static std::unordered_set<int> paymentRequests_;
    static std::unordered_set<int> assistanceRequests_;
    static std::mutex mutex_;

    static nlohmann::json errorJson(const std::string& message, int code = 400);
};

} // namespace controller
} // namespace restaurant
