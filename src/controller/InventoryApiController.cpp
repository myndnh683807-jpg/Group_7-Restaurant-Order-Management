// src/controller/InventoryApiController.cpp
#include "InventoryApiController.h"
#include "../utils/JsonHelper.h"

#include <iostream>
#include <stdexcept>

namespace restaurant {
namespace controller {

static void addCorsHeaders(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, PATCH, DELETE, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

void InventoryApiController::registerRoutes(crow::SimpleApp& app) {

    // CORS preflights
    CROW_ROUTE(app, "/api/inventory")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/api/inventory/<int>")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/, int) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/api/inventory/<int>/stock")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/, int) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    // GET /api/inventory — Lấy toàn bộ mặt hàng trong kho (UC14)
    CROW_ROUTE(app, "/api/inventory")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/) {
        crow::response res;
        addCorsHeaders(res);
        try {
            repository::InventoryRepository repo;
            auto items = repo.getAll();

            nlohmann::json body = {
                {"success", true},
                {"count", items.size()},
                {"data", utils::JsonHelper::inventoriesToJson(items)}
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

    // GET /api/inventory/<int> — Chi tiết mặt hàng
    CROW_ROUTE(app, "/api/inventory/<int>")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/, int id) {
        crow::response res;
        addCorsHeaders(res);
        try {
            repository::InventoryRepository repo;
            auto itemOpt = repo.getById(id);
            if (!itemOpt.has_value()) {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Khong tim thay mat hang ID " + std::to_string(id), 404).dump();
                return res;
            }

            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.body = nlohmann::json({
                {"success", true},
                {"data", utils::JsonHelper::inventoryToJson(itemOpt.value())}
            }).dump(2);
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    // POST /api/inventory — Thêm mặt hàng mới
    CROW_ROUTE(app, "/api/inventory")
        .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto body = nlohmann::json::parse(req.body);
            if (!body.contains("item_name") && !body.contains("itemName")) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Thieu ten mat hang.", 400).dump();
                return res;
            }

            std::string name = body.contains("item_name") ? body["item_name"].get<std::string>() : body["itemName"].get<std::string>();
            int qty = body.contains("quantity") ? body["quantity"].get<int>() : 0;
            std::string unit = body.contains("unit") ? body["unit"].get<std::string>() : "kg";
            int minStock = 0;
            if (body.contains("min_stock")) minStock = body["min_stock"].get<int>();
            else if (body.contains("minStock")) minStock = body["minStock"].get<int>();

            repository::InventoryRepository repo;
            model::Inventory item(name, qty, unit, minStock);
            model::Inventory created = repo.add(item);

            res.code = 201;
            res.set_header("Content-Type", "application/json");
            res.body = nlohmann::json({
                {"success", true},
                {"message", "Them mat hang kho thanh cong."},
                {"data", utils::JsonHelper::inventoryToJson(created)}
            }).dump(2);
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    // PUT /api/inventory/<int> — Cập nhật mặt hàng
    CROW_ROUTE(app, "/api/inventory/<int>")
        .methods(crow::HTTPMethod::PUT)
    ([](const crow::request& req, int id) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto body = nlohmann::json::parse(req.body);
            repository::InventoryRepository repo;
            auto itemOpt = repo.getById(id);
            if (!itemOpt.has_value()) {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Khong tim thay mat hang.", 404).dump();
                return res;
            }

            model::Inventory item = itemOpt.value();
            if (body.contains("item_name") && body["item_name"].is_string()) item.setItemName(body["item_name"].get<std::string>());
            if (body.contains("itemName") && body["itemName"].is_string()) item.setItemName(body["itemName"].get<std::string>());
            if (body.contains("quantity") && body["quantity"].is_number()) item.setQuantity(body["quantity"].get<int>());
            if (body.contains("unit") && body["unit"].is_string()) item.setUnit(body["unit"].get<std::string>());
            if (body.contains("min_stock") && body["min_stock"].is_number()) item.setMinStock(body["min_stock"].get<int>());
            if (body.contains("minStock") && body["minStock"].is_number()) item.setMinStock(body["minStock"].get<int>());

            bool ok = repo.update(item);
            if (ok) {
                res.code = 200;
                res.set_header("Content-Type", "application/json");
                res.body = nlohmann::json({
                    {"success", true},
                    {"message", "Cap nhat mat hang kho thanh cong."},
                    {"data", utils::JsonHelper::inventoryToJson(item)}
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

    // PATCH /api/inventory/<int>/stock — Điều chỉnh số lượng tồn kho
    CROW_ROUTE(app, "/api/inventory/<int>/stock")
        .methods(crow::HTTPMethod::PATCH)
    ([](const crow::request& req, int id) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto body = nlohmann::json::parse(req.body);
            repository::InventoryRepository repo;
            auto itemOpt = repo.getById(id);
            if (!itemOpt.has_value()) {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Khong tim thay mat hang.", 404).dump();
                return res;
            }

            int newQuantity = itemOpt.value().getQuantity();
            if (body.contains("quantity") && body["quantity"].is_number()) {
                newQuantity = body["quantity"].get<int>();
            } else if (body.contains("delta") && body["delta"].is_number()) {
                newQuantity += body["delta"].get<int>();
            } else {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Thieu quantity hoac delta.", 400).dump();
                return res;
            }

            if (newQuantity < 0) newQuantity = 0;

            bool ok = repo.updateQuantity(id, newQuantity);
            if (ok) {
                res.code = 200;
                res.set_header("Content-Type", "application/json");
                res.body = nlohmann::json({
                    {"success", true},
                    {"message", "Dieu chinh ton kho thanh cong."},
                    {"inventory_id", id},
                    {"new_quantity", newQuantity}
                }).dump(2);
            } else {
                res.code = 500;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Cap nhat ton kho that bai.", 500).dump();
            }
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    // DELETE /api/inventory/<int> — Xóa mặt hàng kho
    CROW_ROUTE(app, "/api/inventory/<int>")
        .methods(crow::HTTPMethod::Delete)
    ([](const crow::request& /*req*/, int id) {
        crow::response res;
        addCorsHeaders(res);
        try {
            repository::InventoryRepository repo;
            bool ok = repo.remove(id);
            if (ok) {
                res.code = 200;
                res.set_header("Content-Type", "application/json");
                res.body = nlohmann::json({
                    {"success", true},
                    {"message", "Xoa mat hang kho thanh cong."}
                }).dump(2);
            } else {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Khong tim thay mat hang ID " + std::to_string(id), 404).dump();
            }
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    std::cout << "[InventoryApiController] Routes registered: /api/inventory\n";
}

} // namespace controller
} // namespace restaurant
