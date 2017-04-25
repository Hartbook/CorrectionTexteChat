#ifndef ATOMICFLOAT__H
#define ATOMICFLOAT__H

#include <atomic>
#include <cmath>

class AtomicFloat
{
	public :

	static constexpr float scaling = 1000 * 1000;
	static constexpr long long int scale(float f)
	{
		return std::llround(f * scaling);
	}

	private :

	std::atomic<long long int> value;

	public :

	AtomicFloat(float value = 0.0f);
	AtomicFloat(const AtomicFloat & model);
	operator float() const;
	float load() const;
	AtomicFloat & operator+=(float increment);
	AtomicFloat & operator=(float value);
	AtomicFloat & operator=(const AtomicFloat & model);
};

#endif
