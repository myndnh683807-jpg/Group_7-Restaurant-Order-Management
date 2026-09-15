# ===========================================================================
# CMakeLists.txt — Restaurant Order Management System (Web API Backend)
# ===========================================================================
# Build: mkdir build && cd build && cmake .. && cmake --build .
# ===========================================================================
cmake_minimum_required(VERSION 3.14)
project(RestaurantAPI VERSION 2.0.0 LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Tiêu chuẩn C++17
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Fix lỗi C1902 (PDB mismatch) và lỗi ABI mismatch 0xC0000005 với MySQL Connector C++
if(MSVC)
    add_compile_options(/Z7)

    # Thư viện MySQL Connector C++ (X DevAPI) chính thức từ Oracle chỉ có bản Release (/MD).
    # Nếu chạy Debug với /MDd, khác biệt cấu trúc bộ nhớ của std::string giữa /MDd và /MD
    # sẽ gây crash 0xC0000005 (Access Violation).
    # Do đó, bắt buộc đồng bộ runtime sang /MD và _ITERATOR_DEBUG_LEVEL=0:
    if(POLICY CMP0091)
        cmake_policy(SET CMP0091 NEW)
    endif()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

    string(REPLACE "/MDd" "/MD" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    add_compile_definitions($<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=0>)
endif()

# ---------------------------------------------------------------------------
# FetchContent — Tải thư viện bên ngoài (Crow & nlohmann/json)
# ---------------------------------------------------------------------------
include(FetchContent)

# 0) ASIO — Standalone C++ network library (Yêu cầu bởi Crow)
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG        asio-1-30-2
)
FetchContent_MakeAvailable(asio)
set(ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/asio/include CACHE PATH "ASIO include dir" FORCE)

# 1) Crow C++ — Lightweight HTTP framework
FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG        master
)

# 2) nlohmann/json — JSON serialization
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)

# Tắt build Tests và Examples để tránh lỗi đòi thư viện Boost/GTest
set(CROW_BUILD_EXAMPLES OFF CACHE BOOL "Disable Crow examples" FORCE)
set(CROW_BUILD_TESTS OFF CACHE BOOL "Disable Crow tests" FORCE)
set(JSON_BuildTests OFF CACHE BOOL "Disable JSON tests" FORCE)

FetchContent_MakeAvailable(Crow nlohmann_json)

# ---------------------------------------------------------------------------
# Tìm MySQL Connector C++ (X DevAPI) — đã cài sẵn trên hệ thống
# ---------------------------------------------------------------------------
find_path(MYSQLCPPCONN_INCLUDE_DIR
    NAMES mysqlx/xdevapi.h
    PATHS
        "C:/Program Files/MySQL/MySQL Connector C++ 26.7/include"
        "C:/Program Files/MySQL/MySQL Connector C++ 8.0/include"
        "C:/Program Files/MySQL/Connector C++ 8.0/include"
        "/usr/include/mysql-cppconn-8"
        "/usr/local/include/mysql-cppconn-8"
)

find_library(MYSQLCPPCONN_LIBRARY
    NAMES mysqlcppconnx mysqlcppconn8
    PATHS
        "C:/Program Files/MySQL/MySQL Connector C++ 26.7/lib64/vs14"
        "C:/Program Files/MySQL/MySQL Connector C++ 8.0/lib64/vs14"
        "C:/Program Files/MySQL/Connector C++ 8.0/lib64/vs14"
        "/usr/lib"
        "/usr/local/lib"
        "/usr/lib/x86_64-linux-gnu"
)

if(NOT MYSQLCPPCONN_INCLUDE_DIR OR NOT MYSQLCPPCONN_LIBRARY)
    message(WARNING
        "MySQL Connector C++ not found automatically.\n"
        "Please set MYSQLCPPCONN_INCLUDE_DIR and MYSQLCPPCONN_LIBRARY manually.\n"
        "Example: cmake .. -DMYSQLCPPCONN_INCLUDE_DIR=\"C:/path/to/include\" "
        "-DMYSQLCPPCONN_LIBRARY=\"C:/path/to/lib/mysqlcppconn8.lib\""
    )
endif()

# ---------------------------------------------------------------------------
# Source files (MVC 2.0 Full Architecture)
# ---------------------------------------------------------------------------
set(SOURCES
    # Database layer
    src/database/Database.cpp

    # Model layer (8 models)
    src/model/Category.cpp
    src/model/MenuItem.cpp
    src/model/Table.cpp
    src/model/Order.cpp
    src/model/OrderItem.cpp
    src/model/Invoice.cpp
    src/model/Employee.cpp
    src/model/Inventory.cpp

    # Repository layer (6 repositories)
    src/repository/MenuRepository.cpp
    src/repository/TableRepository.cpp
    src/repository/OrderRepository.cpp
    src/repository/InvoiceRepository.cpp
    src/repository/EmployeeRepository.cpp
    src/repository/InventoryRepository.cpp

    # Controller layer (9 API controllers)
    src/controller/MenuApiController.cpp
    src/controller/OrderApiController.cpp
    src/controller/TableApiController.cpp
    src/controller/ServiceApiController.cpp
    src/controller/KitchenApiController.cpp
    src/controller/PaymentApiController.cpp
    src/controller/EmployeeApiController.cpp
    src/controller/InventoryApiController.cpp
    src/controller/ReportApiController.cpp

    # Utils
    src/utils/JsonHelper.cpp
    src/utils/Validator.cpp

    # Entry point
    src/main.cpp
)

# ---------------------------------------------------------------------------
# Executable target
# ---------------------------------------------------------------------------
add_executable(restaurant_api ${SOURCES})

# Include directories
target_include_directories(restaurant_api PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${MYSQLCPPCONN_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(restaurant_api PRIVATE
    Crow::Crow
    nlohmann_json::nlohmann_json
    ${MYSQLCPPCONN_LIBRARY}
)

# ---------------------------------------------------------------------------
# Platform-specific settings
# ---------------------------------------------------------------------------
if(WIN32)
    # Windows: cần link thêm ws2_32 cho Crow (ASIO networking)
    target_link_libraries(restaurant_api PRIVATE ws2_32 wsock32)

    # Suppress MSVC deprecation warnings
    target_compile_definitions(restaurant_api PRIVATE
        _CRT_SECURE_NO_WARNINGS
        _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
    )
endif()

# ---------------------------------------------------------------------------
# Output
# ---------------------------------------------------------------------------
message(STATUS "==================================================")
message(STATUS "  Project    : ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "  C++ Std    : ${CMAKE_CXX_STANDARD}")
message(STATUS "  MySQL Inc  : ${MYSQLCPPCONN_INCLUDE_DIR}")
message(STATUS "  MySQL Lib  : ${MYSQLCPPCONN_LIBRARY}")
message(STATUS "==================================================")
