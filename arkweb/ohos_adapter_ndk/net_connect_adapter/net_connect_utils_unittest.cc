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

#include "gtest/gtest.h"
#include "arkweb/ohos_adapter_ndk/net_connect_adapter/net_connect_utils.h"

using namespace testing;
using namespace OHOS::NWeb;

class NetConnectUtilsTest : public ::testing::Test {};

/**
 * @tc.name: NetConnectUtilsTest_ConvertToConnectTypeInner_001.
 * @tc.desc: test of ConvertToConnectTypeInner in NetConnectUtilsTest
 * @tc.type: FUNC.
 */
TEST_F(NetConnectUtilsTest, NetConnectUtilsTest_ConvertToConnectTypeInner_001) {
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_CELLULAR,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN), NetConnectType::CONNECTION_UNKNOWN);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_CELLULAR,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_IWLAN), NetConnectType::CONNECTION_UNKNOWN);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_CELLULAR,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_GSM), NetConnectType::CONNECTION_2G);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_CELLULAR,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_1XRTT), NetConnectType::CONNECTION_2G);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_CELLULAR,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_WCDMA), NetConnectType::CONNECTION_3G);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_CELLULAR,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_HSPA), NetConnectType::CONNECTION_3G);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_CELLULAR,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_HSPAP), NetConnectType::CONNECTION_3G);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_CELLULAR,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_TD_SCDMA), NetConnectType::CONNECTION_3G);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_CELLULAR,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_EVDO), NetConnectType::CONNECTION_3G);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_CELLULAR,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_EHRPD), NetConnectType::CONNECTION_3G);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_CELLULAR,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_LTE), NetConnectType::CONNECTION_4G);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_CELLULAR,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_LTE_CA), NetConnectType::CONNECTION_4G);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_CELLULAR,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_NR), NetConnectType::CONNECTION_5G);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_CELLULAR,
      static_cast<Telephony_RadioTechnology>(-1)), NetConnectType::CONNECTION_UNKNOWN);
}

/**
 * @tc.name: NetConnectUtilsTest_ConvertToConnectsubtype_001.
 * @tc.desc: test of ConvertToConnectsubtype in NetConnectUtilsTest
 * @tc.type: FUNC.
 */
TEST_F(NetConnectUtilsTest, NetConnectUtilsTest_ConvertToConnectsubtype_001) {
  EXPECT_EQ(NetConnectUtils::ConvertToConnectsubtype(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN),
      NetConnectSubtype::SUBTYPE_UNKNOWN);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectsubtype(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_IWLAN),
      NetConnectSubtype::SUBTYPE_UNKNOWN);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectsubtype(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_WCDMA),
      NetConnectSubtype::SUBTYPE_UNKNOWN);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectsubtype(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_TD_SCDMA),
      NetConnectSubtype::SUBTYPE_UNKNOWN);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectsubtype(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_EVDO),
      NetConnectSubtype::SUBTYPE_UNKNOWN);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectsubtype(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_NR),
      NetConnectSubtype::SUBTYPE_UNKNOWN);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectsubtype(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_GSM),
      NetConnectSubtype::SUBTYPE_GSM);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectsubtype(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_1XRTT),
      NetConnectSubtype::SUBTYPE_1XRTT);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectsubtype(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_HSPA),
      NetConnectSubtype::SUBTYPE_HSPA);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectsubtype(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_HSPAP),
      NetConnectSubtype::SUBTYPE_HSPAP);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectsubtype(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_EHRPD),
      NetConnectSubtype::SUBTYPE_EHRPD);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectsubtype(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_LTE),
      NetConnectSubtype::SUBTYPE_LTE);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectsubtype(Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_LTE_CA),
      NetConnectSubtype::SUBTYPE_LTE_ADVANCED);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectsubtype(static_cast<Telephony_RadioTechnology>(-1)),
      NetConnectSubtype::SUBTYPE_UNKNOWN);
}

/**
 * @tc.name: NetConnectUtilsTest_ConvertToConnectType_001.
 * @tc.desc: test of ConvertToConnectType in NetConnectUtilsTest
 * @tc.type: FUNC.
 */
TEST_F(NetConnectUtilsTest, NetConnectUtilsTest_ConvertToConnectType_001) {
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_WIFI,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN), NetConnectType::CONNECTION_WIFI);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_BLUETOOTH,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN), NetConnectType::CONNECTION_BLUETOOTH);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_ETHERNET,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN), NetConnectType::CONNECTION_ETHERNET);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(NETCONN_BEARER_VPN,
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN), NetConnectType::CONNECTION_UNKNOWN);
  EXPECT_EQ(NetConnectUtils::ConvertToConnectType(static_cast<NetConn_NetBearerType>(-1),
      Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN), NetConnectType::CONNECTION_UNKNOWN);
}

/**
 * @tc.name: NetConnectUtilsTest_ConnectTypeToString_001.
 * @tc.desc: test of ConnectTypeToString in NetConnectUtilsTest
 * @tc.type: FUNC.
 */
TEST_F(NetConnectUtilsTest, NetConnectUtilsTest_ConnectTypeToString_001) {
  EXPECT_EQ(NetConnectUtils::ConnectTypeToString(NetConnectType::CONNECTION_UNKNOWN), "unknown");
  EXPECT_EQ(NetConnectUtils::ConnectTypeToString(NetConnectType::CONNECTION_ETHERNET), "ethernet");
  EXPECT_EQ(NetConnectUtils::ConnectTypeToString(NetConnectType::CONNECTION_WIFI), "wifi");
  EXPECT_EQ(NetConnectUtils::ConnectTypeToString(NetConnectType::CONNECTION_2G), "2g");
  EXPECT_EQ(NetConnectUtils::ConnectTypeToString(NetConnectType::CONNECTION_3G), "3g");
  EXPECT_EQ(NetConnectUtils::ConnectTypeToString(NetConnectType::CONNECTION_4G), "4g");
  EXPECT_EQ(NetConnectUtils::ConnectTypeToString(NetConnectType::CONNECTION_NONE), "none");
  EXPECT_EQ(NetConnectUtils::ConnectTypeToString(NetConnectType::CONNECTION_BLUETOOTH), "bluetooth");
  EXPECT_EQ(NetConnectUtils::ConnectTypeToString(NetConnectType::CONNECTION_5G), "5g");
}
