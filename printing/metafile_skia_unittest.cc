// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(OHOS_UNITTESTS)
#define private public
#include "printing/metafile_skia.cc"
#include "printing/metafile_skia.h"
#undef private
#else  // OHOS_UNITTESTS
#include "printing/metafile_skia.h"
#endif  // OHOS_UNITTESTS

#include <utility>

#include "build/build_config.h"
#include "cc/paint/paint_op.h"
#include "cc/paint/paint_record.h"
#include "printing/common/metafile_utils.h"
#include "printing/mojom/print.mojom.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "third_party/skia/include/core/SkRect.h"
#include "third_party/skia/include/core/SkRefCnt.h"
#include "third_party/skia/include/core/SkSerialProcs.h"
#include "third_party/skia/include/core/SkSize.h"
#include "third_party/skia/include/core/SkStream.h"
#include "third_party/skia/include/core/SkSurfaceProps.h"
#include "third_party/skia/include/core/SkTextBlob.h"

namespace printing {

TEST(MetafileSkiaTest, TestFrameContent) {
  constexpr int kPictureSideLen = 100;
  constexpr int kPageSideLen = 150;

  // Create a placeholder picture.
  sk_sp<SkPicture> pic_holder = SkPicture::MakePlaceholder(
      SkRect::MakeXYWH(0, 0, kPictureSideLen, kPictureSideLen));

  // Create the page with nested content which is the placeholder and will be
  // replaced later.
  cc::PaintOpBuffer buffer;
  cc::PaintFlags flags;
  flags.setColor(SK_ColorWHITE);
  const SkRect page_rect = SkRect::MakeXYWH(0, 0, kPageSideLen, kPageSideLen);
  buffer.push<cc::DrawRectOp>(page_rect, flags);
  const uint32_t content_id = pic_holder->uniqueID();
  buffer.push<cc::CustomDataOp>(content_id);
  SkSize page_size = SkSize::Make(kPageSideLen, kPageSideLen);

  // Finish creating the entire metafile.
  MetafileSkia metafile(mojom::SkiaDocumentType::kMSKP, 1);
  metafile.AppendPage(page_size, buffer.ReleaseAsRecord());
  metafile.AppendSubframeInfo(content_id, base::UnguessableToken::Create(),
                              std::move(pic_holder));
  metafile.FinishFrameContent();
  SkStreamAsset* metafile_stream = metafile.GetPdfData();
  ASSERT_TRUE(metafile_stream);

  // Draw a 100 by 100 red square which will be the actual content of
  // the placeholder.
  SkPictureRecorder recorder;
  SkCanvas* canvas = recorder.beginRecording(kPictureSideLen, kPictureSideLen);
  SkPaint paint;
  paint.setStyle(SkPaint::kFill_Style);
  paint.setColor(SK_ColorRED);
  paint.setAlpha(SK_AlphaOPAQUE);
  canvas->drawRect(SkRect::MakeXYWH(0, 0, kPictureSideLen, kPictureSideLen),
                   paint);
  sk_sp<SkPicture> picture(recorder.finishRecordingAsPicture());
  EXPECT_TRUE(picture);

  // Get the complete picture by replacing the placeholder.
  PictureDeserializationContext subframes;
  subframes[content_id] = picture;
  SkDeserialProcs procs = DeserializationProcs(&subframes, nullptr);
  sk_sp<SkPicture> pic = SkPicture::MakeFromStream(metafile_stream, &procs);
  ASSERT_TRUE(pic);

  // Verify the resultant picture is as expected by comparing the sizes and
  // detecting the color inside and outside of the square area.
  EXPECT_TRUE(pic->cullRect() == page_rect);
  SkBitmap bitmap;
  bitmap.allocN32Pixels(kPageSideLen, kPageSideLen);
  SkCanvas bitmap_canvas(bitmap, SkSurfaceProps{});
  pic->playback(&bitmap_canvas);
  // Check top left pixel color of the red square.
  EXPECT_EQ(bitmap.getColor(0, 0), SK_ColorRED);
  // Check bottom right pixel of the red square.
  EXPECT_EQ(bitmap.getColor(kPictureSideLen - 1, kPictureSideLen - 1),
            SK_ColorRED);
  // Check inside of the red square.
  EXPECT_EQ(bitmap.getColor(kPictureSideLen / 2, kPictureSideLen / 2),
            SK_ColorRED);
  // Check outside of the red square.
  EXPECT_EQ(bitmap.getColor(kPictureSideLen, kPictureSideLen), SK_ColorWHITE);
}

TEST(MetafileSkiaTest, TestMultiPictureDocumentTypefaces) {
  constexpr int kPictureSideLen = 100;
  constexpr int kPageSideLen = 150;
  constexpr int kDocumentCookie = 1;
  constexpr int kNumDocumentPages = 2;

  // The content tracking for serialization/deserialization.
  ContentProxySet serialize_typeface_ctx;
  PictureDeserializationContext subframes;
  TypefaceDeserializationContext typefaces;
  SkDeserialProcs procs = DeserializationProcs(&subframes, &typefaces);

  // The typefaces which will be reused across the multiple (duplicate) pages.
  constexpr char kTypefaceName1[] = "sans-serif";
#if BUILDFLAG(IS_WIN)
  constexpr char kTypefaceName2[] = "Courier New";
#else
  constexpr char kTypefaceName2[] = "monospace";
#endif
#if defined(OHOS_UNITTESTS)
  constexpr size_t kNumTypefaces = 1;
#else
  constexpr size_t kNumTypefaces = 2;
#endif  // defined(OHOS_UNITTESTS)
  sk_sp<SkTypeface> typeface1 =
      SkTypeface::MakeFromName(kTypefaceName1, SkFontStyle());
  sk_sp<SkTypeface> typeface2 =
      SkTypeface::MakeFromName(kTypefaceName2, SkFontStyle());
  const SkFont font1 = SkFont(typeface1, 10);
  const SkFont font2 = SkFont(typeface2, 12);

  // Node IDs for the text, which will increase for each text blob added.
  cc::NodeId node_id = 7;

  // All text can just be black.
  cc::PaintFlags flags_text;
  flags_text.setColor(SK_ColorBLACK);

  // Mark the text on white pages, each of the same size.
  cc::PaintFlags flags;
  flags.setColor(SK_ColorWHITE);
  const SkRect page_rect = SkRect::MakeXYWH(0, 0, kPageSideLen, kPageSideLen);
  SkSize page_size = SkSize::Make(kPageSideLen, kPageSideLen);

  for (int i = 0; i < kNumDocumentPages; i++) {
    MetafileSkia metafile(mojom::SkiaDocumentType::kMSKP, kDocumentCookie);

    // When the stream is serialized inside FinishFrameContent(), any typeface
    // which is used on any page will be serialized only once by the first
    // page's metafile which needed it.  Any subsequent page that reuses the
    // same typeface will rely upon `serialize_typeface_ctx` which is used by
    // printing::SerializeOopTypeface() to optimize away the need to resend.
    metafile.UtilizeTypefaceContext(&serialize_typeface_ctx);

    sk_sp<SkPicture> pic_holder = SkPicture::MakePlaceholder(
        SkRect::MakeXYWH(0, 0, kPictureSideLen, kPictureSideLen));

    // Create the page for the text content.
    cc::PaintOpBuffer buffer;
    buffer.push<cc::DrawRectOp>(page_rect, flags);
    const uint32_t content_id = pic_holder->uniqueID();
    buffer.push<cc::CustomDataOp>(content_id);

    // Mark the page with some text using multiple fonts.
    // Use the first font.
    sk_sp<SkTextBlob> text_blob1 = SkTextBlob::MakeFromString("foo", font1);
    buffer.push<cc::DrawTextBlobOp>(text_blob1, 0.0f, 0.0f, ++node_id,
                                    flags_text);

    // Use the second font.
    sk_sp<SkTextBlob> text_blob2 = SkTextBlob::MakeFromString("bar", font2);
    buffer.push<cc::DrawTextBlobOp>(text_blob2, 0.0f, 0.0f, ++node_id,
                                    flags_text);

    // Reuse the first font again on same page.
    sk_sp<SkTextBlob> text_blob3 = SkTextBlob::MakeFromString("bar", font2);
    buffer.push<cc::DrawTextBlobOp>(text_blob3, 0.0f, 0.0f, ++node_id,
                                    flags_text);

    metafile.AppendPage(page_size, buffer.ReleaseAsRecord());
    metafile.AppendSubframeInfo(content_id, base::UnguessableToken::Create(),
                                std::move(pic_holder));
    metafile.FinishFrameContent();
    SkStreamAsset* metafile_stream = metafile.GetPdfData();
    ASSERT_TRUE(metafile_stream);

    // Deserialize the stream.  Any given typeface is expected to appear only
    // once in the stream, so the deserialization context of `typefaces` bundled
    // with `procs` should be empty the first time through, and afterwards
    // there should never be more than the number of unique typefaces we used,
    // regardless of number of pages.
    EXPECT_EQ(typefaces.size(), i ? kNumTypefaces : 0);
    ASSERT_TRUE(SkPicture::MakeFromStream(metafile_stream, &procs));
    EXPECT_EQ(typefaces.size(), kNumTypefaces);
  }
}

#if defined(OHOS_UNITTESTS)
class MockSkStreamAsset : public SkStreamAsset {
 public:
  MockSkStreamAsset(size_t length) : fLength_(length), fPosition_(0) {}
  size_t read(void* buffer, size_t size) override {
    if (fPosition_ + size > fLength_) {
      size = fLength_ - fPosition_;
    }
    std::memset(buffer, 'A', size);
    fPosition_ += size;
    return size;
  }

  bool isAtEnd() const override { return fPosition_ >= fLength_; }

  bool rewind() override {
    fPosition_ = 0;
    return true;
  }

  bool hasPosition() const override { return true; }

  size_t getPosition() const override { return fPosition_; }

  bool seek(size_t position) override {
    if (position <= fLength_) {
      fPosition_ = position;
      return true;
    }
    return false;
  }

  bool move(long offset) override {
    size_t newPosition = fPosition_ + offset;
    if (newPosition <= fLength_) {
      fPosition_ = newPosition;
      return true;
    }
    return false;
  }

  size_t getLength() const override { return fLength_; }

  SkStreamAsset* onDuplicate() const override {
    return new MockSkStreamAsset(fLength_);
  }

  SkStreamAsset* onFork() const override {
    return new MockSkStreamAsset(fLength_, fPosition_);
  }

 private:
  MockSkStreamAsset(size_t length, size_t position)
      : fLength_(length), fPosition_(position) {}

  size_t fLength_;
  size_t fPosition_;
};

TEST(MetafileSkiaTest, OhosFinishDocument001) {
  MetafileSkia metafile(mojom::SkiaDocumentType::kPDF, 1);
  metafile.data_->data_stream = nullptr;
  std::function<bool()> checkCancel = []() { return true; };
  auto result = metafile.OhosFinishDocument(checkCancel);
  EXPECT_EQ(true, result);
}

TEST(MetafileSkiaTest, OhosFinishDocument002) {
  MetafileSkia metafile(mojom::SkiaDocumentType::kPDF, 1);
  std::unique_ptr<SkStreamAsset> mock_data_stream =
      std::make_unique<MockSkStreamAsset>(1);
  metafile.data_->data_stream = std::move(mock_data_stream);
  std::function<bool()> checkCancel = []() { return true; };
  auto result = metafile.OhosFinishDocument(checkCancel);
  EXPECT_EQ(false, result);
}

TEST(MetafileSkiaTest, OhosFinishDocument003) {
  MetafileSkia metafile(mojom::SkiaDocumentType::kPDF, 1);
  metafile.data_->recorder.is_recording_ = false;
  std::function<bool()> checkCancel = []() { return true; };
  auto result = metafile.OhosFinishDocument(checkCancel);
  EXPECT_EQ(nullptr, metafile.data_->recorder.getRecordingCanvas());
  EXPECT_EQ(true, result);
}

TEST(MetafileSkiaTest, OhosFinishDocument004) {
  MetafileSkia metafile(mojom::SkiaDocumentType::kPDF, 1);
  metafile.data_->recorder.is_recording_ = false;
  std::function<bool()> checkCancel = []() { return true; };
  auto result = metafile.OhosFinishDocument(checkCancel);
  EXPECT_EQ(nullptr, metafile.data_->recorder.getRecordingCanvas());
  EXPECT_EQ(mojom::SkiaDocumentType::kPDF, metafile.data_->type);
  EXPECT_EQ(true, result);
}

TEST(MetafileSkiaTest, OhosFinishDocument005) {
  MetafileSkia metafile(mojom::SkiaDocumentType::kMSKP, 1);
  metafile.data_->recorder.is_recording_ = false;
  std::function<bool()> checkCancel = []() { return true; };
  auto result = metafile.OhosFinishDocument(checkCancel);
  EXPECT_EQ(nullptr, metafile.data_->recorder.getRecordingCanvas());
  EXPECT_EQ(mojom::SkiaDocumentType::kMSKP, metafile.data_->type);
  EXPECT_EQ(true, result);
}

TEST(MetafileSkiaTest, OhosFinishDocument007) {
  MetafileSkia metafile(mojom::SkiaDocumentType::kPDF, 1);
  metafile.data_->data_stream = nullptr;
  std::function<bool()> checkCancel = []() { return false; };
  auto result = metafile.OhosFinishDocument(checkCancel);
  EXPECT_EQ(true, result);
  EXPECT_NE(metafile.data_->data_stream, nullptr);
}

TEST(MetafileSkiaTest, OhosFinishDocument008) {
  MetafileSkia metafile(mojom::SkiaDocumentType::kMSKP, 1);
  metafile.data_->data_stream = nullptr;
  std::function<bool()> checkCancel = []() { return false; };
  auto result = metafile.OhosFinishDocument(checkCancel);
  EXPECT_EQ(true, result);
  EXPECT_NE(metafile.data_->data_stream, nullptr);
}

TEST(MetafileSkiaTest, OhosFinishDocument009) {
  MetafileSkia metafile(mojom::SkiaDocumentType::kPDF, 1);
  metafile.data_->data_stream = nullptr;
  metafile.data_->pages.clear();
  std::function<bool()> checkCancel = []() { return false; };
  auto result = metafile.OhosFinishDocument(checkCancel);
  EXPECT_EQ(true, result);
  EXPECT_NE(metafile.data_->data_stream, nullptr);
}

TEST(MetafileSkiaTest, OhosFinishDocument010) {
  MetafileSkia metafile(mojom::SkiaDocumentType::kPDF, 1);
  metafile.data_->data_stream = nullptr;
  metafile.data_->recorder.is_recording_ = false;
  std::function<bool()> checkCancel = []() { return false; };
  auto result = metafile.OhosFinishDocument(checkCancel);
  EXPECT_EQ(true, result);
  EXPECT_NE(metafile.data_->data_stream, nullptr);
}
#endif  // OHOS_UNITTESTS

}  // namespace printing
