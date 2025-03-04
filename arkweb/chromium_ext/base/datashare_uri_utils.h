// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_DATASHARE_URI_UTILS_H_
#define BASE_DATASHARE_URI_UTILS_H_

#include <string>

#include "base/base_export.h"
#include "base/files/file.h"
#include "base/files/file_path.h"

namespace base {

// Opens a datashare URI for read and returns the file descriptor to the caller.
BASE_EXPORT File OpenDatashareUriForRead(const FilePath& uri);

// The display name of the file that is to be exposed as File.name in the
// DOM layer.  If it is empty, the base part of the |file_path| is used.
BASE_EXPORT std::u16string GetFileDisplayName(const FilePath& datashare_uri);

// Get the real path for the uri.
BASE_EXPORT std::string GetRealPath(const FilePath& datashare_uri);

}  // namespace base

#endif  // BASE_DATASHARE_URI_UTILS_H_
