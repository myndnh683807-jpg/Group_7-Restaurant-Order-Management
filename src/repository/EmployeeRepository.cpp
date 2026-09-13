#include "EmployeeRepository.h"
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
            "Employee ID must be positive."
        );
    }
}

std::optional<std::string> readNullableString(
    const mysqlx::Value& value
) {
    if (value.isNull()) {
        return std::nullopt;
    }

    return value.get<std::string>();
}

mysqlx::Value bindNullableString(
    const std::optional<std::string>& value
) {
    if (value) {
        return mysqlx::Value(*value);
    }

    return mysqlx::Value(nullptr);
}

model::Employee readEmployee(
    const mysqlx::Row& row
) {
    return model::Employee(
        row[0].get<int>(),
        row[1].get<std::string>(),
        row[2].get<std::string>(),
        row[3].get<std::string>(),
        row[4].get<std::string>(),
        readNullableString(row[5])
    );
}

}

std::vector<model::Employee>
EmployeeRepository::getAll() {
    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlResult result = session.sql(
        "SELECT employee_id, username, password_hash, "
        "full_name, role, phone "
        "FROM Employee "
        "ORDER BY employee_id"
    ).execute();

    std::vector<model::Employee> employees;

    for (mysqlx::Row row : result) {
        employees.push_back(
            readEmployee(row)
        );
    }

    return employees;
}

std::optional<model::Employee>
EmployeeRepository::getById(
    int employeeId
) {
    validateId(employeeId);

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "SELECT employee_id, username, password_hash, "
        "full_name, role, phone "
        "FROM Employee "
        "WHERE employee_id = ?"
    );

    stmt.bind(employeeId);

    mysqlx::SqlResult result =
        stmt.execute();

    mysqlx::Row row =
        result.fetchOne();

    if (row.isNull()) {
        return std::nullopt;
    }

    return readEmployee(row);
}

std::optional<model::Employee>
EmployeeRepository::getByUsername(
    const std::string& username
) {
    if (username.empty()) {
        throw std::invalid_argument(
            "Username cannot be empty."
        );
    }

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "SELECT employee_id, username, password_hash, "
        "full_name, role, phone "
        "FROM Employee "
        "WHERE username = ?"
    );

    stmt.bind(username);

    mysqlx::SqlResult result =
        stmt.execute();

    mysqlx::Row row =
        result.fetchOne();

    if (row.isNull()) {
        return std::nullopt;
    }

    return readEmployee(row);
}

model::Employee EmployeeRepository::add(
    const model::Employee& employee
) {
    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "INSERT INTO Employee "
        "(username, password_hash, full_name, role, phone) "
        "VALUES (?, ?, ?, ?, ?)"
    );

    stmt.bind(
        employee.getUsername(),
        employee.getPasswordHash(),
        employee.getFullName(),
        employee.getRole(),
        bindNullableString(employee.getPhone())
    );

    mysqlx::SqlResult result =
        stmt.execute();

    auto newId =
        result.getAutoIncrementValue();

    if (newId == 0 ||
        newId >
            static_cast<unsigned long long>(
                std::numeric_limits<int>::max()
            )) {

        throw std::runtime_error(
            "Could not read inserted employee ID."
        );
    }

    model::Employee saved = employee;

    saved.setId(
        static_cast<int>(newId)
    );

    return saved;
}

bool EmployeeRepository::update(
    const model::Employee& employee
) {
    validateId(employee.getId());

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "UPDATE Employee "
        "SET username = ?, "
        "full_name = ?, "
        "role = ?, "
        "phone = ? "
        "WHERE employee_id = ?"
    );

    stmt.bind(
        employee.getUsername(),
        employee.getFullName(),
        employee.getRole(),
        bindNullableString(employee.getPhone()),
        employee.getId()
    );

    return stmt
        .execute()
        .getAffectedItemsCount() > 0;
}

bool EmployeeRepository::remove(
    int employeeId
) {
    validateId(employeeId);

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "DELETE FROM Employee "
        "WHERE employee_id = ?"
    );

    stmt.bind(employeeId);

    return stmt
        .execute()
        .getAffectedItemsCount() > 0;
}

bool EmployeeRepository::updatePasswordHash(
    int employeeId,
    const std::string& passwordHash
) {
    validateId(employeeId);

    if (passwordHash.empty() ||
        passwordHash.size() > 255) {

        throw std::invalid_argument(
            "Invalid password hash."
        );
    }

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "UPDATE Employee "
        "SET password_hash = ? "
        "WHERE employee_id = ?"
    );

    stmt.bind(
        passwordHash,
        employeeId
    );

    return stmt
        .execute()
        .getAffectedItemsCount() > 0;
}

}
}