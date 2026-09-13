// src/controller/ServiceApiController.cpp
// Triển khai API endpoints cho module Service (phục vụ, thanh toán, hỗ trợ).
#include "ServiceApiController.h"

#include <algorithm>
#include <iostream>

namespace restaurant {
namespace controller {

// Static members initialization
std::unordered_set<int> ServiceApiController::paymentRequests_;
std::unordered_set<int> ServiceApiController::assistanceRequests_;
std::mutex ServiceApiController::mutex_;

// ---------------------------------------------------------------------------
// Helper
// ---------------------------------------------------------------------------
static void addCorsHeaders(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

nlohmann::json ServiceApiController::errorJson(const std::string& message, int code) {
    return {{"success", false}, {"error", message}, {"code", code}};
}

// ---------------------------------------------------------------------------
// Route Registration
// ---------------------------------------------------------------------------
void ServiceApiController::registerRoutes(crow::SimpleApp& app) {

    // ===== PAYMENT REQUESTS =====

    // ----- POST /api/service/payment/request -----
    CROW_ROUTE(app, "/api/service/payment/request")
        .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto j = nlohmann::json::parse(req.body);
            if (!j.contains("order_id") || !j["order_id"].is_number_integer()) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Truong 'order_id' la bat buoc (so nguyen).", 400).dump();
                return res;
            }
            int orderId = j["order_id"].get<int>();
            if (orderId <= 0) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("order_id phai la so duong.", 400).dump();
                return res;
            }

            std::lock_guard<std::mutex> lock(mutex_);
            if (paymentRequests_.count(orderId) > 0) {
                res.code = 409;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Yeu cau thanh toan cho order " + std::to_string(orderId) + " da ton tai.", 409).dump();
                return res;
            }
            paymentRequests_.insert(orderId);

            nlohmann::json body;
            body["success"]  = true;
            body["message"]  = "Da gui yeu cau thanh toan.";
            body["order_id"] = orderId;
            res.code = 201;
            res.set_header("Content-Type", "application/json");
            res.body = body.dump(2);
        } catch (const nlohmann::json::parse_error&) {
            res.code = 400;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("JSON khong hop le.", 400).dump();
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("Internal server error", 500).dump();
        }
        return res;
    });

    // ----- POST /api/service/payment/cancel -----
    CROW_ROUTE(app, "/api/service/payment/cancel")
        .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto j = nlohmann::json::parse(req.body);
            int orderId = j.value("order_id", 0);
            if (orderId <= 0) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("order_id phai la so duong.", 400).dump();
                return res;
            }

            std::lock_guard<std::mutex> lock(mutex_);
            bool removed = paymentRequests_.erase(orderId) > 0;

            nlohmann::json body;
            body["success"] = removed;
            body["message"] = removed ? "Da huy yeu cau thanh toan." : "Khong tim thay yeu cau.";
            res.code = removed ? 200 : 404;
            res.set_header("Content-Type", "application/json");
            res.body = body.dump(2);
        } catch (const std::exception&) {
            res.code = 400;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("JSON khong hop le.", 400).dump();
        }
        return res;
    });

    // ----- GET /api/service/payment/requests -----
    CROW_ROUTE(app, "/api/service/payment/requests")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request&) {
        crow::response res;
        addCorsHeaders(res);

        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<int> ids(paymentRequests_.begin(), paymentRequests_.end());
        std::sort(ids.begin(), ids.end());

        nlohmann::json body;
        body["success"]   = true;
        body["data"]      = ids;
        body["count"]     = ids.size();
        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.body = body.dump(2);
        return res;
    });

    // ===== ASSISTANCE REQUESTS =====

    // ----- POST /api/service/assistance/request -----
    CROW_ROUTE(app, "/api/service/assistance/request")
        .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto j = nlohmann::json::parse(req.body);
            if (!j.contains("table_id") || !j["table_id"].is_number_integer()) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Truong 'table_id' la bat buoc (so nguyen).", 400).dump();
                return res;
            }
            int tableId = j["table_id"].get<int>();
            if (tableId <= 0) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("table_id phai la so duong.", 400).dump();
                return res;
            }

            // Verify table exists
            repository::TableRepository repo;
            auto table = repo.getById(tableId);
            if (!table.has_value()) {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Khong tim thay ban voi ID = " + std::to_string(tableId), 404).dump();
                return res;
            }

            std::lock_guard<std::mutex> lock(mutex_);
            if (assistanceRequests_.count(tableId) > 0) {
                res.code = 409;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Yeu cau ho tro cho ban " + std::to_string(tableId) + " da ton tai.", 409).dump();
                return res;
            }
            assistanceRequests_.insert(tableId);

            nlohmann::json body;
            body["success"]  = true;
            body["message"]  = "Da gui yeu cau ho tro.";
            body["table_id"] = tableId;
            res.code = 201;
            res.set_header("Content-Type", "application/json");
            res.body = body.dump(2);
        } catch (const nlohmann::json::parse_error&) {
            res.code = 400;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("JSON khong hop le.", 400).dump();
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("Internal server error", 500).dump();
        }
        return res;
    });

    // ----- POST /api/service/assistance/resolve -----
    CROW_ROUTE(app, "/api/service/assistance/resolve")
        .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto j = nlohmann::json::parse(req.body);
            int tableId = j.value("table_id", 0);
            if (tableId <= 0) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("table_id phai la so duong.", 400).dump();
                return res;
            }

            std::lock_guard<std::mutex> lock(mutex_);
            bool removed = assistanceRequests_.erase(tableId) > 0;

            nlohmann::json body;
            body["success"] = removed;
            body["message"] = removed ? "Da giai quyet yeu cau ho tro." : "Khong tim thay yeu cau.";
            res.code = removed ? 200 : 404;
            res.set_header("Content-Type", "application/json");
            res.body = body.dump(2);
        } catch (const std::exception&) {
            res.code = 400;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("JSON khong hop le.", 400).dump();
        }
        return res;
    });

    // ----- GET /api/service/assistance/requests -----
    CROW_ROUTE(app, "/api/service/assistance/requests")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request&) {
        crow::response res;
        addCorsHeaders(res);

        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<int> ids(assistanceRequests_.begin(), assistanceRequests_.end());
        std::sort(ids.begin(), ids.end());

        nlohmann::json body;
        body["success"]   = true;
        body["data"]      = ids;
        body["count"]     = ids.size();
        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.body = body.dump(2);
        return res;
    });

    // ===== OPTIONS preflight =====
    auto optionsHandler = [](const crow::request&) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    };

    CROW_ROUTE(app, "/api/service/payment/request").methods(crow::HTTPMethod::OPTIONS)(optionsHandler);
    CROW_ROUTE(app, "/api/service/payment/cancel").methods(crow::HTTPMethod::OPTIONS)(optionsHandler);
    CROW_ROUTE(app, "/api/service/payment/requests").methods(crow::HTTPMethod::OPTIONS)(optionsHandler);
    CROW_ROUTE(app, "/api/service/assistance/request").methods(crow::HTTPMethod::OPTIONS)(optionsHandler);
    CROW_ROUTE(app, "/api/service/assistance/resolve").methods(crow::HTTPMethod::OPTIONS)(optionsHandler);
    CROW_ROUTE(app, "/api/service/assistance/requests").methods(crow::HTTPMethod::OPTIONS)(optionsHandler);

    std::cout << "[ServiceApiController] Routes registered:\n"
              << "  POST /api/service/payment/request\n"
              << "  POST /api/service/payment/cancel\n"
              << "  GET  /api/service/payment/requests\n"
              << "  POST /api/service/assistance/request\n"
              << "  POST /api/service/assistance/resolve\n"
              << "  GET  /api/service/assistance/requests\n";
}

} // namespace controller
} // namespace restaurant
