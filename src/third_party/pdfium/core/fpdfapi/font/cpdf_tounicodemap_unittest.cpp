// Copyright 2015 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/font/cpdf_tounicodemap.h"

#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/span.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

TEST(CPDFToUnicodeMapTest, StringToCode) {
  EXPECT_THAT(CPDF_ToUnicodeMap::StringToCode("<0001>"), testing::Optional(1u));
  EXPECT_THAT(CPDF_ToUnicodeMap::StringToCode("<c2>"), testing::Optional(194u));
  EXPECT_THAT(CPDF_ToUnicodeMap::StringToCode("<A2>"), testing::Optional(162u));
  EXPECT_THAT(CPDF_ToUnicodeMap::StringToCode("<Af2>"),
              testing::Optional(2802u));
  EXPECT_THAT(CPDF_ToUnicodeMap::StringToCode("<FFFFFFFF>"),
              testing::Optional(4294967295u));

  // Whitespaces within the string are ignored.
  EXPECT_THAT(CPDF_ToUnicodeMap::StringToCode("<00\n0\r1>"),
              testing::Optional(1u));
  EXPECT_THAT(CPDF_ToUnicodeMap::StringToCode("<c 2>"),
              testing::Optional(194u));
  EXPECT_THAT(CPDF_ToUnicodeMap::StringToCode("<A2\r\n>"),
              testing::Optional(162u));

  // Integer overflow
  EXPECT_FALSE(CPDF_ToUnicodeMap::StringToCode("<100000000>").has_value());
  EXPECT_FALSE(CPDF_ToUnicodeMap::StringToCode("<1abcdFFFF>").has_value());

  // Invalid string
  EXPECT_FALSE(CPDF_ToUnicodeMap::StringToCode("").has_value());
  EXPECT_FALSE(CPDF_ToUnicodeMap::StringToCode("<>").has_value());
  EXPECT_FALSE(CPDF_ToUnicodeMap::StringToCode("12").has_value());
  EXPECT_FALSE(CPDF_ToUnicodeMap::StringToCode("<12").has_value());
  EXPECT_FALSE(CPDF_ToUnicodeMap::StringToCode("12>").has_value());
  EXPECT_FALSE(CPDF_ToUnicodeMap::StringToCode("<1-7>").has_value());
  EXPECT_FALSE(CPDF_ToUnicodeMap::StringToCode("00AB").has_value());
  EXPECT_FALSE(CPDF_ToUnicodeMap::StringToCode("<00NN>").has_value());
}

TEST(CPDFToUnicodeMapTest, StringToWideString) {
  EXPECT_EQ(L"", CPDF_ToUnicodeMap::StringToWideString(""));
  EXPECT_EQ(L"", CPDF_ToUnicodeMap::StringToWideString("1234"));
  EXPECT_EQ(L"", CPDF_ToUnicodeMap::StringToWideString("<c2"));
  EXPECT_EQ(L"", CPDF_ToUnicodeMap::StringToWideString("<c2D2"));
  EXPECT_EQ(L"", CPDF_ToUnicodeMap::StringToWideString("c2ab>"));

  WideString res = L"\xc2ab";
  EXPECT_EQ(res, CPDF_ToUnicodeMap::StringToWideString("<c2ab>"));
  EXPECT_EQ(res, CPDF_ToUnicodeMap::StringToWideString("<c2abab>"));

  res += L"\xfaab";
  EXPECT_EQ(res, CPDF_ToUnicodeMap::StringToWideString("<c2abFaAb>"));
  EXPECT_EQ(res, CPDF_ToUnicodeMap::StringToWideString("<c2abFaAb12>"));
  EXPECT_EQ(res, CPDF_ToUnicodeMap::StringToWideString("<c2ab FaAb>"));
  EXPECT_EQ(res, CPDF_ToUnicodeMap::StringToWideString("<c2ab FaAb12>"));
  EXPECT_EQ(res, CPDF_ToUnicodeMap::StringToWideString("<c2ab FaAb 12>"));
  EXPECT_EQ(res, CPDF_ToUnicodeMap::StringToWideString("< c 2 a b  F a A b  1 2 >"));
}

TEST(CPDFToUnicodeMapTest, HandleBeginBFRangeAvoidIntegerOverflow) {
  // Make sure there won't be infinite loops due to integer overflows in
  // HandleBeginBFRange().
  {
    static constexpr uint8_t kInput1[] =
        "beginbfrange<FFFFFFFF><FFFFFFFF>[<0041>]endbfrange";
    auto stream = pdfium::MakeRetain<CPDF_Stream>(kInput1);
    CPDF_ToUnicodeMap map(stream);
    EXPECT_EQ(L"A", map.Lookup(0xffffffff));
  }
  {
    static constexpr uint8_t kInput2[] =
        "beginbfrange<FFFFFFFF><FFFFFFFF><0042>endbfrange";
    auto stream = pdfium::MakeRetain<CPDF_Stream>(kInput2);
    CPDF_ToUnicodeMap map(stream);
    EXPECT_EQ(L"B", map.Lookup(0xffffffff));
  }
  {
    static constexpr uint8_t kInput3[] =
        "beginbfrange<FFFFFFFF><FFFFFFFF><00410042>endbfrange";
    auto stream = pdfium::MakeRetain<CPDF_Stream>(kInput3);
    CPDF_ToUnicodeMap map(stream);
    EXPECT_EQ(L"AB", map.Lookup(0xffffffff));
  }
}

TEST(CPDFToUnicodeMapTest, HandleBeginBFCharTolerateOutOfSpecCount) {
  // Tolerate more than 100 entries.
  static constexpr uint8_t kTooManyEntriesInput[] =
      "112 beginbfchar"
      "<0000><0008>"
      "<0001><0009>"
      "<0002><000A>"
      "<0003><000B>"
      "<0004><000C>"
      "<0005><000D>"
      "<0006><000E>"
      "<0007><000F>"
      "<0008><0000>"
      "<0009><0001>"
      "<000A><0002>"
      "<000B><0003>"
      "<000C><0004>"
      "<000D><0005>"
      "<000E><0006>"
      "<000F><0007>"
      "<0010><0018>"
      "<0011><0019>"
      "<0012><001A>"
      "<0013><001B>"
      "<0014><001C>"
      "<0015><001D>"
      "<0016><001E>"
      "<0017><001F>"
      "<0018><0010>"
      "<0019><0011>"
      "<001A><0012>"
      "<001B><0013>"
      "<001C><0014>"
      "<001D><0015>"
      "<001E><0016>"
      "<001F><0017>"
      "<0020><0028>"
      "<0021><0029>"
      "<0022><002A>"
      "<0023><002B>"
      "<0024><002C>"
      "<0025><002D>"
      "<0026><002E>"
      "<0027><002F>"
      "<0028><0020>"
      "<0029><0021>"
      "<002A><0022>"
      "<002B><0023>"
      "<002C><0024>"
      "<002D><0025>"
      "<002E><0026>"
      "<002F><0027>"
      "<0030><0038>"
      "<0031><0039>"
      "<0032><003A>"
      "<0033><003B>"
      "<0034><003C>"
      "<0035><003D>"
      "<0036><003E>"
      "<0037><003F>"
      "<0038><0030>"
      "<0039><0031>"
      "<003A><0032>"
      "<003B><0033>"
      "<003C><0034>"
      "<003D><0035>"
      "<003E><0036>"
      "<003F><0037>"
      "<0040><0048>"
      "<0041><0049>"
      "<0042><004A>"
      "<0043><004B>"
      "<0044><004C>"
      "<0045><004D>"
      "<0046><004E>"
      "<0047><004F>"
      "<0048><0040>"
      "<0049><0041>"
      "<004A><0042>"
      "<004B><0043>"
      "<004C><0044>"
      "<004D><0045>"
      "<004E><0046>"
      "<004F><0047>"
      "<0050><0058>"
      "<0051><0059>"
      "<0052><005A>"
      "<0053><005B>"
      "<0054><005C>"
      "<0055><005D>"
      "<0056><005E>"
      "<0057><005F>"
      "<0058><0050>"
      "<0059><0051>"
      "<005A><0052>"
      "<005B><0053>"
      "<005C><0054>"
      "<005D><0055>"
      "<005E><0056>"
      "<005F><0057>"
      "<0060><0068>"
      "<0061><0069>"
      "<0062><006A>"
      "<0063><006B>"
      "<0064><006C>"
      "<0065><006D>"
      "<0066><006E>"
      "<0067><006F>"
      "<0068><0060>"
      "<0069><0061>"
      "<006A><0062>"
      "<006B><0063>"
      "<006C><0064>"
      "<006D><0065>"
      "<006E><0066>"
      "<006F><0067>"
      "endbfchar";
  auto stream = pdfium::MakeRetain<CPDF_Stream>(kTooManyEntriesInput);
  CPDF_ToUnicodeMap map(stream);
  EXPECT_EQ(9u, map.ReverseLookup(0x0001));
  EXPECT_EQ(111u, map.ReverseLookup(0x0067));
  EXPECT_EQ(1u, map.GetUnicodeCountByCharcodeForTesting(1u));
  EXPECT_EQ(1u, map.GetUnicodeCountByCharcodeForTesting(111u));
}

TEST(CPDFToUnicodeMapTest, InsertIntoMultimap) {
  {
    // Both the CIDs and the unicodes are different.
    static constexpr uint8_t kInput1[] =
        "beginbfchar<1><0041><2><0042>endbfchar";
    auto stream = pdfium::MakeRetain<CPDF_Stream>(kInput1);
    CPDF_ToUnicodeMap map(stream);
    EXPECT_EQ(1u, map.ReverseLookup(0x0041));
    EXPECT_EQ(2u, map.ReverseLookup(0x0042));
    EXPECT_EQ(1u, map.GetUnicodeCountByCharcodeForTesting(1u));
    EXPECT_EQ(1u, map.GetUnicodeCountByCharcodeForTesting(2u));
  }
  {
    // The same CID with different unicodes.
    static constexpr uint8_t kInput2[] =
        "beginbfrange<0><0><0041><0><0><0042>endbfrange";
    auto stream = pdfium::MakeRetain<CPDF_Stream>(kInput2);
    CPDF_ToUnicodeMap map(stream);
    EXPECT_EQ(0u, map.ReverseLookup(0x0041));
    EXPECT_EQ(0u, map.ReverseLookup(0x0042));
    EXPECT_EQ(2u, map.GetUnicodeCountByCharcodeForTesting(0u));
  }
  {
    // Duplicate mappings of CID 0 to unicode "A". There should be only 1 entry
    // in `multimap_`.
    static constexpr uint8_t kInput3[] =
        "beginbfrange<0><0>[<0041>]endbfrange\n"
        "beginbfchar<0><0041>endbfchar";
    auto stream = pdfium::MakeRetain<CPDF_Stream>(kInput3);
    CPDF_ToUnicodeMap map(stream);
    EXPECT_EQ(0u, map.ReverseLookup(0x0041));
    EXPECT_EQ(1u, map.GetUnicodeCountByCharcodeForTesting(0u));
  }
}

TEST(CPDFToUnicodeMapTest, NonBmpUnicodeLookup) {
  static constexpr uint8_t kInput[] = "beginbfchar<01><d841de76>endbfchar";
  CPDF_ToUnicodeMap map(pdfium::MakeRetain<CPDF_Stream>(kInput));
  EXPECT_EQ(L"\xd841\xde76", map.Lookup(0x01));
#if defined(WCHAR_T_IS_32_BIT)
  // TODO(crbug.com/374947848): Should work if wchar_t is 16-bit.
  // TODO(crbug.com/374947848): Should return 1u.
  EXPECT_EQ(0u, map.ReverseLookup(0x20676));
#endif
}
