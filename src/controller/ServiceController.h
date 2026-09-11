#pragma once

#include "../repository/OrderRepository.h"
#include "../repository/TableRepository.h"

#include <unordered_set>
#include <vector>

namespace restaurant {
namespace controller {

class ServiceController {
public:
    ServiceController(
        repository::OrderRepository& orderRepository,
        repository::TableRepository& tableRepository
    );

    bool requestPayment(int orderId);
    bool cancelPaymentRequest(int orderId);
    bool hasPaymentRequest(int orderId) const;

    bool serveFood(int orderItemId);

    bool requestAssistance(int tableId);
    bool resolveAssistance(int tableId);
    bool hasAssistanceRequest(int tableId) const;

    std::vector<int> getPaymentRequests() const;
    std::vector<int> getAssistanceRequests() const;

private:
    repository::OrderRepository& orderRepository_;
    repository::TableRepository& tableRepository_;

    std::unordered_set<int> paymentRequests_;
    std::unordered_set<int> assistanceRequests_;
};

}
}