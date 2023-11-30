#pragma once

#include <stdint.h>
#include <initializer_list>

#include <array>
#include <GL/glew.h>

namespace IceEngine {

	struct Color {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		Color() : r(0), g(0), b(0), a(255) {}

		// Constructor using individual arguments
		Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
			: r(red), g(green), b(blue), a(alpha) {}

		Color(std::initializer_list <uint8_t> values) {
			auto it = values.begin();
			r = (it != values.end()) ? *it : 0; ++it;
			g = (it != values.end()) ? *it : 0; ++it;
			b = (it != values.end()) ? *it : 0; ++it;
			a = (it != values.end()) ? *it : 255;
		}

		std::array<float, 4> flat_list() const {
			std::array<float, 4> result;
			result[0] = r / 255.0f;
			result[1] = g / 255.0f;
			result[2] = b / 255.0f;
			result[3] = a / 255.0f;
			return result;
		}
	};

	// Wrapper function to use glClearColor with a Color object
	void set_clear_color(Color& c) {
		auto values = c.flat_list();
		glClearColor(values[0], values[1], values[2], values[3]);
	}

	void set_clear_color(std::initializer_list<uint8_t> values) {
		auto c = Color(values);
		set_clear_color(c);
	}

	//// Base case for the variadic template
	//void set_clear_color() {}

	//// Helper function for the variadic template
	//template<typename T, typename... Args>
	//void set_clear_color(const T& arg, Args... args) {
	//	set_clear_color(arg);
	//	set_clear_color(args...);
	//}



}