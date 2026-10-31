// Copyright 2025 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/modules/credentialmanagement/public_key_credential_utils.h"

#include <utility>
#include <vector>

#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/mojom/webauthn/authenticator.mojom-blink.h"
#include "third_party/blink/renderer/bindings/core/v8/idl_types.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise_resolver.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise_tester.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_binding_for_testing.h"
#include "third_party/blink/renderer/platform/bindings/v8_binding.h"
#include "third_party/blink/renderer/platform/testing/task_environment.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"
#include "v8/include/v8.h"

namespace blink {
namespace {

// Helper to build a Vector of WebAuthnClientCapabilityPtr from a list of
// (name, supported) pairs.
Vector<mojom::blink::WebAuthnClientCapabilityPtr> MakeCapabilities(
    const std::vector<std::pair<String, bool>>& entries) {
  Vector<mojom::blink::WebAuthnClientCapabilityPtr> capabilities;
  for (const auto& entry : entries) {
    auto capability = mojom::blink::WebAuthnClientCapability::New();
    capability->name = entry.first;
    capability->supported = entry.second;
    capabilities.push_back(std::move(capability));
  }
  return capabilities;
}

// Extracts the resolved record (IDLRecord<IDLString, IDLBoolean>) as an
// ordered vector of (key, value) pairs. Returns an empty vector if the
// promise is not fulfilled or the value is not an object.
std::vector<std::pair<String, bool>> ExtractRecord(
    ScriptState* script_state,
    ScriptPromiseTester& tester) {
  std::vector<std::pair<String, bool>> result;
  if (!tester.IsFulfilled()) {
    return result;
  }
  v8::Isolate* isolate = script_state->GetIsolate();
  v8::Local<v8::Context> context = script_state->GetContext();
  v8::Local<v8::Value> value = tester.Value().V8Value();
  if (!value->IsObject()) {
    return result;
  }
  v8::Local<v8::Object> object = value.As<v8::Object>();
  v8::Local<v8::Array> property_names;
  if (!object->GetOwnPropertyNames(context).ToLocal(&property_names)) {
    return result;
  }
  // V8 returns own property names in insertion order. The resolver converts
  // the sorted Vector<pair<String,bool>> into a record by inserting entries
  // in order, so iterating own properties preserves the sorted order.
  for (uint32_t i = 0; i < property_names->Length(); ++i) {
    v8::Local<v8::Value> key_value;
    if (!property_names->Get(context, i).ToLocal(&key_value)) {
      continue;
    }
    v8::Local<v8::Value> item_value;
    if (!object->Get(context, key_value).ToLocal(&item_value)) {
      continue;
    }
    String key = ToCoreString(isolate, key_value.As<v8::String>());
    bool supported = item_value->BooleanValue(isolate);
    result.emplace_back(key, supported);
  }
  return result;
}

// Runs `OnGetClientCapabilitiesFullComplete` with the given capabilities and
// returns the sorted record resolved by the promise.
std::vector<std::pair<String, bool>> RunAndExtract(
    Vector<mojom::blink::WebAuthnClientCapabilityPtr> capabilities) {
  test::TaskEnvironment task_environment;
  V8TestingScope scope;
  ScriptState* script_state = scope.GetScriptState();
  auto* resolver = MakeGarbageCollected<
      ScriptPromiseResolver<IDLRecord<IDLString, IDLBoolean>>>(script_state);
  ScriptPromiseTester tester(script_state, resolver->Promise());
  OnGetClientCapabilitiesFullComplete(resolver, std::move(capabilities));
  tester.WaitUntilSettled();
  return ExtractRecord(script_state, tester);
}

TEST(PublicKeyCredentialUtilsTest, EmptyCapabilities) {
  auto result = RunAndExtract({});
  EXPECT_TRUE(result.empty());
}

TEST(PublicKeyCredentialUtilsTest, SingleCapabilitySupportedTrue) {
  auto result = RunAndExtract(MakeCapabilities({{"hybrid", true}}));
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(result[0].first, "hybrid");
  EXPECT_TRUE(result[0].second);
}

TEST(PublicKeyCredentialUtilsTest, SingleCapabilitySupportedFalse) {
  auto result = RunAndExtract(MakeCapabilities({{"hybrid", false}}));
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(result[0].first, "hybrid");
  EXPECT_FALSE(result[0].second);
}

TEST(PublicKeyCredentialUtilsTest, AlreadySortedPreserved) {
  auto result = RunAndExtract(MakeCapabilities(
      {{"android", true}, {"hybrid", true}, {"local", false}}));
  ASSERT_EQ(result.size(), 3u);
  EXPECT_EQ(result[0].first, "android");
  EXPECT_EQ(result[1].first, "hybrid");
  EXPECT_EQ(result[2].first, "local");
  EXPECT_TRUE(result[0].second);
  EXPECT_TRUE(result[1].second);
  EXPECT_FALSE(result[2].second);
}

TEST(PublicKeyCredentialUtilsTest, ReverseSortedIsReordered) {
  auto result = RunAndExtract(MakeCapabilities(
      {{"local", false}, {"hybrid", true}, {"android", true}}));
  ASSERT_EQ(result.size(), 3u);
  EXPECT_EQ(result[0].first, "android");
  EXPECT_EQ(result[1].first, "hybrid");
  EXPECT_EQ(result[2].first, "local");
  EXPECT_TRUE(result[0].second);
  EXPECT_TRUE(result[1].second);
  EXPECT_FALSE(result[2].second);
}

TEST(PublicKeyCredentialUtilsTest, UnsortedIsReordered) {
  auto result = RunAndExtract(MakeCapabilities(
      {{"hybrid", true}, {"android", true}, {"local", false}}));
  ASSERT_EQ(result.size(), 3u);
  EXPECT_EQ(result[0].first, "android");
  EXPECT_EQ(result[1].first, "hybrid");
  EXPECT_EQ(result[2].first, "local");
}

TEST(PublicKeyCredentialUtilsTest, DuplicateKeysDeduplicatedByRecord) {
  // The function does not deduplicate, but IDLRecord (a JS object) keeps only
  // the last value for a given key. So duplicate keys collapse to one entry.
  auto result =
      RunAndExtract(MakeCapabilities({{"hybrid", true}, {"hybrid", false}}));
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(result[0].first, "hybrid");
  EXPECT_FALSE(result[0].second);
}

TEST(PublicKeyCredentialUtilsTest, MixedSupportedValues) {
  auto result = RunAndExtract(MakeCapabilities({
      {"zeta", false},
      {"alpha", true},
      {"middle", false},
      {"beta", true},
  }));
  ASSERT_EQ(result.size(), 4u);
  EXPECT_EQ(result[0].first, "alpha");
  EXPECT_EQ(result[1].first, "beta");
  EXPECT_EQ(result[2].first, "middle");
  EXPECT_EQ(result[3].first, "zeta");
  EXPECT_TRUE(result[0].second);
  EXPECT_TRUE(result[1].second);
  EXPECT_FALSE(result[2].second);
  EXPECT_FALSE(result[3].second);
}

TEST(PublicKeyCredentialUtilsTest, CaseSensitiveLexicographicOrder) {
  // CodeUnitCompare is case-sensitive: uppercase letters sort before lowercase
  // because they have smaller code units.
  auto result = RunAndExtract(
      MakeCapabilities({{"apple", true}, {"Apple", true}, {"APPLE", true}}));
  ASSERT_EQ(result.size(), 3u);
  EXPECT_EQ(result[0].first, "APPLE");
  EXPECT_EQ(result[1].first, "Apple");
  EXPECT_EQ(result[2].first, "apple");
}

TEST(PublicKeyCredentialUtilsTest, EmptyNameCapability) {
  auto result = RunAndExtract(MakeCapabilities({{"", true}, {"a", false}}));
  ASSERT_EQ(result.size(), 2u);
  EXPECT_EQ(result[0].first, "");
  EXPECT_TRUE(result[0].second);
  EXPECT_EQ(result[1].first, "a");
  EXPECT_FALSE(result[1].second);
}

}  // namespace
}  // namespace blink
