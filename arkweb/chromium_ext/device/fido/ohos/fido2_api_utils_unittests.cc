// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/device/fido/ohos/fido2_api_utils.h"

#include <gtest/gtest.h>
#include "base/json/json_reader.h"
#include "base/no_destructor.h"
#include "device/fido/attestation_statement_formats.h"
#include "device/fido/ohos/type_conversions.h"

using namespace testing;
namespace device {

std::optional<base::Value::Dict> ExtractAttestationObjectAsJson(
    const FIDO2_PublicKeyAttestationCredential* credential_ptr);
std::unique_ptr<PackedAttestationStatement> BuildPackedAttestationStatement(
    const base::Value::Dict* att_stmt);

class Fido2ApiUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void Fido2ApiUtilsTest::SetUpTestCase() {}

void Fido2ApiUtilsTest::TearDownTestCase() {}

void Fido2ApiUtilsTest::SetUp() {}

void Fido2ApiUtilsTest::TearDown() {}

TEST_F(Fido2ApiUtilsTest, ExtractAttestationObjectAsJsonTest_001)
{
    const FIDO2_PublicKeyAttestationCredential* credential_ptr = nullptr;
    auto ret = ExtractAttestationObjectAsJson(credential_ptr); 
    EXPECT_FALSE(ret.has_value());
}

TEST_F(Fido2ApiUtilsTest, ExtractAttestationObjectAsJsonTest_002)
{
    FIDO2_PublicKeyAttestationCredential credential;
    Uint8Buff buffer = {0};
    credential.response.attestationObject = buffer;
    auto ret = ExtractAttestationObjectAsJson(&credential); 
    EXPECT_FALSE(ret.has_value());
}

TEST_F(Fido2ApiUtilsTest, ExtractAttestationObjectAsJsonTest_003)
{
    FIDO2_PublicKeyAttestationCredential credential;
    const char* json_str = "\"key\": \"value\"";
    uint8_t* json_data = reinterpret_cast<uint8_t*>(const_cast<char*>(json_str));
    const size_t len = strlen(json_str);
    Uint8Buff buffer = {static_cast<uint32_t>(len), json_data};
    credential.response.attestationObject = buffer;
    auto ret = ExtractAttestationObjectAsJson(&credential); 
    EXPECT_FALSE(ret.has_value());
}

TEST_F(Fido2ApiUtilsTest, ExtractAttestationObjectAsJsonTest_004)
{
    FIDO2_PublicKeyAttestationCredential credential;
    const char* json_str = "{ \"key\": \"value\" }";
    uint8_t* json_data = reinterpret_cast<uint8_t*>(const_cast<char*>(json_str));
    const size_t len = strlen(json_str);
    Uint8Buff buffer = {static_cast<uint32_t>(len), json_data};
    credential.response.attestationObject = buffer;
    auto ret = ExtractAttestationObjectAsJson(&credential); 
    EXPECT_TRUE(ret.has_value());
}

TEST_F(Fido2ApiUtilsTest, BuildPackedAttestationStatementTest_001)
{
    base::Value::Dict att_stmt;
    att_stmt.Set("sig", "signature");
    att_stmt.Set("x5c", base::Value::List());
    auto ret = BuildPackedAttestationStatement(&att_stmt); 
    EXPECT_EQ(ret, nullptr);
}

TEST_F(Fido2ApiUtilsTest, BuildPackedAttestationStatementTest_002)
{
    base::Value::Dict att_stmt;
    att_stmt.Set("alg", 1);
    att_stmt.Set("x5c", base::Value::List());
    auto ret = BuildPackedAttestationStatement(&att_stmt); 
    EXPECT_EQ(ret, nullptr);
}

TEST_F(Fido2ApiUtilsTest, BuildPackedAttestationStatementTest_003)
{
    base::Value::Dict att_stmt;
    att_stmt.Set("alg", 1);
    att_stmt.Set("sig", "signature");
    auto ret = BuildPackedAttestationStatement(&att_stmt); 
    EXPECT_EQ(ret, nullptr);
}

TEST_F(Fido2ApiUtilsTest, BuildPackedAttestationStatementTest_004)
{
    base::Value::Dict att_stmt;
    att_stmt.Set("alg", 1);
    att_stmt.Set("sig", "signature");
    att_stmt.Set("x5c", base::Value::List().Append(base::Value(1)));
    auto ret = BuildPackedAttestationStatement(&att_stmt); 
    EXPECT_EQ(ret, nullptr);
}

TEST_F(Fido2ApiUtilsTest, BuildPackedAttestationStatementTest_005)
{
    base::Value::Dict att_stmt;
    att_stmt.Set("alg", 1);
    att_stmt.Set("sig", "signature");
    att_stmt.Set("x5c", base::Value::List().Append("cert1").Append("cert2"));
    auto ret = BuildPackedAttestationStatement(&att_stmt); 
    EXPECT_NE(ret, nullptr);
}

TEST_F(Fido2ApiUtilsTest, ParseTest_001)
{
    FIDO2_PublicKeyAttestationCredential* credential_ptr = nullptr;
    auto ret = Parse(credential_ptr);
    EXPECT_FALSE(ret.has_value());
}

TEST_F(Fido2ApiUtilsTest, ParseTest_002)
{
    FIDO2_PublicKeyAttestationCredential credential;
    const char* json_str = "{ \"key\": \"value\" }";
    uint8_t* json_data = reinterpret_cast<uint8_t*>(const_cast<char*>(json_str));
    const size_t len = strlen(json_str);
    Uint8Buff buffer = {static_cast<uint32_t>(len), json_data};
    credential.response.attestationObject = buffer;
    auto ret = Parse(&credential);
    EXPECT_FALSE(ret.has_value());
}

TEST_F(Fido2ApiUtilsTest, ParseTest_003)
{
    FIDO2_PublicKeyAttestationCredential credential;
    const char* json_str = "{ \"fmt\": \"packed\" }";
    uint8_t* json_data = reinterpret_cast<uint8_t*>(const_cast<char*>(json_str));
    const size_t len = strlen(json_str);
    Uint8Buff buffer = {static_cast<uint32_t>(len), json_data};
    credential.response.attestationObject = buffer;
    auto ret = Parse(&credential);
    EXPECT_FALSE(ret.has_value());
}

TEST_F(Fido2ApiUtilsTest, ParseTest_004)
{
    FIDO2_PublicKeyAttestationCredential credential;
    const char* json_str = "{ \"fmt\": \"packed\", \"attStmt\": {}}";
    uint8_t* json_data = reinterpret_cast<uint8_t*>(const_cast<char*>(json_str));
    const size_t len = strlen(json_str);
    Uint8Buff buffer = {static_cast<uint32_t>(len), json_data};
    credential.response.attestationObject = buffer;
    auto ret = Parse(&credential);
    EXPECT_FALSE(ret.has_value());
}

TEST_F(Fido2ApiUtilsTest, ParseTest_005)
{
    FIDO2_PublicKeyAttestationCredential credential;
    const char* json_str = "{\"fmt\": \"packed\", \"attStmt\": {\"alg\": 1, \"sig\":"
        " \"signature\", \"x5c\": [\"cert\"]}}";
    uint8_t* json_data = reinterpret_cast<uint8_t*>(const_cast<char*>(json_str));
    const size_t len = strlen(json_str);
    Uint8Buff buffer = {static_cast<uint32_t>(len), json_data};
    credential.response.attestationObject = buffer;
    auto ret = Parse(&credential);
    EXPECT_FALSE(ret.has_value());
}

TEST_F(Fido2ApiUtilsTest, ParseTest_006)
{
    FIDO2_PublicKeyAttestationCredential credential;
    const char* json_str = "{\"fmt\": \"packed\", \"attStmt\": {\"alg\": 1, \"sig\":"
        " \"signature\", \"x5c\": [\"cert\"]}, \"authData\": \"authData\"}";
    uint8_t* json_data = reinterpret_cast<uint8_t*>(const_cast<char*>(json_str));
    const size_t len = strlen(json_str);
    Uint8Buff buffer = {static_cast<uint32_t>(len), json_data};
    credential.response.attestationObject = buffer;
    auto ret = Parse(&credential);
    EXPECT_FALSE(ret.has_value());
}

TEST_F(Fido2ApiUtilsTest, ParseTest_007)
{
    FIDO2_PublicKeyAttestationCredential credential;
    const char* json_str = "{\"fmt\": \"\", \"attStmt\": {\"alg\": 1, \"sig\":"
        " \"signature\", \"x5c\": [\"cert\"]}, \"authData\": \"authData\"}";
    uint8_t* json_data = reinterpret_cast<uint8_t*>(const_cast<char*>(json_str));
    const size_t len = strlen(json_str);
    Uint8Buff buffer = {static_cast<uint32_t>(len), json_data};
    credential.response.attestationObject = buffer;
    auto ret = Parse(&credential);
    EXPECT_FALSE(ret.has_value());
}
} // namespace base