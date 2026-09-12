#include <iostream>
#include "crow.h"
#include "controller/PaymentApiController.h"
#include "controller/ReportApiController.h"

int main() {
    crow::SimpleApp app;

    // Khởi tạo Controller của bạn
    PaymentApiController paymentApi;
    ReportApiController reportApi;

    // API UC11 - Checkout & Payment Processing
    CROW_ROUTE(app, "/api/payment").methods(crow::HTTPMethod::POST)([&paymentApi](const crow::request& req) {
        return paymentApi.handlePayment(req);
    });

    // API Báo cáo doanh thu
    CROW_ROUTE(app, "/api/reports/revenue").methods(crow::HTTPMethod::GET)([&reportApi](const crow::request& req) {
        return reportApi.getRevenueReport(req);
    });

    std::cout << "App is running on port 18080..." << std::endl;
    app.port(18080).multithreaded().run();
    return 0;
}
