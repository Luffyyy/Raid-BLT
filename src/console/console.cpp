#include <windows.h>
#include <conio.h>
#include <FCNTL.H>
#include <io.h>
#include "console.h"
#include <iostream>

namespace
{
	class outbuf : public std::streambuf
	{
	public:
		outbuf()
		{
			setp(0, 0);
		}

		virtual int_type overflow(int_type c = traits_type::eof()) override
		{
			return fputc(c, stdout) == EOF ? traits_type::eof() : c;
		}
	};

	outbuf obuf;
	std::streambuf *sb = nullptr;
}
static BOOL WINAPI MyConsoleCtrlHandler(DWORD dwCtrlEvent) { return dwCtrlEvent == CTRL_C_EVENT; }

CConsole::CConsole() : m_OwnConsole(false) {
	if (!AllocConsole()) return;

	SetConsoleCtrlHandler(MyConsoleCtrlHandler, TRUE);
	RemoveMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);
	const int in = _open_osfhandle(INT_PTR(GetStdHandle(STD_INPUT_HANDLE)), _O_TEXT);
	const int out = _open_osfhandle(INT_PTR(GetStdHandle(STD_OUTPUT_HANDLE)), _O_TEXT);
	
	freopen("CONOUT$", "w", stdout); // start crash fix
	freopen("CONIN$", "r", stdin); // start crash fix

	m_OldStdin = *stdin;
	m_OldStdout = *stdout;

	*stdin = *_fdopen(in, "r");
	*stdout = *_fdopen(out, "w");

	// Redirect std::cout to the same location as stdout, otherwise you it won't appear on the console.
	sb = std::cout.rdbuf(&obuf);

	m_OwnConsole = true;
}

CConsole::~CConsole() {
	if (m_OwnConsole) {
		//std::cout.rdbuf(sb); // removed for exit crash fix
		fclose(stdout);
		fclose(stdin);
		*stdout = m_OldStdout;
		*stdin = m_OldStdin;
		std::ios::sync_with_stdio(); // exit crash fix
		SetConsoleCtrlHandler(MyConsoleCtrlHandler, FALSE);
		FreeConsole();
	}
}