#pragma once

#include <fstream>
#include <sstream>
#include <cstring>  // Include the cstring header for strcpy

#include "Logger.h"

namespace IceEngine 
{
	const char* LoadTextFile(const char* filePath);
}