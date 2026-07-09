/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#include <algorithm>
#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"
#include "base/test/task_environment.h"
#include "base/threading/thread_restrictions.h"
#include "ohos/adapter/permission_manager/permission_manager_adapter.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/platform_test.h"
#include "ui/base/clipboard/clipboard_monitor.h"
#include "ui/base/clipboard/clipboard_observer.h"
#include "ui/base/clipboard/clipboard_ohos.h"
#include "ui/base/clipboard/scoped_clipboard_writer.h"
#include "ui/gfx/image/image_unittest_util.h"

extern "C" {
typedef struct OH_Pasteboard OH_Pasteboard;
typedef struct OH_UdmfData OH_UdmfData;
typedef struct OH_UdmfRecord OH_UdmfRecord;
typedef struct OH_UdsPlainText OH_UdsPlainText;
typedef struct OH_UdsHtml OH_UdsHtml;
typedef struct OH_UdsFileUri OH_UdsFileUri;
typedef struct OH_UdsPixelMap OH_UdsPixelMap;
typedef struct OH_PixelmapNative OH_PixelmapNative;
typedef struct OH_Pixelmap_InitializationOptions
    OH_Pixelmap_InitializationOptions;
typedef struct OH_Pixelmap_ImageInfo OH_Pixelmap_ImageInfo;

int OH_UdsPlainText_SetContent(OH_UdsPlainText*, const char* content);
int OH_UdsHtml_SetContent(OH_UdsHtml*, const char* content);
int OH_UdsFileUri_SetFileUri(OH_UdsFileUri*, const char*);

#define ERR_OK 0
#define UDMF_E_OK 0
#define IMAGE_SUCCESS 0

struct MockRecordData {
  std::string plain_text;
  std::string html;
  std::vector<std::string> file_uris;
  std::map<std::string, std::string> custom_data;
};

static MockRecordData* g_mock_data = nullptr;
static OH_Pasteboard* g_mock_pasteboard = nullptr;
static bool g_mock_has_data = false;
static OH_UdmfData* g_mock_udmf_data = nullptr;
static int g_mock_change_count = 0;
static std::map<OH_UdmfRecord*, MockRecordData*> g_record_data_map;
static std::map<OH_UdsPlainText*, std::string> g_plain_text_map;
static std::map<OH_UdsHtml*, std::string> g_html_map;
static std::map<OH_UdsFileUri*, std::string> g_file_uri_map;
static std::map<OH_UdmfData*, std::vector<OH_UdmfRecord*>> g_udmf_data_records;

void ClearMockData() {
  g_plain_text_map.clear();
  g_html_map.clear();
  g_file_uri_map.clear();
  g_udmf_data_records.clear();
  g_mock_change_count = 0;
  for (auto& kv : g_record_data_map) {
    delete kv.second;
  }
  g_record_data_map.clear();
  g_mock_has_data = false;
  g_mock_udmf_data = nullptr;
  g_mock_pasteboard = nullptr;
}

OH_Pasteboard* OH_Pasteboard_Create() {
  if (!g_mock_pasteboard) {
    g_mock_pasteboard = reinterpret_cast<OH_Pasteboard*>(0x1234);
  }
  return g_mock_pasteboard;
}

void OH_Pasteboard_Destroy(OH_Pasteboard*) {}

bool OH_Pasteboard_HasType(OH_Pasteboard*, const char*) {
  return true;
}

bool OH_Pasteboard_HasData(OH_Pasteboard*) {
  return g_mock_has_data;
}

int OH_Pasteboard_SetData(OH_Pasteboard*, OH_UdmfData* udmf_data) {
  g_mock_has_data = true;
  g_mock_udmf_data = udmf_data;
  g_mock_change_count++;
  if (!g_mock_data) {
    g_mock_data = new MockRecordData();
  }
  return ERR_OK;
}

int OH_Pasteboard_ClearData(OH_Pasteboard*) {
  g_mock_has_data = false;
  g_mock_change_count++;
  return ERR_OK;
}

OH_UdmfData* OH_Pasteboard_GetData(OH_Pasteboard*, int* status) {
  if (status) {
    *status = ERR_OK;
  }
  return g_mock_udmf_data;
}

uint32_t OH_Pasteboard_GetChangeCount(OH_Pasteboard*) {
  std::cout << "OH_Pasteboard_GetChangeCount g_mock_change_count= "
            << g_mock_change_count << std::endl;
  return g_mock_change_count;
}

OH_UdmfData* OH_UdmfData_Create() {
  return reinterpret_cast<OH_UdmfData*>(0x2000);
}

void OH_UdmfData_Destroy(OH_UdmfData*) {}

int OH_UdmfData_AddRecord(OH_UdmfData* udmf_data, OH_UdmfRecord* record) {
  if (!udmf_data || !record) {
    return -1;
  }
  if (g_record_data_map.find(record) == g_record_data_map.end()) {
    g_record_data_map[record] = new MockRecordData();
  }
  g_udmf_data_records[udmf_data].push_back(record);
  return ERR_OK;
}

OH_UdmfRecord** OH_UdmfData_GetRecords(OH_UdmfData* udmf_data,
                                       unsigned int* count) {
  if (!g_mock_has_data) {
    if (count) {
      *count = 0;
    }
    return nullptr;
  }
  auto it = g_udmf_data_records.find(udmf_data);
  if (it == g_udmf_data_records.end() || it->second.empty()) {
    if (count) {
      *count = 0;
    }
    return nullptr;
  }
  if (count) {
    *count = it->second.size();
  }
  static std::vector<OH_UdmfRecord*> records;
  records = it->second;
  return records.data();
}

static uint32_t g_record_id = 0x3000;

OH_UdmfRecord* OH_UdmfRecord_Create() {
  OH_UdmfRecord* record = reinterpret_cast<OH_UdmfRecord*>(g_record_id++);
  g_record_data_map[record] = new MockRecordData();
  return record;
}

void OH_UdmfRecord_Destroy(OH_UdmfRecord*) {}

int OH_UdmfRecord_AddPlainText(OH_UdmfRecord* record,
                               OH_UdsPlainText* plain_text) {
  if (!record || !plain_text) {
    return -1;
  }
  auto it = g_record_data_map.find(record);
  if (it == g_record_data_map.end()) {
    return -1;
  }
  auto pit = g_plain_text_map.find(plain_text);
  if (pit != g_plain_text_map.end()) {
    it->second->plain_text = pit->second;
    it->second->custom_data["text/plain"] = pit->second;
  }
  return ERR_OK;
}

int OH_UdmfRecord_AddHtml(OH_UdmfRecord* record, OH_UdsHtml* html) {
  if (!record || !html) {
    return -1;
  }
  auto it = g_record_data_map.find(record);
  if (it == g_record_data_map.end()) {
    return -1;
  }
  auto hit = g_html_map.find(html);
  if (hit != g_html_map.end()) {
    it->second->html = hit->second;
  }
  return ERR_OK;
}

int OH_UdmfRecord_AddFileUri(OH_UdmfRecord* record, OH_UdsFileUri* file_uri) {
  if (!record || !file_uri) {
    return -1;
  }
  auto it = g_record_data_map.find(record);
  if (it == g_record_data_map.end()) {
    return -1;
  }
  auto fit = g_file_uri_map.find(file_uri);
  if (fit != g_file_uri_map.end()) {
    std::cout << "[MOCK] OH_UdmfRecord_AddFileUri: record=" << record
              << ", file_uri=" << file_uri << ", value=" << fit->second
              << std::endl;
    it->second->file_uris.push_back(fit->second);
  }
  return ERR_OK;
}

int OH_UdmfRecord_AddPixelMap(OH_UdmfRecord*, OH_UdsPixelMap*) {
  return ERR_OK;
}

int OH_UdmfRecord_AddGeneralEntry(OH_UdmfRecord* record,
                                  const char* type,
                                  unsigned char* data,
                                  unsigned int len) {
  if (!record || !type || !data || len == 0) {
    return -1;
  }
  auto it = g_record_data_map.find(record);
  if (it == g_record_data_map.end()) {
    return -1;
  }
  it->second->custom_data[std::string(type)] =
      std::string(reinterpret_cast<char*>(data), len);
  return ERR_OK;
}

char** OH_UdmfRecord_GetTypes(OH_UdmfRecord* record, unsigned int* count) {
  if (!record || !g_mock_has_data) {
    if (count) {
      *count = 0;
    }
    return nullptr;
  }
  auto it = g_record_data_map.find(record);
  if (it == g_record_data_map.end()) {
    if (count) {
      *count = 0;
    }
    return nullptr;
  }
  MockRecordData* data = it->second;
  static std::vector<std::string> types;
  static std::vector<char*> type_ptrs;
  types.clear();
  type_ptrs.clear();

  if (!data->plain_text.empty()) {
    types.push_back("text/plain");
  }
  if (!data->html.empty()) {
    types.push_back("text/html");
  }
  if (!data->file_uris.empty()) {
    types.push_back("text/uri");
  }
  for (const auto& kv : data->custom_data) {
    types.push_back(kv.first);
  }

  for (auto& t : types) {
    type_ptrs.push_back(const_cast<char*>(t.c_str()));
  }

  if (count) {
    *count = type_ptrs.size();
  }
  return type_ptrs.data();
}

int OH_UdmfRecord_GetPlainText(OH_UdmfRecord* record, OH_UdsPlainText* output) {
  if (!record || !output) {
    return -1;
  }
  auto it = g_record_data_map.find(record);
  if (it == g_record_data_map.end()) {
    return -1;
  }
  const std::string& content = it->second->plain_text;
  if (content.empty()) {
    return -1;
  }
  OH_UdsPlainText_SetContent(output, content.c_str());
  return UDMF_E_OK;
}

int OH_UdmfRecord_GetHtml(OH_UdmfRecord* record, OH_UdsHtml* output) {
  if (!record || !output) {
    return -1;
  }
  auto it = g_record_data_map.find(record);
  if (it == g_record_data_map.end()) {
    return -1;
  }
  const std::string& content = it->second->html;
  if (content.empty()) {
    return -1;
  }
  OH_UdsHtml_SetContent(output, content.c_str());
  return UDMF_E_OK;
}

int OH_UdmfRecord_GetFileUri(OH_UdmfRecord* record, OH_UdsFileUri* output) {
  if (!record || !output) {
    return -1;
  }
  auto it = g_record_data_map.find(record);
  if (it == g_record_data_map.end()) {
    return -1;
  }
  if (it->second->file_uris.empty()) {
    return -1;
  }
  OH_UdsFileUri_SetFileUri(output, it->second->file_uris[0].c_str());
  return UDMF_E_OK;
}

int OH_UdmfRecord_GetPixelMap(OH_UdmfRecord*, OH_UdsPixelMap*) {
  return -1;
}

int OH_UdmfRecord_GetGeneralEntry(OH_UdmfRecord* record,
                                  const char* type,
                                  unsigned char** data,
                                  unsigned int* len) {
  if (!record || !type || !data || !len) {
    return -1;
  }
  auto it = g_record_data_map.find(record);
  if (it == g_record_data_map.end()) {
    return -1;
  }
  auto cit = it->second->custom_data.find(std::string(type));
  if (cit == it->second->custom_data.end()) {
    return -1;
  }
  *data =
      reinterpret_cast<unsigned char*>(const_cast<char*>(cit->second.data()));
  *len = cit->second.size();
  return UDMF_E_OK;
}

static uint32_t g_plain_text_id = 0x4000;
static uint32_t g_html_id = 0x5000;
static uint32_t g_file_uri_id = 0x6000;

OH_UdsPlainText* OH_UdsPlainText_Create() {
  OH_UdsPlainText* pt = reinterpret_cast<OH_UdsPlainText*>(g_plain_text_id++);
  g_plain_text_map[pt] = "";
  return pt;
}

void OH_UdsPlainText_Destroy(OH_UdsPlainText* pt) {
  g_plain_text_map.erase(pt);
}

int OH_UdsPlainText_SetContent(OH_UdsPlainText* pt, const char* content) {
  if (!pt || !content) {
    return -1;
  }
  g_plain_text_map[pt] = content;
  return ERR_OK;
}

const char* OH_UdsPlainText_GetContent(OH_UdsPlainText* pt) {
  auto it = g_plain_text_map.find(pt);
  if (it == g_plain_text_map.end()) {
    return "";
  }
  return it->second.c_str();
}

OH_UdsHtml* OH_UdsHtml_Create() {
  OH_UdsHtml* html = reinterpret_cast<OH_UdsHtml*>(g_html_id++);
  g_html_map[html] = "";
  return html;
}

void OH_UdsHtml_Destroy(OH_UdsHtml* html) {
  g_html_map.erase(html);
}

int OH_UdsHtml_SetContent(OH_UdsHtml* html, const char* content) {
  if (!html || !content) {
    return -1;
  }
  g_html_map[html] = content;
  return ERR_OK;
}

const char* OH_UdsHtml_GetContent(OH_UdsHtml* html) {
  auto it = g_html_map.find(html);
  if (it == g_html_map.end()) {
    return "";
  }
  return it->second.c_str();
}

OH_UdsFileUri* OH_UdsFileUri_Create() {
  OH_UdsFileUri* uri = reinterpret_cast<OH_UdsFileUri*>(g_file_uri_id++);
  g_file_uri_map[uri] = "";
  return uri;
}

void OH_UdsFileUri_Destroy(OH_UdsFileUri* uri) {
  g_file_uri_map.erase(uri);
}

int OH_UdsFileUri_SetFileUri(OH_UdsFileUri* uri, const char* uri_str) {
  if (!uri || !uri_str) {
    return -1;
  }
  g_file_uri_map[uri] = uri_str;
  return ERR_OK;
}

const char* OH_UdsFileUri_GetFileUri(OH_UdsFileUri* uri) {
  auto it = g_file_uri_map.find(uri);
  std::cout << "[MOCK] OH_UdsFileUri_GetFileUri: uri=" << uri
            << ", found=" << (it != g_file_uri_map.end());
  if (it != g_file_uri_map.end()) {
    std::cout << ", value=" << it->second;
  }
  std::cout << std::endl;
  if (it == g_file_uri_map.end()) {
    return "";
  }
  return it->second.c_str();
}

int OH_UdsFileUri_SetFileType(OH_UdsFileUri*, int) {
  return ERR_OK;
}

OH_UdsPixelMap* OH_UdsPixelMap_Create() {
  return reinterpret_cast<OH_UdsPixelMap*>(0x7000);
}

void OH_UdsPixelMap_Destroy(OH_UdsPixelMap*) {}

int OH_UdsPixelMap_SetPixelMap(OH_UdsPixelMap*, OH_PixelmapNative*) {
  return ERR_OK;
}

int OH_UdsPixelMap_GetPixelMap(OH_UdsPixelMap*, OH_PixelmapNative*) {
  return ERR_OK;
}

int OH_PixelmapInitializationOptions_Create(
    OH_Pixelmap_InitializationOptions**) {
  return ERR_OK;
}

void OH_PixelmapInitializationOptions_Release(
    OH_Pixelmap_InitializationOptions*) {}

int OH_PixelmapInitializationOptions_SetWidth(
    OH_Pixelmap_InitializationOptions*,
    uint32_t) {
  return ERR_OK;
}

int OH_PixelmapInitializationOptions_SetHeight(
    OH_Pixelmap_InitializationOptions*,
    uint32_t) {
  return ERR_OK;
}

int OH_PixelmapInitializationOptions_SetPixelFormat(
    OH_Pixelmap_InitializationOptions*,
    int32_t) {
  return ERR_OK;
}

int OH_PixelmapInitializationOptions_SetAlphaType(
    OH_Pixelmap_InitializationOptions*,
    int32_t) {
  return ERR_OK;
}

int OH_PixelmapInitializationOptions_SetSrcPixelFormat(
    OH_Pixelmap_InitializationOptions*,
    int32_t) {
  return ERR_OK;
}

#undef IMAGE_SUCCESS
typedef enum Image_ErrorCode {
  IMAGE_SUCCESS = 0,
} Image_ErrorCode;

Image_ErrorCode OH_PixelmapNative_CreateEmptyPixelmap(
    OH_Pixelmap_InitializationOptions*,
    OH_PixelmapNative**) {
  return IMAGE_SUCCESS;
}

Image_ErrorCode OH_PixelmapNative_CreatePixelmap(
    uint8_t*,
    uint64_t,
    OH_Pixelmap_InitializationOptions*,
    OH_PixelmapNative**) {
  return IMAGE_SUCCESS;
}

Image_ErrorCode OH_PixelmapNative_GetImageInfo(OH_PixelmapNative*,
                                               OH_Pixelmap_ImageInfo*) {
  return IMAGE_SUCCESS;
}

Image_ErrorCode OH_PixelmapNative_ReadPixels(OH_PixelmapNative*,
                                             uint8_t*,
                                             uint64_t*) {
  return IMAGE_SUCCESS;
}

Image_ErrorCode OH_PixelmapNative_Release(OH_PixelmapNative*) {
  return IMAGE_SUCCESS;
}

Image_ErrorCode OH_PixelmapImageInfo_Create(OH_Pixelmap_ImageInfo**) {
  return IMAGE_SUCCESS;
}

Image_ErrorCode OH_PixelmapImageInfo_GetWidth(OH_Pixelmap_ImageInfo*,
                                              uint32_t*) {
  return IMAGE_SUCCESS;
}

Image_ErrorCode OH_PixelmapImageInfo_GetHeight(OH_Pixelmap_ImageInfo*,
                                               uint32_t*) {
  return IMAGE_SUCCESS;
}

Image_ErrorCode OH_PixelmapImageInfo_GetRowStride(OH_Pixelmap_ImageInfo*,
                                                  uint32_t*) {
  return IMAGE_SUCCESS;
}

Image_ErrorCode OH_PixelmapImageInfo_GetPixelFormat(OH_Pixelmap_ImageInfo*,
                                                    int32_t*) {
  return IMAGE_SUCCESS;
}

Image_ErrorCode OH_PixelmapImageInfo_Release(OH_Pixelmap_ImageInfo*) {
  return IMAGE_SUCCESS;
}
}

namespace ohos::adapter::permission {
bool CheckAndRequestPermission(int) {
  return true;
}

bool OpenPermissionConfirm(const OHOSPermissionType&) {
  return true;
}
}  // namespace ohos::adapter::permission

namespace ohos::adapter {

class FileManagerAdapter {
 public:
  static FileManagerAdapter& GetInstance() {
    static FileManagerAdapter instance;
    return instance;
  }

  void GetPathForUri(const char* uri, std::string& path) {
    if (!uri || strlen(uri) == 0) {
      path = "";
      return;
    }
    auto it = uri_to_path_map_.find(std::string(uri));
    if (it != uri_to_path_map_.end()) {
      path = it->second;
    } else {
      path = uri;
    }
  }

  void GetUriForPath(const char* path, std::string& uri) {
    if (!path || strlen(path) == 0) {
      uri = "";
      return;
    }
    auto it = path_to_uri_map_.find(std::string(path));
    if (it != path_to_uri_map_.end()) {
      uri = it->second;
    } else {
      uri = std::string("file://") + path;
      path_to_uri_map_[std::string(path)] = uri;
      uri_to_path_map_[uri] = std::string(path);
    }
  }

  void ClearMaps() {
    uri_to_path_map_.clear();
    path_to_uri_map_.clear();
  }

  void AddUriPathMapping(const std::string& uri, const std::string& path) {
    uri_to_path_map_[uri] = path;
    path_to_uri_map_[path] = uri;
  }

 private:
  FileManagerAdapter() = default;
  std::map<std::string, std::string> uri_to_path_map_;
  std::map<std::string, std::string> path_to_uri_map_;
};

}  // namespace ohos::adapter

namespace ui {

namespace {

bool MockIsReadAllowed(const DataTransferEndpoint*,
                       std::optional<ClipboardInternalFormat>,
                       const std::optional<ClipboardFormatType>&) {
  return true;
}

class ClipboardOHOSTest : public PlatformTest, public ClipboardObserver {
 public:
  ClipboardOHOSTest() {
    ClipboardMonitor::GetInstance()->AddObserver(this);
    ClipboardOHOS::SetIsReadAllowedFuncForTest(MockIsReadAllowed);
  }

  ~ClipboardOHOSTest() override {
    ClipboardMonitor::GetInstance()->RemoveObserver(this);
    ClipboardOHOS::SetIsReadAllowedFuncForTest(nullptr);
  }

  void TearDown() override { ClearMockData(); }

  void OnClipboardDataChanged() override { ++data_changed_count_; }

  int data_changed_count() const { return data_changed_count_; }

 private:
  base::test::TaskEnvironment task_environment_{
      base::test::TaskEnvironment::MainThreadType::UI};
  int data_changed_count_ = 0;
};

}  // namespace

TEST_F(ClipboardOHOSTest, DataChangedNotificationOnWrite) {
  {
    ScopedClipboardWriter writer(ClipboardBuffer::kCopyPaste);
    writer.WriteText(u"text");
  }
  ASSERT_EQ(data_changed_count(), 1);

  {
    ScopedClipboardWriter writer(ClipboardBuffer::kCopyPaste);
    writer.WriteHTML(u"html", "https://source.com/");
  }
  ASSERT_EQ(data_changed_count(), 2);

  {
    ScopedClipboardWriter writer(ClipboardBuffer::kCopyPaste);
    writer.WriteImage(gfx::test::CreateBitmap(2, 3));
  }
  ASSERT_EQ(data_changed_count(), 3);

  Clipboard::GetForCurrentThread()->Clear(ClipboardBuffer::kCopyPaste);
  ASSERT_EQ(data_changed_count(), 4);
}

TEST_F(ClipboardOHOSTest, NoDataChangedNotificationOnRead) {
  auto* clipboard = Clipboard::GetForCurrentThread();

  std::vector<std::u16string> types;
  clipboard->ReadAvailableTypes(ClipboardBuffer::kCopyPaste, nullptr, &types);
  ASSERT_EQ(data_changed_count(), 0);

  std::u16string text_result;
  clipboard->ReadText(ClipboardBuffer::kCopyPaste, nullptr, &text_result);
  ASSERT_EQ(data_changed_count(), 0);

  std::u16string html;
  std::string src_url;
  uint32_t start;
  uint32_t end;
  clipboard->ReadHTML(ClipboardBuffer::kCopyPaste, nullptr, &html, &src_url,
                      &start, &end);
  ASSERT_EQ(data_changed_count(), 0);

  std::vector<FileInfo> file_infos;
  clipboard->ReadFilenames(ClipboardBuffer::kCopyPaste, nullptr, &file_infos);
  ASSERT_EQ(data_changed_count(), 0);

  std::string data_result;
  clipboard->ReadData(ClipboardFormatType::PlainTextType(), nullptr,
                      &data_result);
  ASSERT_EQ(data_changed_count(), 0);
}

TEST_F(ClipboardOHOSTest, TextTest) {
  std::u16string text(u"OHOS clipboard test text");

  {
    ScopedClipboardWriter clipboard_writer(ClipboardBuffer::kCopyPaste);
    clipboard_writer.WriteText(text);
  }

  EXPECT_TRUE(Clipboard::GetForCurrentThread()->IsFormatAvailable(
      ClipboardFormatType::PlainTextType(), ClipboardBuffer::kCopyPaste,
      /* data_dst = */ nullptr));

  std::u16string text_result;
  Clipboard::GetForCurrentThread()->ReadText(ClipboardBuffer::kCopyPaste,
                                             /* data_dst = */ nullptr,
                                             &text_result);
  EXPECT_EQ(text, text_result);
}

TEST_F(ClipboardOHOSTest, DISABLED_HTMLTest) {
  std::u16string markup(u"<div>Hello OHOS</div>");
  std::string url("https://example.com/");

  {
    ScopedClipboardWriter clipboard_writer(ClipboardBuffer::kCopyPaste);
    clipboard_writer.WriteHTML(markup, url);
    clipboard_writer.WriteText(u"Hello OHOS");
  }

  EXPECT_TRUE(Clipboard::GetForCurrentThread()->IsFormatAvailable(
      ClipboardFormatType::HtmlType(), ClipboardBuffer::kCopyPaste,
      /* data_dst = */ nullptr));

  std::u16string markup_result;
  std::string url_result;
  uint32_t fragment_start;
  uint32_t fragment_end;
  Clipboard::GetForCurrentThread()->ReadHTML(
      ClipboardBuffer::kCopyPaste, /* data_dst = */ nullptr, &markup_result,
      &url_result, &fragment_start, &fragment_end);
  EXPECT_EQ(markup, markup_result);
  EXPECT_EQ(url, url_result);
}

TEST_F(ClipboardOHOSTest, FilenamesTest) {
  base::ScopedAllowBlockingForTesting allow_blocking;
  base::ScopedTempDir temp_dir;
  ASSERT_TRUE(temp_dir.CreateUniqueTempDir());

  base::FilePath file1;
  base::FilePath file2;
  ASSERT_TRUE(base::CreateTemporaryFileInDir(temp_dir.GetPath(), &file1));
  ASSERT_TRUE(base::CreateTemporaryFileInDir(temp_dir.GetPath(), &file2));

  {
    ScopedClipboardWriter clipboard_writer(ClipboardBuffer::kCopyPaste);
    clipboard_writer.WriteFilenames(
        ui::FileInfosToURIList({ui::FileInfo(file1, base::FilePath()),
                                ui::FileInfo(file2, base::FilePath())}));
  }

  EXPECT_TRUE(Clipboard::GetForCurrentThread()->IsFormatAvailable(
      ClipboardFormatType::FilenamesType(), ClipboardBuffer::kCopyPaste,
      /* data_dst = */ nullptr));

  std::vector<std::u16string> types;
  Clipboard::GetForCurrentThread()->ReadAvailableTypes(
      ClipboardBuffer::kCopyPaste, /* data_dst = */ nullptr, &types);

  bool has_uri_list = false;
  for (const auto& type : types) {
    if (type == u"text/uri-list" || type == u"text/uri") {
      has_uri_list = true;
      break;
    }
  }
  EXPECT_TRUE(has_uri_list);

  std::vector<ui::FileInfo> filenames;
  Clipboard::GetForCurrentThread()->ReadFilenames(
      ClipboardBuffer::kCopyPaste, /* data_dst = */ nullptr, &filenames);
  EXPECT_EQ(2u, filenames.size());
}

TEST_F(ClipboardOHOSTest, SingleFilenameTest) {
  base::ScopedAllowBlockingForTesting allow_blocking;
  base::ScopedTempDir temp_dir;
  ASSERT_TRUE(temp_dir.CreateUniqueTempDir());

  base::FilePath file;
  ASSERT_TRUE(base::CreateTemporaryFileInDir(temp_dir.GetPath(), &file));

  {
    ScopedClipboardWriter clipboard_writer(ClipboardBuffer::kCopyPaste);
    clipboard_writer.WriteFilenames(
        ui::FileInfosToURIList({ui::FileInfo(file, base::FilePath())}));
  }

  std::vector<ui::FileInfo> filenames;
  Clipboard::GetForCurrentThread()->ReadFilenames(
      ClipboardBuffer::kCopyPaste, /* data_dst = */ nullptr, &filenames);

  EXPECT_EQ(1u, filenames.size());
  EXPECT_EQ(file, filenames[0].path);
}

TEST_F(ClipboardOHOSTest, MultiFormatTest) {
  std::u16string text(u"Hi!");
  std::u16string markup(u"<strong>Hi!</string>");

  {
    ScopedClipboardWriter clipboard_writer(ClipboardBuffer::kCopyPaste);
    clipboard_writer.WriteHTML(markup, "http://example.com/");
    clipboard_writer.WriteText(text);
  }

  std::vector<std::u16string> types;
  Clipboard::GetForCurrentThread()->ReadAvailableTypes(
      ClipboardBuffer::kCopyPaste, /* data_dst = */ nullptr, &types);

  EXPECT_GT(types.size(), 1u);

  std::u16string text_result;
  Clipboard::GetForCurrentThread()->ReadText(ClipboardBuffer::kCopyPaste,
                                             /* data_dst = */ nullptr,
                                             &text_result);
  EXPECT_EQ(text, text_result);
}

TEST_F(ClipboardOHOSTest, ImageTest) {
  SkBitmap bitmap = gfx::test::CreateBitmap(4, 3);

  {
    ScopedClipboardWriter clipboard_writer(ClipboardBuffer::kCopyPaste);
    clipboard_writer.WriteImage(bitmap);
  }

  EXPECT_TRUE(Clipboard::GetForCurrentThread()->IsFormatAvailable(
      ClipboardFormatType::BitmapType(), ClipboardBuffer::kCopyPaste,
      /* data_dst = */ nullptr));
}

TEST_F(ClipboardOHOSTest, ClearTest) {
  {
    ScopedClipboardWriter clipboard_writer(ClipboardBuffer::kCopyPaste);
    clipboard_writer.WriteText(u"clear me");
  }

  std::string data_before;
  Clipboard::GetForCurrentThread()->ReadData(
      ClipboardFormatType::PlainTextType(), /* data_dst = */ nullptr,
      &data_before);
  EXPECT_FALSE(data_before.empty());

  Clipboard::GetForCurrentThread()->Clear(ClipboardBuffer::kCopyPaste);

  std::string data_after;
  Clipboard::GetForCurrentThread()->ReadData(
      ClipboardFormatType::PlainTextType(), /* data_dst = */ nullptr,
      &data_after);
  EXPECT_TRUE(data_after.empty());
}

TEST_F(ClipboardOHOSTest, EmptyFilenamesTest) {
  {
    ScopedClipboardWriter clipboard_writer(ClipboardBuffer::kCopyPaste);
    clipboard_writer.WriteFilenames({});
  }

  std::vector<ui::FileInfo> filenames;
  Clipboard::GetForCurrentThread()->ReadFilenames(
      ClipboardBuffer::kCopyPaste, /* data_dst = */ nullptr, &filenames);
  EXPECT_EQ(0u, filenames.size());
}

TEST_F(ClipboardOHOSTest, GetStandardFormats) {
  auto* clipboard = Clipboard::GetForCurrentThread();

  std::vector<std::u16string> types = clipboard->GetStandardFormats(
      ClipboardBuffer::kCopyPaste, /* data_dst = */ nullptr);

  EXPECT_GT(types.size(), 0u);
}

TEST_F(ClipboardOHOSTest, WriteTextEmptyParams) {
  ScopedClipboardWriter scw(ClipboardBuffer::kCopyPaste);
  scw.WriteText(std::u16string());
}

TEST_F(ClipboardOHOSTest, WriteHTMLEmptyParams) {
  ScopedClipboardWriter scw(ClipboardBuffer::kCopyPaste);
  scw.WriteHTML(std::u16string(), std::string());
}

TEST_F(ClipboardOHOSTest, WriteImageEmptyParams) {
  ScopedClipboardWriter scw(ClipboardBuffer::kCopyPaste);
  scw.WriteImage(SkBitmap());
}

TEST_F(ClipboardOHOSTest, DISABLED_SequenceNumberChangesOnWrite) {
  const ClipboardSequenceNumberToken first_sequence_number =
      Clipboard::GetForCurrentThread()->GetSequenceNumber(
          ClipboardBuffer::kCopyPaste);

  {
    ScopedClipboardWriter writer(ClipboardBuffer::kCopyPaste);
    writer.WriteText(u"World");
  }

  base::RunLoop().RunUntilIdle();

  const ClipboardSequenceNumberToken second_sequence_number =
      Clipboard::GetForCurrentThread()->GetSequenceNumber(
          ClipboardBuffer::kCopyPaste);

  EXPECT_NE(first_sequence_number, second_sequence_number);
}

TEST_F(ClipboardOHOSTest, WriteEverything) {
  {
    ScopedClipboardWriter writer(ClipboardBuffer::kCopyPaste);
    writer.WriteText(u"foo");
    writer.WriteHTML(u"foo", "bar");
    writer.WriteWebSmartPaste();
  }
}

}  // namespace ui
