// src/controller/PaymentApiController.cpp
#include "PaymentApiController.h"
#include "../utils/JsonHelper.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

namespace restaurant {
namespace controller {

static void addCorsHeaders(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, PATCH, DELETE, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

void PaymentApiController::registerRoutes(crow::SimpleApp& app) {

    // CORS preflights
    CROW_ROUTE(app, "/api/payment/check-bill")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/api/payment/checkout")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/api/payment/invoices")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/api/payment/invoices/<int>")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/, int) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    // POST /api/payment/check-bill — Tính hóa đơn tạm tính (UC07)
    CROW_ROUTE(app, "/api/payment/check-bill")
        .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto body = nlohmann::json::parse(req.body);
            if (!body.contains("order_id") || !body["order_id"].is_number()) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Thieu order_id.", 400).dump();
                return res;
            }

            int orderId = body["order_id"].get<int>();
            bool isVip = false;
            if (body.contains("is_vip") && body["is_vip"].is_boolean()) {
                isVip = body["is_vip"].get<bool>();
            }

            double discountPercent = 0.0;
            if (body.contains("discount_percent") && body["discount_percent"].is_number()) {
                discountPercent = body["discount_percent"].get<double>();
            } else if (isVip) {
                discountPercent = 10.0; // VIP giam 10%
            }

            repository::OrderRepository orderRepo;
            auto orderOpt = orderRepo.getById(orderId);
            if (!orderOpt.has_value()) {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Khong tim thay don hang.", 404).dump();
                return res;
            }

            auto items = orderRepo.getItems(orderId);
            double subtotal = 0.0;
            for (const auto& item : items) {
                subtotal += (item.getUnitPrice() * item.getQuantity());
            }
            if (subtotal <= 0.0) {
                subtotal = orderOpt.value().getTotalAmount();
            }

            double discountAmount = subtotal * (discountPercent / 100.0);
            double afterDiscount = subtotal - discountAmount;
            double vat = afterDiscount * 0.08; // 8% VAT
            double finalAmount = afterDiscount + vat;

            // Làm tròn 2 chữ số thập phân
            subtotal = std::round(subtotal * 100.0) / 100.0;
            discountAmount = std::round(discountAmount * 100.0) / 100.0;
            vat = std::round(vat * 100.0) / 100.0;
            finalAmount = std::round(finalAmount * 100.0) / 100.0;

            nlohmann::json responseData = {
                {"order_id", orderId},
                {"table_id", orderOpt.value().getTableId()},
                {"subtotal", subtotal},
                {"discount_percent", discountPercent},
                {"discount_amount", discountAmount},
                {"vat_amount", vat},
                {"final_amount", finalAmount},
                {"items_count", items.size()},
                {"items", utils::JsonHelper::orderItemsToJson(items)}
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

    // POST /api/payment/checkout — Xử lý thanh toán và lập hóa đơn (UC08)
    CROW_ROUTE(app, "/api/payment/checkout")
        .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto body = nlohmann::json::parse(req.body);
            if (!body.contains("order_id") || !body["order_id"].is_number()) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Thieu order_id.", 400).dump();
                return res;
            }

            int orderId = body["order_id"].get<int>();
            int cashierId = body.contains("cashier_id") && body["cashier_id"].is_number() 
                ? body["cashier_id"].get<int>() : 1;
            std::string paymentMethod = body.contains("payment_method") && body["payment_method"].is_string() 
                ? body["payment_method"].get<std::string>() : "Cash";

            repository::OrderRepository orderRepo;
            auto orderOpt = orderRepo.getById(orderId);
            if (!orderOpt.has_value()) {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Khong tim thay don hang.", 404).dump();
                return res;
            }

            auto items = orderRepo.getItems(orderId);
            double subtotal = 0.0;
            for (const auto& item : items) {
                subtotal += (item.getUnitPrice() * item.getQuantity());
            }
            if (subtotal <= 0.0) {
                subtotal = orderOpt.value().getTotalAmount();
            }

            double discountAmount = 0.0;
            if (body.contains("discount_amount") && body["discount_amount"].is_number()) {
                discountAmount = body["discount_amount"].get<double>();
            }

            double finalAmount = (subtotal - discountAmount) * 1.08;
            if (body.contains("final_amount") && body["final_amount"].is_number()) {
                finalAmount = body["final_amount"].get<double>();
            }

            double amountPaid = finalAmount;
            if (body.contains("amount_paid") && body["amount_paid"].is_number()) {
                amountPaid = body["amount_paid"].get<double>();
            }

            double changeDue = std::max(0.0, amountPaid - finalAmount);

            // Tạo bản ghi Invoice
            repository::InvoiceRepository invoiceRepo;
            model::Invoice invoice(orderId, cashierId, subtotal, discountAmount, finalAmount, paymentMethod, "Paid");
            model::Invoice createdInvoice = invoiceRepo.add(invoice);

            // Cập nhật trạng thái Order -> Completed
            orderRepo.updateOrderStatus(orderId, "Completed");

            // Cập nhật trạng thái Table -> Available
            int tableId = orderOpt.value().getTableId();
            try {
                repository::TableRepository tableRepo;
                tableRepo.updateStatus(tableId, "Available");
            } catch (...) {}

            nlohmann::json responseData = utils::JsonHelper::invoiceToJson(createdInvoice);
            responseData["amount_paid"] = amountPaid;
            responseData["change_due"] = changeDue;
            responseData["table_id"] = tableId;

            res.code = 201;
            res.set_header("Content-Type", "application/json");
            res.body = nlohmann::json({
                {"success", true},
                {"message", "Thanh toan thanh cong."},
                {"data", responseData}
            }).dump(2);
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    // GET /api/payment/invoices — Lấy danh sách hóa đơn (UC09)
    CROW_ROUTE(app, "/api/payment/invoices")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/) {
        crow::response res;
        addCorsHeaders(res);
        try {
            repository::InvoiceRepository repo;
            auto invoices = repo.getAll();

            nlohmann::json body = {
                {"success", true},
                {"count", invoices.size()},
                {"data", utils::JsonHelper::invoicesToJson(invoices)}
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

    // GET /api/payment/invoices/<int> — Lấy chi tiết hóa đơn
    CROW_ROUTE(app, "/api/payment/invoices/<int>")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/, int id) {
        crow::response res;
        addCorsHeaders(res);
        try {
            repository::InvoiceRepository repo;
            auto invOpt = repo.getById(id);
            if (!invOpt.has_value()) {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = utils::JsonHelper::error("Khong tim thay hoa don ID " + std::to_string(id), 404).dump();
                return res;
            }

            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.body = nlohmann::json({
                {"success", true},
                {"data", utils::JsonHelper::invoiceToJson(invOpt.value())}
            }).dump(2);
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = utils::JsonHelper::error(e.what(), 500).dump();
        }
        return res;
    });

    std::cout << "[PaymentApiController] Routes registered: /api/payment/check-bill, /api/payment/checkout, /api/payment/invoices\n";
}

} // namespace controller
} // namespace restaurant
