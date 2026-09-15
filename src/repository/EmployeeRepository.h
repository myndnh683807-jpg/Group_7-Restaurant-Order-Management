#pragma once

#include "../model/Employee.h"

#include <optional>
#include <string>
#include <vector>

namespace restaurant {
namespace repository {

class EmployeeRepository {
public:
    EmployeeRepository() = default;
    ~EmployeeRepository() = default;

    std::vector<model::Employee> getAll();

    std::optional<model::Employee> getById(
        int employeeId
    );

    std::optional<model::Employee> getByUsername(
        const std::string& username
    );

    model::Employee add(
        const model::Employee& employee
    );

    bool update(
        const model::Employee& employee
    );

    bool remove(
        int employeeId
    );

    bool updatePasswordHash(
        int employeeId,
        const std::string& passwordHash
    );
};

}
}