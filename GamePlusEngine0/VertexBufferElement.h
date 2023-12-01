#pragma once

#include <stdint.h>

namespace IceEngine 
{
	struct VertexBufferElement 
	{
		uint32_t type;
		uint32_t count;
		uint8_t normalized;

		static uint32_t GetSizeOfType(uint32_t type);
	};
}