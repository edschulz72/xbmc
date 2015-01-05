#ifndef __LIBVIDONUTILS_COMMONHEADER_H__

#define __LIBVIDONUTILS_COMMONHEADER_H__

#ifndef int64_t
typedef long long int64_t;
#endif
namespace libVidonUtils
{
#define VIDON_UTILS_MAX_PATH 255
  enum ErrorCode { ErrorCode_sUnknown = 0, ErrorCode_sNormal = 1, ErrorCode_sErrorUserPass = 2, ErrorCode_sErrorChipId = 3, ErrorCode_sErrorNetwork = 4 };
  enum FunctionID { SUPPORT_BD_NAV = 101, SUPPORT_HD_AUDIO = 102 };

  // license status£¬1 normal£¬2 expired£¬3 prohibit¡£
  enum CurrentLicenseStatus { CurrentLicenseStatus_sUnknown = 0, CurrentLicenseStatus_sNormal = 1, CurrentLicenseStatus_sExpire = 2, CurrentLicenseStatus_sProhibit = 3 };
  enum CurrentLicenseType { CurrentLicenseType_sUnknown = 0, CurrentLicenseType_sNormal = 1, CurrentLicenseType_sTrail = 101 };

  enum OtherLicenseStatus { OtherLicenseStatus_sUnbound = 0, OtherLicenseStatus_sBound = 1 };

  enum CodeStatus { CodeStatus_sUnused = 0, CodeStatus_sUsed = 1 };

  enum CountryCode { CountryCode_Unkown = 0, CountryCode_China = 1, CountryCode_Others = 300 };

  enum TopupResults { TopupResults_Failed = 0, TopupResults_Success = 1, TopupResults_NetworkError = 2, TopupResults_Unknow = 100 };

  enum VidonJobStatus { VidonJobStatus_Running = 0, VidonJobStatus_Finished = 1 };

  enum VidonUploadParams{ VidonUploadParams_Title = 0, VidonUploadParams_Version, VidonUploadParams_Username, VidonUploadParams_Email, VidonUploadParams_Subject, VdionUpload_TempDir, VdionUploadParams_RecordType };

}//namespace libVidonUtils

#endif //ifndef __LIBVIDONUTILS_COMMONHEADER_H__