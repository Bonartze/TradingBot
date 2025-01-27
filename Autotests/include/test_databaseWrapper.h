#ifndef TEST_DATABASEWRAPPER_H
#define TEST_DATABASEWRAPPER_H

#include <gtest/gtest.h>
#include "../../Database/include/DatabaseWrapper.h"

class DatabaseWrapperTest : public ::testing::Test {
protected:
    void SetUp() override;

    void TearDown() override;
};

#endif
