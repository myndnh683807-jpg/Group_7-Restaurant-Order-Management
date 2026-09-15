#include "OrderItem.h"

#include <stdexcept>
#include <utility>

namespace restaurant {
namespace model {

OrderItem::OrderItem(
    int orderId,
    int itemId,
    int quantity,
    double unitPrice,
    std::optional<std::string> specialNote,
    std::string itemStatus
) {
    setOrderId(orderId);
    setItemId(itemId);
    setQuantity(quantity);
    setUnitPrice(unitPrice);
    setSpecialNote(std::move(specialNote));
    setItemStatus(itemStatus);
}

OrderItem::OrderItem(
    int id,
    int orderId,
    int itemId,
    int quantity,
    double unitPrice,
    std::optional<std::string> specialNote,
    std::string itemStatus
) {
    setId(id);
    setOrderId(orderId);
    setItemId(itemId);
    setQuantity(quantity);
    setUnitPrice(unitPrice);
    setSpecialNote(std::move(specialNote));
    setItemStatus(itemStatus);
}

int OrderItem::getId() const noexcept {
    return id_;
}

int OrderItem::getOrderId() const noexcept {
    return orderId_;
}

int OrderItem::getItemId() const noexcept {
    return itemId_;
}

int OrderItem::getQuantity() const noexcept {
    return quantity_;
}

double OrderItem::getUnitPrice() const noexcept {
    return unitPrice_;
}

const std::optional<std::string>&
OrderItem::getSpecialNote() const noexcept {
    return specialNote_;
}

const std::string&
OrderItem::getItemStatus() const noexcept {
    return itemStatus_;
}

void OrderItem::setId(int id) {
    if (id < 0) {
        throw std::invalid_argument(
            "Invalid order item ID."
        );
    }

    id_ = id;
}

void OrderItem::setOrderId(int orderId) {
    if (orderId <= 0) {
        throw std::invalid_argument(
            "Invalid order ID."
        );
    }

    orderId_ = orderId;
}

void OrderItem::setItemId(int itemId) {
    if (itemId <= 0) {
        throw std::invalid_argument(
            "Invalid item ID."
        );
    }

    itemId_ = itemId;
}

void OrderItem::setQuantity(int quantity) {
    if (quantity <= 0) {
        throw std::invalid_argument(
            "Quantity must be positive."
        );
    }

    quantity_ = quantity;
}

void OrderItem::setUnitPrice(
    double unitPrice
) {
    if (unitPrice < 0) {
        throw std::invalid_argument(
            "Unit price cannot be negative."
        );
    }

    unitPrice_ = unitPrice;
}

void OrderItem::setSpecialNote(
    std::optional<std::string> specialNote
) {
    specialNote_ = std::move(specialNote);
}

void OrderItem::setItemStatus(
    const std::string& itemStatus
) {
    if (itemStatus.empty() ||
        itemStatus.size() > 20) {
        throw std::invalid_argument(
            "Invalid item status."
        );
    }

    itemStatus_ = itemStatus;
}

}
}