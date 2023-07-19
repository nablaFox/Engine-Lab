#pragma once

#include "vel_types.hpp"

#include <iostream>

#define VK_CHECK(x)													\
	do {															\
		VkResult err = x;											\
		if (err) {													\
			std::cout <<"Vulkan error: " << err << std::endl;		\
			std::abort();											\
		}															\
	} while (0)