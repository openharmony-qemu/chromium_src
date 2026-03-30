// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Constants definitions

#include "arkweb/build/features/features.h"
#include "google_apis/gaia/gaia_constants.h"

namespace GaiaConstants {

// Gaia uses this for accounting where login is coming from.
const char kChromeOSSource[] = "chromeos";
const char kChromeSource[] = "ChromiumBrowser";
const char kUnexpectedServiceResponse[] = "UnexpectedServiceResponse";

// OAuth scopes.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kOAuth1LoginScope[] = "https://x.x.x";
#else
const char kOAuth1LoginScope[] = "https://www.google.com/accounts/OAuthLogin";
#endif

// Service/scope names for device management (cloud-based policy) server.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kDeviceManagementServiceOAuth[] = "https://x.x.x";
#else
const char kDeviceManagementServiceOAuth[] =
    "https://www.googleapis.com/auth/chromeosdevicemanagement";
#endif

// OAuth2 scope for access to all Google APIs.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kAnyApiOAuth2Scope[] = "https://x.x.x";
#else
const char kAnyApiOAuth2Scope[] = "https://www.googleapis.com/auth/any-api";
#endif

// OAuth2 scope for access to Chrome sync APIs
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kChromeSyncOAuth2Scope[] = "https://x.x.x";
#else
const char kChromeSyncOAuth2Scope[] =
    "https://www.googleapis.com/auth/chromesync";
#endif
// OAuth2 scope for access to the Chrome Sync APIs for managed profiles.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kChromeSyncSupervisedOAuth2Scope[] = "https://x.x.x";
#else
const char kChromeSyncSupervisedOAuth2Scope[] =
    "https://www.googleapis.com/auth/chromesync_playpen";
#endif

// OAuth2 scope for parental consent logging for secondary account addition.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kKidManagementPrivilegedOAuth2Scope[] = "https://x.x.x";
#else
const char kKidManagementPrivilegedOAuth2Scope[] =
    "https://www.googleapis.com/auth/kid.management.privileged";
#endif

// OAuth2 scope for access to Google Family Link Supervision Setup.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kKidsSupervisionSetupChildOAuth2Scope[] = "https://x.x.x";
#else
const char kKidsSupervisionSetupChildOAuth2Scope[] =
    "https://www.googleapis.com/auth/kids.supervision.setup.child";
#endif

// OAuth2 scope for access to Google Talk APIs (XMPP).
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kGoogleTalkOAuth2Scope[] = "https://x.x.x";
#else
const char kGoogleTalkOAuth2Scope[] =
    "https://www.googleapis.com/auth/googletalk";
#endif

// OAuth2 scope for access to Google account information.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kGoogleUserInfoEmail[] = "https://x.x.x";
#else
const char kGoogleUserInfoEmail[] =
    "https://www.googleapis.com/auth/userinfo.email";
#endif
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kGoogleUserInfoProfile[] = "https://x.x.x";
#else
const char kGoogleUserInfoProfile[] =
    "https://www.googleapis.com/auth/userinfo.profile";
#endif

// OAuth2 scope for IP protection proxy authentication
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kIpProtectionAuthScope[] = "https://x.x.x";
#else
const char kIpProtectionAuthScope[] =
    "https://www.googleapis.com/auth/ip-protection";
#endif

// OAuth2 scope for access to the parent approval widget.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kParentApprovalOAuth2Scope[] = "https://x.x.x";
#else
const char kParentApprovalOAuth2Scope[] =
    "https://www.googleapis.com/auth/kids.parentapproval";
#endif

// OAuth2 scope for access to the people API (read-only).
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kPeopleApiReadOnlyOAuth2Scope[] = "https://x.x.x";
#else
const char kPeopleApiReadOnlyOAuth2Scope[] =
    "https://www.googleapis.com/auth/peopleapi.readonly";
#endif

// OAuth2 scope for access to the people API (read-write).
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kPeopleApiReadWriteOAuth2Scope[] = "https://x.x.x";
#else
const char kPeopleApiReadWriteOAuth2Scope[] =
    "https://www.googleapis.com/auth/peopleapi.readwrite";
#endif

// OAuth2 scope for read-write access to contacts.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kContactsOAuth2Scope[] = "https://x.x.x";
#else
const char kContactsOAuth2Scope[] = "https://www.googleapis.com/auth/contacts";
#endif

// OAuth2 scope for access to the people API person's locale preferences
// (read-only).
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kProfileLanguageReadOnlyOAuth2Scope[] = "https://x.x.x";
#else
const char kProfileLanguageReadOnlyOAuth2Scope[] =
    "https://www.googleapis.com/auth/profile.language.read";
#endif

// OAuth2 scope for access to the programmatic challenge API (read-only).
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kProgrammaticChallengeOAuth2Scope[] = "https://x.x.x";
#else
const char kProgrammaticChallengeOAuth2Scope[] =
    "https://www.googleapis.com/auth/accounts.programmaticchallenge";
#endif

// OAuth2 scope for access to the Reauth flow.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kAccountsReauthOAuth2Scope[] = "https://x.x.x";
#else
const char kAccountsReauthOAuth2Scope[] =
    "https://www.googleapis.com/auth/accounts.reauth";
#endif

// OAuth2 scope for access to audit recording (ARI).
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kAuditRecordingOAuth2Scope[] = "https://x.x.x";
#else
const char kAuditRecordingOAuth2Scope[] =
    "https://www.googleapis.com/auth/auditrecording-pa";
#endif

// OAuth2 scope for access to clear cut logs.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kClearCutOAuth2Scope[] = "https://x.x.x";
#else
const char kClearCutOAuth2Scope[] = "https://www.googleapis.com/auth/cclog";
#endif

// OAuth2 scope for FCM, the Firebase Cloud Messaging service.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kFCMOAuthScope[] = "https://x.x.x";
#else
const char kFCMOAuthScope[] =
    "https://www.googleapis.com/auth/firebase.messaging";
#endif

// OAuth2 scope for access to Tachyon api.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kTachyonOAuthScope[] = "https://x.x.x";
#else
const char kTachyonOAuthScope[] = "https://www.googleapis.com/auth/tachyon";
#endif

// OAuth2 scope for School Tools API.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kSchoolToolsAuthScope[] = "https://x.x.x";
#else
const char kSchoolToolsAuthScope[] =
    "https://www.googleapis.com/auth/chromeosschooltools";
#endif

// OAuth2 scope for access to the Photos API.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kPhotosOAuth2Scope[] = "https://x.x.x";
#else
const char kPhotosOAuth2Scope[] = "https://www.googleapis.com/auth/photos";
#endif

// OAuth2 scope for access to the SecureConnect API.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
extern const char kSecureConnectOAuth2Scope[] = "https://x.x.x";
#else
extern const char kSecureConnectOAuth2Scope[] =
    "https://www.googleapis.com/auth/bce.secureconnect";
#endif

// OAuth2 scope for access to Cast backdrop API.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kCastBackdropOAuth2Scope[] = "https://x.x.x";
#else
const char kCastBackdropOAuth2Scope[] =
    "https://www.googleapis.com/auth/cast.backdrop";
#endif

// OAuth2 scope for access to passwords leak checking API.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kPasswordsLeakCheckOAuth2Scope[] = "https://x.x.x";
#else
const char kPasswordsLeakCheckOAuth2Scope[] =
    "https://www.googleapis.com/auth/identity.passwords.leak.check";
#endif

// OAuth2 scope for access to Chrome safe browsing API.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kChromeSafeBrowsingOAuth2Scope[] = "https://x.x.x";
#else
const char kChromeSafeBrowsingOAuth2Scope[] =
    "https://www.googleapis.com/auth/chrome-safe-browsing";
#endif

// OAuth2 scope for access to kid permissions by URL.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kClassifyUrlKidPermissionOAuth2Scope[] = "https://x.x.x";
#else
const char kClassifyUrlKidPermissionOAuth2Scope[] =
    "https://www.googleapis.com/auth/kid.permission";
#endif
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kKidFamilyReadonlyOAuth2Scope[] = "https://x.x.x";
#else
const char kKidFamilyReadonlyOAuth2Scope[] =
    "https://www.googleapis.com/auth/kid.family.readonly";
#endif

// OAuth2 scope for access to payments.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kPaymentsOAuth2Scope[] = "https://x.x.x";
#else
const char kPaymentsOAuth2Scope[] =
    "https://www.googleapis.com/auth/wallet.chrome";
#endif

#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kCryptAuthOAuth2Scope[] = "https://x.x.x";
#else
const char kCryptAuthOAuth2Scope[] =
    "https://www.googleapis.com/auth/cryptauth";
#endif

// OAuth2 scope for access to Drive.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kDriveOAuth2Scope[] = "https://x.x.x";
#else
const char kDriveOAuth2Scope[] = "https://www.googleapis.com/auth/drive";
#endif

// OAuth2 scope for access to Drive Apps.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kDriveAppsOAuth2Scope[] = "https://x.x.x";
#else
const char kDriveAppsOAuth2Scope[] =
    "https://www.googleapis.com/auth/drive.apps";
#endif

// OAuth2 scope for access for DriveFS to access flags.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kExperimentsAndConfigsOAuth2Scope[] = "https://x.x.x";
#else
const char kExperimentsAndConfigsOAuth2Scope[] =
    "https://www.googleapis.com/auth/experimentsandconfigs";
#endif

// OAuth2 scope for access for DriveFS to use client-side notifications.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kClientChannelOAuth2Scope[] = "https://x.x.x";
#else
const char kClientChannelOAuth2Scope[] =
    "https://www.googleapis.com/auth/client_channel";
#endif

// The scope required for an access token in order to query ItemSuggest.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kDriveReadOnlyOAuth2Scope[] = "https://x.x.x";
#else
const char kDriveReadOnlyOAuth2Scope[] =
    "https://www.googleapis.com/auth/drive.readonly";
#endif

// OAuth2 scope for access to Assistant SDK.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kAssistantOAuth2Scope[] = "https://x.x.x";
#else
const char kAssistantOAuth2Scope[] =
    "https://www.googleapis.com/auth/assistant-sdk-prototype";
#endif

// OAuth2 scope for access to nearby devices (fast pair) APIs.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kNearbyDevicesOAuth2Scope[] = "https://x.x.x";
#else
const char kNearbyDevicesOAuth2Scope[] =
    "https://www.googleapis.com/auth/nearbydevices-pa";
#endif

// OAuth2 scope for access to readonly Drive Apps.
const char kDriveAppsReadonlyOAuth2Scope[] =
    "https://www.googleapis.com/auth/drive.apps.readonly";

// OAuth2 scope for access to nearby sharing.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kNearbyShareOAuth2Scope[] = "https://x.x.x";
#else
const char kNearbyShareOAuth2Scope[] =
    "https://www.googleapis.com/auth/nearbysharing-pa";
#endif

// OAuth2 scope for access to nearby sharing.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kNearbyPresenceOAuth2Scope[] = "https://x.x.x";
#else
const char kNearbyPresenceOAuth2Scope[] =
    "https://www.googleapis.com/auth/nearbypresence-pa";
#endif

// OAuth2 scopes for access to GCM account tracker.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kGCMGroupServerOAuth2Scope[] = "https://x.x.x";
#else
const char kGCMGroupServerOAuth2Scope[] = "https://www.googleapis.com/auth/gcm";
#endif
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kGCMCheckinServerOAuth2Scope[] = "https://x.x.x";
#else
const char kGCMCheckinServerOAuth2Scope[] =
    "https://www.googleapis.com/auth/android_checkin";
#endif

// OAuth2 scope for access to readonly Chrome web store.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kChromeWebstoreOAuth2Scope[] = "https://x.x.x";
#else
const char kChromeWebstoreOAuth2Scope[] =
    "https://www.googleapis.com/auth/chromewebstore.readonly";
#endif

// OAuth2 scope for access to Account Capabilities API.
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kAccountCapabilitiesOAuth2Scope[] = "https://x.x.x";
#else
const char kAccountCapabilitiesOAuth2Scope[] =
    "https://www.googleapis.com/auth/account.capabilities";
#endif

#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kSupportContentOAuth2Scope[] = "https://x.x.x";
const char kPhotosModuleOAuth2Scope[] = "https://x.x.x";
const char kPhotosModuleImageOAuth2Scope[] = "https://x.x.x";
const char kFeedOAuth2Scope[] = "https://x.x.x";
const char kKAnonymityServiceOAuth2Scope[] = "https://x.x.x";
const char kCalendarReadOnlyOAuth2Scope[] = "https://x.x.x";
const char kPasskeysEnclaveOAuth2Scope[] = "https://x.x.x";
const char kCloudSearchQueryOAuth2Scope[] = "https://x.x.x";
const char kDiscoveryEngineCompleteQueryOAuth2Scope[] = "https://x.x.x";
const char kDiscoveryOAuth2Scope[] = "https://x.x.x";
const char kGeminiOAuth2Scope[] = "https://x.x.x";
const char kChromeMemexOAuth2Scope[] = "https://x.x.x";
const char kChromebookOAuth2Scope[] = "https://x.x.x";
const char kTasksReadOnlyOAuth2Scope[] = "https://x.x.x";
const char kTasksOAuth2Scope[] = "https://x.x.x";
const char kYouTubeMusicOAuth2Scope[] = "https://x.x.x";
const char kClassroomReadOnlyCoursesOAuth2Scope[] = "https://x.x.x";
const char kClassroomReadOnlyCourseWorkSelfOAuth2Scope[] = "https://x.x.x";
const char kClassroomReadOnlyCourseWorkStudentsOAuth2Scope[] = "https://x.x.x";
const char kClassroomReadOnlyStudentSubmissionsSelfOAuth2Scope[] = "https://x.x.x";
const char kClassroomReadOnlyRostersOAuth2Scope[] = "https://x.x.x";
const char kClassroomProfileEmailOauth2Scope[] = "https://x.x.x";
const char kClassroomProfilePhotoUrlScope[] = "https://x.x.x";
const char kClassroomCourseWorkMaterialsOAuthScope[] = "https://x.x.x";
const char kOptimizationGuideServiceGetHintsOAuth2Scope[] = "https://x.x.x";
const char kOptimizationGuideServiceModelExecutionOAuth2Scope[] = "https://x.x.x";
const char kLensOAuth2Scope[] = "https://x.x.x";
const char kAidaOAuth2Scope[] = "https://x.x.x";
const char kGdpOAuth2Scope[] = "https://x.x.x";
const char kWebstoreOAuth2Scope[] = "https://x.x.x";
const char kPushNotificationOAuth2Scope[] = "https://x.x.x";
const char kLicenseCheckOAuth2Scope[] = "https://x.x.x";
const char kMantaOAuth2Scope[] = "https://x.x.x";
#else
// OAuth2 scope for support content API.
const char kSupportContentOAuth2Scope[] =
    "https://www.googleapis.com/auth/supportcontent";

// OAuth 2 scope for NTP Photos module API.
const char kPhotosModuleOAuth2Scope[] =
    "https://www.googleapis.com/auth/photos.firstparty.readonly";

// OAuth 2 scope for NTP Photos module image API.
const char kPhotosModuleImageOAuth2Scope[] =
    "https://www.googleapis.com/auth/photos.image.readonly";

// OAuth 2 scope for the Discover feed.
const char kFeedOAuth2Scope[] = "https://www.googleapis.com/auth/googlenow";

// OAuth 2 scope for the k-Anonymity Service API.
const char kKAnonymityServiceOAuth2Scope[] =
    "https://www.googleapis.com/auth/chromekanonymity";

// OAuth 2 scope for readonly access to Calendar.
const char kCalendarReadOnlyOAuth2Scope[] =
    "https://www.googleapis.com/auth/calendar.readonly";

// OAuth 2 scope for Google Password Manager passkey enclaves.
const char kPasskeysEnclaveOAuth2Scope[] =
    "https://www.googleapis.com/auth/secureidentity.action";

// OAuth2 scope for Cloud Search query API.
const char kCloudSearchQueryOAuth2Scope[] =
    "https://www.googleapis.com/auth/cloud_search.query";

// OAuth2 scope for Discovery Engine suggestion API.
const char kDiscoveryEngineCompleteQueryOAuth2Scope[] =
    "https://www.googleapis.com/auth/discoveryengine.complete_query";

// OAuth2 scope for Access Code Cast.
const char kDiscoveryOAuth2Scope[] =
    "https://www.googleapis.com/auth/cast-edu-messaging";

// OAuth2 scope for Gemini app.
const char kGeminiOAuth2Scope[] = "https://www.googleapis.com/auth/gemini";

// OAuth2 scope for Chrome Memex API, used in the Chrome Shopping Insights
// Side Panel.
const char kChromeMemexOAuth2Scope[] =
    "https://www.googleapis.com/auth/chromememex";

// OAuth2 scope to access the ChromebookEmailService API.
const char kChromebookOAuth2Scope[] =
    "https://www.googleapis.com/auth/chromebook.email";

// OAuth 2 scopes for Google Tasks API.
const char kTasksReadOnlyOAuth2Scope[] =
    "https://www.googleapis.com/auth/tasks.readonly";

const char kTasksOAuth2Scope[] = "https://www.googleapis.com/auth/tasks";

// OAuth 2 scope for YouTube Music API.
const char kYouTubeMusicOAuth2Scope[] = "https://www.googleapis.com/auth/music";

// OAuth 2 scopes for Google Classroom API.
const char kClassroomReadOnlyCoursesOAuth2Scope[] =
    "https://www.googleapis.com/auth/classroom.courses.readonly";

const char kClassroomReadOnlyCourseWorkSelfOAuth2Scope[] =
    "https://www.googleapis.com/auth/classroom.coursework.me.readonly";

const char kClassroomReadOnlyCourseWorkStudentsOAuth2Scope[] =
    "https://www.googleapis.com/auth/classroom.coursework.students.readonly";

const char kClassroomReadOnlyStudentSubmissionsSelfOAuth2Scope[] =
    "https://www.googleapis.com/auth/classroom.student-submissions.me.readonly";

const char kClassroomReadOnlyRostersOAuth2Scope[] =
    "https://www.googleapis.com/auth/classroom.rosters.readonly";

const char kClassroomProfileEmailOauth2Scope[] =
    "https://www.googleapis.com/auth/classroom.profile.emails";

const char kClassroomProfilePhotoUrlScope[] =
    "https://www.googleapis.com/auth/classroom.profile.photos";

const char kClassroomCourseWorkMaterialsOAuthScope[] =
    "https://www.googleapis.com/auth/classroom.courseworkmaterials";

// OAuth2 scopes for Optimization Guide.
const char kOptimizationGuideServiceGetHintsOAuth2Scope[] =
    "https://www.googleapis.com/auth/chrome-optimization-guide";

const char kOptimizationGuideServiceModelExecutionOAuth2Scope[] =
    "https://www.googleapis.com/auth/chrome-model-execution";

// OAuth2 scopes for Lens.
const char kLensOAuth2Scope[] = "https://www.googleapis.com/auth/lens";

// OAuth2 scope for DevTools GenAI features.
const char kAidaOAuth2Scope[] = "https://www.googleapis.com/auth/aida";

// OAuth2 scope for Google Developer Program <-> DevTools integration features.
const char kGdpOAuth2Scope[] =
    "https://www.googleapis.com/auth/devprofiles.full_control";

// OAuth2 scope for Chrome Web Store.
const char kWebstoreOAuth2Scope[] =
    "https://www.googleapis.com/auth/chromewebstore.readonly";

// OAuth2 scope for push notifications.
const char kPushNotificationOAuth2Scope[] =
    "https://www.googleapis.com/auth/notifications";

// OAuth2 scope for app license check.
const char kLicenseCheckOAuth2Scope[] =
    "https://www.googleapis.com/auth/applicense.bytebot";

// OAuth2 scope for manta.
const char kMantaOAuth2Scope[] = "https://www.googleapis.com/auth/mdi.aratea";
#endif

// Used to build ClientOAuth requests.  These are the names of keys used when
// building base::DictionaryValue that represent the json data that makes up
// the ClientOAuth endpoint protocol.  The comment above each constant explains
// what value is associated with that key.

// Canonical email of the account to sign in.
const char kClientOAuthEmailKey[] = "email";

// Used as an Invalid refresh token.
const char kInvalidRefreshToken[] = "invalid_refresh_token";

// Name of the Google authentication cookie.
const char kGaiaSigninCookieName[] = "SAPISID";

}  // namespace GaiaConstants
