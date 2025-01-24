#include "DatabaseWrapper.h"

DatabaseWrapper::DatabaseWrapper(const std::string &database_name) {
    sqlite3 *raw_db = nullptr;
    if (sqlite3_open(database_name.c_str(), &raw_db) != SQLITE_OK) {
        Logger(LogLevel::ERROR) << "Failed to open database: " << database_name;
        std::string error_message = sqlite3_errmsg(raw_db);
        sqlite3_close(raw_db);
        throw SQLiteException(error_message);
    };
    Logger(LogLevel::INFO) << "Opened database: " << database_name;
    database.reset(raw_db);
}

void DatabaseWrapper::create_table(const CreateTableInput &input) const {
    if (input.table_name.empty()) {
        Logger(LogLevel::ERROR) << "Table name cannot be empty.";
        throw std::runtime_error("Table name cannot be empty.");
    }
    if (input.columns.empty()) {
        Logger(LogLevel::ERROR) << "Cannot create a table with no columns.";
        throw std::runtime_error("Cannot create a table with no columns.");
    }

    std::ostringstream sql_query;
    sql_query << "CREATE TABLE IF NOT EXISTS " << input.table_name << " (";

    for (size_t i = 0; i < input.columns.size(); ++i) {
        const auto &column = input.columns[i];
        sql_query << column.name << " " << column.type;

        if (!column.constraints.empty()) {
            sql_query << " " << join(column.constraints, " ");
        }

        if (i < input.columns.size() - 1) {
            sql_query << ", ";
        }
    }
    sql_query << ");";

    CHECK_SQLITE_EXEC(database.get(), sql_query.str().c_str());
    Logger(LogLevel::INFO) << "Table " << input.table_name << " created.";
}

void DatabaseWrapper::insert_into_table(const std::string &table_name,
                                        const std::vector<std::string> &columns,
                                        const std::vector<std::string> &values) const {
    if (columns.size() != values.size()) {
        Logger(LogLevel::ERROR) << "Mismatch between columns and values.";
        throw std::runtime_error("Mismatch between columns and values.");
    }

    std::ostringstream sql_query;
    sql_query << "INSERT INTO " << table_name << " (";
    sql_query << join(columns, ", ") << ") VALUES (";
    for (size_t i = 0; i < values.size(); ++i) {
        sql_query << "?";
        if (i < values.size() - 1) {
            sql_query << ", ";
        }
    }
    sql_query << ");";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(database.get(), sql_query.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::string error_message = sqlite3_errmsg(database.get());
        sqlite3_finalize(stmt);
        Logger(LogLevel::ERROR) << "SQL error: " << error_message;
        throw std::runtime_error("SQL error: " + error_message);
    }

    for (size_t i = 0; i < values.size(); ++i)
        sqlite3_bind_text(stmt, i + 1, values[i].c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::string error_message = sqlite3_errmsg(database.get());
        sqlite3_finalize(stmt);
        Logger(LogLevel::ERROR) << "SQL error: " << error_message;
        throw std::runtime_error("SQL error: " + error_message);
    }
    Logger(LogLevel::INFO) << "Inserted into table " << table_name;
    sqlite3_finalize(stmt);
}

std::string DatabaseWrapper::select(const std::string &table_name,
                                    bool is_distinct,
                                    const std::vector<std::string> &column_names,
                                    const std::optional<std::vector<std::string> > &conditions) const {
    if (table_name.empty()) {
        Logger(LogLevel::ERROR) << "Table name cannot be empty.";
        throw std::runtime_error("Table name cannot be empty.");
    }

    std::stringstream result;

    std::ostringstream sql_query;
    sql_query << "SELECT ";
    if (is_distinct) {
        sql_query << "DISTINCT ";
    }
    sql_query << (column_names.empty() ? "*" : join(column_names, ", "));
    sql_query << " FROM " << table_name;

    if (conditions && !conditions->empty()) {
        sql_query << " WHERE " << join(*conditions, " AND ");
    }

    sql_query << ";";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(database.get(), sql_query.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::string error_message = sqlite3_errmsg(database.get());
        sqlite3_finalize(stmt);
        Logger(LogLevel::ERROR) << "SQL error: " << error_message;
        throw std::runtime_error("SQL error: " + error_message);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            result << sqlite3_column_name(stmt, i) << " = ";
            const char *text = reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));
            result << (text ? text : "NULL") << std::endl;
        }
        result << std::endl;
    }

    sqlite3_finalize(stmt);
    Logger(LogLevel::INFO) << "Selected from table " << table_name;
    return result.str();
}


void DatabaseWrapper::delete_from_table(const std::string &table_name,
                                        const std::optional<std::vector<std::string> > &conditions) const {
    if (table_name.empty()) {
        Logger(LogLevel::ERROR) << "Table name cannot be empty.";
        throw std::runtime_error("Table name cannot be empty.");
    }

    std::ostringstream sql_query;
    sql_query << "DELETE FROM " << table_name;

    if (conditions && !conditions->empty()) {
        sql_query << " WHERE " << join(*conditions, " AND ");
    }

    sql_query << ";";

    CHECK_SQLITE_EXEC(database.get(), sql_query.str().c_str());
    Logger(LogLevel::INFO) << "Deleted from table " << table_name;
}

void DatabaseWrapper::update_table(const std::string &table_name,
                                   const std::vector<std::pair<std::string, std::string> > &updates,
                                   const std::vector<std::string> &conditions) const {
    if (table_name.empty()) {
        Logger(LogLevel::ERROR) << "Table name cannot be empty.";
        throw std::runtime_error("Table name cannot be empty.");
    }
    if (updates.empty()) {
        Logger(LogLevel::ERROR) << "No updates provided.";
        throw std::runtime_error("No updates provided.");
    }

    std::ostringstream sql_query;
    sql_query << "UPDATE " << table_name << " SET ";
    for (size_t i = 0; i < updates.size(); ++i) {
        sql_query << updates[i].first << " = ?";
        if (i < updates.size() - 1) {
            sql_query << ", ";
        }
    }

    if (!conditions.empty()) {
        sql_query << " WHERE " << join(conditions, " AND ");
    }
    sql_query << ";";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(database.get(), sql_query.str().c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::string error_message = sqlite3_errmsg(database.get());
        sqlite3_finalize(stmt);
        Logger(LogLevel::ERROR) << "SQL error: " << error_message;
        throw std::runtime_error("SQL error: " + error_message);
    }

    for (size_t i = 0; i < updates.size(); ++i) {
        sqlite3_bind_text(stmt, i + 1, updates[i].second.c_str(), -1, SQLITE_TRANSIENT);
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::string error_message = sqlite3_errmsg(database.get());
        sqlite3_finalize(stmt);
        Logger(LogLevel::ERROR) << "SQL error: " << error_message;
        throw std::runtime_error("SQL error: " + error_message);
    }
    Logger(LogLevel::INFO) << "Updated table " << table_name;
    sqlite3_finalize(stmt);
}
