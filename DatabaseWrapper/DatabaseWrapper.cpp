#include "DatabaseWrapper.h"

DatabaseWrapper::DatabaseWrapper(const std::string &database_name) {
    // Сonnecting to the database
    sqlite3 *raw_db = nullptr;
    if (sqlite3_open(database_name.c_str(), &raw_db) != SQLITE_OK) {
        std::string error_message = sqlite3_errmsg(raw_db);
        sqlite3_close(raw_db);
        throw SQLiteException(error_message);
    };
    database.reset(raw_db);
}

void DatabaseWrapper::create_table(const CreateTableInput &input) const {
    // Lambda for joining elements with a delimiter
    auto join = [](const std::vector<std::string> &elements, const std::string &delimiter) {
        std::ostringstream os;
        for (size_t i = 0; i < elements.size(); ++i) {
            os << elements[i];
            if (i < elements.size() - 1) {
                os << delimiter;
            }
        }
        return os.str();
    };

    // Validate table name and columns
    if (input.table_name.empty()) {
        throw std::runtime_error("Table name cannot be empty.");
    }
    if (input.columns.empty()) {
        throw std::runtime_error("Cannot create a table with no columns.");
    }

    // Construct the SQL query
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

    // Execute the query and handle errors
    CHECK_SQLITE_EXEC(database.get(), sql_query.str().c_str());
}

void DatabaseWrapper::insert_into_table(const std::string &table_name,
                                        const std::vector<std::string> &columns,
                                        const std::vector<std::string> &values) const {
    if (columns.size() != values.size()) {
        throw std::runtime_error("Mismatch between columns and values.");
    }

    std::ostringstream sql_query;
    sql_query << "INSERT INTO " << table_name << " (";

    for (size_t i = 0; i < columns.size(); ++i) {
        sql_query << columns[i];
        if (i < columns.size() - 1) {
            sql_query << ", ";
        }
    }

    sql_query << ") VALUES (";

    for (size_t i = 0; i < values.size(); ++i) {
        sql_query << "'" << values[i] << "'";
        if (i < values.size() - 1) {
            sql_query << ", ";
        }
    }

    sql_query << ");";

    CHECK_SQLITE_EXEC(database.get(), sql_query.str().c_str());
}

void DatabaseWrapper::select(const std::string &table_name,
                             bool is_distinct,
                             const std::vector<std::string> &column_names,
                             const std::vector<std::string> &additions) const {
    if (table_name.empty()) {
        throw std::runtime_error("Table name cannot be empty.");
    }

    auto join = [](const std::vector<std::string> &elements, const std::string &delimiter) {
        std::ostringstream os;
        for (size_t i = 0; i < elements.size(); ++i) {
            os << elements[i];
            if (i < elements.size() - 1) {
                os << delimiter;
            }
        }
        return os.str();
    };

    std::ostringstream sql_query;
    sql_query << "SELECT ";

    if (is_distinct) {
        sql_query << "DISTINCT ";
    }

    if (column_names.empty()) {
        sql_query << "*";
    } else {
        sql_query << join(column_names, ", ");
    }

    sql_query << " FROM " << table_name;

    if (!additions.empty()) {
        sql_query << " " << join(additions, " ");
    }

    sql_query << ";";

    CHECK_SQLITE_EXEC(database.get(), sql_query.str().c_str());
}
