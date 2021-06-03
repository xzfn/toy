
#include "system.h"

#include <iostream>

#include <windows.h>

void set_thread_priority(uint64_t thread_id, int level) {
#ifdef WIN32
		int native_priority = THREAD_PRIORITY_NORMAL;
		switch (level) {
		case 0:
			native_priority = THREAD_PRIORITY_NORMAL;
			break;
		case 1:
			native_priority = THREAD_PRIORITY_ABOVE_NORMAL;
			break;
		case 2:
			native_priority = THREAD_PRIORITY_HIGHEST;
			break;
		case -1:
			native_priority = THREAD_PRIORITY_BELOW_NORMAL;
			break;
		case -2:
			native_priority = THREAD_PRIORITY_LOWEST;
			break;
		}
		HANDLE h_thread = OpenThread(THREAD_SET_INFORMATION, false, thread_id);
		if (h_thread == NULL) {
			std::cout << "ThreadPriority OpenThread failed " << thread_id << "\n";
			return;
		}
		BOOL bret = SetThreadPriority(h_thread, native_priority);
		CloseHandle(h_thread);
		std::cout << "ThreadPriority " << thread_id << " " << bret << "\n";
		if (!bret) {
			std::cout << "    GetLastError " << GetLastError() << "\n";
		}
#endif
}
