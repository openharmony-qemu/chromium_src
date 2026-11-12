// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/safe_browsing/download_protection/path_sanitizer.h"

#include <vector>

#include "base/notreached.h"
#include "base/path_service.h"
#include "testing/gtest/include/gtest/gtest.h"

#if BUILDFLAG(IS_OHOS)
#include "base/files/scoped_temp_dir.h"
#include "base/test/scoped_path_override.h"
#include "base/test/test_file_util.h"
#endif

namespace {

// Returns the root directory with a trailing separator. Works on all platforms.
base::FilePath GetRootDirectory() {
  base::FilePath dir_temp;
  if (!base::PathService::Get(base::DIR_TEMP, &dir_temp)) {
    NOTREACHED();
  }

  std::vector<base::FilePath::StringType> components = dir_temp.GetComponents();

  return base::FilePath(components[0]).AsEndingWithSeparator();
}

}  // namespace

namespace safe_browsing {

#if BUILDFLAG(IS_OHOS)
class OverrideHomeDir {
 public:
  OverrideHomeDir() { SetHomeDir(); }

  ~OverrideHomeDir() { DeleteHomeDir(); }

 private:
  void SetHomeDir() {
    ASSERT_TRUE(
        temp_dir_.CreateUniqueTempDirUnderPath(base::GetTempDirForTesting()));
    base::FilePath home_dir = temp_dir_.GetPath().AppendASCII("home");
    home_override_ = std::make_unique<base::ScopedPathOverride>(
        base::DIR_HOME, home_dir, true, true);
  }

  void DeleteHomeDir() { ASSERT_TRUE(temp_dir_.Delete()); }

  base::ScopedTempDir temp_dir_;
  std::unique_ptr<base::ScopedPathOverride> home_override_;
};
#endif

TEST(SafeBrowsingPathSanitizerTest, HomeDirectoryIsNotEmpty) {
#if BUILDFLAG(IS_OHOS)
  OverrideHomeDir override_home_dir;
#endif

  PathSanitizer path_sanitizer;

  ASSERT_FALSE(path_sanitizer.GetHomeDirectory().empty());
}

TEST(SafeBrowsingPathSanitizerTest, DontStripHomeDirectoryTest) {
#if BUILDFLAG(IS_OHOS)
  OverrideHomeDir override_home_dir;
#endif

  // Test with path not in home directory.
  base::FilePath path =
      GetRootDirectory().Append(FILE_PATH_LITERAL("not_in_home_directory.ext"));
  base::FilePath path_expected = path;

  PathSanitizer path_sanitizer;
  path_sanitizer.StripHomeDirectory(&path);

  ASSERT_EQ(path.value(), path_expected.value());
}

TEST(SafeBrowsingPathSanitizerTest, DoStripHomeDirectoryTest) {
#if BUILDFLAG(IS_OHOS)
  OverrideHomeDir override_home_dir;
#endif

  // Test with path in home directory.
  PathSanitizer path_sanitizer;

  base::FilePath path = path_sanitizer.GetHomeDirectory().Append(
      FILE_PATH_LITERAL("in_home_directory.ext"));
  base::FilePath path_expected =
      base::FilePath(FILE_PATH_LITERAL("~"))
          .Append(FILE_PATH_LITERAL("in_home_directory.ext"));

  path_sanitizer.StripHomeDirectory(&path);

  ASSERT_EQ(path.value(), path_expected.value());
}

}  // namespace safe_browsing
