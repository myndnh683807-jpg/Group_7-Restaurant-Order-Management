// src/controller/ReportApiController.cpp
#include "ReportApiController.h"
#include "../database/Database.h"
#include "../utils/JsonHelper.h"

#include <iostream>
#include <stdexcept>
#include <unordered_map>

namespace restaurant {
namespace controller {

static void addCorsHeaders(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

void ReportApiController::registerRoutes(crow::SimpleApp& app) {

    // CORS preflights
    CROW_ROUTE(app, "/api/reports/revenue")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/api/reports/summary")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    // GET /api/reports/revenue — Báo cáo doanh thu (UC15)
    CROW_ROUTE(app, "/api/reports/revenue")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        try {
            std::string timeRange = "all";
            if (req.url_params.get("timeRange") != nullptr) {
                timeRange = req.url_params.get("timeRange");
            } else if (req.url_params.get("range") != nullptr) {
                timeRange = req.url_params.get("range");
            }

            auto& session = database::Database::getInstance().getSession();

            // Tổng doanh thu và số hóa đơn
            mysqlx::SqlResult invResult = session.sql(
                "SELECT invoice_id, final_amount, payment_method, created_at, payment_status "
                "FROM Invoice "
                "ORDER BY created_at DESC"
            ).execute();

            double totalRevenue = 0.0;
            int totalInvoices = 0;
            std::unordered_map<std::string, double> methodRevenue;
            std::unordered_map<std::string, int> methodCount;
            nlohmann::json recentInvoices = nlohmann::json::array();

            for (mysqlx::Row row : invResult) {
                int id = row[0].get<int>();
                double amount = row[1].get<double>();
                std::string method = row[2].get<std::string>();
                std::string createdAt = row[3].get<std::string>();
                std::string status = row[4].get<std::string>();

                totalRevenue += amount;
                totalInvoices++;

                methodRevenue[method] += amount;
                methodCount[method]++;

                if (recentInvoices.size() < 10) {
                    recentInvoices.push_back({
                        {"invoice_id", id},
                        {"final_amount", amount},
                        {"payment_method", method},
                        {"created_at", createdAt},
                        {"payment_status", status}
                    });
                }
            }

            nlohmann::json methodsBreakdown = nlohmann::json::object();
            for (const auto& [method, amt] : methodRevenue) {
                methodsBreakdown[method] = {
                    {"total_amount", amt},
                    {"transaction_count", methodCount[method]}
                };
            }

            nlohmann::json responseData = {
                {"time_range", timeRange},
                {"total_revenue", totalRevenue},
                {"total_invoices", totalInvoices},
                {"total_orders", totalInvoices},
                {"method_breakdown", methodsBreakdown},
                {"recent_invoices", recentInvoices}
            };

            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.body = nlohmann::json({
                {"success", true},
                {"data", responseData}
            }).dump(2);
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    // GET /api/reports/summary — Tổng hợp các chỉ số KPI
    CROW_ROUTE(app, "/api/reports/summary")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto& session = database::Database::getInstance().getSession();

            // Thống kê bàn
            mysqlx::SqlResult tableRes = session.sql(
                "SELECT status, COUNT(*) FROM Tables GROUP BY status"
            ).execute();

            int totalTables = 0;
            int availableTables = 0;
            int occupiedTables = 0;
            int reservedTables = 0;

            for (mysqlx::Row row : tableRes) {
                std::string status = row[0].get<std::string>();
                int count = static_cast<int>(row[1].get<int64_t>());
                totalTables += count;
                if (status == "Available") availableTables = count;
                else if (status == "Occupied") occupiedTables = count;
                else if (status == "Reserved") reservedTables = count;
            }

            // Thống kê đơn hàng
            mysqlx::SqlResult orderRes = session.sql(
                "SELECT order_status, COUNT(*) FROM Orders GROUP BY order_status"
            ).execute();

            int pendingOrders = 0;
            int completedOrders = 0;
            for (mysqlx::Row row : orderRes) {
                std::string st = row[0].get<std::string>();
                int count = static_cast<int>(row[1].get<int64_t>());
                if (st == "Pending" || st == "In Progress") pendingOrders += count;
                else if (st == "Completed") completedOrders += count;
            }

            // Thống kê cảnh báo tồn kho (quantity <= min_stock)
            mysqlx::SqlResult invRes = session.sql(
                "SELECT COUNT(*) FROM Inventory WHERE quantity <= min_stock"
            ).execute();
            int lowStockCount = 0;
            mysqlx::Row invRow = invRes.fetchOne();
            if (!invRow.isNull()) {
                lowStockCount = static_cast<int>(invRow[0].get<int64_t>());
            }

            // Doanh thu tổng
            mysqlx::SqlResult revRes = session.sql(
                "SELECT COALESCE(SUM(final_amount), 0.0) FROM Invoice"
            ).execute();
            double totalRevenue = 0.0;
            mysqlx::Row revRow = revRes.fetchOne();
            if (!revRow.isNull()) {
                totalRevenue = revRow[0].get<double>();
            }

            nlohmann::json summary = {
                {"tables", {
                    {"total", totalTables},
                    {"available", availableTables},
                    {"occupied", occupiedTables},
                    {"reserved", reservedTables}
                }},
                {"orders", {
                    {"pending", pendingOrders},
                    {"completed", completedOrders}
                }},
                {"inventory", {
                    {"low_stock_count", lowStockCount}
                }},
                {"revenue", {
                    {"total", totalRevenue}
                }}
            };

            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.body = nlohmann::json({
                {"success", true},
                {"data", summary}
            }).dump(2);
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    std::cout << "[ReportApiController] Routes registered: /api/reports/revenue, /api/reports/summary\n";
}

} // namespace controller
} // namespace restaurant
