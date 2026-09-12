#include "Table.h"

#include <stdexcept>
#include <utility>

namespace restaurant {
namespace model {

Table::Table(
    std::string tableNumber,
    int capacity,
    std::string status
) {
    setTableNumber(tableNumber);
    setCapacity(capacity);
    setStatus(status);
}

Table::Table(
    int id,
    std::string tableNumber,
    int capacity,
    std::string status
)
    : Table(
          std::move(tableNumber),
          capacity,
          std::move(status)
      ) {
    setId(id);
}

int Table::getId() const noexcept {
    return id_;
}

const std::string& Table::getTableNumber() const noexcept {
    return tableNumber_;
}

int Table::getCapacity() const noexcept {
    return capacity_;
}

const std::string& Table::getStatus() const noexcept {
    return status_;
}

void Table::setId(int id) {
    if (id < 0) {
        throw std::invalid_argument(
            "Table ID cannot be negative"
        );
    }

    id_ = id;
}

void Table::setTableNumber(
    const std::string& tableNumber
) {
    if (tableNumber.empty() ||
        tableNumber.size() > 20) {

        throw std::invalid_argument(
            "Table number must contain 1 to 20 bytes"
        );
    }

    tableNumber_ = tableNumber;
}

void Table::setCapacity(int capacity) {
    if (capacity <= 0) {
        throw std::invalid_argument(
            "Table capacity must be positive"
        );
    }

    capacity_ = capacity;
}

void Table::setStatus(const std::string& status) {
    if (status != "Available" &&
        status != "Occupied" &&
        status != "Reserved") {

        throw std::invalid_argument(
            "Invalid table status"
        );
    }

    status_ = status;
}

}
}