
#include <windows.h>
#include <string>
#include <locale>
#include <map>

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::System::UserProfile;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::Security::Cryptography;
using namespace Windows::Security::Cryptography::Certificates;
using namespace Windows::Foundation;

std::wstring Utf8ToWide(const std::string &text)
{
  if (text.empty())
    return L"";

  int bufSize = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.c_str(), -1, NULL, 0);
  if (bufSize == 0)
    return L"";
  wchar_t *converted = new wchar_t[bufSize];
  if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.c_str(), -1, converted, bufSize) != bufSize)
  {
    delete[] converted;
    return L"";
  }

  std::wstring Wret(converted);
  delete[] converted;
  return Wret;
}

std::string WideToUtf8(const std::wstring &text)
{
  if (text.empty())
    return "";

  int bufSize = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text.c_str(), -1, NULL, 0, NULL, NULL);
  if (bufSize == 0)
    return "";
  char * converted = new char[bufSize];
  if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text.c_str(), -1, converted, bufSize, NULL, NULL) != bufSize)
  {
    delete[] converted;
    return "";
  }

  std::string ret(converted);
  delete[] converted;

  return ret;
}

extern "C" {
    
size_t uwp_Utf8ToW(const char* src, wchar_t* buffer, int maxlen)
{
  std::wstring converted = Utf8ToWide(std::string(src));
  int len = min(converted.length(), maxlen - 1);
  wcsncpy(buffer, converted.c_str(), len);
  buffer[len] = '\0';

  return len;
}

size_t uwp_cachepath(char *buffer, size_t cch)
{
  try
  {
    Platform::String^ path = Windows::Storage::ApplicationData::Current->LocalFolder->Path;
    path = Platform::String::Concat(path, "\\dvdcss");

    std::string utf8path = WideToUtf8(std::wstring(path->Data()));

    strncpy(buffer, utf8path.c_str(), min(cch, path->Length()));
    return min(cch, path->Length());
  }
  catch (Platform::Exception^)
  {
    return 0;
  }
}

char* uwp_getenv(const char* n)
{
  static std::map<std::string, std::string> sEnvironment;

  if (n == nullptr)
    return nullptr;

  std::string name(n);

  // check key
  if (!name.empty())
  {
    uint32_t valLen = GetEnvironmentVariableA(name.c_str(), nullptr, 0);
    if (!valLen)
      return nullptr;

    std::string value(valLen, 0);
    GetEnvironmentVariableA(name.c_str(), const_cast<char*>(value.c_str()), valLen);

    sEnvironment[name] = value;
    return const_cast<char*>(sEnvironment[name].c_str());
  }
  return nullptr;
}
}
