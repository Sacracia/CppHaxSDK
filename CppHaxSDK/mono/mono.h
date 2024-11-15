#pragma once

#include <string>

namespace mono {
	void Initialize();
    void* GetFuncAddress(std::string signature);
}