#include "Employee.h"

#include <stdexcept>
#include <utility>

namespace restaurant {
namespace model {

namespace {

void validateRequired(
    const std::string& value,
    std::size_t maxLength,
    const std::string& field
) {
    if (value.empty() || value.size() > maxLength) {
        throw std::invalid_argument(
            field + " must contain 1 to " +
            std::to_string(maxLength) + " bytes."
        );
    }
}

}

Employee::Employee(
    std::string username,
    std::string passwordHash,
    std::string fullName,
    std::string role,
    std::optional<std::string> phone
) {
    setUsername(username);
    setPasswordHash(passwordHash);
    setFullName(fullName);
    setRole(role);
    setPhone(phone);
}

Employee::Employee(
    int id,
    std::string username,
    std::string passwordHash,
    std::string fullName,
    std::string role,
    std::optional<std::string> phone
)
    : Employee(
          std::move(username),
          std::move(passwordHash),
          std::move(fullName),
          std::move(role),
          std::move(phone)
      ) {
    setId(id);
}

int Employee::getId() const noexcept {
    return id_;
}

const std::string& Employee::getUsername() const noexcept {
    return username_;
}

const std::string& Employee::getPasswordHash() const noexcept {
    return passwordHash_;
}

const std::string& Employee::getFullName() const noexcept {
    return fullName_;
}

const std::string& Employee::getRole() const noexcept {
    return role_;
}

const std::optional<std::string>& Employee::getPhone() const noexcept {
    return phone_;
}

void Employee::setId(int id) {
    if (id < 0) {
        throw std::invalid_argument(
            "Employee ID cannot be negative."
        );
    }

    id_ = id;
}

void Employee::setUsername(const std::string& username) {
    validateRequired(username, 50, "Username");
    username_ = username;
}

void Employee::setPasswordHash(
    const std::string& passwordHash
) {
    validateRequired(passwordHash, 255, "Password hash");
    passwordHash_ = passwordHash;
}

void Employee::setFullName(const std::string& fullName) {
    validateRequired(fullName, 100, "Full name");
    fullName_ = fullName;
}

void Employee::setRole(const std::string& role) {
    validateRequired(role, 20, "Role");
    role_ = role;
}

void Employee::setPhone(
    const std::optional<std::string>& phone
) {
    if (phone && phone->size() > 20) {
        throw std::invalid_argument(
            "Phone must not exceed 20 bytes."
        );
    }

    phone_ = phone;
}

}
}