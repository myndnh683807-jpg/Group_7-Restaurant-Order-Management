#pragma once

#include "../model/Order.h"
#include "../model/OrderItem.h"

#include <optional>
#include <string>
#include <vector>

namespace restaurant {
namespace repository {

class OrderRepository {
public:
    OrderRepository() = default;
    ~OrderRepository() = default;

    std::vector<model::Order> getAll();

    std::optional<model::Order> getById(
        int orderId
    );

    model::Order add(
        const model::Order& order
    );

    bool update(
        const model::Order& order
    );

    bool remove(
        int orderId
    );

    bool updateOrderStatus(
        int orderId,
        const std::string& status
    );

    std::vector<model::OrderItem> getItems(
        int orderId
    );

    std::optional<model::OrderItem> getItemById(
        int orderItemId
    );

    model::OrderItem addItem(
        const model::OrderItem& item
    );

    bool updateItem(
        const model::OrderItem& item
    );

    bool removeItem(
        int orderItemId
    );

    bool updateItemStatus(
        int orderItemId,
        const std::string& status
    );

private:
    void recalculateTotal(
        int orderId
    );
};

}
}