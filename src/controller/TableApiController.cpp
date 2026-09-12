#include "TableApiController.h"

#include <nlohmann/json.hpp>

#include <exception>
#include <stdexcept>
#include <string>

namespace restaurant {
namespace controller {

using json = nlohmann::json;

namespace {

json tableToJson(
    const model::Table& table
) {
    return {
        {"id", table.getId()},
        {"tableNumber", table.getTableNumber()},
        {"capacity", table.getCapacity()},
        {"status", table.getStatus()}
    };
}

crow::response jsonResponse(
    int statusCode,
    const json& data
) {
    crow::response response;

    response.code = statusCode;
    response.set_header(
        "Content-Type",
        "application/json"
    );

    response.body = data.dump();

    return response;
}

crow::response errorResponse(
    int statusCode,
    const std::string& message
) {
    return jsonResponse(
        statusCode,
        {
            {"success", false},
            {"message", message}
        }
    );
}

}

TableApiController::TableApiController(
    TableController& tableController
)
    : tableController_(tableController) {
}

void TableApiController::registerRoutes(
    crow::App<crow::CORSHandler>& app
) {

    CROW_ROUTE(app, "/api/tables")
    .methods(crow::HTTPMethod::GET)
    ([this]() {

        try {
            auto tables =
                tableController_.getAll();

            json result =
                json::array();

            for (const auto& table : tables) {
                result.push_back(
                    tableToJson(table)
                );
            }

            return jsonResponse(
                200,
                result
            );
        }
        catch (const std::exception& e) {
            return errorResponse(
                500,
                e.what()
            );
        }
    });


    CROW_ROUTE(app, "/api/tables")
    .methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req) {

        try {
            json body =
                json::parse(req.body);

            if (!body.contains("tableNumber") ||
                !body.contains("capacity")) {

                return errorResponse(
                    400,
                    "tableNumber and capacity are required."
                );
            }

            std::string tableNumber =
                body.at("tableNumber")
                    .get<std::string>();

            int capacity =
                body.at("capacity")
                    .get<int>();

            auto table =
                tableController_.add(
                    tableNumber,
                    capacity
                );

            return jsonResponse(
                201,
                {
                    {"success", true},
                    {"table", tableToJson(table)}
                }
            );
        }
        catch (const json::exception& e) {
            return errorResponse(
                400,
                e.what()
            );
        }
        catch (const std::invalid_argument& e) {
            return errorResponse(
                400,
                e.what()
            );
        }
        catch (const std::exception& e) {
            return errorResponse(
                500,
                e.what()
            );
        }
    });


    CROW_ROUTE(
        app,
        "/api/tables/<int>"
    )
    .methods(crow::HTTPMethod::PUT)
    ([this](
        const crow::request& req,
        int tableId
    ) {

        try {
            json body =
                json::parse(req.body);

            if (!body.contains("tableNumber") ||
                !body.contains("capacity")) {

                return errorResponse(
                    400,
                    "tableNumber and capacity are required."
                );
            }

            std::string tableNumber =
                body.at("tableNumber")
                    .get<std::string>();

            int capacity =
                body.at("capacity")
                    .get<int>();

            bool updated =
                tableController_.update(
                    tableId,
                    tableNumber,
                    capacity
                );

            if (!updated) {
                return errorResponse(
                    404,
                    "Table not found."
                );
            }

            auto table =
                tableController_.getById(
                    tableId
                );

            if (!table) {
                return errorResponse(
                    404,
                    "Table not found."
                );
            }

            return jsonResponse(
                200,
                {
                    {"success", true},
                    {"table", tableToJson(*table)}
                }
            );
        }
        catch (const json::exception& e) {
            return errorResponse(
                400,
                e.what()
            );
        }
        catch (const std::invalid_argument& e) {
            return errorResponse(
                400,
                e.what()
            );
        }
        catch (const std::exception& e) {
            return errorResponse(
                500,
                e.what()
            );
        }
    });


    CROW_ROUTE(
        app,
        "/api/tables/<int>/status"
    )
    .methods(crow::HTTPMethod::PUT)
    ([this](
        const crow::request& req,
        int tableId
    ) {

        try {
            json body =
                json::parse(req.body);

            if (!body.contains("status")) {
                return errorResponse(
                    400,
                    "status is required."
                );
            }

            std::string status =
                body.at("status")
                    .get<std::string>();

            bool updated =
                tableController_.updateStatus(
                    tableId,
                    status
                );

            if (!updated) {
                return errorResponse(
                    404,
                    "Table not found."
                );
            }

            return jsonResponse(
                200,
                {
                    {"success", true},
                    {"message", "Table status updated."}
                }
            );
        }
        catch (const json::exception& e) {
            return errorResponse(
                400,
                e.what()
            );
        }
        catch (const std::invalid_argument& e) {
            return errorResponse(
                400,
                e.what()
            );
        }
        catch (const std::exception& e) {
            return errorResponse(
                500,
                e.what()
            );
        }
    });


    CROW_ROUTE(
        app,
        "/api/tables/<int>"
    )
    .methods(crow::HTTPMethod::DELETE)
    ([this](int tableId) {

        try {
            bool removed =
                tableController_.remove(
                    tableId
                );

            if (!removed) {
                return errorResponse(
                    404,
                    "Table not found."
                );
            }

            return jsonResponse(
                200,
                {
                    {"success", true},
                    {"message", "Table deleted."}
                }
            );
        }
        catch (const std::logic_error& e) {
            return errorResponse(
                409,
                e.what()
            );
        }
        catch (const std::invalid_argument& e) {
            return errorResponse(
                400,
                e.what()
            );
        }
        catch (const std::exception& e) {
            return errorResponse(
                500,
                e.what()
            );
        }
    });
}

}
}