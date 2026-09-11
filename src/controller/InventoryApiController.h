#ifndef INVENTORY_API_CONTROLLER_H
#define INVENTORY_API_CONTROLLER_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct InventoryItem {
    std::string itemName;
    double quantity;
    std::string unit;
    double minQuantity;
};

class InventoryApiController {
private:
    std::vector<InventoryItem> inventoryList;

public:
    InventoryApiController();
    json createItem(const json& requestJson);
    json getAllItems() const;
    json getLowStockItems() const;
};

#endif // INVENTORY_API_CONTROLLER_H