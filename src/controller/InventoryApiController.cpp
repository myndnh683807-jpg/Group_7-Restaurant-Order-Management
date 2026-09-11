#include "InventoryApiController.h"

InventoryApiController::InventoryApiController() {}

json InventoryApiController::createItem(const json& requestJson) {
    json response;
    try {
        if (!requestJson.contains("itemName") || !requestJson.contains("quantity") ||
            !requestJson.contains("unit") || !requestJson.contains("minQuantity")) {
            response["status"] = "error";
            response["message"] = "Missing inventory fields";
            return response;
        }

        std::string itemName = requestJson["itemName"];
        double quantity = requestJson["quantity"].get<double>();
        std::string unit = requestJson["unit"];
        double minQuantity = requestJson["minQuantity"].get<double>();

        for (auto& item : inventoryList) {
            if (item.itemName == itemName) {
                item.quantity += quantity;
                response["status"] = "success";
                response["message"] = "Updated quantity for existing item";
                response["data"] = {{"itemName", item.itemName}, {"quantity", item.quantity}, {"unit", item.unit}, {"minQuantity", item.minQuantity}};
                return response;
            }
        }

        inventoryList.push_back({itemName, quantity, unit, minQuantity});
        response["status"] = "success";
        response["message"] = "Inventory item added successfully";
        response["data"] = {{"itemName", itemName}, {"quantity", quantity}, {"unit", unit}, {"minQuantity", minQuantity}};
    } catch (const std::exception& e) {
        response["status"] = "error";
        response["message"] = e.what();
    }
    return response;
}

json InventoryApiController::getAllItems() const {
    json response;
    json list = json::array();
    for (const auto& item : inventoryList) {
        list.push_back({{"itemName", item.itemName}, {"quantity", item.quantity}, {"unit", item.unit}, {"minQuantity", item.minQuantity}});
    }
    response["status"] = "success";
    response["data"] = list;
    return response;
}

json InventoryApiController::getLowStockItems() const {
    json response;
    json list = json::array();
    for (const auto& item : inventoryList) {
        if (item.quantity <= item.minQuantity) {
            list.push_back({{"itemName", item.itemName}, {"quantity", item.quantity}, {"unit", item.unit}, {"minQuantity", item.minQuantity}});
        }
    }
    response["status"] = "success";
    response["data"] = list;
    return response;
}