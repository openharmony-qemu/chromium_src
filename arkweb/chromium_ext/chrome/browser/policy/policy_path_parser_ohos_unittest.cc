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

#include "chrome/browser/policy/policy_path_parser.h"
#include "gtest/gtest.h"

namespace policy {

namespace path_parser {
class PolicyPathParserOhosTest : public ::testing::Test {};

TEST_F(PolicyPathParserOhosTest, ExpandPathVariablesTest) {
  base::FilePath::StringType input = "";
  base::FilePath::StringType result =
      policy::path_parser::ExpandPathVariables(input);
  EXPECT_EQ(result, "");
}

TEST_F(PolicyPathParserOhosTest, ExpandPathVariablesTest2) {
  base::FilePath::StringType input = "\"/home/${user_name}/documents\"";
  base::FilePath::StringType result =
      policy::path_parser::ExpandPathVariables(input);
  EXPECT_EQ(result, "/home/user/documents");

  input = "'/home/${machine_name}/downloads'";
  result = policy::path_parser::ExpandPathVariables(input);
  EXPECT_EQ(result, "/home/machine/downloads");
}

TEST_F(PolicyPathParserOhosTest, TestPathWithVariables) {
  base::FilePath::StringType input = "/home/${user_name}/files/${machine_name}";
  base::FilePath::StringType result =
      policy::path_parser::ExpandPathVariables(input);
  EXPECT_EQ(result, "/home/user/files/machine");
}

TEST_F(PolicyPathParserOhosTest, TestOnlyVariables) {
  base::FilePath::StringType input = "${user_name}/${machine_name}";
  base::FilePath::StringType result =
      policy::path_parser::ExpandPathVariables(input);
  EXPECT_EQ(result, "user/machine");
}

TEST_F(PolicyPathParserOhosTest, TestPathWithoutQuotes) {
  base::FilePath::StringType input = "/home/user/documents";
  base::FilePath::StringType result =
      policy::path_parser::ExpandPathVariables(input);
  EXPECT_EQ(result, "/home/user/documents");
}

TEST_F(PolicyPathParserOhosTest, TestPathWithOnlyUserNameVariable) {
  base::FilePath::StringType input = "/home/${user_name}/documents";
  base::FilePath::StringType result =
      policy::path_parser::ExpandPathVariables(input);
  EXPECT_EQ(result, "/home/user/documents");
}

TEST_F(PolicyPathParserOhosTest, TestPathWithOnlyMachineNameVariable) {
  base::FilePath::StringType input = "/home/${machine_name}/downloads";
  base::FilePath::StringType result =
      policy::path_parser::ExpandPathVariables(input);
  EXPECT_EQ(result, "/home/machine/downloads");
}

TEST_F(PolicyPathParserOhosTest, TestUnpairedQuotes) {
  base::FilePath::StringType input = "\"/home/user/documents";
  base::FilePath::StringType result =
      policy::path_parser::ExpandPathVariables(input);
  EXPECT_EQ(result, "\"/home/user/documents");

  input = "'/home/user/documents";
  result = policy::path_parser::ExpandPathVariables(input);
  EXPECT_EQ(result, "'/home/user/documents");

  input = "/home/user/documents\"";
  result = policy::path_parser::ExpandPathVariables(input);
  EXPECT_EQ(result, "/home/user/documents\"");

  input = "/home/user/documents'";
  result = policy::path_parser::ExpandPathVariables(input);
  EXPECT_EQ(result, "/home/user/documents'");
}

TEST_F(PolicyPathParserOhosTest, TestSingleQuoteOnly) {
  base::FilePath::StringType input = "\"";
  base::FilePath::StringType result =
      policy::path_parser::ExpandPathVariables(input);
  EXPECT_EQ(result, "\"");

  input = "'";
  result = policy::path_parser::ExpandPathVariables(input);
  EXPECT_EQ(result, "'");
}

}  // namespace path_parser

}  // namespace policy
