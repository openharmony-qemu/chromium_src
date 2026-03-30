/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include <map>
#include <aki/jsbind.h>

namespace {
constexpr long NUM_FOUR = 4;
constexpr long NUM_FIVE = 5;
constexpr long NUM_SIX = 6;
}

class JsonObjectHandle {
  public:
    JsonObjectHandle() = default;
    std::map<std::string, long> PassingVoidReturnMapLong() {
        std::map<std::string, long> map = {{"Monday", 1}, {"Tuesday", 2}, {"Wednesday", 3}};
        return map;
    }

    std::map<std::string, long> PassingMapLongReturnMapLong(std::map<std::string, long> map) {
        map["Thursday"] = 4;
        return map;
    }
};

JSBIND_CLASS(JsonObjectHandle) {
    JSBIND_CONSTRUCTOR<>();
    JSBIND_METHOD(PassingVoidReturnMapLong, "passingVoidReturnMapLong");
    JSBIND_METHOD(PassingMapLongReturnMapLong, "passingMapLongReturnMapLong");
}

std::map<std::string, long> PassingVoidReturnMapLong() {
    std::map<std::string, long> map = {{"Monday", 1}, {"Tuesday", 2}, {"Wednesday", 3}};
    return map;
}

std::map<std::string, long> PassingMapLongReturnMapLong(std::map<std::string, long> map) {
    map["Thursday"] = 4;
    return map;
}

std::map<long, std::string> PassingVoidReturnMapLongString()
{
    std::map<long, std::string> map = {{1, "Monday"}, {2, "Tuesday"}, {3, "Wednesday"}};
    return map;
}

std::map<long, std::string> PassingMapLongStringReturnMapLongString(std::map<long, std::string> map)
{
    map[NUM_FOUR] = "Thursday";
    return map;
}

std::map<long, std::string> PassingMapAkiLongStringReturnMapLongString(std::map<aki::Value, aki::Value> map)
{
    std::map<long, std::string> mapResult;
    for (const auto& pair : map) {
        long key = 0;
        if (pair.first.IsString()) {
            if (!aki::Value::StringToNumber<long>(pair.first.ObjectAs<std::string>(), key)) {
                AKI_LOG(ERROR) << "StringToNumber failed";
            }
        } else if (pair.first.IsNumber()) {
            key = pair.first.ObjectAs<long>();
        } else {
            AKI_LOG(WARNING) << "key is other data types";
            continue;
        }

        std::string value = "";
        if (pair.second.IsString()) {
            value = pair.second.ObjectAs<std::string>();
        } else if (pair.second.IsNumber()) {
            value = std::to_string(pair.second.ObjectAs<double>());
        } else {
            AKI_LOG(WARNING) << "value is other data types" << ", key: " << key;
            continue;
        }

        mapResult[key] = value;
    }

    mapResult[NUM_SIX] = "Saturday";
    return mapResult;
}

std::map<std::string, std::string> PassingMapAkiStringStringReturnMapStringString(std::map<aki::Value, aki::Value> map)
{
    std::map<std::string, std::string> mapResult;
    for (const auto& pair : map) {
        std::string key = "";
        if (pair.first.IsString()) {
            key = pair.first.ObjectAs<std::string>();
        } else if (pair.first.IsNumber()) {
            key = std::to_string(pair.first.ObjectAs<double>());
        } else {
            AKI_LOG(WARNING) << "key is other data types";
            continue;
        }

        std::string value = "";
        if (pair.second.IsString()) {
            value = pair.second.ObjectAs<std::string>();
        } else if (pair.second.IsNumber()) {
            value = std::to_string(pair.second.ObjectAs<double>());
        } else {
            AKI_LOG(WARNING) << "value is other data types" << ", key: " << key;
            continue;
        }

        mapResult[key] = value;
    }

    mapResult["0"] = "value0";
    return mapResult;
}

std::map<aki::Value, std::string> PassingMapAkiStringReturnMapAkiString(std::map<aki::Value, std::string> map)
{
    map[aki::Value(0)] = "value0";
    return map;
}

std::unordered_map<std::string, long> PassingVoidReturnHashMapLong()
{
    std::unordered_map<std::string, long> hashMap = {{"Monday", 1}, {"Tuesday", 2}, {"Wednesday", 3}};
    return hashMap;
}

std::unordered_map<std::string, long> PassingMapLongReturnHashMapLong(std::unordered_map<std::string, long> hashMap)
{
    hashMap["Thursday"] = NUM_FOUR;
    return hashMap;
}

std::unordered_map<long, std::string> PassingVoidReturnHashMapLongString()
{
    std::unordered_map<long, std::string> hashMap = {{1, "Monday"}, {2, "Tuesday"}, {3, "Wednesday"}};
    return hashMap;
}

std::unordered_map<long, std::string> PassingMapLongStringReturnHashMapLongString(
    std::unordered_map<long, std::string> hashMap)
{
    hashMap[NUM_FOUR] = "Thursday";
    return hashMap;
}

std::unordered_map<long, std::string> PassingMapAkiLongStringReturnHashMapLongString(
    std::unordered_map<aki::Value, std::string, aki::Value::HashPair> hashMap)
{
    std::unordered_map<long, std::string> hashMapResult;
    for (const auto& pair : hashMap) {
        long key = 0;
        if (pair.first.IsString()) {
            if (!aki::Value::StringToNumber<long>(pair.first.ObjectAs<std::string>(), key)) {
                AKI_LOG(ERROR) << "StringToNumber failed";
            }
        } else if (pair.first.IsNumber()) {
            key = pair.first.ObjectAs<long>();
        } else {
            AKI_LOG(WARNING) << "Other data types" << ", Value: " << pair.second;
            continue;
        }

        hashMapResult[key] = pair.second;
    }

    hashMapResult[NUM_FIVE] = "Friday";
    return hashMapResult;
}

std::unordered_map<std::string, std::string> PassingMapAkiStringReturnHashMapStringString(
    std::unordered_map<aki::Value, aki::Value, aki::Value::HashPair> hashMap)
{
    std::unordered_map<std::string, std::string> hashMapResult;
    for (const auto& pair : hashMap) {
        std::string key = "";
        if (pair.first.IsString()) {
            key = pair.first.ObjectAs<std::string>();
        } else if (pair.first.IsNumber()) {
            key = std::to_string(pair.first.ObjectAs<double>());
        } else {
            AKI_LOG(WARNING) << "key is other data types";
            continue;
        }

        std::string value = "";
        if (pair.second.IsString()) {
            value = pair.second.ObjectAs<std::string>();
        } else if (pair.second.IsNumber()) {
            value = std::to_string(pair.second.ObjectAs<double>());
        } else {
            AKI_LOG(WARNING) << "value is other data types" << ", key: " << key;
            continue;
        }

        hashMapResult[key] = value;
    }

    hashMapResult["0"] = "value0";
    return hashMapResult;
}

std::unordered_map<aki::Value, std::string, aki::Value::HashPair> PassingMapAkiStringReturnHashMapAkiString(
    std::unordered_map<aki::Value, std::string, aki::Value::HashPair> hashMap)
{
    hashMap[aki::Value(0)] = "value0";
    return hashMap;
}

std::unordered_set<long> PassingVoidReturnHashSetLong()
{
    std::unordered_set<long> hashSet = {1, 2, 3};
    return hashSet;
}

std::unordered_set<long> PassingSetLongReturnHashSetLong(std::unordered_set<long> hashSet)
{
    hashSet.insert(NUM_FOUR);
    return hashSet;
}

std::unordered_set<long> PassingSetAkiLongReturnHashSetLong(
    std::unordered_set<aki::Value, aki::Value::HashPair> hashSet)
{
    std::unordered_set<long> hashSetResult;
    for (const auto& akiValue : hashSet) {
        long value = 0;
        if (akiValue.IsString()) {
            if (!aki::Value::StringToNumber<long>(akiValue.ObjectAs<std::string>(), value)) {
                AKI_LOG(ERROR) << "StringToNumber failed";
            }
        } else if (akiValue.IsNumber()) {
            value = akiValue.ObjectAs<long>();
        } else {
            AKI_LOG(WARNING) << "Other data types";
            continue;
        }

        hashSetResult.insert(value);
    }

    hashSetResult.insert(NUM_FIVE);
    return hashSetResult;
}

std::unordered_set<aki::Value, aki::Value::HashPair> PassingSetAkiLongReturnHashSetAkiLong(
    std::unordered_set<aki::Value, aki::Value::HashPair> hashSet)
{
    hashSet.insert(aki::Value(0));
    return hashSet;
}

std::unordered_set<std::string> PassingVoidReturnHashSetString()
{
    std::unordered_set<std::string> hashSet = {"Monday", "Tuesday", "Wednesday"};
    return hashSet;
}

std::unordered_set<std::string> PassingSetStringReturnHashSetString(std::unordered_set<std::string> hashSet)
{
    hashSet.insert("Thursday");
    return hashSet;
}

std::unordered_set<std::string> PassingSetAkiStringReturnHashSetString(
    std::unordered_set<aki::Value, aki::Value::HashPair> hashSet)
{
    std::unordered_set<std::string> hashSetResult;
    for (const auto& akiValue : hashSet) {
        std::string value = "";
        if (akiValue.IsString()) {
            value = akiValue.ObjectAs<std::string>();
        } else if (akiValue.IsNumber()) {
            value = std::to_string(akiValue.ObjectAs<double>());
        } else {
            AKI_LOG(WARNING) << "Other data types";
            continue;
        }

        hashSetResult.insert(value);
    }

    hashSetResult.insert("value8");
    return hashSetResult;
}

std::unordered_set<aki::Value, aki::Value::HashPair> PassingSetAkiStringReturnHashSetAkiString(
    std::unordered_set<aki::Value, aki::Value::HashPair> hashSet)
{
    hashSet.insert(aki::Value(std::string("Friday")));
    return hashSet;
}

JSBIND_GLOBAL() {
    JSBIND_FUNCTION(PassingVoidReturnMapLong, "passingVoidReturnMapLong");
    JSBIND_FUNCTION(PassingMapLongReturnMapLong, "passingMapLongReturnMapLong");
    JSBIND_FUNCTION(PassingVoidReturnMapLongString, "passingVoidReturnMapLongString");
    JSBIND_FUNCTION(PassingMapLongStringReturnMapLongString, "passingMapLongStringReturnMapLongString");
    JSBIND_FUNCTION(PassingMapAkiLongStringReturnMapLongString, "passingMapAkiLongStringReturnMapLongString");
    JSBIND_FUNCTION(PassingMapAkiStringStringReturnMapStringString, "passingMapAkiStringStringReturnMapStringString");
    JSBIND_FUNCTION(PassingMapAkiStringReturnMapAkiString, "passingMapAkiStringReturnMapAkiString");
    JSBIND_FUNCTION(PassingVoidReturnHashMapLong, "passingVoidReturnHashMapLong");
    JSBIND_FUNCTION(PassingMapLongReturnHashMapLong, "passingMapLongReturnHashMapLong");
    JSBIND_FUNCTION(PassingVoidReturnHashMapLongString, "passingVoidReturnHashMapLongString");
    JSBIND_FUNCTION(PassingMapLongStringReturnHashMapLongString, "passingMapLongStringReturnHashMapLongString");
    JSBIND_FUNCTION(PassingMapAkiLongStringReturnHashMapLongString, "passingMapAkiLongStringReturnHashMapLongString");
    JSBIND_FUNCTION(PassingMapAkiStringReturnHashMapStringString, "passingMapAkiStringReturnHashMapStringString");
    JSBIND_FUNCTION(PassingMapAkiStringReturnHashMapAkiString, "passingMapAkiStringReturnHashMapAkiString");
    JSBIND_FUNCTION(PassingVoidReturnHashSetLong, "passingVoidReturnHashSetLong");
    JSBIND_FUNCTION(PassingSetLongReturnHashSetLong, "passingSetLongReturnHashSetLong");
    JSBIND_FUNCTION(PassingSetAkiLongReturnHashSetLong, "passingSetAkiLongReturnHashSetLong");
    JSBIND_FUNCTION(PassingSetAkiLongReturnHashSetAkiLong, "passingSetAkiLongReturnHashSetAkiLong");
    JSBIND_FUNCTION(PassingVoidReturnHashSetString, "passingVoidReturnHashSetString");
    JSBIND_FUNCTION(PassingSetStringReturnHashSetString, "passingSetStringReturnHashSetString");
    JSBIND_FUNCTION(PassingSetAkiStringReturnHashSetString, "passingSetAkiStringReturnHashSetString");
    JSBIND_FUNCTION(PassingSetAkiStringReturnHashSetAkiString, "passingSetAkiStringReturnHashSetAkiString");
}