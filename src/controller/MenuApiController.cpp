// src/controller/MenuApiController.cpp
#include "MenuApiController.h"
#include "../database/Database.h"
#include "../utils/JsonHelper.h"

#include <iostream>
#include <stdexcept>

namespace restaurant {
namespace controller {

static void addCorsHeaders(crow::response& res) {
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

nlohmann::json MenuApiController::categoryToJson(const model::Category& category) {
    return utils::JsonHelper::categoryToJson(category);
}

nlohmann::json MenuApiController::categoriesToJson(const std::vector<model::Category>& categories) {
    return utils::JsonHelper::categoriesToJson(categories);
}

nlohmann::json MenuApiController::menuItemToJson(const model::MenuItem& item) {
    return utils::JsonHelper::menuItemToJson(item);
}

nlohmann::json MenuApiController::menuItemsToJson(const std::vector<model::MenuItem>& items) {
    return utils::JsonHelper::menuItemsToJson(items);
}

nlohmann::json MenuApiController::errorJson(const std::string& message, int code) {
    return utils::JsonHelper::error(message, code);
}

std::vector<model::MenuItem> MenuApiController::fetchAllMenuItems(std::optional<int> categoryId) {
    auto& session = database::Database::getInstance().getSession();
    std::vector<model::MenuItem> items;

    std::string sqlQuery = "SELECT item_id, category_id, item_name, description, price, image_url, is_available FROM MenuItem";
    if (categoryId.has_value()) {
        sqlQuery += " WHERE category_id = ?";
    }
    sqlQuery += " ORDER BY category_id, item_id";

    mysqlx::SqlStatement stmt = session.sql(sqlQuery);
    if (categoryId.has_value()) {
        stmt.bind(categoryId.value());
    }

    mysqlx::SqlResult result = stmt.execute();
    for (mysqlx::Row row : result) {
        int id = row[0].get<int>();
        int catId = row[1].get<int>();
        std::string name = row[2].get<std::string>();
        std::optional<std::string> desc = row[3].isNull() ? std::nullopt : std::make_optional(row[3].get<std::string>());
        double price = row[4].get<double>();
        std::optional<std::string> img = row[5].isNull() ? std::nullopt : std::make_optional(row[5].get<std::string>());
        bool isAvail = row[6].isNull() ? true : (row[6].get<int>() != 0);

        items.emplace_back(id, catId, std::move(name), std::move(desc), price, std::move(img), isAvail);
    }
    return items;
}

std::optional<model::MenuItem> MenuApiController::fetchMenuItemById(int itemId) {
    auto& session = database::Database::getInstance().getSession();
    mysqlx::SqlStatement stmt = session.sql(
        "SELECT item_id, category_id, item_name, description, price, image_url, is_available FROM MenuItem WHERE item_id = ?"
    );
    stmt.bind(itemId);
    mysqlx::SqlResult result = stmt.execute();
    mysqlx::Row row = result.fetchOne();

    if (row.isNull()) {
        return std::nullopt;
    }

    int id = row[0].get<int>();
    int catId = row[1].get<int>();
    std::string name = row[2].get<std::string>();
    std::optional<std::string> desc = row[3].isNull() ? std::nullopt : std::make_optional(row[3].get<std::string>());
    double price = row[4].get<double>();
    std::optional<std::string> img = row[5].isNull() ? std::nullopt : std::make_optional(row[5].get<std::string>());
    bool isAvail = row[6].isNull() ? true : (row[6].get<int>() != 0);

    return model::MenuItem(id, catId, std::move(name), std::move(desc), price, std::move(img), isAvail);
}

void MenuApiController::registerRoutes(crow::SimpleApp& app) {

    // OPTIONS preflight
    CROW_ROUTE(app, "/api/menu")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/api/menu/items")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    CROW_ROUTE(app, "/api/menu/items/<int>")
        .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request& /*req*/, int) {
        crow::response res(204);
        addCorsHeaders(res);
        return res;
    });

    // GET /api/menu — Lấy toàn bộ danh mục
    CROW_ROUTE(app, "/api/menu")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/) {
        crow::response res;
        addCorsHeaders(res);
        try {
            repository::MenuRepository repo;
            auto categories = repo.getAllCategories();

            nlohmann::json body;
            body["success"] = true;
            body["data"]    = categoriesToJson(categories);
            body["count"]   = categories.size();

            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.body = body.dump(2);
        } catch (const std::exception& e) {
            std::cerr << "[MenuApiController] GET /api/menu error: " << e.what() << "\n";
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("Internal server error: " + std::string(e.what()), 500).dump();
        }
        return res;
    });

    // GET /api/menu/<id> — Tìm danh mục theo ID
    CROW_ROUTE(app, "/api/menu/<int>")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/, int id) {
        crow::response res;
        addCorsHeaders(res);
        if (id <= 0) {
            res.code = 400;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson("ID khong hop le.", 400).dump();
            return res;
        }
        try {
            repository::MenuRepository repo;
            auto category = repo.getCategoryById(id);
            if (category.has_value()) {
                nlohmann::json body;
                body["success"] = true;
                body["data"] = categoryToJson(category.value());
                res.code = 200;
                res.set_header("Content-Type", "application/json");
                res.body = body.dump(2);
            } else {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Khong tim thay danh muc voi ID = " + std::to_string(id), 404).dump();
            }
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson(e.what(), 500).dump();
        }
        return res;
    });

    // POST /api/menu — Tạo danh mục mới
    CROW_ROUTE(app, "/api/menu")
        .methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto jsonBody = nlohmann::json::parse(req.body);
            if (!jsonBody.contains("category_name") || !jsonBody["category_name"].is_string() || jsonBody["category_name"].get<std::string>().empty()) {
                res.code = 400;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Truong 'category_name' la bat buoc.", 400).dump();
                return res;
            }
            std::string name = jsonBody["category_name"].get<std::string>();
            std::optional<std::string> description = std::nullopt;
            if (jsonBody.contains("description") && jsonBody["description"].is_string()) {
                description = jsonBody["description"].get<std::string>();
            }

            model::Category category(name, description);
            repository::MenuRepository repo;
            int newId = repo.addCategory(category);

            if (newId > 0) {
                auto created = repo.getCategoryById(newId);
                nlohmann::json body;
                body["success"] = true;
                body["message"] = "Tao danh muc thanh cong.";
                body["data"] = created.has_value() ? categoryToJson(created.value()) : nlohmann::json({{"category_id", newId}, {"category_name", name}});
                res.code = 201;
                res.set_header("Content-Type", "application/json");
                res.body = body.dump(2);
            } else {
                res.code = 500;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Khong the tao danh muc.", 500).dump();
            }
        } catch (const std::exception& e) {
            res.code = 400;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson(e.what(), 400).dump();
        }
        return res;
    });

    // GET /api/menu/items — Lấy danh sách toàn bộ món ăn
    CROW_ROUTE(app, "/api/menu/items")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& req) {
        crow::response res;
        addCorsHeaders(res);
        try {
            std::optional<int> catId = std::nullopt;
            if (req.url_params.get("category_id") != nullptr) {
                catId = std::stoi(req.url_params.get("category_id"));
            }

            auto items = fetchAllMenuItems(catId);
            nlohmann::json body;
            body["success"] = true;
            body["data"]    = menuItemsToJson(items);
            body["count"]   = items.size();

            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.body = body.dump(2);
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson(e.what(), 500).dump();
        }
        return res;
    });

    // GET /api/menu/items/<id> — Chi tiết món ăn
    CROW_ROUTE(app, "/api/menu/items/<int>")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/, int id) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto item = fetchMenuItemById(id);
            if (item.has_value()) {
                nlohmann::json body;
                body["success"] = true;
                body["data"] = menuItemToJson(item.value());
                res.code = 200;
                res.set_header("Content-Type", "application/json");
                res.body = body.dump(2);
            } else {
                res.code = 404;
                res.set_header("Content-Type", "application/json");
                res.body = errorJson("Khong tim thay mon an voi ID = " + std::to_string(id), 404).dump();
            }
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson(e.what(), 500).dump();
        }
        return res;
    });

    // GET /api/menu/categories/<id>/items — Món ăn theo danh mục
    CROW_ROUTE(app, "/api/menu/categories/<int>/items")
        .methods(crow::HTTPMethod::GET)
    ([](const crow::request& /*req*/, int categoryId) {
        crow::response res;
        addCorsHeaders(res);
        try {
            auto items = fetchAllMenuItems(categoryId);
            nlohmann::json body;
            body["success"] = true;
            body["data"] = menuItemsToJson(items);
            body["count"] = items.size();

            res.code = 200;
            res.set_header("Content-Type", "application/json");
            res.body = body.dump(2);
        } catch (const std::exception& e) {
            res.code = 500;
            res.set_header("Content-Type", "application/json");
            res.body = errorJson(e.what(), 500).dump();
        }
        return res;
    });

    std::cout << "[MenuApiController] Routes registered: /api/menu, /api/menu/items\n";
}

} // namespace controller
} // namespace restaurant
