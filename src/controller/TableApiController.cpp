// src/controller/TableApiController.cpp
// Triển khai API endpoints cho module Table (Quản lý bàn).
// Routes:
//   GET    /api/tables            — Lấy toàn bộ bàn
//   GET    /api/tables/available  — Lấy bàn trống
//   GET    /api/tables/<id>       — Tìm bàn theo ID
//   POST   /api/tables            — Thêm bàn mới
//   PUT    /api/tables/<id>       — Cập nhật thông tin bàn
//   PATCH  /api/tables/<id>/status— Đổi trạng thái bàn
//   DELETE /api/tables/<id>       — Xóa bàn
#include "TableApiController.h"

#include <iostream>
#include <stdexcept>

namespace restaurant {
namespace controller {

// ---------------------------------------------------------------------------
// Helper — CORS
// ---------------------------------------------------------------------------
static void addCorsHeaders(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, PATCH, DELETE, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

// ---------------------------------------------------------------------------
// Serialize
// ---------------------------------------------------------------------------
nlohmann::json TableApiController::tableToJson(const model::Table& table) {
    return {
        {"table_id",     table.getId()},
        {"table_number", table.getTableNumber()},
        {"capacity",     table.getCapacity()},
        {"status",       table.getStatus()}
    };
}

nlohmann::json TableApiController::tablesToJson(const std::vector<model::Table>& tables) {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& t : tables) {
        arr.push_back(tableToJson(t));
    }
    return arr;
}

nlohmann::json TableApiController::errorJson(const std::string& message, int code) {
    return {{"success", false}, {"error", message}, {"code", code}};
}

// ---------------------------------------------------------------------------
// Route Registration
// ---------------------------------------------------------------------------
void TableApiController::registerRoutes(crow::SimpleApp& app) {

    // ----- GET /api/tables -----
    CROW_ROUTE(app, "/api/tables")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/) {
        crow::response res;
        addCorsHeaders(res);
        try {
            repository::TableRepository repo;
            auto tables = repo.getAll();
            nlohmann::json body;
            body["success"] = true;
            body["data"]    = tablesToJson(tables);
            body["count"]   = tables.size();
            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.body = body.dump(2);
        } catch (const std::exception& e) {
            std::cerr << "[TableApiController] GET /api/tables error: " << e.what() << "\n";
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("Internal server error", 500).dump();
        }
        return res;
    });

    // ----- GET /api/tables/available -----
    CROW_ROUTE(app, "/api/tables/available")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/) {
        crow::response res;
        addCorsHeaders(res);
        try {
            repository::TableRepository repo;
            auto allTables = repo.getAll();
            std::vector<model::Table> available;
            for (const auto& t : allTables) {
                if (t.getStatus() == "Available") {
                    available.push_back(t);
                }
            }
            nlohmann::json body;
            body["success"] = true;
            body["data"]    = tablesToJson(available);
            body["count"]   = available.size();
            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.body = body.dump(2);
        } catch (const std::exception& e) {
            std::cerr << "[TableApiController] GET /api/tables/available error: " << e.what() << "\n";
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("Internal server error", 500).dump();
        }
        return res;
    });

    // ----- GET /api/tables/<id> -----
    CROW_ROUTE(app, "/api/tables/<int>")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/, int id) {
        crow::response res;
        addCorsHeaders(res);
        if (id <= 0) {
            res.code = 400;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("ID khong hop le.", 400).dump();
            return res;
        }
        try {
            repository::TableRepository repo;
            auto table = repo.getById(id);
            if (table.has_value()) {
                nlohmann::json body;
                body["success"] = true;
                body["data"]    = tableToJson(table.value());
                res.code = 200;
                res.set_header("Content-Type", "application/json");
                res.body = body.dump(2);
            } else {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Khong tim thay ban voi ID = " + std::to_string(id), 404).dump();
            }
        } catch (const std::exception& e) {
            std::cerr << "[TableApiController] GET /api/tables/" << id << " error: " << e.what() << "\n";
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("Internal server error", 500).dump();
        }
        return res;
    });

    // ----- POST /api/tables -----
    CROW_ROUTE(app, "/api/tables")
        .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto j = nlohmann::json::parse(req.body);
            if (!j.contains("table_number") || !j["table_number"].is_string() ||
                j["table_number"].get<std::string>().empty()) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Truong 'table_number' la bat buoc.", 400).dump();
                return res;
            }
            std::string tableNumber = j["table_number"].get<std::string>();
            int capacity = j.value("capacity", 4);
            if (capacity <= 0) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Capacity phai la so duong.", 400).dump();
                return res;
            }

            model::Table table(tableNumber, capacity, "Available");
            repository::TableRepository repo;
            model::Table saved = repo.add(table);

            nlohmann::json body;
            body["success"] = true;
            body["message"] = "Tao ban thanh cong.";
            body["data"]    = tableToJson(saved);
            res.code = 201;
            res.set_header("Content-Type", "application/json");
            res.body = body.dump(2);
        } catch (const nlohmann::json::parse_error& e) {
            res.code = 400;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("JSON khong hop le: " + std::string(e.what()), 400).dump();
        } catch (const std::exception& e) {
            std::cerr << "[TableApiController] POST /api/tables error: " << e.what() << "\n";
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("Internal server error", 500).dump();
        }
        return res;
    });

    // ----- PUT /api/tables/<id> -----
    CROW_ROUTE(app, "/api/tables/<int>")
        .methods(crow::HTTPMethod::PUT)
    ([](const crow::request& req, int id) {
        crow::response res;
        addCorsHeaders(res);
        if (id <= 0) {
            res.code = 400;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("ID khong hop le.", 400).dump();
            return res;
        }
        try {
            auto j = nlohmann::json::parse(req.body);
            repository::TableRepository repo;
            auto existing = repo.getById(id);
            if (!existing.has_value()) {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Khong tim thay ban.", 404).dump();
                return res;
            }
            std::string tableNumber = j.value("table_number", existing->getTableNumber());
            int capacity = j.value("capacity", existing->getCapacity());
            existing->setTableNumber(tableNumber);
            existing->setCapacity(capacity);

            bool ok = repo.update(*existing);
            nlohmann::json body;
            body["success"] = ok;
            body["message"] = ok ? "Cap nhat ban thanh cong." : "Cap nhat that bai.";
            body["data"]    = tableToJson(*existing);
            res.code = ok ? 200 : 500;
            res.set_header("Content-Type", "application/json");
            res.body = body.dump(2);
        } catch (const nlohmann::json::parse_error& e) {
            res.code = 400;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("JSON khong hop le.", 400).dump();
        } catch (const std::exception& e) {
            std::cerr << "[TableApiController] PUT /api/tables/" << id << " error: " << e.what() << "\n";
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("Internal server error", 500).dump();
        }
        return res;
    });

    // ----- PATCH /api/tables/<id>/status -----
    CROW_ROUTE(app, "/api/tables/<int>/status")
        .methods(crow::HTTPMethod::PATCH)
    ([](const crow::request& req, int id) {
        crow::response res;
        addCorsHeaders(res);
        if (id <= 0) {
            res.code = 400;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("ID khong hop le.", 400).dump();
            return res;
        }
        try {
            auto j = nlohmann::json::parse(req.body);
            if (!j.contains("status") || !j["status"].is_string()) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Truong 'status' la bat buoc.", 400).dump();
                return res;
            }
            std::string status = j["status"].get<std::string>();
            repository::TableRepository repo;
            bool ok = repo.updateStatus(id, status);
            nlohmann::json body;
            body["success"] = ok;
            body["message"] = ok ? "Cap nhat trang thai thanh cong." : "Cap nhat that bai.";
            res.code = ok ? 200 : 404;
            res.set_header("Content-Type", "application/json");
            res.body = body.dump(2);
        } catch (const std::invalid_argument& e) {
            res.code = 400;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson(e.what(), 400).dump();
        } catch (const nlohmann::json::parse_error& e) {
            res.code = 400;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("JSON khong hop le.", 400).dump();
        } catch (const std::exception& e) {
            std::cerr << "[TableApiController] PATCH status error: " << e.what() << "\n";
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("Internal server error", 500).dump();
        }
        return res;
    });

    // ----- DELETE /api/tables/<id> -----
    CROW_ROUTE(app, "/api/tables/<int>")
        .methods(crow::HTTPMethod::Delete)
    ([](const crow::request& /*req*/, int id) {
        crow::response res;
        addCorsHeaders(res);
        if (id <= 0) {
            res.code = 400;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("ID khong hop le.", 400).dump();
            return res;
        }
        try {
            repository::TableRepository repo;
            auto existing = repo.getById(id);
            if (!existing.has_value()) {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Khong tim thay ban.", 404).dump();
                return res;
            }
            if (existing->getStatus() != "Available") {
                res.code = 409;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Chi co the xoa ban o trang thai 'Available'.", 409).dump();
                return res;
            }
            bool ok = repo.remove(id);
            nlohmann::json body;
            body["success"] = ok;
            body["message"] = ok ? "Xoa ban thanh cong." : "Xoa that bai.";
            res.code = ok ? 200 : 500;
            res.set_header("Content-Type", "application/json");
            res.body = body.dump(2);
        } catch (const std::exception& e) {
            std::cerr << "[TableApiController] DELETE /api/tables/" << id << " error: " << e.what() << "\n";
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("Internal server error", 500).dump();
        }
        return res;
    });

    // ----- OPTIONS preflight -----
    CROW_ROUTE(app, "/api/tables")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request&) { crow::response res(204); addCorsHeaders(res); return res; });

    CROW_ROUTE(app, "/api/tables/<int>")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request&, int) { crow::response res(204); addCorsHeaders(res); return res; });

    CROW_ROUTE(app, "/api/tables/<int>/status")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request&, int) { crow::response res(204); addCorsHeaders(res); return res; });

    CROW_ROUTE(app, "/api/tables/available")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request&) { crow::response res(204); addCorsHeaders(res); return res; });

    std::cout << "[TableApiController] Routes registered:\n"
              << "  GET    /api/tables\n"
              << "  GET    /api/tables/available\n"
              << "  GET    /api/tables/<id>\n"
              << "  POST   /api/tables\n"
              << "  PUT    /api/tables/<id>\n"
              << "  PATCH  /api/tables/<id>/status\n"
              << "  DELETE /api/tables/<id>\n";
}

} // namespace controller
} // namespace restaurant
