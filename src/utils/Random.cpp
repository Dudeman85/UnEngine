#include "utils/Random.h"

inline static int Random(const int Min, const int Max)                      // Generate a random, uniformly distributed integer
{
    std::uniform_int_distribution<int> distribution(Min, Max);
    return distribution(generator);
}
inline static float Random(const float Min, const float Max)                // Generate a random, uniformly distributed float
{
    std::uniform_real_distribution<float> distribution(Min, Max);
    return distribution(generator);
}
inline static double Random(const double Min, const double Max)             // Generate a random, uniformly distributed double
{
    std::uniform_real_distribution<double> distribution(Min, Max);
    return distribution(generator);
}
template<typename T>
inline static T Random(const T Min, const T Max)                            // Generate a random, uniformly distributed templated type
{
    std::uniform_real_distribution<T> distribution(Min, Max);
    return distribution(generator);
}
template<typename T>
inline static std::vector<T> Random(const std::vector<T>& Vector)           // Generate a random, uniformly distributed templated vector type
{
    std::vector<T> ReturnVector;

    int i = 0;
    for (T Operand : Vector)
    {
        ReturnVector.push_back(Random(-Operand[i], Operand[i]));
        i++;
    }

    return ReturnVector;
}