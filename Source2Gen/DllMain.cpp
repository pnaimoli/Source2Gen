#include <stdexcept>
#include <thread>
#include <chrono>

#include "Source2Gen.hpp"

using namespace std;

//asdfasdf
void StartupThread()
{
	Source2Gen generator(SOURCE2_OUTPUT);
	generator.GenerateHeaders();
}

#ifdef _WIN32

int __stdcall DllMain(void* instance, unsigned int reason, void* reserved)
{
	if (reason == 1)
	{
		thread t(StartupThread);
		t.detach();
	}

	return 1;
}

#else

void __attribute__((constructor)) DylibMain()
{
    thread t(StartupThread);
    t.detach();
}

#endif
