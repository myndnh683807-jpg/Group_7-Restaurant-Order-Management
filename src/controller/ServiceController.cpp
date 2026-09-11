#include "ServiceController.h"

#include <algorithm>
#include <stdexcept>

namespace restaurant {
namespace controller {

ServiceController::ServiceController(
    repository::OrderRepository& orderRepository,
    repository::TableRepository& tableRepository
)
    : orderRepository_(orderRepository),
      tableRepository_(tableRepository) {
}

bool ServiceController::requestPayment(int orderId) {
    if (orderId <= 0) {
        throw std::invalid_argument(
            "Order ID must be positive."
        );
    }

    auto order = orderRepository_.getById(orderId);

    if (!order) {
        return false;
    }

    if (paymentRequests_.count(orderId) > 0) {
        return false;
    }

    paymentRequests_.insert(orderId);

    return true;
}

bool ServiceController::cancelPaymentRequest(int orderId) {
    if (orderId <= 0) {
        throw std::invalid_argument(
            "Order ID must be positive."
        );
    }

    return paymentRequests_.erase(orderId) > 0;
}

bool ServiceController::hasPaymentRequest(int orderId) const {
    return paymentRequests_.count(orderId) > 0;
}

bool ServiceController::serveFood(int orderItemId) {
    if (orderItemId <= 0) {
        throw std::invalid_argument(
            "Order item ID must be positive."
        );
    }

    auto item =
        orderRepository_.getItemById(orderItemId);

    if (!item) {
        return false;
    }

    if (item->getItemStatus() == "Served") {
        return false;
    }

    return orderRepository_.updateItemStatus(
        orderItemId,
        "Served"
    );
}

bool ServiceController::requestAssistance(int tableId) {
    if (tableId <= 0) {
        throw std::invalid_argument(
            "Table ID must be positive."
        );
    }

    auto table =
        tableRepository_.getById(tableId);

    if (!table) {
        return false;
    }

    if (assistanceRequests_.count(tableId) > 0) {
        return false;
    }

    assistanceRequests_.insert(tableId);

    return true;
}

bool ServiceController::resolveAssistance(int tableId) {
    if (tableId <= 0) {
        throw std::invalid_argument(
            "Table ID must be positive."
        );
    }

    return assistanceRequests_.erase(tableId) > 0;
}

bool ServiceController::hasAssistanceRequest(
    int tableId
) const {
    return assistanceRequests_.count(tableId) > 0;
}

std::vector<int>
ServiceController::getPaymentRequests() const {
    std::vector<int> result(
        paymentRequests_.begin(),
        paymentRequests_.end()
    );

    std::sort(
        result.begin(),
        result.end()
    );

    return result;
}

std::vector<int>
ServiceController::getAssistanceRequests() const {
    std::vector<int> result(
        assistanceRequests_.begin(),
        assistanceRequests_.end()
    );

    std::sort(
        result.begin(),
        result.end()
    );

    return result;
}

}
}