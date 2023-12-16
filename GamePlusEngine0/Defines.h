#pragma once


#define SCREEN_WIDTH 1834
#define SCREEN_HEIGHT 760

namespace IceEngine
{
	/*inline float convert_x_from_world_to_ndc(float value)
	{
		return 2 * ((value - SCREEN_WIDTH) / SCREEN_WIDTH) + 1;
	}

	inline float convert_y_from_world_to_ndc(float value)
	{
		return 2 * ((value - SCREEN_HEIGHT) / SCREEN_HEIGHT) + 1;
	}*/

	inline float convert_x_from_world_to_ndc(float value)
	{
		return (2.0f * value / SCREEN_WIDTH) - 1.0f;
	}

	inline float convert_y_from_world_to_ndc(float value)
	{
		return 1.0f - (2.0f * value / SCREEN_HEIGHT);
	}
}