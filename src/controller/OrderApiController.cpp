// src/controller/OrderApiController.cpp
#include "OrderApiController.h"
#include "../database/Database.h"
#include "../repository/TableRepository.h"
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

nlohmann::json OrderApiController::orderWithItemsToJson(
    const model::Order& order, 
    const std::vector<model::OrderItem>& items
) {
    nlohmann::json j = utils::JsonHelper::orderToJson(order);
    j["items"] = utils::JsonHelper::orderItemsToJson(items);
    return j;
}

void OrderApiController::registerRoutes(crow::SimpleApp& app) {

    // CORS preflights
    CROW_ROUTE(app, "/api/orders")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/api/orders/<int>")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/, int) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/api/orders/<int>/items")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/, int) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/api/orders/<int>/status")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/, int) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    // POST /api/orders — Tạo đơn hàng mới từ giao diện Đặt món
    CROW_ROUTE(app, "/api/orders")
        .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);

        try {
            auto body = nlohmann::json::parse(req.body);
            if (!body.contains("table_id") || !body["table_id"].is_number()) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Thieu table_id.", 400).dump();
                return res;
            }

            int tableId = body["table_id"].get<int>();
            std::optional<int> staffId = std::nullopt;
            if (body.contains("staff_id") && body["staff_id"].is_number()) {
                staffId = body["staff_id"].get<int>();
            }

            repository::OrderRepository orderRepo;
            model::Order newOrder(tableId, staffId, "Pending", 0.0);
            model::Order createdOrder = orderRepo.add(newOrder);

            // Tự động cập nhật bàn sang trạng thái 'Occupied'
            try {
                repository::TableRepository tableRepo;
                tableRepo.updateStatus(tableId, "Occupied");
            } catch (...) {}

            // Thêm các món ăn vào đơn hàng (nếu có)
            std::vector<model::OrderItem> createdItems;
            if (body.contains("items") && body["items"].is_array()) {
                auto& session = database::Database::getInstance().getSession();

                for (const auto& itemJson : body["items"]) {
                    if (!itemJson.contains("item_id") || !itemJson.contains("quantity")) continue;
                    int itemId = itemJson["item_id"].get<int>();
                    int quantity = itemJson["quantity"].get<int>();
                    if (quantity <= 0) quantity = 1;

                    std::optional<std::string> note = std::nullopt;
                    if (itemJson.contains("special_note") && itemJson["special_note"].is_string()) {
                        note = itemJson["special_note"].get<std::string>();
                    }

                    // Lấy đơn giá món ăn từ bảng MenuItem
                    double price = 0.0;
                    if (itemJson.contains("unit_price") && itemJson["unit_price"].is_number()) {
                        price = itemJson["unit_price"].get<double>();
                    } else {
                        mysqlx::SqlStatement priceStmt = session.sql("SELECT price FROM MenuItem WHERE item_id = ?");
                        priceStmt.bind(itemId);
                        mysqlx::Row row = priceStmt.execute().fetchOne();
                        if (!row.isNull()) {
                            price = row[0].get<double>();
                        }
                    }

                    model::OrderItem item(createdOrder.getId(), itemId, quantity, price, note, "Pending");
                    createdItems.push_back(orderRepo.addItem(item));
                }
            }

            // Lấy lại đơn hàng hoàn chỉnh đã tính tổng tiền
            auto refreshedOrder = orderRepo.getById(createdOrder.getId());
            model::Order finalOrder = refreshedOrder.has_value() ? refreshedOrder.value() : createdOrder;

            nlohmann::json responseData = orderWithItemsToJson(finalOrder, createdItems);
            nlohmann::json responseJson = {
                {"success", true},
                {"message", "Tao don hang thanh cong."},
                {"data", responseData}
            };

            res.code = 201;
            res.set_header("Content-Type", "application/json");
            res.body = responseJson.dump(2);
        } catch (const std::exception& e) {
            std::cerr << "[OrderApiController] POST /api/orders error: " << e.what() << "\n";
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    // GET /api/orders — Lấy toàn bộ đơn hàng
    CROW_ROUTE(app, "/api/orders")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        try {
            repository::OrderRepository repo;
            auto orders = repo.getAll();

            std::string statusFilter = "";
            if (req.url_params.get("status") != nullptr) {
                statusFilter = req.url_params.get("status");
            }

            nlohmann::json arr = nlohmann::json::array();
            for (const auto& o : orders) {
                if (!statusFilter.empty() && o.getOrderStatus() != statusFilter) {
                    continue;
                }
                auto items = repo.getItems(o.getId());
                arr.push_back(orderWithItemsToJson(o, items));
            }

            nlohmann::json body = {
                {"success", true},
                {"count", arr.size()},
                {"data", arr}
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

    // GET /api/orders/<id> — Lấy chi tiết đơn hàng
    CROW_ROUTE(app, "/api/orders/<int>")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/, int id) {
        crow::response res;
        addCorsHeaders(res);
        try {
            repository::OrderRepository repo;
            auto orderOpt = repo.getById(id);
            if (!orderOpt.has_value()) {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Khong tim thay don hang ID " + std::to_string(id), 404).dump();
                return res;
            }

            auto items = repo.getItems(id);
            nlohmann::json body = {
                {"success", true},
                {"data", orderWithItemsToJson(orderOpt.value(), items)}
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

    // POST /api/orders/<id>/items — Thêm món vào đơn hàng hiện có
    CROW_ROUTE(app, "/api/orders/<int>/items")
        .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req, int orderId) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto body = nlohmann::json::parse(req.body);
            if (!body.contains("item_id") || !body.contains("quantity")) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Thieu item_id hoac quantity.", 400).dump();
                return res;
            }

            int itemId = body["item_id"].get<int>();
            int quantity = body["quantity"].get<int>();
            if (quantity <= 0) quantity = 1;

            std::optional<std::string> note = std::nullopt;
            if (body.contains("special_note") && body["special_note"].is_string()) {
                note = body["special_note"].get<std::string>();
            }

            double price = 0.0;
            if (body.contains("unit_price") && body["unit_price"].is_number()) {
                price = body["unit_price"].get<double>();
            } else {
                auto& session = database::Database::getInstance().getSession();
                mysqlx::SqlStatement priceStmt = session.sql("SELECT price FROM MenuItem WHERE item_id = ?");
                priceStmt.bind(itemId);
                mysqlx::Row row = priceStmt.execute().fetchOne();
                if (!row.isNull()) {
                    price = row[0].get<double>();
                }
            }

            repository::OrderRepository repo;
            model::OrderItem item(orderId, itemId, quantity, price, note, "Pending");
            auto added = repo.addItem(item);

            res.code = 201;
            res.set_header("Content-Type", "application/json");
            res.body = nlohmann::json({
                {"success", true},
                {"message", "Them mon thanh cong."},
                {"data", utils::JsonHelper::orderItemToJson(added)}
            }).dump(2);
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    // PATCH /api/orders/<id>/status — Đổi trạng thái đơn hàng
    CROW_ROUTE(app, "/api/orders/<int>/status")
        .methods(crow::HTTPMethod::PATCH)
    ([](const crow::request& req, int orderId) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto body = nlohmann::json::parse(req.body);
            if (!body.contains("status") || !body["status"].is_string()) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Thieu status.", 400).dump();
                return res;
            }

            std::string status = body["status"].get<std::string>();
            repository::OrderRepository repo;
            bool ok = repo.updateOrderStatus(orderId, status);

            if (ok) {
                res.code = 200;
                res.set_header("Content-Type", "application/json");
                res.body = nlohmann::json({
                    {"success", true},
                    {"message", "Cap nhat trang thai don hang thanh cong."}
                }).dump(2);
            } else {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Khong tim thay don hang.", 404).dump();
            }
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    std::cout << "[OrderApiController] Routes registered: /api/orders\n";
}

} // namespace controller
} // namespace restaurant
