#include "Invoice.h"

#include <stdexcept>

namespace restaurant {
namespace model {

Invoice::Invoice(
    int orderId,
    int cashierId,
    double subtotal,
    double discountAmount,
    double finalAmount,
    std::string paymentMethod,
    std::string paymentStatus
) {
    setOrderId(orderId);
    setCashierId(cashierId);
    setSubtotal(subtotal);
    setDiscountAmount(discountAmount);
    setFinalAmount(finalAmount);
    setPaymentMethod(paymentMethod);
    setPaymentStatus(paymentStatus);
}

Invoice::Invoice(
    int id,
    int orderId,
    int cashierId,
    std::string createdAt,
    double subtotal,
    double discountAmount,
    double finalAmount,
    std::string paymentMethod,
    std::string paymentStatus
) {
    setId(id);
    setOrderId(orderId);
    setCashierId(cashierId);
    setCreatedAt(createdAt);
    setSubtotal(subtotal);
    setDiscountAmount(discountAmount);
    setFinalAmount(finalAmount);
    setPaymentMethod(paymentMethod);
    setPaymentStatus(paymentStatus);
}

int Invoice::getId() const noexcept {
    return id_;
}

int Invoice::getOrderId() const noexcept {
    return orderId_;
}

int Invoice::getCashierId() const noexcept {
    return cashierId_;
}

const std::string&
Invoice::getCreatedAt() const noexcept {
    return createdAt_;
}

double Invoice::getSubtotal() const noexcept {
    return subtotal_;
}

double Invoice::getDiscountAmount() const noexcept {
    return discountAmount_;
}

double Invoice::getFinalAmount() const noexcept {
    return finalAmount_;
}

const std::string&
Invoice::getPaymentMethod() const noexcept {
    return paymentMethod_;
}

const std::string&
Invoice::getPaymentStatus() const noexcept {
    return paymentStatus_;
}

void Invoice::setId(int id) {
    if (id < 0) {
        throw std::invalid_argument(
            "Invalid invoice ID."
        );
    }

    id_ = id;
}

void Invoice::setOrderId(int orderId) {
    if (orderId <= 0) {
        throw std::invalid_argument(
            "Invalid order ID."
        );
    }

    orderId_ = orderId;
}

void Invoice::setCashierId(int cashierId) {
    if (cashierId <= 0) {
        throw std::invalid_argument(
            "Invalid cashier ID."
        );
    }

    cashierId_ = cashierId;
}

void Invoice::setCreatedAt(
    const std::string& createdAt
) {
    createdAt_ = createdAt;
}

void Invoice::setSubtotal(
    double subtotal
) {
    if (subtotal < 0) {
        throw std::invalid_argument(
            "Subtotal cannot be negative."
        );
    }

    subtotal_ = subtotal;
}

void Invoice::setDiscountAmount(
    double discountAmount
) {
    if (discountAmount < 0) {
        throw std::invalid_argument(
            "Discount cannot be negative."
        );
    }

    discountAmount_ = discountAmount;
}

void Invoice::setFinalAmount(
    double finalAmount
) {
    if (finalAmount < 0) {
        throw std::invalid_argument(
            "Final amount cannot be negative."
        );
    }

    finalAmount_ = finalAmount;
}

void Invoice::setPaymentMethod(
    const std::string& paymentMethod
) {
    if (paymentMethod != "Cash" &&
        paymentMethod != "Card" &&
        paymentMethod != "QR") {
        throw std::invalid_argument(
            "Invalid payment method."
        );
    }

    paymentMethod_ = paymentMethod;
}

void Invoice::setPaymentStatus(
    const std::string& paymentStatus
) {
    if (paymentStatus != "Pending" &&
        paymentStatus != "Paid" &&
        paymentStatus != "Cancelled") {
        throw std::invalid_argument(
            "Invalid payment status."
        );
    }

    paymentStatus_ = paymentStatus;
}

}
}