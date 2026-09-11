#pragma once

#include "../model/Table.h"
#include "../repository/TableRepository.h"

#include <optional>
#include <string>
#include <vector>

namespace restaurant {
namespace controller {

class TableController {
public:
    explicit TableController(
        repository::TableRepository& tableRepository
    );

    std::vector<model::Table> getAll();

    std::optional<model::Table> getById(int tableId);

    std::vector<model::Table> getAvailableTables();

    model::Table add(
        const std::string& tableNumber,
        int capacity
    );

    bool update(
        int tableId,
        const std::string& tableNumber,
        int capacity
    );

    bool remove(int tableId);

    bool updateStatus(
        int tableId,
        const std::string& status
    );

private:
    repository::TableRepository& tableRepository_;
};

}
}