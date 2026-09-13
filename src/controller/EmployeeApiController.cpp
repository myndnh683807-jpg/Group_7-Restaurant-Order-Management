// src/controller/EmployeeApiController.cpp
#include "EmployeeApiController.h"
#include "../utils/JsonHelper.h"

#include <iostream>
#include <stdexcept>

namespace restaurant {
namespace controller {

static void addCorsHeaders(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

void EmployeeApiController::registerRoutes(crow::SimpleApp& app) {

    // CORS preflights
    CROW_ROUTE(app, "/api/employees")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/api/employees/<int>")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/, int) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    // GET /api/employees — Lấy toàn bộ nhân viên (UC13)
    CROW_ROUTE(app, "/api/employees")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/) {
        crow::response res;
        addCorsHeaders(res);
        try {
            repository::EmployeeRepository repo;
            auto employees = repo.getAll();

            nlohmann::json body = {
                {"success", true},
                {"count", employees.size()},
                {"data", utils::JsonHelper::employeesToJson(employees)}
            };

            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.body = body.dump(2);
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    // GET /api/employees/<int> — Chi tiết nhân viên
    CROW_ROUTE(app, "/api/employees/<int>")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/, int id) {
        crow::response res;
        addCorsHeaders(res);
        try {
            repository::EmployeeRepository repo;
            auto empOpt = repo.getById(id);
            if (!empOpt.has_value()) {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Khong tim thay nhan vien ID " + std::to_string(id), 404).dump();
                return res;
            }

            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.body = nlohmann::json({
                {"success", true},
                {"data", utils::JsonHelper::employeeToJson(empOpt.value())}
            }).dump(2);
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    // POST /api/employees — Thêm nhân viên mới
    CROW_ROUTE(app, "/api/employees")
        .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto body = nlohmann::json::parse(req.body);
            if (!body.contains("username") || !body.contains("password") || !body.contains("full_name") || !body.contains("role")) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Thieu thong tin: username, password, full_name, role la bat buoc.", 400).dump();
                return res;
            }

            std::string username = body["username"].get<std::string>();
            std::string password = body["password"].get<std::string>();
            std::string fullName = body["full_name"].get<std::string>();
            std::string role = body["role"].get<std::string>();
            std::optional<std::string> phone = std::nullopt;
            if (body.contains("phone") && body["phone"].is_string()) {
                phone = body["phone"].get<std::string>();
            }

            repository::EmployeeRepository repo;
            auto existing = repo.getByUsername(username);
            if (existing.has_value()) {
                res.code = 409;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Username da ton tai.", 409).dump();
                return res;
            }

            // Mã hóa đơn giản hoặc lưu hash
            std::string passwordHash = "hash_" + password;
            model::Employee newEmp(username, passwordHash, fullName, role, phone);
            model::Employee created = repo.add(newEmp);

            res.code = 201;
            res.set_header("Content-Type", "application/json");
            res.body = nlohmann::json({
                {"success", true},
                {"message", "Them nhan vien thanh cong."},
                {"data", utils::JsonHelper::employeeToJson(created)}
            }).dump(2);
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    // PUT /api/employees/<int> — Cập nhật nhân viên
    CROW_ROUTE(app, "/api/employees/<int>")
        .methods(crow::HTTPMethod::PUT)
    ([](const crow::request& req, int id) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto body = nlohmann::json::parse(req.body);
            repository::EmployeeRepository repo;
            auto empOpt = repo.getById(id);
            if (!empOpt.has_value()) {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Khong tim thay nhan vien.", 404).dump();
                return res;
            }

            model::Employee emp = empOpt.value();
            if (body.contains("full_name") && body["full_name"].is_string()) {
                emp.setFullName(body["full_name"].get<std::string>());
            }
            if (body.contains("role") && body["role"].is_string()) {
                emp.setRole(body["role"].get<std::string>());
            }
            if (body.contains("phone") && body["phone"].is_string()) {
                emp.setPhone(body["phone"].get<std::string>());
            }

            bool ok = repo.update(emp);
            if (ok) {
                res.code = 200;
                res.set_header("Content-Type", "application/json");
                res.body = nlohmann::json({
                    {"success", true},
                    {"message", "Cap nhat nhan vien thanh cong."},
                    {"data", utils::JsonHelper::employeeToJson(emp)}
                }).dump(2);
            } else {
                res.code = 500;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Cap nhat that bai.", 500).dump();
            }
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    // DELETE /api/employees/<int> — Xóa nhân viên
    CROW_ROUTE(app, "/api/employees/<int>")
        .methods(crow::HTTPMethod::Delete)
    ([](const crow::request& /*req*/, int id) {
        crow::response res;
        addCorsHeaders(res);
        try {
            repository::EmployeeRepository repo;
            bool ok = repo.remove(id);
            if (ok) {
                res.code = 200;
                res.set_header("Content-Type", "application/json");
                res.body = nlohmann::json({
                    {"success", true},
                    {"message", "Xoa nhan vien thanh cong."}
                }).dump(2);
            } else {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Khong tim thay nhan vien ID " + std::to_string(id), 404).dump();
            }
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    std::cout << "[EmployeeApiController] Routes registered: /api/employees\n";
}

} // namespace controller
} // namespace restaurant
