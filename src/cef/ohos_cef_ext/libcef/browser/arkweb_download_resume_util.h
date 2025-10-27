#ifndef CEF_LIBCEF_BROWSER_DOWNLOAD_RESUME_UTIL_H_
#define CEF_LIBCEF_BROWSER_DOWNLOAD_RESUME_UTIL_H_
#pragma once

#include <string>

#include "base/files/file_path.h"
#include "components/download/public/common/download_item.h"
#include "content/public/browser/download_manager.h"
#include "url/gurl.h"

namespace download_resume_util {
// ResumeDownloadWithId is callback param in DownloadManager::GetNextId to
// resume an interrupted download.
void ResumeDownloadWithId(
    content::DownloadManager* manager,
    const std::string& guid,
    const GURL& url,
    const base::FilePath& full_path,
    int64_t received_bytes,
    int64_t total_bytes,
    const std::string& etag,
    const std::string& mime_type,
    const std::string& last_modified,
    std::vector<download::DownloadItem::ReceivedSlice> received_slices,
    uint32_t next_id);
}  // namespace download_resume_util

#endif  // CEF_LIBCEF_BROWSER_DOWNLOAD_RESUME_UTIL_H_
