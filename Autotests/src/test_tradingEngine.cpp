#include "../include/test_tradingEngine.h"

TEST_F(OrderManagerTest, BhmacCorrectness) {
    std::string key = "secret";
    std::string data = "message";

    unsigned char expected_digest[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    HMAC(EVP_sha256(), key.c_str(), key.length(),
         reinterpret_cast<const unsigned char *>(data.c_str()), data.length(),
         expected_digest, &len);

    std::string result = orderManager.hmac(key, data, EVP_sha256);

    ASSERT_EQ(result.size(), len);

    EXPECT_EQ(result, std::string(reinterpret_cast<char*>(expected_digest), len));
}

TEST_F(OrderManagerTest, Base64EncodeCorrectness) {
    std::string input = "hello";
    std::string expected_output = "aGVsbG8=";

    std::string encoded = orderManager.base64_encode(input);

    EXPECT_EQ(encoded, expected_output);
}
