// src/database/Database.cpp
// Triển khai kết nối MySQL X DevAPI với cấu hình từ .env / biến môi trường.
#include "Database.h"

#include <cstdlib>   // std::getenv
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace restaurant {
namespace database {

// ---------------------------------------------------------------------------
// Singleton accessor
// ---------------------------------------------------------------------------
Database& Database::getInstance() {
    static Database instance;
    return instance;
}

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------
Database::Database()
    : config_(loadConfig()) {}

Database::~Database() {
    disconnect();
}

// ---------------------------------------------------------------------------
// Kết nối
// ---------------------------------------------------------------------------
void Database::connect() {
    std::lock_guard<std::mutex> lock(mutex_);

    // Nếu đã có session hợp lệ thì không tạo lại.
    if (session_) {
        return;
    }

    try {
        std::cout << "[Database] Dang ket noi toi MySQL tai "
                  << config_.host << ":" << config_.port
                  << " (user: " << config_.user
                  << ", db: " << config_.database << ")...\n";

        // Tạo session qua MySQL X Protocol.
        session_ = std::make_unique<mysqlx::Session>(
            mysqlx::SessionOption::HOST,   config_.host,
            mysqlx::SessionOption::PORT,   config_.port,
            mysqlx::SessionOption::USER,   config_.user,
            mysqlx::SessionOption::PWD,    config_.password
        );

        // Chọn schema mặc định.
        session_->sql("USE " + config_.database).execute();

        std::cout << "[Database] Ket noi thanh cong toi "
                  << config_.host << ":" << config_.port
                  << "/" << config_.database << "\n\n";
    } catch (const mysqlx::Error& e) {
        session_.reset();
        std::cerr << "[Database] Loi ket noi MySQL: " << e.what() << "\n";
        throw;
    } catch (const std::exception& e) {
        session_.reset();
        std::cerr << "[Database] Loi ket noi: " << e.what() << "\n";
        throw;
    } catch (...) {
        session_.reset();
        std::cerr << "[Database] Loi khong xac dinh khi khoi tao session MySQL.\n";
        throw std::runtime_error("Loi khong xac dinh khi ket noi MySQL.");
    }
}

void Database::disconnect() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (session_) {
        session_->close();
        session_.reset();
        std::cout << "[Database] Da ngat ket noi.\n";
    }
}

// ---------------------------------------------------------------------------
// Trả về session (lazy-connect)
// ---------------------------------------------------------------------------
mysqlx::Session& Database::getSession() {
    if (!session_) {
        connect();
    }
    if (!session_) {
        throw std::runtime_error("[Database] Khong the tao session MySQL.");
    }
    return *session_;
}

const DbConfig& Database::getConfig() const noexcept {
    return config_;
}

// ---------------------------------------------------------------------------
// Đọc cấu hình
// ---------------------------------------------------------------------------
DbConfig Database::loadConfig() const {
    DbConfig cfg;

    // 1) Thử đọc file .env ở nhiều vị trí (thư mục chạy hoặc các cấp thư mục cha).
    std::unordered_map<std::string, std::string> envMap;
    const std::vector<std::string> searchPaths = {
        ".env", "../.env", "../../.env", "../../../.env"
    };
    for (const auto& p : searchPaths) {
        envMap = parseEnvFile(p);
        if (!envMap.empty()) {
            std::cout << "[Database] Da nap cau hinh tu file: " << p << "\n";
            break;
        }
    }

    // Helper: lấy giá trị từ biến môi trường, fallback sang envMap, rồi default.
    auto resolve = [&](const char* envKey, const std::string& fallback) -> std::string {
        // Ưu tiên 1: biến môi trường hệ thống.
        const char* val = std::getenv(envKey);
        if (val && val[0] != '\0') {
            return std::string(val);
        }
        // Ưu tiên 2: file .env
        auto it = envMap.find(envKey);
        if (it != envMap.end()) {
            return it->second;
        }
        // Ưu tiên 3: giá trị mặc định.
        return fallback;
    };

    cfg.host     = resolve("DB_HOST",     cfg.host);
    cfg.port     = std::stoi(resolve("DB_PORT", std::to_string(cfg.port)));
    cfg.user     = resolve("DB_USER",     cfg.user);
    cfg.password = resolve("DB_PASSWORD", cfg.password);
    cfg.database = resolve("DB_NAME",     cfg.database);

    return cfg;
}

// ---------------------------------------------------------------------------
// Parse file .env
// ---------------------------------------------------------------------------
std::unordered_map<std::string, std::string>
Database::parseEnvFile(const std::string& filepath) const {
    std::unordered_map<std::string, std::string> result;
    std::ifstream file(filepath);

    if (!file.is_open()) {
        // Không tìm thấy .env → trả map rỗng, sẽ dùng biến môi trường / default.
        return result;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Bỏ qua dòng trống và comment.
        if (line.empty() || line[0] == '#') {
            continue;
        }

        auto pos = line.find('=');
        if (pos == std::string::npos) {
            continue;
        }

        std::string key   = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        // Trim khoảng trắng đầu/cuối cho key và value.
        auto trim = [](std::string& s) {
            const char* ws = " \t\r\n";
            s.erase(0, s.find_first_not_of(ws));
            s.erase(s.find_last_not_of(ws) + 1);
        };
        trim(key);
        trim(value);

        // Loại bỏ dấu ngoặc kép bao quanh value (nếu có).
        if (value.size() >= 2 &&
            ((value.front() == '"'  && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }

        result[key] = value;
    }

    return result;
}

} // namespace database
} // namespace restaurant
