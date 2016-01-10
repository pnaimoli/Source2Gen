#pragma once

#include <string>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#define GetModuleHandle(NAME) dlopen(NAME, RTLD_LAZY)
#define GetProcAddress dlsym
#define HMODULE void*
#endif

template <typename T>
class Interface
{
public:
	typedef void*(*CreateInterfaceFn)(const char*, int*);

	static T* Get(const std::string& dllName, const std::string& fullName)
	{
		static T* intyface = nullptr;

		if (intyface)
			return intyface;

		void* dll = nullptr;

#ifdef _WIN32
        const std::string& fullDllName = dllName + ".dll";
#else
        const std::string& fullDllName = std::string("lib") + dllName + ".dylib";
#endif

		while (true)
		{
			dll = (void*)GetModuleHandle(fullDllName.c_str());

			if (dll)
				break;
		}

		std::string createInterface("CreateInterface");
		CreateInterfaceFn factory = (CreateInterfaceFn)GetProcAddress((HMODULE)dll, createInterface.c_str());

		intyface = reinterpret_cast<T*>(factory(fullName.c_str(), nullptr));
		return intyface;
	}
};

#ifndef _WIN32
#undef GetModuleHandle
#undef GetProcAddress
#undef HMODULE
#endif
