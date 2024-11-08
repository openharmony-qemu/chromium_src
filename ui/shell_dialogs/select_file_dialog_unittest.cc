// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stddef.h>
#include <memory>
#define private public
#include "select_file_dialog_factory.h"
#include "select_file_policy.h"
#include "ui/shell_dialogs/select_file_dialog.h"
#undef private
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

TEST(ShellDialogs, ShortenFileNameIfNeeded) {
  struct ShortenFileNameTestCase {
    base::FilePath::StringType input;
    base::FilePath::StringType expected;
  } test_cases[] = {
      // Paths with short paths/file names don't get shortened.
      {FILE_PATH_LITERAL("folder1111/folder2222/file1.html"),
       FILE_PATH_LITERAL("folder1111/folder2222/file1.html")},

      // Path with long filename gets shortened to 255 chars
      {FILE_PATH_LITERAL("folder1111/"
                         "abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvw"
                         "xyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnop"
                         "qrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghi"
                         "jklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234ab"
                         "cdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxy"
                         "z1234.html"),
       FILE_PATH_LITERAL("folder1111/"
                         "abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvw"
                         "xyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnop"
                         "qrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghi"
                         "jklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234ab"
                         "cdefghijklmnopqrstuvwxyz1234abcdefghij.html")},

      // Long path but short filename is not truncated, handled by system open
      // file dialog.
      {FILE_PATH_LITERAL(
           "folder1111/folder2222/folder3333/folder4444/folder5555/folder6666/"
           "folder7777/folder8888/folder9999/folder0000/folder1111/folder2222/"
           "folder3333/folder4444/folder5555/folder6666/folder7777/folder8888/"
           "folder9999/folder0000/folder1111/folder2222/folder3333/folder4444/"
           "folder5555/folder6666/folder7777/folder8888/folder9999/folder0000/"
           "file1.pdf"),
       FILE_PATH_LITERAL(
           "folder1111/folder2222/folder3333/folder4444/folder5555/folder6666/"
           "folder7777/folder8888/folder9999/folder0000/folder1111/folder2222/"
           "folder3333/folder4444/folder5555/folder6666/folder7777/folder8888/"
           "folder9999/folder0000/folder1111/folder2222/folder3333/folder4444/"
           "folder5555/folder6666/folder7777/folder8888/folder9999/folder0000/"
           "file1.pdf")},

      // Long extension with total file name length < 255 is not truncated.
      {FILE_PATH_LITERAL("folder1111/folder2222/"
                         "file1."
                         "abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvw"
                         "xyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnop"
                         "qrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghi"
                         "jklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234"),
       FILE_PATH_LITERAL("folder1111/folder2222/"
                         "file1."
                         "abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvw"
                         "xyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnop"
                         "qrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghi"
                         "jklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz123"
                         "4")},

      // Long extension, medium length file name is truncated so that total
      // file name length = 255
      {FILE_PATH_LITERAL("folder1111/folder2222/"
                         "file1234567890123456789012345678901234567890123456789"
                         "0."
                         "abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvw"
                         "xyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnop"
                         "qrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghi"
                         "jklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234"),
       FILE_PATH_LITERAL("folder1111/folder2222/"
                         "file1234567890123456789012345678901234567890123456789"
                         "0."
                         "abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvw"
                         "xyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnop"
                         "qrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghi"
                         "jklmnopqrstuvwxyz1234abcdefghijklmnopqrst")},

      // Long extension and long file name -> extension truncated to 13 chars
      // and file name truncated to 255-13.
      {FILE_PATH_LITERAL("folder1111/folder2222/"
                         "abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvw"
                         "xyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnop"
                         "qrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghi"
                         "jklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234ab"
                         "cdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxy"
                         "z1234."
                         "abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvw"
                         "xyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnop"
                         "qrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghi"
                         "jklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234"),
       FILE_PATH_LITERAL("folder1111/folder2222/"
                         "abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvw"
                         "xyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghijklmnop"
                         "qrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234abcdefghi"
                         "jklmnopqrstuvwxyz1234abcdefghijklmnopqrstuvwxyz1234ab"
                         "cdefghijklmnopqrstuvwxyz1234ab.abcdefghijkl")}};

  for (size_t i = 0; i < std::size(test_cases); ++i) {
    base::FilePath input =
        base::FilePath(test_cases[i].input).NormalizePathSeparators();
    base::FilePath output =
        base::FilePath(test_cases[i].expected).NormalizePathSeparators();
    EXPECT_EQ(output.value(),
              ui::SelectFileDialog::GetShortenedFilePath(input).value());
    EXPECT_LE(ui::SelectFileDialog::GetShortenedFilePath(input)
                  .BaseName()
                  .value()
                  .length(),
              255u);
  }
}

namespace ui {

class ConcreteListener : public ui::SelectFileDialog::Listener {
 public:
  void FileSelected(const base::FilePath& path,
                    int index,
                    void* params) override {}
  void FileSelectedWithExtraInfo(const ui::SelectedFileInfo& file,
                                 int index,
                                 void* params) override {}
  void MultiFilesSelected(const std::vector<base::FilePath>& files,
                          void* params) override {}
  void MultiFilesSelectedWithExtraInfo(
      const std::vector<ui::SelectedFileInfo>& files,
      void* params) override {}
  void FileSelectionCanceled(void* params) override {}
};

class MockSelectFileDialogFactory : public ui::SelectFileDialogFactory {
 public:
  bool isCefFactory = false;
  bool IsCefFactory() const override { return isCefFactory; }
  MOCK_METHOD(SelectFileDialog*,
              Create,
              (ui::SelectFileDialog::Listener * listener,
               std::unique_ptr<ui::SelectFilePolicy> policy),
              (override));
};

TEST(ShellDialogs, CreateTest1) {
  MockSelectFileDialogFactory* factory = new MockSelectFileDialogFactory();
  std::unique_ptr<ConcreteListener> listener =
      std::make_unique<ConcreteListener>();
  std::unique_ptr<ui::SelectFilePolicy> policy;
  bool run_from_cef = false;
  SelectFileDialog::SetFactory(factory);
  EXPECT_CALL(*factory, Create(::testing::_, ::testing::_)).Times(1);
  SelectFileDialog::Create(listener.get(), std::move(policy), run_from_cef);
}

TEST(ShellDialogs, CreateTest2) {
  MockSelectFileDialogFactory* factory = new MockSelectFileDialogFactory();
  std::unique_ptr<ConcreteListener> listener =
      std::make_unique<ConcreteListener>();
  std::unique_ptr<ui::SelectFilePolicy> policy;
  bool run_from_cef = true;
  SelectFileDialog::SetFactory(factory);
  EXPECT_CALL(*factory, Create(::testing::_, ::testing::_)).Times(1);
  SelectFileDialog::Create(listener.get(), std::move(policy), run_from_cef);
}

TEST(ShellDialogs, CreateTest3) {
  MockSelectFileDialogFactory* factory = new MockSelectFileDialogFactory();
  std::unique_ptr<ConcreteListener> listener =
      std::make_unique<ConcreteListener>();
  std::unique_ptr<ui::SelectFilePolicy> policy;
  bool run_from_cef = false;
  factory->isCefFactory = true;
  SelectFileDialog::SetFactory(factory);
  EXPECT_CALL(*factory, Create(::testing::_, ::testing::_)).Times(1);
  SelectFileDialog::Create(listener.get(), std::move(policy), run_from_cef);
}

TEST(ShellDialogs, CreateTest4) {
  MockSelectFileDialogFactory* factory = new MockSelectFileDialogFactory();
  std::unique_ptr<ConcreteListener> listener =
      std::make_unique<ConcreteListener>();
  std::unique_ptr<ui::SelectFilePolicy> policy;
  bool run_from_cef = true;
  factory->isCefFactory = true;
  SelectFileDialog::SetFactory(factory);
  EXPECT_CALL(*factory, Create(::testing::_, ::testing::_)).Times(0);
  SelectFileDialog::Create(listener.get(), std::move(policy), run_from_cef);
}

TEST(ShellDialogs, CreateTest5) {
  MockSelectFileDialogFactory* factory = new MockSelectFileDialogFactory();
  std::unique_ptr<ConcreteListener> listener =
      std::make_unique<ConcreteListener>();
  std::unique_ptr<ui::SelectFilePolicy> policy;
  bool run_from_cef = true;
  SelectFileDialog::SetFactory(factory);
  EXPECT_CALL(*factory, Create(::testing::_, ::testing::_)).Times(1);
  SelectFileDialog::Create(listener.get(), std::move(policy), run_from_cef);
}

TEST(ShellDialogs, CreateTest6) {
  ui::SelectFileDialogFactory* factory_ = nullptr;
  std::unique_ptr<ConcreteListener> listener =
      std::make_unique<ConcreteListener>();
  std::unique_ptr<ui::SelectFilePolicy> policy;
  bool run_from_cef = false;
  SelectFileDialog::SetFactory(factory_);
  SelectFileDialog::Create(listener.get(), std::move(policy), run_from_cef);
  EXPECT_FALSE(factory_);
}

}  // namespace ui
