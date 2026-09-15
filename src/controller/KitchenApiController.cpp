// src/controller/KitchenApiController.cpp
#include "KitchenApiController.h"
#include "../database/Database.h"
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

void KitchenApiController::registerRoutes(crow::SimpleApp& app) {

    // CORS preflights
    CROW_ROUTE(app, "/api/kitchen/orders")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/api/kitchen/items")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/api/kitchen/items/<int>/status")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/, int) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    // GET /api/kitchen/orders — Lấy danh sách các đơn hàng còn hoạt động trong bếp
    CROW_ROUTE(app, "/api/kitchen/orders")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto& session = database::Database::getInstance().getSession();
            
            // Lấy các order chưa hoàn tất (Pending, In Progress, Kitchen)
            std::string sqlOrder = 
                "SELECT o.order_id, o.table_id, t.table_number, o.order_date, o.order_status "
                "FROM Orders o "
                "JOIN Tables t ON o.table_id = t.table_id "
                "WHERE o.order_status NOT IN ('Completed', 'Cancelled') "
                "ORDER BY o.order_date ASC";

            mysqlx::SqlResult orderResult = session.sql(sqlOrder).execute();
            nlohmann::json orderList = nlohmann::json::array();

            for (mysqlx::Row oRow : orderResult) {
                int orderId = oRow[0].get<int>();
                int tableId = oRow[1].get<int>();
                std::string tableNumber = oRow[2].get<std::string>();
                std::string orderDate = oRow[3].get<std::string>();
                std::string orderStatus = oRow[4].get<std::string>();

                // Lấy các items trong đơn
                std::string sqlItems = 
                    "SELECT oi.order_item_id, oi.item_id, m.item_name, oi.quantity, oi.special_note, oi.item_status "
                    "FROM OrderItem oi "
                    "JOIN MenuItem m ON oi.item_id = m.item_id "
                    "WHERE oi.order_id = ? "
                    "ORDER BY oi.order_item_id ASC";

                mysqlx::SqlStatement itemStmt = session.sql(sqlItems);
                itemStmt.bind(orderId);
                mysqlx::SqlResult itemResult = itemStmt.execute();

                nlohmann::json itemList = nlohmann::json::array();
                for (mysqlx::Row iRow : itemResult) {
                    int orderItemId = iRow[0].get<int>();
                    int itemId = iRow[1].get<int>();
                    std::string itemName = iRow[2].get<std::string>();
                    int quantity = iRow[3].get<int>();
                    std::string specialNote = iRow[4].isNull() ? "" : iRow[4].get<std::string>();
                    std::string itemStatus = iRow[5].get<std::string>();

                    itemList.push_back({
                        {"order_item_id", orderItemId},
                        {"item_id", itemId},
                        {"item_name", itemName},
                        {"quantity", quantity},
                        {"special_note", specialNote},
                        {"item_status", itemStatus}
                    });
                }

                orderList.push_back({
                    {"order_id", orderId},
                    {"table_id", tableId},
                    {"table_number", tableNumber},
                    {"order_date", orderDate},
                    {"order_status", orderStatus},
                    {"items", itemList}
                });
            }

            nlohmann::json body = {
                {"success", true},
                {"count", orderList.size()},
                {"data", orderList}
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

    // GET /api/kitchen/items — Danh sách toàn bộ các món cần chế biến
    CROW_ROUTE(app, "/api/kitchen/items")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto& session = database::Database::getInstance().getSession();

            std::string statusFilter = "";
            if (req.url_params.get("status") != nullptr) {
                statusFilter = req.url_params.get("status");
            }

            std::string sqlQuery = 
                "SELECT oi.order_item_id, oi.order_id, t.table_number, oi.item_id, "
                "m.item_name, oi.quantity, oi.special_note, oi.item_status, o.order_date "
                "FROM OrderItem oi "
                "JOIN Orders o ON oi.order_id = o.order_id "
                "JOIN Tables t ON o.table_id = t.table_id "
                "JOIN MenuItem m ON oi.item_id = m.item_id "
                "WHERE o.order_status NOT IN ('Completed', 'Cancelled') ";

            if (!statusFilter.empty()) {
                sqlQuery += "AND oi.item_status = ? ";
            }
            sqlQuery += "ORDER BY oi.order_item_id ASC";

            mysqlx::SqlStatement stmt = session.sql(sqlQuery);
            if (!statusFilter.empty()) {
                stmt.bind(statusFilter);
            }

            mysqlx::SqlResult result = stmt.execute();
            nlohmann::json itemList = nlohmann::json::array();

            for (mysqlx::Row row : result) {
                itemList.push_back({
                    {"order_item_id", row[0].get<int>()},
                    {"order_id", row[1].get<int>()},
                    {"table_number", row[2].get<std::string>()},
                    {"item_id", row[3].get<int>()},
                    {"item_name", row[4].get<std::string>()},
                    {"quantity", row[5].get<int>()},
                    {"special_note", row[6].isNull() ? "" : row[6].get<std::string>()},
                    {"item_status", row[7].get<std::string>()},
                    {"order_date", row[8].get<std::string>()}
                });
            }

            nlohmann::json body = {
                {"success", true},
                {"count", itemList.size()},
                {"data", itemList}
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

    // PATCH /api/kitchen/items/<id>/status — Đổi trạng thái món ăn (Pending, Preparing, Ready, Served)
    CROW_ROUTE(app, "/api/kitchen/items/<int>/status")
        .methods(crow::HTTPMethod::PATCH)
    ([](const crow::request& req, int orderItemId) {
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
            bool ok = repo.updateItemStatus(orderItemId, status);

            if (ok) {
                res.code = 200;
                res.set_header("Content-Type", "application/json");
                res.body = nlohmann::json({
                    {"success", true},
                    {"message", "Cap nhat trang thai mon an thanh cong."},
                    {"order_item_id", orderItemId},
                    {"new_status", status}
                }).dump(2);
            } else {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Khong tim thay mon an voi ID " + std::to_string(orderItemId), 404).dump();
            }
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    std::cout << "[KitchenApiController] Routes registered: /api/kitchen/orders, /api/kitchen/items\n";
}

} // namespace controller
} // namespace restaurant
