#include "test_databaseWrapper.h"
#include <cstdio>

void DatabaseWrapperTest::SetUp() {
    std::remove("test.db");
}

void DatabaseWrapperTest::TearDown() {
    std::remove("test.db");
}

TEST_F(DatabaseWrapperTest, InsertIntoTable) {
    DatabaseWrapper db("test.db");

    CreateTableInput table_input = {
        "test_table",
        {
            {"id", "INTEGER", {"PRIMARY KEY"}},
            {"name", "TEXT", {}}
        }
    };

    ASSERT_NO_THROW(db.create_table(table_input));

    std::vector<std::string> columns = {"id", "name"};
    std::vector<std::string> values = {"1", "Test Name"};
    EXPECT_NO_THROW(db.insert_into_table("test_table", columns, values));
}

TEST_F(DatabaseWrapperTest, SelectFromTable) {
    DatabaseWrapper db("test.db");

    CreateTableInput table_input = {
        "test_table",
        {
            {"id", "INTEGER", {"PRIMARY KEY"}},
            {"name", "TEXT", {}}
        }
    };
    db.create_table(table_input);

    std::vector<std::string> columns = {"id", "name"};
    std::vector<std::string> values = {"1", "Test Name"};
    db.insert_into_table("test_table", columns, values);

    std::optional<std::vector<std::string>> conditions = std::nullopt;
    EXPECT_NO_THROW(db.select("test_table", false, columns, conditions));
}

TEST_F(DatabaseWrapperTest, CreateTableWithConstraints) {
    DatabaseWrapper db("test.db");

    CreateTableInput table_input = {
        "test_table",
        {
            {"id", "INTEGER", {"PRIMARY KEY", "NOT NULL"}},
            {"name", "TEXT", {"UNIQUE"}},
            {"age", "INTEGER", {"CHECK(age > 0)"}}
        }
    };

    EXPECT_NO_THROW(db.create_table(table_input));
}

TEST_F(DatabaseWrapperTest, InsertMultipleRows) {
    DatabaseWrapper db("test.db");

    CreateTableInput table_input = {
        "test_table",
        {
            {"id", "INTEGER", {"PRIMARY KEY"}},
            {"name", "TEXT", {}}
        }
    };
    db.create_table(table_input);

    std::vector<std::string> columns = {"id", "name"};
    std::vector<std::string> values1 = {"1", "Name1"};
    std::vector<std::string> values2 = {"2", "Name2"};
    EXPECT_NO_THROW(db.insert_into_table("test_table", columns, values1));
    EXPECT_NO_THROW(db.insert_into_table("test_table", columns, values2));
}

TEST_F(DatabaseWrapperTest, SelectWithConditions) {
    DatabaseWrapper db("test.db");

    CreateTableInput table_input = {
        "test_table",
        {
            {"id", "INTEGER", {"PRIMARY KEY"}},
            {"name", "TEXT", {}}
        }
    };
    db.create_table(table_input);

    std::vector<std::string> columns = {"id", "name"};
    std::vector<std::string> values = {"1", "Test Name"};
    db.insert_into_table("test_table", columns, values);

    std::optional<std::vector<std::string>> conditions = std::make_optional(std::vector<std::string>{"id = 1"});
    EXPECT_NO_THROW(db.select("test_table", false, columns, conditions));
}

TEST_F(DatabaseWrapperTest, UpdateData) {
    DatabaseWrapper db("test.db");

    CreateTableInput table_input = {
        "test_table",
        {
            {"id", "INTEGER", {"PRIMARY KEY"}},
            {"name", "TEXT", {}}
        }
    };
    db.create_table(table_input);

    std::vector<std::string> columns = {"id", "name"};
    std::vector<std::string> values = {"1", "Test Name"};
    db.insert_into_table("test_table", columns, values);

    std::vector<std::pair<std::string, std::string>> updates = {{"name", "Updated Name"}};
    std::vector<std::string> conditions = {"id = 1"};
    EXPECT_NO_THROW(db.update_table("test_table", updates, conditions));
}

TEST_F(DatabaseWrapperTest, DeleteData) {
    DatabaseWrapper db("test.db");

    CreateTableInput table_input = {
        "test_table",
        {
            {"id", "INTEGER", {"PRIMARY KEY"}},
            {"name", "TEXT", {}}
        }
    };
    db.create_table(table_input);

    std::vector<std::string> columns = {"id", "name"};
    std::vector<std::string> values = {"1", "Test Name"};
    db.insert_into_table("test_table", columns, values);

    std::optional<std::vector<std::string>> conditions = std::make_optional(std::vector<std::string>{"id = 1"});
    EXPECT_NO_THROW(db.delete_from_table("test_table", conditions));
}

TEST_F(DatabaseWrapperTest, InsertLargeNumberOfRows) {
    DatabaseWrapper db("test.db");

    CreateTableInput table_input = {
        "test_table",
        {
            {"id", "INTEGER", {"PRIMARY KEY"}},
            {"name", "TEXT", {}}
        }
    };
    db.create_table(table_input);

    std::vector<std::string> columns = {"id", "name"};
    for (int i = 1; i <= 1000; ++i) {
        std::vector<std::string> values = {std::to_string(i), "Name" + std::to_string(i)};
        EXPECT_NO_THROW(db.insert_into_table("test_table", columns, values));
    }
}

TEST_F(DatabaseWrapperTest, SelectFromEmptyTable) {
    DatabaseWrapper db("test.db");

    CreateTableInput table_input = {
        "test_table",
        {
            {"id", "INTEGER", {"PRIMARY KEY"}},
            {"name", "TEXT", {}}
        }
    };
    db.create_table(table_input);

    std::vector<std::string> columns = {"id", "name"};
    std::optional<std::vector<std::string>> conditions = std::nullopt;
    auto result = db.select("test_table", false, columns, conditions);
    EXPECT_TRUE(result.empty());
}

TEST_F(DatabaseWrapperTest, InsertAndSelectNoData) {
    DatabaseWrapper db("test.db");

    CreateTableInput table_input = {
        "test_table",
        {
            {"id", "INTEGER", {"PRIMARY KEY"}},
            {"name", "TEXT", {}}
        }
    };
    db.create_table(table_input);

    std::vector<std::string> columns = {"id", "name"};
    std::vector<std::string> values = {"", ""};
    EXPECT_THROW(db.insert_into_table("test_table", columns, values), std::runtime_error);

    std::optional<std::vector<std::string>> conditions = std::nullopt;
    auto result = db.select("test_table", false, columns, conditions);
    EXPECT_TRUE(result.empty());
}

TEST_F(DatabaseWrapperTest, DeleteFromEmptyTable) {
    DatabaseWrapper db("test.db");

    CreateTableInput table_input = {
        "test_table",
        {
            {"id", "INTEGER", {"PRIMARY KEY"}},
            {"name", "TEXT", {}}
        }
    };
    db.create_table(table_input);

    std::optional<std::vector<std::string>> conditions = std::make_optional(std::vector<std::string>{"id = 1"});
    EXPECT_NO_THROW(db.delete_from_table("test_table", conditions));
}