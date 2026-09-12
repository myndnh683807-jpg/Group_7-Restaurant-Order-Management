#pragma once

#include <string>

namespace restaurant {
namespace model {

class Inventory {
public:
    Inventory() = default;

    Inventory(
        std::string itemName,
        int quantity,
        std::string unit,
        int minStock = 0
    );

    Inventory(
        int id,
        std::string itemName,
        int quantity,
        std::string unit,
        int minStock,
        std::string lastUpdated
    );

    int getId() const noexcept;
    const std::string& getItemName() const noexcept;
    int getQuantity() const noexcept;
    const std::string& getUnit() const noexcept;
    int getMinStock() const noexcept;
    const std::string& getLastUpdated() const noexcept;

    void setId(int id);
    void setItemName(const std::string& itemName);
    void setQuantity(int quantity);
    void setUnit(const std::string& unit);
    void setMinStock(int minStock);
    void setLastUpdated(const std::string& lastUpdated);

private:
    int id_{0};
    std::string itemName_;
    int quantity_{0};
    std::string unit_;
    int minStock_{0};
    std::string lastUpdated_;
};

}
}