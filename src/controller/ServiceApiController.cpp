#include "ServiceApiController.h"

#include <nlohmann/json.hpp>

#include <exception>
#include <stdexcept>
#include <string>

namespace restaurant {
namespace controller {

using json = nlohmann::json;

namespace {

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

    response.body =
        data.dump();

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

ServiceApiController::ServiceApiController(
    ServiceController& serviceController
)
    : serviceController_(
        serviceController
    ) {
}

void ServiceApiController::registerRoutes(
    crow::App<crow::CORSHandler>& app
) {

    CROW_ROUTE(
        app,
        "/api/service/payment"
    )
    .methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req) {

        try {
            json body =
                json::parse(req.body);

            if (!body.contains("orderId")) {
                return errorResponse(
                    400,
                    "orderId is required."
                );
            }

            int orderId =
                body.at("orderId")
                    .get<int>();

            bool success =
                serviceController_
                    .requestPayment(
                        orderId
                    );

            if (!success) {
                return errorResponse(
                    400,
                    "Cannot request payment."
                );
            }

            return jsonResponse(
                200,
                {
                    {"success", true},
                    {"message", "Payment requested."},
                    {"orderId", orderId}
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
        "/api/service/assist"
    )
    .methods(crow::HTTPMethod::POST)
    ([this](const crow::request& req) {

        try {
            json body =
                json::parse(req.body);

            if (!body.contains("tableId")) {
                return errorResponse(
                    400,
                    "tableId is required."
                );
            }

            int tableId =
                body.at("tableId")
                    .get<int>();

            bool success =
                serviceController_
                    .requestAssistance(
                        tableId
                    );

            if (!success) {
                return errorResponse(
                    400,
                    "Cannot request assistance."
                );
            }

            return jsonResponse(
                200,
                {
                    {"success", true},
                    {"message", "Assistance requested."},
                    {"tableId", tableId}
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
        "/api/service/serve/<int>"
    )
    .methods(crow::HTTPMethod::PUT)
    ([this](int orderItemId) {

        try {
            bool success =
                serviceController_
                    .serveFood(
                        orderItemId
                    );

            if (!success) {
                return errorResponse(
                    400,
                    "Cannot serve food."
                );
            }

            return jsonResponse(
                200,
                {
                    {"success", true},
                    {"message", "Food served."},
                    {"orderItemId", orderItemId}
                }
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