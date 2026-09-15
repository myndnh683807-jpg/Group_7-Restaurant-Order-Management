// src/database/Database.h
// Quản lý kết nối MySQL X DevAPI (Singleton Pattern)
#pragma once

#include <mysqlx/xdevapi.h>

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace restaurant {
namespace database {

/// @brief Cấu hình kết nối database, đọc từ file .env hoặc biến môi trường.
struct DbConfig {
    std::string host = "localhost";
    int         port = 33060;       // MySQL X Protocol port
    std::string user = "root";
    std::string password;
    std::string database = "restaurant_db";
};

/// @brief Singleton class quản lý kết nối tới MySQL thông qua X DevAPI.
///
/// Thứ tự ưu tiên khi đọc cấu hình:
///   1. Biến môi trường hệ thống (DB_HOST, DB_PORT, DB_USER, DB_PASSWORD, DB_NAME).
///   2. File `.env` ở thư mục gốc dự án.
///   3. Giá trị mặc định trong DbConfig.
class Database final {
public:
    /// Lấy instance duy nhất (thread-safe).
    static Database& getInstance();

    // Xóa copy & move để đảm bảo Singleton.
    Database(const Database&)            = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&&)                 = delete;
    Database& operator=(Database&&)      = delete;

    /// Mở kết nối tới MySQL. Gọi lại sẽ tái sử dụng session hiện có.
    /// @throws mysqlx::Error nếu không kết nối được.
    void connect();

    /// Đóng session hiện tại (nếu có).
    void disconnect();

    /// Trả về session hiện tại. Tự động gọi connect() nếu chưa kết nối.
    /// @throws std::runtime_error nếu session không khả dụng.
    mysqlx::Session& getSession();

    /// Trả về cấu hình đang dùng (read-only).
    const DbConfig& getConfig() const noexcept;

private:
    Database();
    ~Database();

    /// Đọc cấu hình từ biến môi trường, fallback sang file .env.
    DbConfig loadConfig() const;

    /// Parse file .env thành map key-value.
    std::unordered_map<std::string, std::string> parseEnvFile(const std::string& filepath) const;

    DbConfig                          config_;
    std::unique_ptr<mysqlx::Session>  session_;
    std::mutex                        mutex_;
};

} // namespace database
} // namespace restaurant
