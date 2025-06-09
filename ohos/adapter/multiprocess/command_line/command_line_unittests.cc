/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "gtest/gtest.h"
#include "ohos/adapter/multiprocess/command_line/command_line.h"

namespace ohos::adapter::multiprocess {
class CommandLineTest : public ::testing::Test {
 protected:
  void TearDown() override {
    CommandLine::ResetForCurrentProcess();
  }
};

TEST(CommandLineTest, CurrentProcessTest) {
  const std::vector<std::string> args = {
      "program",
      "--foo=",
      "-bAr",
      "-spaetzel=pierogi",
      "-baz",
      "flim",
      "--other-switches=--dog=canine --cat=feline",
      "-spaetzle=Crepe",
      "-=loosevalue",
      "-",
      "FLAN",
      "a",
      "--input-translation=45--output-rotation",
      "--",
      "\"in the time of submarines...\"",
      "unquoted arg-with-space"};
  CommandLine::InitForCurrentProcess(args);

  CommandLine* command_line = CommandLine::ForCurrentProcess();
  EXPECT_NE(nullptr, command_line);
  EXPECT_FALSE(command_line->HasSwitch("cruller"));
  EXPECT_FALSE(command_line->HasSwitch("flim"));
  EXPECT_FALSE(command_line->HasSwitch("program"));
  EXPECT_FALSE(command_line->HasSwitch("dog"));
  EXPECT_FALSE(command_line->HasSwitch("cat"));
  EXPECT_FALSE(command_line->HasSwitch("output-rotation"));
  EXPECT_FALSE(command_line->HasSwitch("--"));

  EXPECT_TRUE(command_line->HasSwitch("foo"));
  EXPECT_FALSE(command_line->HasSwitch("bar"));
  EXPECT_TRUE(command_line->HasSwitch("baz"));
  EXPECT_TRUE(command_line->HasSwitch("spaetzle"));
  EXPECT_TRUE(command_line->HasSwitch("other-switches"));
  EXPECT_TRUE(command_line->HasSwitch("input-translation"));
}

TEST(CommandLineTest, CurrentProcessResetTest) {
  CommandLine::InitForCurrentProcess(std::vector<std::string>());
  EXPECT_NE(nullptr, CommandLine::ForCurrentProcess());
  CommandLine::ResetForCurrentProcess();
  EXPECT_EQ(nullptr, CommandLine::ForCurrentProcess());
}
} // namespace ohos::adapter::multiprocess
