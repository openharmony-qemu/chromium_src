#include <stddef.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "base/command_line.h"
#include "base/containers/contains.h"
#include "base/containers/flat_set.h"
#include "base/feature_list.h"
#include "base/logging.h"
#include "build/build_config.h"
#include "build/chromeos_buildflags.h"
#include "components/cdm/renderer/external_clear_key_key_system_info.h"
#include "components/cdm/renderer/key_system_support_update.h"
#include "content/public/renderer/key_system_support.h"
#include "content/public/renderer/render_frame.h"
#include "media/base/audio_codecs.h"
#include "media/base/cdm_capability.h"
#include "media/base/content_decryption_module.h"
#include "media/base/eme_constants.h"
#include "media/base/key_system_capability.h"
#include "media/base/key_system_info.h"
#include "media/base/media_switches.h"
#include "media/base/video_codecs.h"
#include "media/cdm/clear_key_cdm_common.h"
#include "media/media_buildflags.h"
#include "third_party/widevine/cdm/buildflags.h"

#if BUILDFLAG(ARKWEB_ENABLE_WISEPLAY)
#include "components/cdm/renderer/wiseplay_key_system_info.h"
#include "media/cdm/wiseplay_cdm_common.h"
#endif

#if BUILDFLAG(ARKWEB_ENABLE_WISEPLAY)
void AddWiseplay(const media::KeySystemCapability& capability,
                 bool can_persist_data,
                 KeySystemInfos* key_systems) {
  if (!can_persist_data) {
    return;
  }
  SupportedCodecs codecs = media::EME_CODEC_NONE;
  SupportedCodecs hw_secure_codecs = media::EME_CODEC_NONE;
  base::flat_set<::media::EncryptionScheme> encryption_schemes;
  base::flat_set<::media::EncryptionScheme> hw_secure_encryption_schemes;
  base::flat_set<CdmSessionType> session_types;
  base::flat_set<CdmSessionType> hw_secure_session_types;
  if (capability.sw_secure_capability) {
    codecs = GetSupportedCodecs(capability.sw_secure_capability.value());
    encryption_schemes = capability.sw_secure_capability->encryption_schemes;
    session_types = UpdatePersistentLicenseSupport(
        can_persist_data, capability.sw_secure_capability->session_types);
    if (!base::Contains(session_types, CdmSessionType::kTemporary)) {
      LOG(INFO) << "[DRM]AddWiseplay, Temporary sessions must be supported.";
      return;
    }
    LOG(INFO) << "[DRM]AddWiseplay, Software secure Wiseplay supported";
  }
  if (capability.hw_secure_capability) {
    const bool force_support_clear_lead =
        media::kHardwareSecureDecryptionForceSupportClearLead.Get();
    hw_secure_codecs = GetSupportedCodecs(
        capability.hw_secure_capability.value(), !force_support_clear_lead);
    hw_secure_encryption_schemes =
        capability.hw_secure_capability->encryption_schemes;
    hw_secure_session_types = UpdatePersistentLicenseSupport(
        can_persist_data, capability.hw_secure_capability->session_types);
    if (!base::Contains(hw_secure_session_types, CdmSessionType::kTemporary)) {
      LOG(INFO) << "[DRM]AddWiseplay, Temporary sessions must be supported.";
      return;
    }
    LOG(INFO) << "[DRM]AddWiseplay, Hardware secure Wiseplay supported";
  }
  using Robustness = WiseplayKeySystemInfo::Robustness;
  auto max_audio_robustness = Robustness::SW_SECURE_DECODE;
  auto max_video_robustness = Robustness::SW_SECURE_DECODE;
  auto persistent_state_support = EmeFeatureSupport::REQUESTABLE;
  auto distinctive_identifier_support = EmeFeatureSupport::NOT_SUPPORTED;
  persistent_state_support = EmeFeatureSupport::REQUESTABLE;
  distinctive_identifier_support = EmeFeatureSupport::NOT_SUPPORTED;
  if (key_systems) {
    key_systems->emplace_back(std::make_unique<WiseplayKeySystemInfo>(
        codecs, encryption_schemes, session_types, hw_secure_codecs,
        hw_secure_encryption_schemes, hw_secure_session_types,
        max_audio_robustness, max_video_robustness, persistent_state_support,
        distinctive_identifier_support));
  }
}
#endif