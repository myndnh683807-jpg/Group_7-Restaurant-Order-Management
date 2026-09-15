// src/controller/ReportApiController.h
// API Controller — Báo cáo doanh thu & Thống kê tổng hợp (UI-04 POS / Admin - UC15).
#pragma once

#include <crow.h>
#include <nlohmann/json.hpp>

#include <string>

namespace restaurant {
namespace controller {

/// @brief API Controller phục vụ module Báo cáo & Thống kê (UI-04 POS / Admin).
class ReportApiController {
public:
    ReportApiController() = default;
    ~ReportApiController() = default;

    static void registerRoutes(crow::SimpleApp& app);
};

} // namespace controller
} // namespace restaurant
