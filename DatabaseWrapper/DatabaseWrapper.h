#ifndef DATABASEWRAPPER_H
#define DATABASEWRAPPER_H

#pragma once

#include <iostream>
#include <sqlite3.h>
#include <memory>
#include <vector>
#include <sstream>

#define CHECK_SQLITE_EXEC(db, query)                                     \
    do {                                                                 \
        char* zErrMsg = nullptr;                                         \
        if (sqlite3_exec(db, query, nullptr, nullptr, &zErrMsg) != SQLITE_OK) { \
            std::string error_message = "SQL error: " + std::string(zErrMsg);   \
            sqlite3_free(zErrMsg);                                       \
            throw std::runtime_error(error_message);                     \
        }                                                                \
    } while (0)


// SQLite3 settings (exceptions, custom deletes)

class SQLiteException : public std::runtime_error {
public:
    explicit SQLiteException(const std::string &message)
        : std::runtime_error(message) {
    }
};

auto sqlite_deleter = [](sqlite3 *db) {
    if (db) {
        sqlite3_close(db);
        std::cout << "Database connection closed." << std::endl;
    }
};

//

class DatabaseWrapper {
    struct Column {
        std::string name;
        std::string type;
        std::vector<std::string> constraints;
    };

    struct CreateTableInput {
        std::string table_name;
        std::vector<Column> columns;
    };

public:
    DatabaseWrapper() = delete;

    explicit DatabaseWrapper(const std::string &);

    void create_table(const CreateTableInput &) const;

    void insert_into_table(const std::string &, const std::vector<std::string> &,
                           const std::vector<std::string> &) const;

    void select(const std::string &, bool, const std::vector<std::string> &, const std::vector<std::string> &) const;

private:
    std::unique_ptr<sqlite3, decltype(sqlite_deleter)> database{nullptr, sqlite_deleter};
};


#endif
