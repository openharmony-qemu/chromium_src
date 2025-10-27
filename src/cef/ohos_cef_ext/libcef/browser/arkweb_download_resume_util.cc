#include "cef/ohos_cef_ext/libcef/browser/arkweb_download_resume_util.h"

#include "base/time/time.h"
#include "base/uuid.h"
#include "components/download/public/common/download_item_impl.h"
#include "content/public/browser/storage_partition_config.h"

namespace download_resume_util {
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
const char kNWebId[] = "nweb_id";
#endif  //  ARKWEB_EX_DOWNLOAD

void ResumeDownloadWithId(
    content::DownloadManager* manager,
    const std::string& guid,
    const GURL& gurl,
    const base::FilePath& full_path,
    int64_t received_bytes,
    int64_t total_bytes,
    const std::string& etag,
    const std::string& mime_type,
    const std::string& last_modified,
    std::vector<download::DownloadItem::ReceivedSlice> received_slices,
    uint32_t next_id) {
  std::vector<GURL> url_chain;
  url_chain.push_back(gurl);
  auto download_item = manager->CreateDownloadItem(
      guid,                                         /*guid*/
      next_id,                                      /*id*/
      full_path,                                    /*current_path*/
      full_path,                                    /*target_path*/
      url_chain,                                    /*url_chain*/
      GURL(),                                       /*referrer_url*/
      content::StoragePartitionConfig(),            /*storage_partition_config*/
      GURL(),                                       /*tab_url*/
      GURL(),                                       /*tab_referrer_url*/
      url::Origin(),                                /*request_initiator*/
      mime_type,                                    /*mime_type*/
      mime_type,                                    /*original_mime_type*/
      base::Time::Now(),                            /*start_time*/
      base::Time::Now(),                            /*end_time*/
      etag,                                         /*etag*/
      last_modified,                                /*last_modified*/
      received_bytes,                               /*received_bytes*/
      total_bytes,                                  /*total_bytes*/
      std::string(),                                /*hash*/
      download::DownloadItem::INTERRUPTED,          /*state*/
      download::DOWNLOAD_DANGER_TYPE_NOT_DANGEROUS, /*danger_type*/
      download::DOWNLOAD_INTERRUPT_REASON_NONE,     /*interrupt_reason*/
      false,                                        /*opened*/
      base::Time(),                                 /*last_access_time*/
      false,                                        /*transient*/
      received_slices                               /*received_slices*/
  );
  if (download_item) {
#if BUILDFLAG(ARKWEB_EX_DOWNLOAD)
    download_item->SetUserData(
        kNWebId,
        std::make_unique<download::ArkWebDownloadItemImplExt::NWebIdData>(-1));
#endif  //  ARKWEB_EX_DOWNLOAD
    download_item->Resume(true /*is_user_resume*/);
  }
}
}  //  namespace download_resume_util
