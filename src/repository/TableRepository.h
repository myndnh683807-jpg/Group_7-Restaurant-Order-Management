#pragma once

#include "../model/Table.h"

#include <optional>
#include <string>
#include <vector>

namespace restaurant {
namespace repository {

class TableRepository {
public:
    TableRepository() = default;
    ~TableRepository() = default;

    std::vector<model::Table> getAll();

    std::optional<model::Table> getById(int tableId);

    model::Table add(const model::Table& table);

    bool update(const model::Table& table);

    bool remove(int tableId);

    bool updateStatus(
        int tableId,
        const std::string& status
    );
};

}
}