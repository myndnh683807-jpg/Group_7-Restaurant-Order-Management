#include "crow.h"
#include "nlohmann/json.hpp"
#include "controller/EmployeeApiController.h"
#include "controller/InventoryApiController.h"
#include "controller/PaymentApiController.h"
#include "controller/ReportApiController.h"

int main() {
    crow::SimpleApp app;

    EmployeeApiController empApi;
    InventoryApiController invApi;
    PaymentApiController paymentApi;
    ReportApiController reportApi;

    // API UC13 
    CROW_ROUTE(app, "/api/employees").methods(crow::HTTPMethod::GET)([&empApi]() {
        return crow::response(empApi.getAllEmployees().dump());
    });

    CROW_ROUTE(app, "/api/employees").methods(crow::HTTPMethod::POST)([&empApi](const crow::request& req) {
        auto jsonReq = nlohmann::json::parse(req.body);
        return crow::response(empApi.createEmployee(jsonReq).dump());
    });

    // API UC14 
    CROW_ROUTE(app, "/api/inventory").methods(crow::HTTPMethod::GET)([&invApi]() {
        return crow::response(invApi.getAllItems().dump());
    });

    CROW_ROUTE(app, "/api/inventory").methods(crow::HTTPMethod::POST)([&invApi](const crow::request& req) {
        auto jsonReq = nlohmann::json::parse(req.body);
        return crow::response(invApi.createItem(jsonReq).dump());
    });

    CROW_ROUTE(app, "/api/inventory/low-stock").methods(crow::HTTPMethod::GET)([&invApi]() {
        return crow::response(invApi.getLowStockItems().dump());
    });

    // API UC11 - Checkout & Payment Processing (Bổ sung của bạn)
    CROW_ROUTE(app, "/api/payment").methods(crow::HTTPMethod::POST)([&paymentApi](const crow::request& req) {
        return paymentApi.handlePayment(req);
    });

    // API Báo cáo doanh thu (Bổ sung của bạn)
    CROW_ROUTE(app, "/api/reports/revenue").methods(crow::HTTPMethod::GET)([&reportApi](const crow::request& req) {
        return reportApi.getRevenueReport(req);
    });

    app.port(8080).multithreaded().run();
}
