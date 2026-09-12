#pragma once

#include <string>

namespace restaurant {
namespace model {

class Table {
public:
    Table() = default;

    Table(
        std::string tableNumber,
        int capacity = 4,
        std::string status = "Available"
    );

    Table(
        int id,
        std::string tableNumber,
        int capacity,
        std::string status
    );

    int getId() const noexcept;
    const std::string& getTableNumber() const noexcept;
    int getCapacity() const noexcept;
    const std::string& getStatus() const noexcept;

    void setId(int id);
    void setTableNumber(const std::string& tableNumber);
    void setCapacity(int capacity);
    void setStatus(const std::string& status);

private:
    int id_{0};
    std::string tableNumber_;
    int capacity_{4};
    std::string status_{"Available"};
};

}
}