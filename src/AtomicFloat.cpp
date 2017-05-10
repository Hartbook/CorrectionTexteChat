#include "AtomicFloat.hpp"

AtomicFloat::AtomicFloat(float value) : value(scale(value))
{
}

AtomicFloat::AtomicFloat(const AtomicFloat & model) : AtomicFloat(model.load())
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

AtomicFloat & AtomicFloat::operator=(const AtomicFloat & model)
{
	this->value = model.value.load();
	return *this;
}

float AtomicFloat::load() const
{
	return *this;
}

