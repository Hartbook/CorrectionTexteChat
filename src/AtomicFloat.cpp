#include "AtomicFloat.hpp"

AtomicFloat::AtomicFloat(float value) : value(scale(value))
{
}

AtomicFloat::operator float() const
{
	return value / scaling;
}

AtomicFloat & AtomicFloat::operator+=(float increment)
{
	value += scale(increment);
	return *this;
}

AtomicFloat & AtomicFloat::operator=(float value)
{
	this->value = scale(value);
	return *this;
}

float AtomicFloat::load() const
{
	return *this;
}

