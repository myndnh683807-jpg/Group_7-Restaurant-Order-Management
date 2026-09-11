    #pragma once

    #include "../controller/TableController.h"

    namespace restaurant {
    namespace view {

    class TableView {
    public:
        explicit TableView(
            controller::TableController& tableController
        );

        void run();
        void showTableMap();

    private:
        controller::TableController& tableController_;

        void showMenu();
        void showAllTables();
        void addTable();
        void updateTable();
        void updateTableStatus();
        void deleteTable();
    };

    }
    }