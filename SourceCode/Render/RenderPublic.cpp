#include "stdafx.h"
#include "Render/RenderPublic.h"


#if (defined(_WIN32) && !defined(OS_WP8) && !defined(OS_W8))

void debug_out(const char* fmt, ...)
{
	char str[1024];
	va_list arg_list;

	va_start(arg_list, fmt);
	vsprintf(str, fmt, arg_list);
	BEATS_ASSERT(strlen(str) < 1022);

	if (str[strlen(str)-1] != '\n')
		strcat(str, "\n");

	printf("%s", str);
	OutputDebugStringA(str);
}

void getErrorText(const char* error, char* text)
{
	const char* p = error;
	char* d = text;
	char start = -1;

	*d = '\0';

	while (*p != 0 && start < 1)
	{
		if (start >= 0)
		{
			*d++ = *p;
		}

		if (*p == '\"')
		{
			++start;
		}

		++p;
	}

	if (d != text)
	{
		*(d - 1) = '\0';
	}
}


int coloredPrintLine(int color, const char* format, ...)
{

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	FlushConsoleInputBuffer(hConsole);
	SetConsoleTextAttribute(hConsole, (WORD)color);

	va_list arg;
	va_start(arg, format);
	int ret = vprintf(format, arg);
	va_end(arg);
	putchar('\n');

	SetConsoleTextAttribute(hConsole, 15);
	return ret;
}


void CheckGLError(const char * x)
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		ERROR_OUT("GL ERROR when calling %s\nERROR CODE:%x\n", x, error);
		BEATS_ASSERT(false);
	}
}

#endif