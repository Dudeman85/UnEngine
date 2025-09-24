#pragma once

#include <random>
#include <vector>

inline std::random_device rd;
inline std::mt19937 generator(rd());

inline static int Random(const int Min, const int Max);
inline static float Random(const float Min, const float Max);
inline static double Random(const double Min, const double Max);
template<typename T>
inline static T Random(const T Min, const T Max);
template<typename T>
inline static std::vector<T> Random(const std::vector<T>& Vector);