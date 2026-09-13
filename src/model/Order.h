#pragma once

#include <optional>
#include <string>

namespace restaurant {
namespace model {

class Order {
public:
    Order() = default;

    Order(
        int tableId,
        std::optional<int> staffId = std::nullopt,
        std::string orderStatus = "Pending",
        double totalAmount = 0.0
    );

    Order(
        int id,
        int tableId,
        std::optional<int> staffId,
        std::string orderDate,
        std::string orderStatus,
        double totalAmount
    );

    int getId() const noexcept;
    int getTableId() const noexcept;
    const std::optional<int>& getStaffId() const noexcept;
    const std::string& getOrderDate() const noexcept;
    const std::string& getOrderStatus() const noexcept;
    double getTotalAmount() const noexcept;

    void setId(int id);
    void setTableId(int tableId);
    void setStaffId(std::optional<int> staffId);
    void setOrderDate(const std::string& orderDate);
    void setOrderStatus(const std::string& orderStatus);
    void setTotalAmount(double totalAmount);

private:
    int id_{0};
    int tableId_{0};
    std::optional<int> staffId_;
    std::string orderDate_;
    std::string orderStatus_{"Pending"};
    double totalAmount_{0.0};
};

}
}