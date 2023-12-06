

#include "Random.h"

std::mt19937 IceEngine::Random::s_RandomEngine;

std::uniform_int_distribution<std::mt19937::result_type> IceEngine::Random::s_Distribution;


