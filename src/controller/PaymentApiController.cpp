#include "PaymentApiController.h"

std::string PaymentApiController::handleCheckBill(int orderId, double subtotal, bool isVip) {
    double vat = subtotal * 0.08;
    double discount = isVip ? subtotal * 0.10 : 0.0;
    double total = (subtotal - discount) + vat;
    return "{\"orderId\": " + std::to_string(orderId) + ", \"grandTotal\": " + std::to_string(total) + "}";
}

std::string PaymentApiController::handleProcessPayment(int orderId, double grandTotal, std::string method, double cash) {
    return "{\"status\": \"SUCCESS\", \"orderId\": " + std::to_string(orderId) + ", \"method\": \"" + method + "\"}";
}
