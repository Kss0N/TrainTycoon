#include "ctx_logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


static size_t severity_toString(wchar_t zBuffer[16], enum ctx_logger_severity eSeverity)
{
	if (CTX_LOG_SEV_NONE >= eSeverity || eSeverity >= CTX_LOG_SEV_MAX)
	{
		zBuffer = '\0';
		return 0;
	}

	const wchar_t zSeverities[CTX_LOG_SEV_MAX] = {
		[CTX_LOG_SEV_INFO]  = L"INFO",
		[CTX_LOG_SEV_DEBUG] = L"DEBUG",
		[CTX_LOG_SEV_WARN]  = L"WARN",
		[CTX_LOG_SEV_ERROR] = L"ERROR",
		[CTX_LOG_SEV_FATAL] = L"FATAL ERROR",
	};

	errno_t err = wcscpy_s(zBuffer, 16, zSeverities[eSeverity]);
	return wcslen(zBuffer);
}


#define BUFFER_MAX_SIZE 512


extern void ctx_logger_entry(enum ctx_logger_severity eSeverity, const wchar_t* zFormat, ...)
{
	wchar_t 
		buffer[BUFFER_MAX_SIZE],
		*ptr = buffer;
	va_list vargs;

	va_start(vargs, zFormat),
	eSeverity != CTX_LOG_SEV_NONE ? 
		*ptr++ = L'[',
		ptr += severity_toString(ptr, eSeverity),
		*ptr++ = L']',
		*ptr++ = L' ' : 0,
	ptr += vswprintf_s(ptr, _countof(buffer) - (ptrdiff_t)(ptr - buffer), zFormat, vargs),
	*ptr++ = L'\n',
	*ptr++ = L'\0',
	ctx_logger_output(eSeverity, buffer, (size_t)(ptrdiff_t)(ptr - buffer));
}
