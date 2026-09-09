// src/repository/MenuRepository.cpp
// Triển khai truy vấn CSDL cho bảng Category (prepared statements / bind).
#include "MenuRepository.h"
#include "../database/Database.h"

#include <iostream>
#include <stdexcept>

namespace restaurant {
namespace repository {

// ---------------------------------------------------------------------------
// CREATE — Thêm danh mục mới
// ---------------------------------------------------------------------------
int MenuRepository::addCategory(const model::Category& category) {
    auto& session = database::Database::getInstance().getSession();

    // Prepared statement với placeholder `?` để chống SQL Injection.
    mysqlx::SqlStatement stmt = session.sql(
        "INSERT INTO Category (category_name, description) VALUES (?, ?)"
    );

    // Bind giá trị vào placeholder.
    if (category.getDescription().has_value()) {
        stmt.bind(category.getName(), category.getDescription().value());
    } else {
        stmt.bind(category.getName(), nullptr);  // NULL cho cột description.
    }

    mysqlx::SqlResult result = stmt.execute();

    // Lấy auto-generated ID.
    int newId = static_cast<int>(result.getAutoIncrementValue());

    std::cout << "[MenuRepository] Da them Category moi, id=" << newId << "\n";
    return newId;
}

// ---------------------------------------------------------------------------
// READ — Lấy toàn bộ danh mục
// ---------------------------------------------------------------------------
std::vector<model::Category> MenuRepository::getAllCategories() {
    auto& session = database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "SELECT category_id, category_name, description FROM Category ORDER BY category_id"
    );

    mysqlx::SqlResult result = stmt.execute();

    std::vector<model::Category> categories;

    for (mysqlx::Row row : result) {
        int         id   = row[0].get<int>();
        std::string name = row[1].get<std::string>();

        // Cột description có thể NULL.
        std::optional<std::string> desc;
        if (!row[2].isNull()) {
            desc = row[2].get<std::string>();
        }

        categories.emplace_back(id, std::move(name), std::move(desc));
    }

    return categories;
}

// ---------------------------------------------------------------------------
// READ — Tìm danh mục theo ID
// ---------------------------------------------------------------------------
std::optional<model::Category> MenuRepository::getCategoryById(int categoryId) {
    auto& session = database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "SELECT category_id, category_name, description FROM Category WHERE category_id = ?"
    );
    stmt.bind(categoryId);

    mysqlx::SqlResult result = stmt.execute();
    mysqlx::Row row = result.fetchOne();

    if (row.isNull()) {
        return std::nullopt;
    }

    int         id   = row[0].get<int>();
    std::string name = row[1].get<std::string>();

    std::optional<std::string> desc;
    if (!row[2].isNull()) {
        desc = row[2].get<std::string>();
    }

    return model::Category(id, std::move(name), std::move(desc));
}

} // namespace repository
} // namespace restaurant
