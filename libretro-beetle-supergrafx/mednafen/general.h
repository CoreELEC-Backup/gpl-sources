#ifndef _GENERAL_H
#define _GENERAL_H

#include <string>

extern uint32 MDFN_RoundUpPow2(uint32);

void GetFileBase(const char *f);

// File-inclusion for-read-only path, for PSF and CUE/TOC sheet usage.
bool MDFN_IsFIROPSafe(const std::string &path);

void MDFN_ltrim(char *string);
void MDFN_rtrim(char *string);
void MDFN_trim(char *string);

void MDFN_ltrim(std::string &string);
void MDFN_rtrim(std::string &string);
void MDFN_trim(std::string &string);

void MDFN_GetFilePathComponents(const std::string &file_path, std::string *dir_path_out, std::string *file_base_out = NULL, std::string *file_ext_out = NULL);
std::string MDFN_EvalFIP(const std::string &dir_path, const std::string &rel_path, bool skip_safety_check = false);
#endif
