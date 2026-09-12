#include <iostream>
#include "crow.h"
#include "controller/PaymentApiController.h"
#include "controller/ReportApiController.h"
int main() {
    crow::SimpleApp app;
    PaymentApiController paymentApi;
    ReportApiController reportApi;
    CROW_ROUTE(app, "/api/payment").methods(crow::HTTPMethod::POST)([&paymentApi](const crow::request& req) {
        return paymentApi.handlePayment(req);
    });
    CROW_ROUTE(app, "/api/reports/revenue").methods(crow::HTTPMethod::GET)([&reportApi](const crow::request& req) {
        return reportApi.getRevenueReport(req);
    });
    std::cout << "App is running on port 18080..." << std::endl;
    app.port(18080).multithreaded().run();
    return 0;
}
