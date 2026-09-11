#include "TableView.h"

#include "../utils/UIHelper.h"
#include "../utils/Validator.h"

#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace restaurant {
namespace view {

using utils::Align;
using utils::TableColumn;
using utils::UIHelper;
using utils::Validator;

TableView::TableView(
    controller::TableController& tableController
)
    : tableController_(tableController) {
}

void TableView::run() {
    while (true) {
        UIHelper::clearScreen();
        showMenu();

        int choice = Validator::readInt(
            "Choose: ",
            0,
            6
        );

        try {
            switch (choice) {
                case 1:
                    showTableMap();
                    break;

                case 2:
                    showAllTables();
                    break;

                case 3:
                    addTable();
                    break;

                case 4:
                    updateTable();
                    break;

                case 5:
                    updateTableStatus();
                    break;

                case 6:
                    deleteTable();
                    break;

                case 0:
                    return;
            }
        }
        catch (const std::exception& ex) {
            std::cout
                << "\nError: "
                << ex.what()
                << "\n";

            UIHelper::pause();
        }
    }
}

void TableView::showMenu() {
    UIHelper::drawBox(
        "TABLE MANAGEMENT",
        50
    );

    std::cout
        << "1. Table map\n"
        << "2. View all tables\n"
        << "3. Add table\n"
        << "4. Update table\n"
        << "5. Update table status\n"
        << "6. Delete table\n"
        << "0. Back\n\n";
}

void TableView::showTableMap() {
    UIHelper::clearScreen();

    UIHelper::drawBox(
        "TABLE MAP",
        50
    );

    auto tables =
        tableController_.getAll();

    if (tables.empty()) {
        std::cout
            << "No tables found.\n";

        UIHelper::pause();
        return;
    }

    std::vector<TableColumn> columns = {
        {"ID", 5, Align::Center},
        {"TABLE", 15, Align::Center},
        {"CAPACITY", 10, Align::Center},
        {"STATUS", 15, Align::Center}
    };

    std::vector<std::vector<std::string>> rows;

    for (const auto& table : tables) {
        rows.push_back({
            std::to_string(table.getId()),
            table.getTableNumber(),
            std::to_string(table.getCapacity()),
            table.getStatus()
        });
    }

    UIHelper::drawTable(
        columns,
        rows
    );

    UIHelper::pause();
}

void TableView::showAllTables() {
    showTableMap();
}

void TableView::addTable() {
    UIHelper::clearScreen();

    UIHelper::drawBox(
        "ADD TABLE",
        50
    );

    std::string tableNumber =
        Validator::readString(
            "Table number: ",
            1,
            20
        );

    int capacity =
        Validator::readInt(
            "Capacity: ",
            1,
            100
        );

    auto table =
        tableController_.add(
            tableNumber,
            capacity
        );

    std::cout
        << "\nTable created successfully.\n"
        << "ID: "
        << table.getId()
        << "\n";

    UIHelper::pause();
}

void TableView::updateTable() {
    UIHelper::clearScreen();

    UIHelper::drawBox(
        "UPDATE TABLE",
        50
    );

    int tableId =
        Validator::readInt(
            "Table ID: ",
            1,
            1000000
        );

    auto table =
        tableController_.getById(
            tableId
        );

    if (!table) {
        std::cout
            << "\nTable not found.\n";

        UIHelper::pause();
        return;
    }

    std::cout
        << "Current number: "
        << table->getTableNumber()
        << "\n";

    std::cout
        << "Current capacity: "
        << table->getCapacity()
        << "\n\n";

    std::string tableNumber =
        Validator::readString(
            "New table number: ",
            1,
            20
        );

    int capacity =
        Validator::readInt(
            "New capacity: ",
            1,
            100
        );

    bool updated =
        tableController_.update(
            tableId,
            tableNumber,
            capacity
        );

    if (updated) {
        std::cout
            << "\nTable updated successfully.\n";
    }
    else {
        std::cout
            << "\nTable was not updated.\n";
    }

    UIHelper::pause();
}

void TableView::updateTableStatus() {
    UIHelper::clearScreen();

    UIHelper::drawBox(
        "UPDATE TABLE STATUS",
        50
    );

    int tableId =
        Validator::readInt(
            "Table ID: ",
            1,
            1000000
        );

    auto table =
        tableController_.getById(
            tableId
        );

    if (!table) {
        std::cout
            << "\nTable not found.\n";

        UIHelper::pause();
        return;
    }

    std::cout
        << "Current status: "
        << table->getStatus()
        << "\n\n";

    std::cout
        << "1. Available\n"
        << "2. Occupied\n"
        << "3. Reserved\n";

    std::string choice =
        Validator::readChoice(
            "Choose status: ",
            {"1", "2", "3"}
        );

    std::string status;

    if (choice == "1") {
        status = "Available";
    }
    else if (choice == "2") {
        status = "Occupied";
    }
    else {
        status = "Reserved";
    }

    bool updated =
        tableController_.updateStatus(
            tableId,
            status
        );

    if (updated) {
        std::cout
            << "\nStatus updated successfully.\n";
    }
    else {
        std::cout
            << "\nStatus was not updated.\n";
    }

    UIHelper::pause();
}

void TableView::deleteTable() {
    UIHelper::clearScreen();

    UIHelper::drawBox(
        "DELETE TABLE",
        50
    );

    int tableId =
        Validator::readInt(
            "Table ID: ",
            1,
            1000000
        );

    auto table =
        tableController_.getById(
            tableId
        );

    if (!table) {
        std::cout
            << "\nTable not found.\n";

        UIHelper::pause();
        return;
    }

    std::cout
        << "\nTable: "
        << table->getTableNumber()
        << "\nStatus: "
        << table->getStatus()
        << "\n";

    std::string confirm =
        Validator::readChoice(
            "Delete this table? (Y/N): ",
            {"Y", "N"}
        );

    if (confirm == "N") {
        return;
    }

    bool removed =
        tableController_.remove(
            tableId
        );

    if (removed) {
        std::cout
            << "\nTable deleted successfully.\n";
    }
    else {
        std::cout
            << "\nTable was not deleted.\n";
    }

    UIHelper::pause();
}

}
}