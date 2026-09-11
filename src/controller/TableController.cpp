#include "TableController.h"

#include <stdexcept>

namespace restaurant {
namespace controller {

TableController::TableController(
    repository::TableRepository& tableRepository
)
    : tableRepository_(tableRepository) {
}

std::vector<model::Table> TableController::getAll() {
    return tableRepository_.getAll();
}

std::optional<model::Table>
TableController::getById(int tableId) {
    if (tableId <= 0) {
        throw std::invalid_argument(
            "Table ID must be positive."
        );
    }

    return tableRepository_.getById(tableId);
}

std::vector<model::Table>
TableController::getAvailableTables() {
    std::vector<model::Table> available;

    for (const auto& table : tableRepository_.getAll()) {
        if (table.getStatus() == "Available") {
            available.push_back(table);
        }
    }

    return available;
}

model::Table TableController::add(
    const std::string& tableNumber,
    int capacity
) {
    model::Table table(
        tableNumber,
        capacity,
        "Available"
    );

    return tableRepository_.add(table);
}

bool TableController::update(
    int tableId,
    const std::string& tableNumber,
    int capacity
) {
    auto existing = getById(tableId);

    if (!existing) {
        return false;
    }

    existing->setTableNumber(tableNumber);
    existing->setCapacity(capacity);

    return tableRepository_.update(*existing);
}

bool TableController::remove(int tableId) {
    auto existing = getById(tableId);

    if (!existing) {
        return false;
    }

    if (existing->getStatus() != "Available") {
        throw std::logic_error(
            "Only available tables can be deleted."
        );
    }

    return tableRepository_.remove(tableId);
}

bool TableController::updateStatus(
    int tableId,
    const std::string& status
) {
    auto existing = getById(tableId);

    if (!existing) {
        return false;
    }

    existing->setStatus(status);

    if (existing->getStatus() == status) {
        return tableRepository_.updateStatus(
            tableId,
            status
        );
    }

    return false;
}

}
}