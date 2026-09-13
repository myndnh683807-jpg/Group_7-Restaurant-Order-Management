#include "Inventory.h"

#include <stdexcept>
#include <utility>

namespace restaurant {
namespace model {

Inventory::Inventory(
    std::string itemName,
    int quantity,
    std::string unit,
    int minStock
) {
    setItemName(itemName);
    setQuantity(quantity);
    setUnit(unit);
    setMinStock(minStock);
}

Inventory::Inventory(
    int id,
    std::string itemName,
    int quantity,
    std::string unit,
    int minStock,
    std::string lastUpdated
) {
    setId(id);
    setItemName(itemName);
    setQuantity(quantity);
    setUnit(unit);
    setMinStock(minStock);
    setLastUpdated(lastUpdated);
}

int Inventory::getId() const noexcept {
    return id_;
}

const std::string& Inventory::getItemName() const noexcept {
    return itemName_;
}

int Inventory::getQuantity() const noexcept {
    return quantity_;
}

const std::string& Inventory::getUnit() const noexcept {
    return unit_;
}

int Inventory::getMinStock() const noexcept {
    return minStock_;
}

const std::string& Inventory::getLastUpdated() const noexcept {
    return lastUpdated_;
}

void Inventory::setId(int id) {
    if (id < 0) {
        throw std::invalid_argument("Inventory ID cannot be negative.");
    }
    id_ = id;
}

void Inventory::setItemName(const std::string& itemName) {
    if (itemName.empty() || itemName.size() > 100) {
        throw std::invalid_argument("Item name must contain 1 to 100 bytes.");
    }
    itemName_ = itemName;
}

void Inventory::setQuantity(int quantity) {
    if (quantity < 0) {
        throw std::invalid_argument("Quantity cannot be negative.");
    }
    quantity_ = quantity;
}

void Inventory::setUnit(const std::string& unit) {
    if (unit.empty() || unit.size() > 20) {
        throw std::invalid_argument("Unit must contain 1 to 20 bytes.");
    }
    unit_ = unit;
}

void Inventory::setMinStock(int minStock) {
    if (minStock < 0) {
        throw std::invalid_argument("Minimum stock cannot be negative.");
    }
    minStock_ = minStock;
}

void Inventory::setLastUpdated(const std::string& lastUpdated) {
    lastUpdated_ = lastUpdated;
}

}
}