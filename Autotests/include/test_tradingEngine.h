#pragma once
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include "../../TradingEngine/include/AbstractOrderManager.h"
#include <gtest/gtest.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>


class OrderManagerTest : public ::testing::Test {
protected:
    class TestOrderManager : public AbstractOrderManager {
    public:
        TestOrderManager() : AbstractOrderManager("test_key", "test_secret") {
        }

        using AbstractOrderManager::hmac;
        using AbstractOrderManager::base64_encode;
    };

    TestOrderManager orderManager;
};
