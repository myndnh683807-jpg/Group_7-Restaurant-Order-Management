#pragma once

#include <string>

namespace restaurant {
namespace model {

class Invoice {
public:
    Invoice() = default;

    Invoice(
        int orderId,
        int cashierId,
        double subtotal,
        double discountAmount,
        double finalAmount,
        std::string paymentMethod,
        std::string paymentStatus = "Paid"
    );

    Invoice(
        int id,
        int orderId,
        int cashierId,
        std::string createdAt,
        double subtotal,
        double discountAmount,
        double finalAmount,
        std::string paymentMethod,
        std::string paymentStatus
    );

    int getId() const noexcept;
    int getOrderId() const noexcept;
    int getCashierId() const noexcept;
    const std::string& getCreatedAt() const noexcept;
    double getSubtotal() const noexcept;
    double getDiscountAmount() const noexcept;
    double getFinalAmount() const noexcept;
    const std::string& getPaymentMethod() const noexcept;
    const std::string& getPaymentStatus() const noexcept;

    void setId(int id);
    void setOrderId(int orderId);
    void setCashierId(int cashierId);
    void setCreatedAt(const std::string& createdAt);
    void setSubtotal(double subtotal);
    void setDiscountAmount(double discountAmount);
    void setFinalAmount(double finalAmount);
    void setPaymentMethod(const std::string& paymentMethod);
    void setPaymentStatus(const std::string& paymentStatus);

private:
    int id_{0};
    int orderId_{0};
    int cashierId_{0};
    std::string createdAt_;
    double subtotal_{0.0};
    double discountAmount_{0.0};
    double finalAmount_{0.0};
    std::string paymentMethod_;
    std::string paymentStatus_{"Paid"};
};

}
}