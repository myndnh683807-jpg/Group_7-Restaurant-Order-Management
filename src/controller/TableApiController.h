// src/controller/TableApiController.h
// API Controller — HTTP endpoints cho quản lý bàn (Table).
#pragma once

#include "../model/Table.h"
#include "../repository/TableRepository.h"

#include <crow.h>
#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace restaurant {
namespace controller {

/// @brief API Controller xử lý HTTP request cho module Table.
///
/// Routes:
///   GET    /api/tables           — Lấy toàn bộ bàn
///   GET    /api/tables/available — Lấy bàn trống
///   GET    /api/tables/<id>      — Tìm bàn theo ID
///   POST   /api/tables           — Thêm bàn mới
///   PUT    /api/tables/<id>      — Cập nhật thông tin bàn
///   PATCH  /api/tables/<id>/status — Đổi trạng thái bàn
///   DELETE /api/tables/<id>      — Xóa bàn
class TableApiController {
public:
    TableApiController() = default;
    ~TableApiController() = default;

    static void registerRoutes(crow::SimpleApp& app);

private:
    static nlohmann::json tableToJson(const model::Table& table);
    static nlohmann::json tablesToJson(const std::vector<model::Table>& tables);
    static nlohmann::json errorJson(const std::string& message, int code = 400);
};

} // namespace controller
} // namespace restaurant
