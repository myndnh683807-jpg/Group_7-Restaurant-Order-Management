#include "InventoryRepository.h"
#include "../database/Database.h"

#include <mysqlx/xdevapi.h>

#include <limits>
#include <stdexcept>

namespace restaurant {
namespace repository {

namespace {

void validateId(int id) {
    if (id <= 0) {
        throw std::invalid_argument("Inventory ID must be positive.");
    }
}

model::Inventory readInventory(const mysqlx::Row& row) {
    return model::Inventory(
        row[0].get<int>(),
        row[1].get<std::string>(),
        row[2].get<int>(),
        row[3].get<std::string>(),
        row[4].get<int>(),
        row[5].get<std::string>()
    );
}

}

std::vector<model::Inventory> InventoryRepository::getAll() {
    auto& session = database::Database::getInstance().getSession();

    mysqlx::SqlResult result = session.sql(
        "SELECT inventory_id, item_name, quantity, unit, min_stock, "
        "DATE_FORMAT(last_updated, '%Y-%m-%d %H:%i:%s') AS last_updated "
        "FROM Inventory ORDER BY inventory_id"
    ).execute();

    std::vector<model::Inventory> items;

    for (mysqlx::Row row : result) {
        items.push_back(readInventory(row));
    }

    return items;
}

std::optional<model::Inventory> InventoryRepository::getById(int inventoryId) {
    validateId(inventoryId);

    auto& session = database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "SELECT inventory_id, item_name, quantity, unit, min_stock, "
        "DATE_FORMAT(last_updated, '%Y-%m-%d %H:%i:%s') AS last_updated "
        "FROM Inventory WHERE inventory_id = ?"
    );

    stmt.bind(inventoryId);
    mysqlx::SqlResult result = stmt.execute();
    mysqlx::Row row = result.fetchOne();

    if (row.isNull()) {
        return std::nullopt;
    }

    return readInventory(row);
}

model::Inventory InventoryRepository::add(const model::Inventory& inventory) {
    auto& session = database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "INSERT INTO Inventory (item_name, quantity, unit, min_stock) "
        "VALUES (?, ?, ?, ?)"
    );

    stmt.bind(
        inventory.getItemName(),
        inventory.getQuantity(),
        inventory.getUnit(),
        inventory.getMinStock()
    );

    mysqlx::SqlResult result = stmt.execute();
    auto newId = result.getAutoIncrementValue();

    if (newId == 0 ||
        newId > static_cast<unsigned long long>(std::numeric_limits<int>::max())) {
        throw std::runtime_error("Could not read inserted inventory ID.");
    }

    auto saved = getById(static_cast<int>(newId));
    if (!saved) {
        throw std::runtime_error("Could not read inserted inventory item.");
    }

    return *saved;
}

bool InventoryRepository::update(const model::Inventory& inventory) {
    validateId(inventory.getId());

    auto& session = database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "UPDATE Inventory SET item_name = ?, quantity = ?, unit = ?, min_stock = ? "
        "WHERE inventory_id = ?"
    );

    stmt.bind(
        inventory.getItemName(),
        inventory.getQuantity(),
inventory.getUnit(),
        inventory.getMinStock(),
        inventory.getId()
    );

    return stmt.execute().getAffectedItemsCount() > 0;
}

bool InventoryRepository::remove(int inventoryId) {
    validateId(inventoryId);

    auto& session = database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "DELETE FROM Inventory WHERE inventory_id = ?"
    );

    stmt.bind(inventoryId);
    return stmt.execute().getAffectedItemsCount() > 0;
}

bool InventoryRepository::updateQuantity(int inventoryId, int quantity) {
    validateId(inventoryId);

    if (quantity < 0) {
        throw std::invalid_argument("Quantity cannot be negative.");
    }

    auto& session = database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "UPDATE Inventory SET quantity = ? WHERE inventory_id = ?"
    );

    stmt.bind(quantity, inventoryId);
    return stmt.execute().getAffectedItemsCount() > 0;
}

}
}
