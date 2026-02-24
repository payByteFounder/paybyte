// Copyright (c) 2012-2018 The Bitcoin Core developers
// Copyright (c) 2011-2020 The Litecoin Core developers
// Copyright (c) 2026 The PayByte developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <clientversion.h>

#include <tinyformat.h>

/**
 * Name of client reported in the 'version' message.
 * Same name for daemon and GUI.
 */
const std::string CLIENT_NAME("PayByte");

#ifdef HAVE_BUILD_INFO
#include <obj/build.h>
#endif

//! git will put "#define GIT_COMMIT_ID ..." on the next line inside archives. $Format:%n#define GIT_COMMIT_ID "%H"$

#ifdef BUILD_GIT_TAG
    #define BUILD_DESC BUILD_GIT_TAG
    #define BUILD_SUFFIX ""
#else
    #define BUILD_DESC "v" STRINGIZE(CLIENT_VERSION_MAJOR) "." STRINGIZE(CLIENT_VERSION_MINOR) "." STRINGIZE(CLIENT_VERSION_REVISION)
    #ifdef BUILD_GIT_COMMIT
        #define BUILD_SUFFIX "-" BUILD_GIT_COMMIT
    #elif defined(GIT_COMMIT_ID)
        #define BUILD_SUFFIX "-g" GIT_COMMIT_ID
    #else
        #define BUILD_SUFFIX ""
    #endif
#endif

const std::string CLIENT_BUILD(std::string(BUILD_DESC) + BUILD_SUFFIX);

static std::string FormatVersion(int nVersion)
{
    return strprintf("%d.%d.%d",
                     nVersion / 1000000,
                     (nVersion / 10000) % 100,
                     (nVersion / 100) % 100);
}

std::string FormatFullVersion()
{
    return CLIENT_BUILD;
}

/**
 * Format the subversion field according to BIP 14 spec
 * Example output:
 *   /PayByte:1.0.0/
 */
std::string FormatSubVersion(const std::string& name, int nClientVersion, const std::vector<std::string>& comments)
{
    std::ostringstream ss;
    ss << "/";
    ss << name << ":" << FormatVersion(nClientVersion);

    if (!comments.empty())
    {
        ss << "(";
        for (size_t i = 0; i < comments.size(); ++i)
        {
            if (i != 0)
                ss << "; ";
            ss << comments[i];
        }
        ss << ")";
    }

    ss << "/";
    return ss.str();
}