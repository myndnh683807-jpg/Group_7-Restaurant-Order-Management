#include "TableApiController.h"

#include "../utils/JsonHelper.h"

#include <nlohmann/json.hpp>

#include <exception>
#include <optional>
#include <string>
#include <vector>

namespace restaurant {
namespace controller {

using json = nlohmann::json;


namespace {

crow::response jsonResponse(
    int statusCode,
    const json& data
) {
    crow::response response;

    response.code =
        statusCode;

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


bool isActiveOrder(
    const model::Order& order
) {
    return
        order.getOrderStatus() !=
            "Completed"
        &&
        order.getOrderStatus() !=
            "Cancelled";
}


std::optional<model::Order>
findActiveOrder(
    int tableId,
    const std::vector<model::Order>& orders
) {
    std::optional<model::Order>
        result;


    for (const auto& order : orders) {

        if (
            order.getTableId() !=
            tableId
        ) {
            continue;
        }


        if (!isActiveOrder(order)) {
            continue;
        }


        if (
            !result ||
            order.getId() >
            result->getId()
        ) {
            result = order;
        }
    }


    return result;
}

}


TableApiController::TableApiController(
    repository::TableRepository&
        tableRepository,

    repository::OrderRepository&
        orderRepository
)
    : tableRepository_(
          tableRepository
      ),
      orderRepository_(
          orderRepository
      ) {
}


void TableApiController::registerRoutes(
    crow::App<crow::CORSHandler>& app
) {

    CROW_ROUTE(
        app,
        "/api/tables"
    )
    .methods(
        crow::HTTPMethod::GET
    )
    ([this]() {

        try {

            auto tables =
                tableRepository_.getAll();

            auto orders =
                orderRepository_.getAll();


            json result =
                json::array();


            for (
                const auto& table :
                tables
            ) {

                json data =
                    utils::JsonHelper::
                        tableToJson(
                            table
                        );


                auto activeOrder =
                    findActiveOrder(
                        table.getId(),
                        orders
                    );


                if (activeOrder) {

                    data["activeOrderId"] =
                        activeOrder->getId();

                    data["orderStatus"] =
                        activeOrder
                            ->getOrderStatus();

                    data["orderDate"] =
                        activeOrder
                            ->getOrderDate();

                    data["totalAmount"] =
                        activeOrder
                            ->getTotalAmount();

                } else {

                    data["activeOrderId"] =
                        nullptr;

                    data["orderStatus"] =
                        nullptr;

                    data["orderDate"] =
                        nullptr;

                    data["totalAmount"] =
                        0;
                }


                result.push_back(
                    data
                );
            }


            return jsonResponse(
                200,
                result
            );

        } catch (
            const std::exception& e
        ) {

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
    .methods(
        crow::HTTPMethod::PUT
    )
    ([this](
        const crow::request& req,
        int tableId
    ) {

        try {

            json body =
                json::parse(
                    req.body
                );


            if (
                !body.contains(
                    "status"
                )
            ) {

                return errorResponse(
                    400,
                    "status is required."
                );
            }


            std::string status =
                body.at(
                    "status"
                ).get<std::string>();


            auto table =
                tableRepository_
                    .getById(
                        tableId
                    );


            if (!table) {

                return errorResponse(
                    404,
                    "Table not found."
                );
            }


            bool updated =
                tableRepository_
                    .updateStatus(
                        tableId,
                        status
                    );


            if (!updated) {

                return errorResponse(
                    400,
                    "Table status was not changed."
                );
            }


            return jsonResponse(
                200,
                {
                    {"success", true},
                    {"message",
                     "Table status updated."}
                }
            );

        } catch (
            const json::exception& e
        ) {

            return errorResponse(
                400,
                e.what()
            );

        } catch (
            const std::invalid_argument& e
        ) {

            return errorResponse(
                400,
                e.what()
            );

        } catch (
            const std::exception& e
        ) {

            return errorResponse(
                500,
                e.what()
            );
        }
    });


    CROW_ROUTE(
        app,
        "/api/tables/<int>/order"
    )
    .methods(
        crow::HTTPMethod::GET
    )
    ([this](
        int tableId
    ) {

        try {

            auto table =
                tableRepository_
                    .getById(
                        tableId
                    );


            if (!table) {

                return errorResponse(
                    404,
                    "Table not found."
                );
            }


            auto orders =
                orderRepository_
                    .getAll();


            auto order =
                findActiveOrder(
                    tableId,
                    orders
                );


            if (!order) {

                return errorResponse(
                    404,
                    "No active order for this table."
                );
            }


            auto items =
                orderRepository_
                    .getItems(
                        order->getId()
                    );


            json itemList =
                json::array();


            for (
                const auto& item :
                items
            ) {

                json itemData = {
                    {
                        "orderItemId",
                        item.getId()
                    },
                    {
                        "itemId",
                        item.getItemId()
                    },
                    {
                        "quantity",
                        item.getQuantity()
                    },
                    {
                        "unitPrice",
                        item.getUnitPrice()
                    },
                    {
                        "status",
                        item.getItemStatus()
                    }
                };


                if (
                    item.getSpecialNote()
                ) {

                    itemData[
                        "specialNote"
                    ] =
                        *item
                            .getSpecialNote();

                } else {

                    itemData[
                        "specialNote"
                    ] =
                        nullptr;
                }


                itemList.push_back(
                    itemData
                );
            }


            json result = {
                {
                    "orderId",
                    order->getId()
                },
                {
                    "tableId",
                    tableId
                },
                {
                    "tableNumber",
                    table->getTableNumber()
                },
                {
                    "orderDate",
                    order->getOrderDate()
                },
                {
                    "orderStatus",
                    order->getOrderStatus()
                },
                {
                    "totalAmount",
                    order->getTotalAmount()
                },
                {
                    "items",
                    itemList
                }
            };


            return jsonResponse(
                200,
                result
            );

        } catch (
            const std::invalid_argument& e
        ) {

            return errorResponse(
                400,
                e.what()
            );

        } catch (
            const std::exception& e
        ) {

            return errorResponse(
                500,
                e.what()
            );
        }
    });
}

}
}