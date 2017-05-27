#pragma once

extern CRITICAL_SECTION g_csConsole; //When threads write to console we need mutual exclusion

void WriteToConsole(char *szFormat, ...);
int GetNoOfProcessors();
