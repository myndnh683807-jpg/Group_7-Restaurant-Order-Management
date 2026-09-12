#include "InvoiceRepository.h"
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
            "Invoice ID must be positive."
        );
    }
}

void validatePaymentMethod(
    const std::string& method
) {
    if (method != "Cash" &&
        method != "Card" &&
        method != "QR") {

        throw std::invalid_argument(
            "Invalid payment method."
        );
    }
}

void validatePaymentStatus(
    const std::string& status
) {
    if (status != "Pending" &&
        status != "Paid" &&
        status != "Cancelled") {

        throw std::invalid_argument(
            "Invalid payment status."
        );
    }
}

model::Invoice readInvoice(
    const mysqlx::Row& row
) {
    return model::Invoice(
        row[0].get<int>(),
        row[1].get<int>(),
        row[2].get<int>(),
        row[3].get<std::string>(),
        row[4].get<double>(),
        row[5].get<double>(),
        row[6].get<double>(),
        row[7].get<std::string>(),
        row[8].get<std::string>()
    );
}

}

std::vector<model::Invoice>
InvoiceRepository::getAll() {
    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlResult result = session.sql(
        "SELECT invoice_id, order_id, cashier_id, "
        "created_at, subtotal, discount_amount, "
        "final_amount, payment_method, payment_status "
        "FROM Invoice "
        "ORDER BY invoice_id DESC"
    ).execute();

    std::vector<model::Invoice> invoices;

    for (mysqlx::Row row : result) {
        invoices.push_back(
            readInvoice(row)
        );
    }

    return invoices;
}

std::optional<model::Invoice>
InvoiceRepository::getById(
    int invoiceId
) {
    validateId(invoiceId);

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "SELECT invoice_id, order_id, cashier_id, "
        "created_at, subtotal, discount_amount, "
        "final_amount, payment_method, payment_status "
        "FROM Invoice "
        "WHERE invoice_id = ?"
    );

    stmt.bind(invoiceId);

    mysqlx::SqlResult result =
        stmt.execute();

    mysqlx::Row row =
        result.fetchOne();

    if (row.isNull()) {
        return std::nullopt;
    }

    return readInvoice(row);
}

std::optional<model::Invoice>
InvoiceRepository::getByOrderId(
    int orderId
) {
    if (orderId <= 0) {
        throw std::invalid_argument(
            "Order ID must be positive."
        );
    }

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "SELECT invoice_id, order_id, cashier_id, "
        "created_at, subtotal, discount_amount, "
        "final_amount, payment_method, payment_status "
        "FROM Invoice "
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

    return readInvoice(row);
}

model::Invoice InvoiceRepository::add(
    const model::Invoice& invoice
) {
    if (invoice.getOrderId() <= 0) {
        throw std::invalid_argument(
            "Order ID must be positive."
        );
    }

    if (invoice.getCashierId() <= 0) {
        throw std::invalid_argument(
            "Cashier ID must be positive."
        );
    }

    if (invoice.getSubtotal() < 0 ||
        invoice.getDiscountAmount() < 0 ||
        invoice.getFinalAmount() < 0) {

        throw std::invalid_argument(
            "Invoice amount cannot be negative."
        );
    }

    validatePaymentMethod(
        invoice.getPaymentMethod()
    );

    validatePaymentStatus(
        invoice.getPaymentStatus()
    );

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "INSERT INTO Invoice "
        "(order_id, cashier_id, subtotal, "
        "discount_amount, final_amount, "
        "payment_method, payment_status) "
        "VALUES (?, ?, ?, ?, ?, ?, ?)"
    );

    stmt.bind(
        invoice.getOrderId(),
        invoice.getCashierId(),
        invoice.getSubtotal(),
        invoice.getDiscountAmount(),
        invoice.getFinalAmount(),
        invoice.getPaymentMethod(),
        invoice.getPaymentStatus()
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
            "Could not read inserted invoice ID."
        );
    }

    auto saved =
        getById(
            static_cast<int>(newId)
        );

    if (!saved) {
        throw std::runtime_error(
            "Could not read inserted invoice."
        );
    }

    return *saved;
}

bool InvoiceRepository::update(
    const model::Invoice& invoice
) {
    validateId(invoice.getId());

    if (invoice.getCashierId() <= 0) {
        throw std::invalid_argument(
            "Cashier ID must be positive."
        );
    }

    if (invoice.getSubtotal() < 0 ||
        invoice.getDiscountAmount() < 0 ||
        invoice.getFinalAmount() < 0) {

        throw std::invalid_argument(
            "Invoice amount cannot be negative."
        );
    }

    validatePaymentMethod(
        invoice.getPaymentMethod()
    );

    validatePaymentStatus(
        invoice.getPaymentStatus()
    );

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "UPDATE Invoice "
        "SET cashier_id = ?, "
        "subtotal = ?, "
        "discount_amount = ?, "
        "final_amount = ?, "
        "payment_method = ?, "
        "payment_status = ? "
        "WHERE invoice_id = ?"
    );

    stmt.bind(
        invoice.getCashierId(),
        invoice.getSubtotal(),
        invoice.getDiscountAmount(),
        invoice.getFinalAmount(),
        invoice.getPaymentMethod(),
        invoice.getPaymentStatus(),
        invoice.getId()
    );

    return stmt
        .execute()
        .getAffectedItemsCount() > 0;
}

bool InvoiceRepository::remove(
    int invoiceId
) {
    validateId(invoiceId);

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "DELETE FROM Invoice "
        "WHERE invoice_id = ?"
    );

    stmt.bind(invoiceId);

    return stmt
        .execute()
        .getAffectedItemsCount() > 0;
}

bool InvoiceRepository::updatePaymentStatus(
    int invoiceId,
    const std::string& status
) {
    validateId(invoiceId);

    validatePaymentStatus(status);

    auto& session =
        database::Database::getInstance().getSession();

    mysqlx::SqlStatement stmt = session.sql(
        "UPDATE Invoice "
        "SET payment_status = ? "
        "WHERE invoice_id = ?"
    );

    stmt.bind(
        status,
        invoiceId
    );

    return stmt
        .execute()
        .getAffectedItemsCount() > 0;
}

}
}