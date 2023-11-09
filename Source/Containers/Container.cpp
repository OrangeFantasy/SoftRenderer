#include "Containers/Container.h"

#include <Windows.h>

FAString FStringUtils::WStringToAString(const FWString& SrcString)
{
#ifdef _WIN32
    std::size_t StrLen = WideCharToMultiByte(CP_ACP, 0, SrcString.c_str(), SrcString.size(), NULL, 0, NULL, NULL);
    FAChar* Buffer = new FAChar[StrLen + 1];
    WideCharToMultiByte(CP_ACP, 0, SrcString.c_str(), SrcString.size(), Buffer, StrLen, NULL, NULL);
    Buffer[StrLen] = '\0';

    FAString Result(Buffer);
    delete[] Buffer;
    Buffer = nullptr;

    return Result;
#else
    return "Only support Windows platform";
#endif
}

void FDebugString::Printf(const FString& String)
{
#ifdef _WIN32
    OutputDebugString(String.c_str());
#endif
}
