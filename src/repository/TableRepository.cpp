#include "TableRepository.h"
#include "../database/Database.h"

#include <mysqlx/xdevapi.h>

#include <limits>
#include <stdexcept>

namespace restaurant {
namespace repository {

namespace {

void validateId(int id) {
    if (id <= 0) {
        throw std::invalid_argument(
            "Table ID must be positive."
        );
    }
}

void validateStatus(const std::string& status) {
    if (status != "Available" &&
        status != "Occupied" &&
        status != "Reserved") {

        throw std::invalid_argument(
            "Invalid table status."
        );
    }
}

void validateTable(const model::Table& table) {
    if (table.getTableNumber().empty() ||
        table.getTableNumber().size() > 20) {

        throw std::invalid_argument(
            "Invalid table number."
        );
    }

    if (table.getCapacity() <= 0) {
        throw std::invalid_argument(
            "Table capacity must be positive."
        );
    }

    validateStatus(table.getStatus());
}

model::Table readTable(const mysqlx::Row& row) {
    return model::Table(
        row[0].get<int>(),
        row[1].get<std::string>(),
        row[2].get<int>(),
        row[3].get<std::string>()
    );
}

}


// ========================================
// READ ALL
// ========================================

std::vector<model::Table> TableRepository::getAll() {
    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlResult result = session.sql(
        "SELECT table_id, table_number, capacity, status "
        "FROM Tables "
        "ORDER BY table_id"
    ).execute();

    std::vector<model::Table> tables;

    for (mysqlx::Row row : result) {
        tables.push_back(readTable(row));
    }

    return tables;
}


// ========================================
// READ BY ID
// ========================================

std::optional<model::Table>
TableRepository::getById(int tableId) {
    validateId(tableId);

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "SELECT table_id, table_number, capacity, status "
        "FROM Tables "
        "WHERE table_id = ?"
    );

    stmt.bind(tableId);

    mysqlx::SqlResult result = stmt.execute();
    mysqlx::Row row = result.fetchOne();

    if (row.isNull()) {
        return std::nullopt;
    }

    return readTable(row);
}


// ========================================
// CREATE
// ========================================

model::Table TableRepository::add(
    const model::Table& table
) {
    validateTable(table);

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "INSERT INTO Tables "
        "(table_number, capacity, status) "
        "VALUES (?, ?, ?)"
    );

    stmt.bind(
        table.getTableNumber(),
        table.getCapacity(),
        table.getStatus()
    );

    mysqlx::SqlResult result = stmt.execute();

    auto newId = result.getAutoIncrementValue();

    if (newId == 0 ||
        newId > static_cast<unsigned long long>(
            std::numeric_limits<int>::max()
        )) {

        throw std::runtime_error(
            "Could not read inserted table ID."
        );
    }

    model::Table saved = table;
    saved.setId(static_cast<int>(newId));

    return saved;
}


// ========================================
// UPDATE
// ========================================

bool TableRepository::update(
    const model::Table& table
) {
    validateId(table.getId());
    validateTable(table);

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "UPDATE Tables "
        "SET table_number = ?, "
        "capacity = ?, "
        "status = ? "
        "WHERE table_id = ?"
    );

    stmt.bind(
        table.getTableNumber(),
        table.getCapacity(),
        table.getStatus(),
        table.getId()
    );

    return stmt.execute().getAffectedItemsCount() > 0;
}


// ========================================
// DELETE
// ========================================

bool TableRepository::remove(int tableId) {
    validateId(tableId);

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "DELETE FROM Tables "
        "WHERE table_id = ?"
    );

    stmt.bind(tableId);

    return stmt.execute().getAffectedItemsCount() > 0;
}


// ========================================
// UPDATE STATUS
// ========================================

bool TableRepository::updateStatus(
    int tableId,
    const std::string& status
) {
    validateId(tableId);
    validateStatus(status);

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "UPDATE Tables "
        "SET status = ? "
        "WHERE table_id = ?"
    );

    stmt.bind(status, tableId);

    return stmt.execute().getAffectedItemsCount() > 0;
}

}
}