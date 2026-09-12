// src/model/Category.cpp
#include "Category.h"

#include <stdexcept>
#include <utility>

namespace restaurant {
namespace model {

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------
Category::Category(std::string name, std::optional<std::string> description)
    : id_(0)
    , name_(std::move(name))
    , description_(std::move(description)) {}

Category::Category(int id, std::string name, std::optional<std::string> description)
    : id_(id)
    , name_(std::move(name))
    , description_(std::move(description)) {}

// ---------------------------------------------------------------------------
// Getters
// ---------------------------------------------------------------------------
int Category::getId() const noexcept {
    return id_;
}

const std::string& Category::getName() const noexcept {
    return name_;
}

const std::optional<std::string>& Category::getDescription() const noexcept {
    return description_;
}

// ---------------------------------------------------------------------------
// Setters
// ---------------------------------------------------------------------------
void Category::setId(int id) {
    if (id < 0) {
        throw std::invalid_argument("Category id khong duoc am.");
    }
    id_ = id;
}

void Category::setName(const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument("Category name khong duoc rong.");
    }
    name_ = name;
}

void Category::setDescription(const std::optional<std::string>& description) {
    description_ = description;
}

} // namespace model
} // namespace restaurant
