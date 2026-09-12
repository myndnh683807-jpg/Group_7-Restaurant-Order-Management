#include "OrderRepository.h"
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
            "ID must be positive."
        );
    }
}

model::Order readOrder(
    const mysqlx::Row& row
) {
    std::optional<int> staffId;

    if (!row[2].isNull()) {
        staffId = row[2].get<int>();
    }

    return model::Order(
        row[0].get<int>(),
        row[1].get<int>(),
        staffId,
        row[3].get<std::string>(),
        row[4].get<std::string>(),
        row[5].get<double>()
    );
}

model::OrderItem readOrderItem(
    const mysqlx::Row& row
) {
    std::optional<std::string> note;

    if (!row[5].isNull()) {
        note = row[5].get<std::string>();
    }

    return model::OrderItem(
        row[0].get<int>(),
        row[1].get<int>(),
        row[2].get<int>(),
        row[3].get<int>(),
        row[4].get<double>(),
        note,
        row[6].get<std::string>()
    );
}

}

std::vector<model::Order>
OrderRepository::getAll() {
    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlResult result = session.sql(
        "SELECT order_id, table_id, staff_id, "
        "order_date, order_status, total_amount "
        "FROM Orders "
        "ORDER BY order_id DESC"
    ).execute();

    std::vector<model::Order> orders;

    for (mysqlx::Row row : result) {
        orders.push_back(
            readOrder(row)
        );
    }

    return orders;
}

std::optional<model::Order>
OrderRepository::getById(
    int orderId
) {
    validateId(orderId);

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "SELECT order_id, table_id, staff_id, "
        "order_date, order_status, total_amount "
        "FROM Orders "
        "WHERE order_id = ?"
    );

    stmt.bind(orderId);

    mysqlx::SqlResult result =
        stmt.execute();

    mysqlx::Row row =
        result.fetchOne();

    if (row.isNull()) {
        return std::nullopt;
    }

    return readOrder(row);
}

model::Order OrderRepository::add(
    const model::Order& order
) {
    if (order.getTableId() <= 0) {
        throw std::invalid_argument(
            "Table ID must be positive."
        );
    }

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt;

    if (order.getStaffId()) {
        stmt = session.sql(
            "INSERT INTO Orders "
            "(table_id, staff_id, order_status, total_amount) "
            "VALUES (?, ?, ?, ?)"
        );

        stmt.bind(
            order.getTableId(),
            *order.getStaffId(),
            order.getOrderStatus(),
            order.getTotalAmount()
        );
    }
    else {
        stmt = session.sql(
            "INSERT INTO Orders "
            "(table_id, staff_id, order_status, total_amount) "
            "VALUES (?, NULL, ?, ?)"
        );

        stmt.bind(
            order.getTableId(),
            order.getOrderStatus(),
            order.getTotalAmount()
        );
    }

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
            "Could not read inserted order ID."
        );
    }

    auto saved =
        getById(
            static_cast<int>(newId)
        );

    if (!saved) {
        throw std::runtime_error(
            "Could not read inserted order."
        );
    }

    return *saved;
}

bool OrderRepository::update(
    const model::Order& order
) {
    validateId(order.getId());

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt;

    if (order.getStaffId()) {
        stmt = session.sql(
            "UPDATE Orders "
            "SET table_id = ?, "
            "staff_id = ?, "
            "order_status = ?, "
            "total_amount = ? "
            "WHERE order_id = ?"
        );

        stmt.bind(
            order.getTableId(),
            *order.getStaffId(),
            order.getOrderStatus(),
            order.getTotalAmount(),
            order.getId()
        );
    }
    else {
        stmt = session.sql(
            "UPDATE Orders "
            "SET table_id = ?, "
            "staff_id = NULL, "
            "order_status = ?, "
            "total_amount = ? "
            "WHERE order_id = ?"
        );

        stmt.bind(
            order.getTableId(),
            order.getOrderStatus(),
            order.getTotalAmount(),
            order.getId()
        );
    }

    return stmt
        .execute()
        .getAffectedItemsCount() > 0;
}

bool OrderRepository::remove(
    int orderId
) {
    validateId(orderId);

    auto& session =
        database::Database::getInstance().getSession();

    session.startTransaction();

    try {
        mysqlx::SqlStatement itemStmt =
            session.sql(
                "DELETE FROM OrderItem "
                "WHERE order_id = ?"
            );

        itemStmt.bind(orderId);
        itemStmt.execute();

        mysqlx::SqlStatement orderStmt =
            session.sql(
                "DELETE FROM Orders "
                "WHERE order_id = ?"
            );

        orderStmt.bind(orderId);

        bool removed =
            orderStmt
                .execute()
                .getAffectedItemsCount() > 0;

        session.commit();

        return removed;
    }
    catch (...) {
        session.rollback();
        throw;
    }
}

bool OrderRepository::updateOrderStatus(
    int orderId,
    const std::string& status
) {
    validateId(orderId);

    if (status.empty() ||
        status.size() > 20) {

        throw std::invalid_argument(
            "Invalid order status."
        );
    }

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt =
        session.sql(
            "UPDATE Orders "
            "SET order_status = ? "
            "WHERE order_id = ?"
        );

    stmt.bind(
        status,
        orderId
    );

    return stmt
        .execute()
        .getAffectedItemsCount() > 0;
}

std::vector<model::OrderItem>
OrderRepository::getItems(
    int orderId
) {
    validateId(orderId);

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt =
        session.sql(
            "SELECT order_item_id, order_id, item_id, "
            "quantity, unit_price, special_note, item_status "
            "FROM OrderItem "
            "WHERE order_id = ? "
            "ORDER BY order_item_id"
        );

    stmt.bind(orderId);

    mysqlx::SqlResult result =
        stmt.execute();

    std::vector<model::OrderItem> items;

    for (mysqlx::Row row : result) {
        items.push_back(
            readOrderItem(row)
        );
    }

    return items;
}

std::optional<model::OrderItem>
OrderRepository::getItemById(
    int orderItemId
) {
    validateId(orderItemId);

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt =
        session.sql(
            "SELECT order_item_id, order_id, item_id, "
            "quantity, unit_price, special_note, item_status "
            "FROM OrderItem "
            "WHERE order_item_id = ?"
        );

    stmt.bind(orderItemId);

    mysqlx::SqlResult result =
        stmt.execute();

    mysqlx::Row row =
        result.fetchOne();

    if (row.isNull()) {
        return std::nullopt;
    }

    return readOrderItem(row);
}

model::OrderItem
OrderRepository::addItem(
    const model::OrderItem& item
) {
    if (item.getOrderId() <= 0 ||
        item.getItemId() <= 0 ||
        item.getQuantity() <= 0) {

        throw std::invalid_argument(
            "Invalid order item."
        );
    }

    auto& session =
        database::Database::getInstance().getSession();

    session.startTransaction();

    try {
        mysqlx::SqlStatement stmt;

        if (item.getSpecialNote()) {
            stmt = session.sql(
                "INSERT INTO OrderItem "
                "(order_id, item_id, quantity, unit_price, "
                "special_note, item_status) "
                "VALUES (?, ?, ?, ?, ?, ?)"
            );

            stmt.bind(
                item.getOrderId(),
                item.getItemId(),
                item.getQuantity(),
                item.getUnitPrice(),
                *item.getSpecialNote(),
                item.getItemStatus()
            );
        }
        else {
            stmt = session.sql(
                "INSERT INTO OrderItem "
                "(order_id, item_id, quantity, unit_price, "
                "special_note, item_status) "
                "VALUES (?, ?, ?, ?, NULL, ?)"
            );

            stmt.bind(
                item.getOrderId(),
                item.getItemId(),
                item.getQuantity(),
                item.getUnitPrice(),
                item.getItemStatus()
            );
        }

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
                "Could not read order item ID."
            );
        }

        mysqlx::SqlStatement totalStmt =
            session.sql(
                "UPDATE Orders "
                "SET total_amount = ("
                "SELECT COALESCE("
                "SUM(quantity * unit_price), 0"
                ") "
                "FROM OrderItem "
                "WHERE order_id = ?"
                ") "
                "WHERE order_id = ?"
            );

        totalStmt.bind(
            item.getOrderId(),
            item.getOrderId()
        );

        totalStmt.execute();

        session.commit();

        auto saved =
            getItemById(
                static_cast<int>(newId)
            );

        if (!saved) {
            throw std::runtime_error(
                "Could not read inserted order item."
            );
        }

        return *saved;
    }
    catch (...) {
        session.rollback();
        throw;
    }
}

bool OrderRepository::updateItem(
    const model::OrderItem& item
) {
    validateId(item.getId());

    auto oldItem =
        getItemById(
            item.getId()
        );

    if (!oldItem) {
        return false;
    }

    if (oldItem->getOrderId() !=
        item.getOrderId()) {

        throw std::logic_error(
            "Cannot move item to another order."
        );
    }

    auto& session =
        database::Database::getInstance().getSession();

    session.startTransaction();

    try {
        mysqlx::SqlStatement stmt;

        if (item.getSpecialNote()) {
            stmt = session.sql(
                "UPDATE OrderItem "
                "SET item_id = ?, "
                "quantity = ?, "
                "unit_price = ?, "
                "special_note = ?, "
                "item_status = ? "
                "WHERE order_item_id = ?"
            );

            stmt.bind(
                item.getItemId(),
                item.getQuantity(),
                item.getUnitPrice(),
                *item.getSpecialNote(),
                item.getItemStatus(),
                item.getId()
            );
        }
        else {
            stmt = session.sql(
                "UPDATE OrderItem "
                "SET item_id = ?, "
                "quantity = ?, "
                "unit_price = ?, "
                "special_note = NULL, "
                "item_status = ? "
                "WHERE order_item_id = ?"
            );

            stmt.bind(
                item.getItemId(),
                item.getQuantity(),
                item.getUnitPrice(),
                item.getItemStatus(),
                item.getId()
            );
        }

        bool updated =
            stmt
                .execute()
                .getAffectedItemsCount() > 0;

        mysqlx::SqlStatement totalStmt =
            session.sql(
                "UPDATE Orders "
                "SET total_amount = ("
                "SELECT COALESCE("
                "SUM(quantity * unit_price), 0"
                ") "
                "FROM OrderItem "
                "WHERE order_id = ?"
                ") "
                "WHERE order_id = ?"
            );

        totalStmt.bind(
            item.getOrderId(),
            item.getOrderId()
        );

        totalStmt.execute();

        session.commit();

        return updated;
    }
    catch (...) {
        session.rollback();
        throw;
    }
}

bool OrderRepository::removeItem(
    int orderItemId
) {
    validateId(orderItemId);

    auto item =
        getItemById(orderItemId);

    if (!item) {
        return false;
    }

    auto& session =
        database::Database::getInstance().getSession();

    session.startTransaction();

    try {
        mysqlx::SqlStatement stmt =
            session.sql(
                "DELETE FROM OrderItem "
                "WHERE order_item_id = ?"
            );

        stmt.bind(orderItemId);

        bool removed =
            stmt
                .execute()
                .getAffectedItemsCount() > 0;

        mysqlx::SqlStatement totalStmt =
            session.sql(
                "UPDATE Orders "
                "SET total_amount = ("
                "SELECT COALESCE("
                "SUM(quantity * unit_price), 0"
                ") "
                "FROM OrderItem "
                "WHERE order_id = ?"
                ") "
                "WHERE order_id = ?"
            );

        totalStmt.bind(
            item->getOrderId(),
            item->getOrderId()
        );

        totalStmt.execute();

        session.commit();

        return removed;
    }
    catch (...) {
        session.rollback();
        throw;
    }
}

bool OrderRepository::updateItemStatus(
    int orderItemId,
    const std::string& status
) {
    validateId(orderItemId);

    if (status.empty() ||
        status.size() > 20) {

        throw std::invalid_argument(
            "Invalid item status."
        );
    }

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt =
        session.sql(
            "UPDATE OrderItem "
            "SET item_status = ? "
            "WHERE order_item_id = ?"
        );

    stmt.bind(
        status,
        orderItemId
    );

    return stmt
        .execute()
        .getAffectedItemsCount() > 0;
}

void OrderRepository::recalculateTotal(
    int orderId
) {
    validateId(orderId);

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt =
        session.sql(
            "UPDATE Orders "
            "SET total_amount = ("
            "SELECT COALESCE("
            "SUM(quantity * unit_price), 0"
            ") "
            "FROM OrderItem "
            "WHERE order_id = ?"
            ") "
            "WHERE order_id = ?"
        );

    stmt.bind(
        orderId,
        orderId
    );

    stmt.execute();
}

}
}