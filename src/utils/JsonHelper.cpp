// src/utils/JsonHelper.cpp
#include "JsonHelper.h"

namespace restaurant {
namespace utils {

// --- Table ---
nlohmann::json JsonHelper::tableToJson(const model::Table& table) {
    return {
        {"id", table.getId()},
        {"table_id", table.getId()},
        {"table_number", table.getTableNumber()},
        {"tableNumber", table.getTableNumber()},
        {"capacity", table.getCapacity()},
        {"status", table.getStatus()}
    };
}

nlohmann::json JsonHelper::tablesToJson(const std::vector<model::Table>& tables) {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& t : tables) {
        arr.push_back(tableToJson(t));
    }
    return arr;
}

// --- Category ---
nlohmann::json JsonHelper::categoryToJson(const model::Category& category) {
    nlohmann::json j;
    j["category_id"] = category.getId();
    j["id"] = category.getId();
    j["category_name"] = category.getName();
    j["name"] = category.getName();
    j["description"] = category.getDescription().has_value() 
        ? nlohmann::json(category.getDescription().value()) 
        : nlohmann::json(nullptr);
    return j;
}

nlohmann::json JsonHelper::categoriesToJson(const std::vector<model::Category>& categories) {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& c : categories) {
        arr.push_back(categoryToJson(c));
    }
    return arr;
}

// --- MenuItem ---
nlohmann::json JsonHelper::menuItemToJson(const model::MenuItem& item) {
    nlohmann::json j;
    j["item_id"] = item.getId();
    j["id"] = item.getId();
    j["category_id"] = item.getCategoryId();
    j["item_name"] = item.getName();
    j["name"] = item.getName();
    j["description"] = item.getDescription().has_value()
        ? nlohmann::json(item.getDescription().value())
        : nlohmann::json(nullptr);
    j["price"] = item.getPrice();
    j["image_url"] = item.getImageUrl().has_value()
        ? nlohmann::json(item.getImageUrl().value())
        : nlohmann::json("food_default.jpg");
    j["is_available"] = item.getIsAvailable();
    return j;
}

nlohmann::json JsonHelper::menuItemsToJson(const std::vector<model::MenuItem>& items) {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& item : items) {
        arr.push_back(menuItemToJson(item));
    }
    return arr;
}

// --- Order & OrderItem ---
nlohmann::json JsonHelper::orderToJson(const model::Order& order) {
    nlohmann::json j;
    j["order_id"] = order.getId();
    j["id"] = order.getId();
    j["table_id"] = order.getTableId();
    j["staff_id"] = order.getStaffId().has_value() 
        ? nlohmann::json(order.getStaffId().value()) 
        : nlohmann::json(nullptr);
    j["order_date"] = order.getOrderDate();
    j["order_status"] = order.getOrderStatus();
    j["total_amount"] = order.getTotalAmount();
    return j;
}

nlohmann::json JsonHelper::ordersToJson(const std::vector<model::Order>& orders) {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& o : orders) {
        arr.push_back(orderToJson(o));
    }
    return arr;
}

nlohmann::json JsonHelper::orderItemToJson(const model::OrderItem& item) {
    nlohmann::json j;
    j["order_item_id"] = item.getId();
    j["id"] = item.getId();
    j["order_id"] = item.getOrderId();
    j["item_id"] = item.getItemId();
    j["quantity"] = item.getQuantity();
    j["unit_price"] = item.getUnitPrice();
    j["special_note"] = item.getSpecialNote().has_value()
        ? nlohmann::json(item.getSpecialNote().value())
        : nlohmann::json(nullptr);
    j["item_status"] = item.getItemStatus();
    return j;
}

nlohmann::json JsonHelper::orderItemsToJson(const std::vector<model::OrderItem>& items) {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& item : items) {
        arr.push_back(orderItemToJson(item));
    }
    return arr;
}

// --- Invoice ---
nlohmann::json JsonHelper::invoiceToJson(const model::Invoice& invoice) {
    return {
        {"invoice_id", invoice.getId()},
        {"id", invoice.getId()},
        {"order_id", invoice.getOrderId()},
        {"cashier_id", invoice.getCashierId()},
        {"created_at", invoice.getCreatedAt()},
        {"subtotal", invoice.getSubtotal()},
        {"discount_amount", invoice.getDiscountAmount()},
        {"final_amount", invoice.getFinalAmount()},
        {"payment_method", invoice.getPaymentMethod()},
        {"payment_status", invoice.getPaymentStatus()}
    };
}

nlohmann::json JsonHelper::invoicesToJson(const std::vector<model::Invoice>& invoices) {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& inv : invoices) {
        arr.push_back(invoiceToJson(inv));
    }
    return arr;
}

// --- Employee (bảo mật: không xuất passwordHash) ---
nlohmann::json JsonHelper::employeeToJson(const model::Employee& employee) {
    nlohmann::json j;
    j["employee_id"] = employee.getId();
    j["id"] = employee.getId();
    j["username"] = employee.getUsername();
    j["full_name"] = employee.getFullName();
    j["fullName"] = employee.getFullName();
    j["role"] = employee.getRole();
    j["phone"] = employee.getPhone().has_value()
        ? nlohmann::json(employee.getPhone().value())
        : nlohmann::json(nullptr);
    return j;
}

nlohmann::json JsonHelper::employeesToJson(const std::vector<model::Employee>& employees) {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& emp : employees) {
        arr.push_back(employeeToJson(emp));
    }
    return arr;
}

// --- Inventory ---
nlohmann::json JsonHelper::inventoryToJson(const model::Inventory& item) {
    return {
        {"inventory_id", item.getId()},
        {"id", item.getId()},
        {"item_name", item.getItemName()},
        {"itemName", item.getItemName()},
        {"quantity", item.getQuantity()},
        {"unit", item.getUnit()},
        {"min_stock", item.getMinStock()},
        {"minStock", item.getMinStock()},
        {"last_updated", item.getLastUpdated()}
    };
}

nlohmann::json JsonHelper::inventoriesToJson(const std::vector<model::Inventory>& items) {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& it : items) {
        arr.push_back(inventoryToJson(it));
    }
    return arr;
}

// --- Response Helpers ---
nlohmann::json JsonHelper::success(const std::string& message) {
    return {
        {"success", true},
        {"message", message}
    };
}

nlohmann::json JsonHelper::successData(const nlohmann::json& data, const std::string& message) {
    nlohmann::json res = {
        {"success", true},
        {"data", data}
    };
    if (!message.empty()) {
        res["message"] = message;
    }
    return res;
}

nlohmann::json JsonHelper::error(const std::string& message, int code) {
    return {
        {"success", false},
        {"error", message},
        {"message", message},
        {"code", code}
    };
}

} // namespace utils
} // namespace restaurant
