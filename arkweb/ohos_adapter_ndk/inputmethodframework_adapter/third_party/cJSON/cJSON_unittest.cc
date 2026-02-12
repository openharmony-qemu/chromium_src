/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "cJSON.h"

namespace {

class CJsonTest : public testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(CJsonTest, cJSON_Version) {
    const char* version = cJSON_Version();
    ASSERT_NE(version, nullptr);
    EXPECT_STRNE("", version);
}

TEST_F(CJsonTest, cJSON_CreateNull) {
    cJSON* item = cJSON_CreateNull();
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(cJSON_IsNull(item));
    EXPECT_FALSE(cJSON_IsInvalid(item));
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_CreateTrue) {
    cJSON* item = cJSON_CreateTrue();
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(cJSON_IsTrue(item));
    EXPECT_TRUE(cJSON_IsBool(item));
    EXPECT_FALSE(cJSON_IsFalse(item));
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_CreateFalse) {
    cJSON* item = cJSON_CreateFalse();
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(cJSON_IsFalse(item));
    EXPECT_TRUE(cJSON_IsBool(item));
    EXPECT_FALSE(cJSON_IsTrue(item));
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_CreateBool) {
    cJSON* item1 = cJSON_CreateBool(1);
    ASSERT_NE(item1, nullptr);
    EXPECT_TRUE(cJSON_IsTrue(item1));
    cJSON_Delete(item1);

    cJSON* item2 = cJSON_CreateBool(0);
    ASSERT_NE(item2, nullptr);
    EXPECT_TRUE(cJSON_IsFalse(item2));
    cJSON_Delete(item2);
}

TEST_F(CJsonTest, cJSON_CreateNumber) {
    cJSON* item = cJSON_CreateNumber(3.14);
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(cJSON_IsNumber(item));
    EXPECT_DOUBLE_EQ(3.14, item->valuedouble);
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_CreateString) {
    cJSON* item = cJSON_CreateString("Hello, World!");
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(cJSON_IsString(item));
    EXPECT_STREQ("Hello, World!", item->valuestring);
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_CreateArray) {
    cJSON* item = cJSON_CreateArray();
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(cJSON_IsArray(item));
    EXPECT_EQ(cJSON_GetArraySize(item), 0);
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_CreateObject) {
    cJSON* item = cJSON_CreateObject();
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(cJSON_IsObject(item));
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_CreateIntArray) {
    int numbers[3] = {1, 2, 3};
    cJSON* item = cJSON_CreateIntArray(numbers, 3);
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(cJSON_IsArray(item));
    EXPECT_EQ(cJSON_GetArraySize(item), 3);
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_CreateStringArray) {
    const char* strings[3] = {"hello", "world", "test"};
    cJSON* item = cJSON_CreateStringArray(strings, 3);
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(cJSON_IsArray(item));
    EXPECT_EQ(cJSON_GetArraySize(item), 3);
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_AddItemToArray) {
    cJSON* array = cJSON_CreateArray();
    cJSON* item = cJSON_CreateNumber(42);
    cJSON_AddItemToArray(array, item);
    EXPECT_EQ(cJSON_GetArraySize(array), 1);
    cJSON_Delete(array);
}

TEST_F(CJsonTest, cJSON_AddItemToObject) {
    cJSON* object = cJSON_CreateObject();
    cJSON* item = cJSON_CreateString("value");
    cJSON_AddItemToObject(object, "key", item);
    cJSON* result = cJSON_GetObjectItem(object, "key");
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(cJSON_IsString(result));
    cJSON_Delete(object);
}

TEST_F(CJsonTest, cJSON_AddNullToObject) {
    cJSON* object = cJSON_CreateObject();
    cJSON_AddNullToObject(object, "nullKey");
    cJSON* result = cJSON_GetObjectItem(object, "nullKey");
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(cJSON_IsNull(result));
    cJSON_Delete(object);
}

TEST_F(CJsonTest, cJSON_AddTrueToObject) {
    cJSON* object = cJSON_CreateObject();
    cJSON_AddTrueToObject(object, "trueKey");
    cJSON* result = cJSON_GetObjectItem(object, "trueKey");
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(cJSON_IsTrue(result));
    cJSON_Delete(object);
}

TEST_F(CJsonTest, cJSON_AddFalseToObject) {
    cJSON* object = cJSON_CreateObject();
    cJSON_AddFalseToObject(object, "falseKey");
    cJSON* result = cJSON_GetObjectItem(object, "falseKey");
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(cJSON_IsFalse(result));
    cJSON_Delete(object);
}

TEST_F(CJsonTest, cJSON_AddNumberToObject) {
    cJSON* object = cJSON_CreateObject();
    cJSON_AddNumberToObject(object, "numKey", 3.14);
    cJSON* result = cJSON_GetObjectItem(object, "numKey");
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(cJSON_IsNumber(result));
    EXPECT_DOUBLE_EQ(3.14, result->valuedouble);
    cJSON_Delete(object);
}

TEST_F(CJsonTest, cJSON_AddStringToObject) {
    cJSON* object = cJSON_CreateObject();
    cJSON_AddStringToObject(object, "strKey", "test");
    cJSON* result = cJSON_GetObjectItem(object, "strKey");
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ("test", result->valuestring);
    cJSON_Delete(object);
}

TEST_F(CJsonTest, cJSON_Parse_SimpleObject) {
    const char* json = "{\"name\":\"test\",\"value\":123}";
    cJSON* item = cJSON_Parse(json);
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(cJSON_IsObject(item));

    cJSON* name = cJSON_GetObjectItem(item, "name");
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ("test", name->valuestring);

    cJSON* value = cJSON_GetObjectItem(item, "value");
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(123, value->valueint);

    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_Parse_SimpleArray) {
    const char* json = "[1,2,3,4,5]";
    cJSON* item = cJSON_Parse(json);
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(cJSON_IsArray(item));
    EXPECT_EQ(cJSON_GetArraySize(item), 5);
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_Parse_NullValue) {
    const char* json = "{\"value\":null}";
    cJSON* item = cJSON_Parse(json);
    ASSERT_NE(item, nullptr);
    cJSON* value = cJSON_GetObjectItem(item, "value");
    EXPECT_TRUE(cJSON_IsNull(value));
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_Print_SimpleObject) {
    cJSON* item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "name", "test");
    cJSON_AddNumberToObject(item, "value", 42);

    char* printed = cJSON_Print(item);
    ASSERT_NE(printed, nullptr);
    EXPECT_NE(strlen(printed), 0u);
    free(printed);
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_PrintUnformatted_SimpleObject) {
    cJSON* item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "key", "value");

    char* printed = cJSON_PrintUnformatted(item);
    ASSERT_NE(printed, nullptr);
    EXPECT_NE(strlen(printed), 0u);
    free(printed);
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_GetArraySize) {
    cJSON* array = cJSON_CreateArray();
    EXPECT_EQ(cJSON_GetArraySize(array), 0);
    cJSON_AddItemToArray(array, cJSON_CreateNumber(1));
    EXPECT_EQ(cJSON_GetArraySize(array), 1);
    cJSON_AddItemToArray(array, cJSON_CreateNumber(2));
    EXPECT_EQ(cJSON_GetArraySize(array), 2);
    cJSON_Delete(array);
}

TEST_F(CJsonTest, cJSON_GetArrayItem) {
    cJSON* array = cJSON_CreateArray();
    cJSON_AddItemToArray(array, cJSON_CreateNumber(10));
    cJSON_AddItemToArray(array, cJSON_CreateNumber(20));
    cJSON_AddItemToArray(array, cJSON_CreateNumber(30));

    cJSON* item0 = cJSON_GetArrayItem(array, 0);
    ASSERT_NE(item0, nullptr);
    EXPECT_EQ(10, item0->valueint);

    cJSON* item2 = cJSON_GetArrayItem(array, 2);
    ASSERT_NE(item2, nullptr);
    EXPECT_EQ(30, item2->valueint);

    cJSON_Delete(array);
}

TEST_F(CJsonTest, cJSON_GetObjectItem) {
    cJSON* object = cJSON_CreateObject();
    cJSON_AddStringToObject(object, "name", "test");
    cJSON_AddNumberToObject(object, "value", 100);

    cJSON* nameItem = cJSON_GetObjectItem(object, "name");
    ASSERT_NE(nameItem, nullptr);
    EXPECT_STREQ("test", nameItem->valuestring);

    cJSON* valueItem = cJSON_GetObjectItem(object, "value");
    ASSERT_NE(valueItem, nullptr);
    EXPECT_EQ(100, valueItem->valueint);

    cJSON_Delete(object);
}

TEST_F(CJsonTest, cJSON_IsTypes) {
    EXPECT_TRUE(cJSON_IsNull(cJSON_CreateNull()));
    EXPECT_TRUE(cJSON_IsTrue(cJSON_CreateTrue()));
    EXPECT_TRUE(cJSON_IsFalse(cJSON_CreateFalse()));
    EXPECT_TRUE(cJSON_IsBool(cJSON_CreateTrue()));
    EXPECT_TRUE(cJSON_IsNumber(cJSON_CreateNumber(1)));
    EXPECT_TRUE(cJSON_IsString(cJSON_CreateString("test")));
    EXPECT_TRUE(cJSON_IsArray(cJSON_CreateArray()));
    EXPECT_TRUE(cJSON_IsObject(cJSON_CreateObject()));
    EXPECT_TRUE(cJSON_IsRaw(cJSON_CreateRaw("raw")));
}

TEST_F(CJsonTest, cJSON_Duplicate) {
    cJSON* original = cJSON_CreateObject();
    cJSON_AddStringToObject(original, "name", "test");
    cJSON_AddNumberToObject(original, "value", 42);

    cJSON* duplicate = cJSON_Duplicate(original, 1);
    ASSERT_NE(duplicate, nullptr);
    EXPECT_TRUE(cJSON_IsObject(duplicate));

    cJSON* name = cJSON_GetObjectItem(duplicate, "name");
    ASSERT_NE(name, nullptr);
    EXPECT_STREQ("test", name->valuestring);

    cJSON_Delete(original);
    cJSON_Delete(duplicate);
}

TEST_F(CJsonTest, cJSON_Compare) {
    cJSON* item1 = cJSON_CreateString("test");
    cJSON* item2 = cJSON_CreateString("test");
    cJSON* item3 = cJSON_CreateString("other");

    EXPECT_TRUE(cJSON_Compare(item1, item2, 1));
    EXPECT_FALSE(cJSON_Compare(item1, item3, 1));

    cJSON_Delete(item1);
    cJSON_Delete(item2);
    cJSON_Delete(item3);
}

TEST_F(CJsonTest, cJSON_Minify) {
    char json[] = "{\n  \"name\":  \"test\",\n  \"value\": 123\n}";
    cJSON_Minify(json);
    EXPECT_EQ(strchr(json, '\n'), nullptr);
    EXPECT_EQ(strchr(json, ' '), nullptr);
}

TEST_F(CJsonTest, cJSON_DeleteItemFromArray) {
    cJSON* array = cJSON_CreateArray();
    cJSON_AddItemToArray(array, cJSON_CreateNumber(1));
    cJSON_AddItemToArray(array, cJSON_CreateNumber(2));
    cJSON_AddItemToArray(array, cJSON_CreateNumber(3));

    cJSON_DeleteItemFromArray(array, 1);
    EXPECT_EQ(cJSON_GetArraySize(array), 2);

    cJSON* item = cJSON_GetArrayItem(array, 1);
    ASSERT_NE(item, nullptr);
    EXPECT_EQ(3, item->valueint);

    cJSON_Delete(array);
}

TEST_F(CJsonTest, cJSON_DeleteItemFromObject) {
    cJSON* object = cJSON_CreateObject();
    cJSON_AddStringToObject(object, "key1", "value1");
    cJSON_AddStringToObject(object, "key2", "value2");

    cJSON_DeleteItemFromObject(object, "key1");
    EXPECT_EQ(cJSON_GetObjectItem(object, "key1"), nullptr);

    cJSON* key2 = cJSON_GetObjectItem(object, "key2");
    ASSERT_NE(key2, nullptr);
    EXPECT_STREQ("value2", key2->valuestring);

    cJSON_Delete(object);
}

TEST_F(CJsonTest, cJSON_DetachItemFromArray) {
    cJSON* array = cJSON_CreateArray();
    cJSON_AddItemToArray(array, cJSON_CreateNumber(10));
    cJSON_AddItemToArray(array, cJSON_CreateNumber(20));

    cJSON* detached = cJSON_DetachItemFromArray(array, 0);
    ASSERT_NE(detached, nullptr);
    EXPECT_EQ(10, detached->valueint);
    EXPECT_EQ(cJSON_GetArraySize(array), 1);

    cJSON_Delete(detached);
    cJSON_Delete(array);
}

TEST_F(CJsonTest, cJSON_DetachItemFromObject) {
    cJSON* object = cJSON_CreateObject();
    cJSON_AddStringToObject(object, "key", "value");

    cJSON* detached = cJSON_DetachItemFromObject(object, "key");
    ASSERT_NE(detached, nullptr);
    EXPECT_STREQ("value", detached->valuestring);
    EXPECT_EQ(cJSON_GetObjectItem(object, "key"), nullptr);

    cJSON_Delete(detached);
    cJSON_Delete(object);
}

TEST_F(CJsonTest, cJSON_InsertItemInArray) {
    cJSON* array = cJSON_CreateArray();
    cJSON_AddItemToArray(array, cJSON_CreateNumber(1));
    cJSON_AddItemToArray(array, cJSON_CreateNumber(3));

    cJSON* newItem = cJSON_CreateNumber(2);
    cJSON_InsertItemInArray(array, 1, newItem);

    EXPECT_EQ(cJSON_GetArraySize(array), 3);

    cJSON* item1 = cJSON_GetArrayItem(array, 1);
    EXPECT_EQ(2, item1->valueint);

    cJSON_Delete(array);
}

TEST_F(CJsonTest, cJSON_ReplaceItemInArray) {
    cJSON* array = cJSON_CreateArray();
    cJSON_AddItemToArray(array, cJSON_CreateNumber(1));
    cJSON_AddItemToArray(array, cJSON_CreateNumber(2));

    cJSON* newItem = cJSON_CreateNumber(99);
    cJSON_ReplaceItemInArray(array, 0, newItem);

    cJSON* item = cJSON_GetArrayItem(array, 0);
    EXPECT_EQ(99, item->valueint);

    cJSON_Delete(array);
}

TEST_F(CJsonTest, cJSON_ReplaceItemInObject) {
    cJSON* object = cJSON_CreateObject();
    cJSON_AddStringToObject(object, "key", "old");

    cJSON* newItem = cJSON_CreateString("new");
    cJSON_ReplaceItemInObject(object, "key", newItem);

    cJSON* item = cJSON_GetObjectItem(object, "key");
    EXPECT_STREQ("new", item->valuestring);

    cJSON_Delete(object);
}

TEST_F(CJsonTest, cJSON_GetStringValue) {
    cJSON* item = cJSON_CreateString("test");
    EXPECT_STREQ("test", cJSON_GetStringValue(item));
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_GetNumberValue) {
    cJSON* item = cJSON_CreateNumber(3.14);
    EXPECT_DOUBLE_EQ(3.14, cJSON_GetNumberValue(item));
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_SetValuestring) {
    cJSON* item = cJSON_CreateString("old");
    cJSON_SetValuestring(item, "new");
    EXPECT_STREQ("new", item->valuestring);
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_SetNumberValue) {
    cJSON* item = cJSON_CreateNumber(1.0);
    cJSON_SetNumberValue(item, 99.99);
    EXPECT_DOUBLE_EQ(99.99, item->valuedouble);
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_HasObjectItem) {
    cJSON* object = cJSON_CreateObject();
    cJSON_AddStringToObject(object, "exists", "value");

    EXPECT_TRUE(cJSON_HasObjectItem(object, "exists"));
    EXPECT_FALSE(cJSON_HasObjectItem(object, "notexists"));

    cJSON_Delete(object);
}

TEST_F(CJsonTest, cJSON_AddArrayToObject) {
    cJSON* object = cJSON_CreateObject();
    cJSON* array = cJSON_CreateArray();
    cJSON_AddItemToArray(array, cJSON_CreateNumber(1));

    cJSON_AddArrayToObject(object, "arr");

    cJSON* result = cJSON_GetObjectItem(object, "arr");
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(cJSON_IsArray(result));

    cJSON_Delete(object);
}

TEST_F(CJsonTest, cJSON_AddObjectToObject) {
    cJSON* object = cJSON_CreateObject();
    cJSON* nested = cJSON_CreateObject();
    cJSON_AddStringToObject(nested, "key", "value");

    cJSON_AddObjectToObject(object, "nested");

    cJSON* result = cJSON_GetObjectItem(object, "nested");
    ASSERT_NE(result, nullptr);
    EXPECT_TRUE(cJSON_IsObject(result));

    cJSON_Delete(object);
}

TEST_F(CJsonTest, cJSON_Parse_WithWhitespace) {
    const char* json = "  {  \"key\"  :  \"value\"  }  ";
    cJSON* item = cJSON_Parse(json);
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(cJSON_IsObject(item));

    cJSON* key = cJSON_GetObjectItem(item, "key");
    ASSERT_NE(key, nullptr);
    EXPECT_STREQ("value", key->valuestring);

    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_Parse_EmptyString) {
    cJSON* item = cJSON_Parse("");
    EXPECT_EQ(item, nullptr);
}

TEST_F(CJsonTest, cJSON_Parse_InvalidJson) {
    cJSON* item = cJSON_Parse("{invalid json}");
    EXPECT_EQ(item, nullptr);
}

TEST_F(CJsonTest, cJSON_ParseWithLength) {
    const char* json = "{\"key\":\"value\"}extra";
    cJSON* item = cJSON_ParseWithLength(json, 16);
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(cJSON_IsObject(item));
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_PrintBuffered) {
    cJSON* item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "key", "value");

    char* buffer = cJSON_PrintBuffered(item, 1024, 1);
    ASSERT_NE(buffer, nullptr);
    EXPECT_NE(strlen(buffer), 0u);

    free(buffer);
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_PrintPreallocated) {
    cJSON* item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "key", "value");

    char buffer[256];
    int result = cJSON_PrintPreallocated(item, buffer, 256, cJSON_True);
    EXPECT_EQ(result, 1);

    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_malloc_free) {
    void* ptr = cJSON_malloc(100);
    ASSERT_NE(ptr, nullptr);
    cJSON_free(ptr);
}

TEST_F(CJsonTest, cJSON_InitHooks) {
    cJSON_Hooks hooks;
    hooks.malloc_fn = nullptr;
    hooks.free_fn = nullptr;
    cJSON_InitHooks(&hooks);
}

TEST_F(CJsonTest, cJSON_GetErrorPtr) {
    const char* json = "{\"invalid\":}";
    cJSON_Parse(json);
    const char* error = cJSON_GetErrorPtr();
    EXPECT_NE(error, nullptr);
}

TEST_F(CJsonTest, cJSON_CreateStringReference) {
    cJSON* item = cJSON_CreateStringReference("test");
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(cJSON_IsString(item));
    EXPECT_STREQ("test", item->valuestring);
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_CreateObjectReference) {
    cJSON* original = cJSON_CreateObject();
    cJSON_AddStringToObject(original, "key", "value");

    cJSON* ref = cJSON_CreateObjectReference(original);
    ASSERT_NE(ref, nullptr);
    EXPECT_TRUE(cJSON_IsObject(ref));

    cJSON_Delete(original);
    cJSON_Delete(ref);
}

TEST_F(CJsonTest, cJSON_AddItemReferenceToArray) {
    cJSON* array = cJSON_CreateArray();
    cJSON* item = cJSON_CreateString("test");

    cJSON_AddItemReferenceToArray(array, item);
    EXPECT_EQ(cJSON_GetArraySize(array), 1);

    cJSON_Delete(array);
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_AddItemReferenceToObject) {
    cJSON* object = cJSON_CreateObject();
    cJSON* item = cJSON_CreateString("test");

    cJSON_AddItemReferenceToObject(object, "key", item);

    cJSON* result = cJSON_GetObjectItem(object, "key");
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ("test", result->valuestring);

    cJSON_Delete(object);
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_CreateRaw) {
    cJSON* item = cJSON_CreateRaw("raw_value");
    ASSERT_NE(item, nullptr);
    EXPECT_TRUE(cJSON_IsRaw(item));
    cJSON_Delete(item);
}

TEST_F(CJsonTest, cJSON_ArrayDuplicate) {
    cJSON* array = cJSON_CreateArray();
    cJSON_AddItemToArray(array, cJSON_CreateNumber(1));
    cJSON_AddItemToArray(array, cJSON_CreateNumber(2));

    cJSON* dup = cJSON_Duplicate(array, 1);
    ASSERT_NE(dup, nullptr);
    EXPECT_EQ(cJSON_GetArraySize(array), cJSON_GetArraySize(dup));

    cJSON_Delete(array);
    cJSON_Delete(dup);
}

}  // namespace
