/**

    @file      ctx_logger.h
    @brief     Global Logging module
    @details   To consume this module, procedure `ctx_logger_output()` must be defined and linked against this module. This is done so that logging has a static dependency injection capability.
    @author    Jakob Kristersson <jakob@kristerson.se>
    @date      26.05.2024

**/
#pragma once
#include <wchar.h>

enum ctx_logger_severity
{
    CTX_LOG_SEV_NONE,

    CTX_LOG_SEV_INFO,

    CTX_LOG_SEV_DEBUG,

    CTX_LOG_SEV_WARN,

    CTX_LOG_SEV_ERROR,

    CTX_LOG_SEV_FATAL,

    CTX_LOG_SEV_MAX,
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void ctx_logger_entry(enum ctx_logger_severity eSeverity, const wchar_t* zFormat, ...);

void ctx_logger_output(enum ctx_logger_severity, const wchar_t* zMsg, size_t msgSize);

#ifdef __cplusplus
}
#endif // __cplusplus

/**
    @def   ctxAPPND
    @brief append a line to the output log
    @param format, ... 
**/
#define ctxAPPND(...) ctx_logger_entry(CTX_LOG_SEV_NONE,  L##__VA_ARGS__)

/**
    @def   ctxINFO
    @brief Add an informative line to the output log
    @param format, ... 
**/
#define ctxINFO(...)  ctx_logger_entry(CTX_LOG_SEV_INFO,  L##__VA_ARGS__)

/**
    @def   ctxDEBUG
    @brief Add a debugging line to the output log
    @param format, ... 
**/
#define ctxDEBUG(...) ctx_logger_entry(CTX_LOG_SEV_DEBUG, L##__VA_ARGS__)

/**
    @def   ctxWARN
    @brief Add a warning line to the output log
    @param format, ... 
**/
#define ctxWARN(...)  ctx_logger_entry(CTX_LOG_SEV_WARN,  L##__VA_ARGS__)

/**
    @def   ctxERROR
    @brief Add an error line to the output log
    @param format, ... 
**/
#define ctxERROR(...) ctx_logger_entry(CTX_LOG_SEV_ERROR, L##__VA_ARGS__)

/**
    @def   ctxFATAL
    @brief Add a fatal error line to the output log. Afterwards the app should terminate
    @param format, ... 
**/
#define ctxFATAL(...) ctx_logger_entry(CTX_LOG_SEV_FATAL, L##__VA_ARGS__)