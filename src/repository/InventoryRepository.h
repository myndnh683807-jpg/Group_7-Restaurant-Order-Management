#pragma once

#include "../model/Inventory.h"

#include <optional>
#include <vector>

namespace restaurant {
namespace repository {

class InventoryRepository {
public:
    InventoryRepository() = default;
    ~InventoryRepository() = default;

    std::vector<model::Inventory> getAll();

    std::optional<model::Inventory> getById(
        int inventoryId
    );

    model::Inventory add(
        const model::Inventory& inventory
    );

    bool update(
        const model::Inventory& inventory
    );

    bool remove(
        int inventoryId
    );

    bool updateQuantity(
        int inventoryId,
        int quantity
    );
};

}
}