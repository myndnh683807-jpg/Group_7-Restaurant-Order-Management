// src/utils/JsonHelper.h
// Tiện ích chuyển đổi C++ Models sang nlohmann::json và chuẩn hóa JSON response.
#pragma once

#include "../model/Category.h"
#include "../model/Employee.h"
#include "../model/Inventory.h"
#include "../model/Invoice.h"
#include "../model/MenuItem.h"
#include "../model/Order.h"
#include "../model/OrderItem.h"
#include "../model/Table.h"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace restaurant {
namespace utils {

class JsonHelper {
public:
    // --- Table ---
    static nlohmann::json tableToJson(const model::Table& table);
    static nlohmann::json tablesToJson(const std::vector<model::Table>& tables);

    // --- Category ---
    static nlohmann::json categoryToJson(const model::Category& category);
    static nlohmann::json categoriesToJson(const std::vector<model::Category>& categories);

    // --- MenuItem ---
    static nlohmann::json menuItemToJson(const model::MenuItem& item);
    static nlohmann::json menuItemsToJson(const std::vector<model::MenuItem>& items);

    // --- Order & OrderItem ---
    static nlohmann::json orderToJson(const model::Order& order);
    static nlohmann::json ordersToJson(const std::vector<model::Order>& orders);
    static nlohmann::json orderItemToJson(const model::OrderItem& item);
    static nlohmann::json orderItemsToJson(const std::vector<model::OrderItem>& items);

    // --- Invoice ---
    static nlohmann::json invoiceToJson(const model::Invoice& invoice);
    static nlohmann::json invoicesToJson(const std::vector<model::Invoice>& invoices);

    // --- Employee (bảo mật: không xuất passwordHash) ---
    static nlohmann::json employeeToJson(const model::Employee& employee);
    static nlohmann::json employeesToJson(const std::vector<model::Employee>& employees);

    // --- Inventory ---
    static nlohmann::json inventoryToJson(const model::Inventory& item);
    static nlohmann::json inventoriesToJson(const std::vector<model::Inventory>& items);

    // --- Response Helpers ---
    static nlohmann::json success(const std::string& message = "Thanh cong");
    static nlohmann::json successData(const nlohmann::json& data, const std::string& message = "");
    static nlohmann::json error(const std::string& message, int code = 400);
};

} // namespace utils
} // namespace restaurant
