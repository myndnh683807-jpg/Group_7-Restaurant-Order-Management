#pragma once

#include <optional>
#include <string>

namespace restaurant {
namespace model {

class OrderItem {
public:
    OrderItem() = default;

    OrderItem(
        int orderId,
        int itemId,
        int quantity,
        double unitPrice,
        std::optional<std::string> specialNote = std::nullopt,
        std::string itemStatus = "Pending"
    );

    OrderItem(
        int id,
        int orderId,
        int itemId,
        int quantity,
        double unitPrice,
        std::optional<std::string> specialNote,
        std::string itemStatus
    );

    int getId() const noexcept;
    int getOrderId() const noexcept;
    int getItemId() const noexcept;
    int getQuantity() const noexcept;
    double getUnitPrice() const noexcept;
    const std::optional<std::string>& getSpecialNote() const noexcept;
    const std::string& getItemStatus() const noexcept;

    void setId(int id);
    void setOrderId(int orderId);
    void setItemId(int itemId);
    void setQuantity(int quantity);
    void setUnitPrice(double unitPrice);
    void setSpecialNote(std::optional<std::string> specialNote);
    void setItemStatus(const std::string& itemStatus);

private:
    int id_{0};
    int orderId_{0};
    int itemId_{0};
    int quantity_{1};
    double unitPrice_{0.0};
    std::optional<std::string> specialNote_;
    std::string itemStatus_{"Pending"};
};

}
}