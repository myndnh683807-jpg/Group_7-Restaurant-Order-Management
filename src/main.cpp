// src/main.cpp
// Entry point — Khởi chạy Crow HTTP Server đa luồng (multithreaded).
//
// Server lắng nghe trên port 8080, phục vụ REST API và giao diện Web (frontend/).
// Architecture:
//   Browser (HTML/CSS/JS) ──HTTP──▷ Crow Server ──▷ ApiControllers ──▷ Repository ──▷ MySQL

#include "controller/EmployeeApiController.h"
#include "controller/InventoryApiController.h"
#include "controller/KitchenApiController.h"
#include "controller/MenuApiController.h"
#include "controller/OrderApiController.h"
#include "controller/PaymentApiController.h"
#include "controller/ReportApiController.h"
#include "controller/ServiceApiController.h"
#include "controller/TableApiController.h"
#include "database/Database.h"

#include <crow.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

/// @brief Helper đọc nội dung file từ thư mục frontend và trả response kèm MIME type phù hợp.
static crow::response serveFrontendFile(const std::string& relativePath) {
    fs::path baseDir = fs::current_path() / "frontend";
    
    // Tự động tìm thư mục frontend nếu chạy từ Visual Studio (out\build\x64-Debug)
    if (!fs::exists(baseDir)) {
        if (fs::exists(fs::current_path() / "../../../frontend")) {
            baseDir = fs::current_path() / "../../../frontend";
        } else if (fs::exists(fs::current_path() / "../../frontend")) {
            baseDir = fs::current_path() / "../../frontend";
        } else if (fs::exists(fs::current_path() / "../frontend")) {
            baseDir = fs::current_path() / "../frontend";
        }
    }

    fs::path targetPath = baseDir / relativePath;

    // Chống Directory Traversal
    std::string canonicalBase = fs::weakly_canonical(baseDir).string();
    std::string canonicalTarget = fs::weakly_canonical(targetPath).string();
    if (canonicalTarget.rfind(canonicalBase, 0) != 0) {
        return crow::response(403, "Access denied");
    }

    if (!fs::exists(targetPath) || fs::is_directory(targetPath)) {
        return crow::response(404, "File not found: " + relativePath);
    }

    std::ifstream file(targetPath, std::ios::binary);
    if (!file) {
        return crow::response(500, "Unable to read file: " + relativePath);
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();

    crow::response res(200, content);
    std::string ext = targetPath.extension().string();

    if (ext == ".html" || ext == ".htm") {
        res.set_header("Content-Type", "text/html; charset=utf-8");
    } else if (ext == ".css") {
        res.set_header("Content-Type", "text/css; charset=utf-8");
    } else if (ext == ".js") {
        res.set_header("Content-Type", "application/javascript; charset=utf-8");
    } else if (ext == ".json") {
        res.set_header("Content-Type", "application/json; charset=utf-8");
    } else if (ext == ".png") {
        res.set_header("Content-Type", "image/png");
    } else if (ext == ".jpg" || ext == ".jpeg") {
        res.set_header("Content-Type", "image/jpeg");
    } else if (ext == ".svg") {
        res.set_header("Content-Type", "image/svg+xml");
    } else {
        res.set_header("Content-Type", "application/octet-stream");
    }

    res.set_header("Access-Control-Allow-Origin", "*");
    return res;
}

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Restaurant Order Management System — Web API Server\n";
    std::cout << "  MVC 2.0 Full Architecture (C++ Backend + Web Frontend)\n";
    std::cout << "========================================================\n\n";

    // -----------------------------------------------------------------
    // 1) Khởi tạo kết nối Database (eager connect để fail-fast).
    // -----------------------------------------------------------------
    try {
        std::cout << "[main] Dang khoi tao ket noi Database...\n";
        auto& db = restaurant::database::Database::getInstance();
        db.connect();
        std::cout << "[main] Database connection established successfully.\n\n";
    } catch (const std::exception& e) {
        std::cerr << "[main] WARNING — Could not connect to MySQL: "
                  << e.what() << "\n";
        std::cerr << "[main] Server will continue to start. Please verify your DB configuration.\n\n";
    } catch (...) {
        std::cerr << "[main] WARNING — Could not connect to MySQL (Unknown error).\n\n";
    }

    // -----------------------------------------------------------------
    // 2) Khởi tạo Crow HTTP Server.
    // -----------------------------------------------------------------
    crow::SimpleApp app;

    // -----------------------------------------------------------------
    // 3) Đăng ký API routes từ TOÀN BỘ 9 Controller.
    // -----------------------------------------------------------------
    restaurant::controller::MenuApiController::registerRoutes(app);
    restaurant::controller::OrderApiController::registerRoutes(app);
    restaurant::controller::TableApiController::registerRoutes(app);
    restaurant::controller::ServiceApiController::registerRoutes(app);
    restaurant::controller::KitchenApiController::registerRoutes(app);
    restaurant::controller::PaymentApiController::registerRoutes(app);
    restaurant::controller::EmployeeApiController::registerRoutes(app);
    restaurant::controller::InventoryApiController::registerRoutes(app);
    restaurant::controller::ReportApiController::registerRoutes(app);

    // Health check endpoint.
    CROW_ROUTE(app, "/api/health")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/) {
        crow::response res(200);
        res.set_header("Content-Type", "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
        res.body = R"({"status":"ok","service":"restaurant-order-management-api","version":"2.0.0"})";
        return res;
    });

    // -----------------------------------------------------------------
    // 4) Đăng ký Static Routes phục vụ các module frontend/.
    // -----------------------------------------------------------------
    // UI-01: Đặt Món
    CROW_ROUTE(app, "/ui-01-order")([]() { return serveFrontendFile("ui-01-order/index.html"); });
    CROW_ROUTE(app, "/ui-01-order/index.html")([]() { return serveFrontendFile("ui-01-order/index.html"); });
    CROW_ROUTE(app, "/ui-01-order/style.css")([]() { return serveFrontendFile("ui-01-order/style.css"); });
    CROW_ROUTE(app, "/ui-01-order/api_client.js")([]() { return serveFrontendFile("ui-01-order/api_client.js"); });

    // UI-02: Sơ Đồ Bàn
    CROW_ROUTE(app, "/ui-02-table")([]() { return serveFrontendFile("ui-02-table/index.html"); });
    CROW_ROUTE(app, "/ui-02-table/index.html")([]() { return serveFrontendFile("ui-02-table/index.html"); });
    CROW_ROUTE(app, "/ui-02-table/style.css")([]() { return serveFrontendFile("ui-02-table/style.css"); });
    CROW_ROUTE(app, "/ui-02-table/api_client.js")([]() { return serveFrontendFile("ui-02-table/api_client.js"); });

    // UI-03: Màn Hình Bếp
    CROW_ROUTE(app, "/ui-03-kitchen")([]() { return serveFrontendFile("ui-03-kitchen/index.html"); });
    CROW_ROUTE(app, "/ui-03-kitchen/index.html")([]() { return serveFrontendFile("ui-03-kitchen/index.html"); });
    CROW_ROUTE(app, "/ui-03-kitchen/style.css")([]() { return serveFrontendFile("ui-03-kitchen/style.css"); });
    CROW_ROUTE(app, "/ui-03-kitchen/api_client.js")([]() { return serveFrontendFile("ui-03-kitchen/api_client.js"); });

    // UI-04: Thu Ngân POS
    CROW_ROUTE(app, "/ui-04-pos")([]() { return serveFrontendFile("ui-04-pos/index.html"); });
    CROW_ROUTE(app, "/ui-04-pos/index.html")([]() { return serveFrontendFile("ui-04-pos/index.html"); });
    CROW_ROUTE(app, "/ui-04-pos/style.css")([]() { return serveFrontendFile("ui-04-pos/style.css"); });
    CROW_ROUTE(app, "/ui-04-pos/api_client.js")([]() { return serveFrontendFile("ui-04-pos/api_client.js"); });

    // UI-05: Quản Trị Admin
    CROW_ROUTE(app, "/ui-05-admin")([]() { return serveFrontendFile("ui-05-admin/index.html"); });
    CROW_ROUTE(app, "/ui-05-admin/index.html")([]() { return serveFrontendFile("ui-05-admin/index.html"); });
    CROW_ROUTE(app, "/ui-05-admin/style.css")([]() { return serveFrontendFile("ui-05-admin/style.css"); });
    CROW_ROUTE(app, "/ui-05-admin/api_client.js")([]() { return serveFrontendFile("ui-05-admin/api_client.js"); });

    // Static routes cho root index & assets
    CROW_ROUTE(app, "/index.html")([]() { return serveFrontendFile("index.html"); });
    CROW_ROUTE(app, "/style.css")([]() { return serveFrontendFile("style.css"); });

    // Root endpoint — Cung cấp Navigation Hub và danh mục API
    CROW_ROUTE(app, "/")
    ([](const crow::request& req) {
        // Nếu trình duyệt request HTML, phục vụ file frontend/index.html
        std::string accept = req.get_header_value("Accept");
        if (accept.find("text/html") != std::string::npos || req.url == "/") {
            auto frontendRes = serveFrontendFile("index.html");
            if (frontendRes.code == 200) {
                return frontendRes;
            }
        }

        // Response JSON cho REST API Client nếu không yêu cầu HTML
        crow::response res(200);
        res.set_header("Content-Type", "application/json");
        res.set_header("Access-Control-Allow-Origin", "*");
        res.body = R"({
  "service": "Restaurant Order Management API",
  "version": "2.0.0",
  "modules": [
    "/api/menu",
    "/api/orders",
    "/api/tables",
    "/api/service",
    "/api/kitchen",
    "/api/payment",
    "/api/employees",
    "/api/inventory",
    "/api/reports"
  ]
})";
        return res;
    });

    // -----------------------------------------------------------------
    // 5) Khởi chạy server đa luồng trên port 8080.
    // -----------------------------------------------------------------
    std::cout << "[main] Starting Crow HTTP Server on port 8080...\n";
    std::cout << "[main] Web App Hub:   http://localhost:8080/\n";
    std::cout << "[main] UI-01 Order:   http://localhost:8080/ui-01-order/\n";
    std::cout << "[main] UI-02 Tables:  http://localhost:8080/ui-02-table/\n";
    std::cout << "[main] UI-03 Kitchen: http://localhost:8080/ui-03-kitchen/\n";
    std::cout << "[main] UI-04 POS:     http://localhost:8080/ui-04-pos/\n";
    std::cout << "[main] UI-05 Admin:   http://localhost:8080/ui-05-admin/\n\n";

    app.port(8080)
       .multithreaded()
       .run();

    // -----------------------------------------------------------------
    // 6) Cleanup khi server dừng.
    // -----------------------------------------------------------------
    restaurant::database::Database::getInstance().disconnect();
    std::cout << "\n[main] Server stopped. Goodbye!\n";

    return 0;
}
