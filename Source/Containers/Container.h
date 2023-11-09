#pragma once

#include <string>

#include <vector>
#include <unordered_map>
#include <unordered_set>

using FAChar = char;
using FWChar = wchar_t;

using FAString = std::string;
using FWString = std::wstring;

#ifdef UNICODE
using FChar = wchar_t;
using FString = std::wstring;
using FStringStream = std::wstringstream;
using FFileStream = std::wfstream;
#else
using FChar = char;
using FString = std::string;
using FStringStream = std::stringstream;
using FFileStream = std::fstream;
#endif // !UNICODE

#ifdef UNICODE
#define TEXT_PASTE(Quote) L##Quote
#else
#define TEXT_PASTE(Quote) Quote
#endif // !UNICODE

#ifndef AUTO_TEXT
#define AUTO_TEXT(Quote) TEXT_PASTE(Quote)
#endif // !TEXT

template <typename _Ty>
using TArray = std::vector<_Ty>;

template <typename _Kty, typename _Ty>
using TMap = std::unordered_map<_Kty, _Ty>;

template <typename _Kty, typename _Ty = _Kty>
using TSet = std::unordered_map<_Kty, _Ty>;

class FStringUtils
{
public:
    template <typename StrType>
    static FAString ToAString(const StrType& SrcString)
    {
        if constexpr (std::is_same_v<StrType, FAString>)
        {
            return SrcString;
        }
        else
        {
            return WStringToAString(SrcString);
        }
    }

private:
    static FAString WStringToAString(const FWString& SrcString);
};

class FDebugString
{
public:
    static void Printf(const FString& String);
};
