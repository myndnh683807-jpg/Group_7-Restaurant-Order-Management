#include "Order.h"

#include <stdexcept>
#include <utility>

namespace restaurant {
namespace model {

Order::Order(
    int tableId,
    std::optional<int> staffId,
    std::string orderStatus,
    double totalAmount
) {
    setTableId(tableId);
    setStaffId(staffId);
    setOrderStatus(orderStatus);
    setTotalAmount(totalAmount);
}

Order::Order(
    int id,
    int tableId,
    std::optional<int> staffId,
    std::string orderDate,
    std::string orderStatus,
    double totalAmount
) {
    setId(id);
    setTableId(tableId);
    setStaffId(staffId);
    setOrderDate(orderDate);
    setOrderStatus(orderStatus);
    setTotalAmount(totalAmount);
}

int Order::getId() const noexcept {
    return id_;
}

int Order::getTableId() const noexcept {
    return tableId_;
}

const std::optional<int>& Order::getStaffId() const noexcept {
    return staffId_;
}

const std::string& Order::getOrderDate() const noexcept {
    return orderDate_;
}

const std::string& Order::getOrderStatus() const noexcept {
    return orderStatus_;
}

double Order::getTotalAmount() const noexcept {
    return totalAmount_;
}

void Order::setId(int id) {
    if (id < 0) {
        throw std::invalid_argument("Invalid order ID.");
    }

    id_ = id;
}

void Order::setTableId(int tableId) {
    if (tableId <= 0) {
        throw std::invalid_argument("Invalid table ID.");
    }

    tableId_ = tableId;
}

void Order::setStaffId(
    std::optional<int> staffId
) {
    if (staffId && *staffId <= 0) {
        throw std::invalid_argument("Invalid staff ID.");
    }

    staffId_ = staffId;
}

void Order::setOrderDate(
    const std::string& orderDate
) {
    orderDate_ = orderDate;
}

void Order::setOrderStatus(
    const std::string& orderStatus
) {
    if (orderStatus.empty() ||
        orderStatus.size() > 20) {
        throw std::invalid_argument(
            "Invalid order status."
        );
    }

    orderStatus_ = orderStatus;
}

void Order::setTotalAmount(
    double totalAmount
) {
    if (totalAmount < 0) {
        throw std::invalid_argument(
            "Total amount cannot be negative."
        );
    }

    totalAmount_ = totalAmount;
}

}
}