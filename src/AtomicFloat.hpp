/// \file 
/// \author Franck Dary
/// \author Cindy Aloui
#ifndef ATOMICFLOAT__H
#define ATOMICFLOAT__H

#include <atomic>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////
/// \brief An atomic `float`, replaces `std::atomic<float>`
///
/// Allows for lock-less concurrent manipulation of floating point value by
/// multiple threads. Implements get-and-add atomic operation through the 
/// `+=` operator. `std::atomic<T>` where T is an integer type already provides
/// this feature, that is why AtomicFloat rely on such a member object.
///
/// Usage example :
/// \code
/// AtomicFloat val1; // Initialized to 0.0f
/// AtomicFloat val2(1.3f); // Initialized to 0.0f
/// 
/// val1 += val2 - 0.5f; // Works as expected from numeric types
/// float posVal1 = abs(val1); // implicit conversion to float
/// val1 = posVal1; // assignement from float
/// printf("%f", val1.load()); // explicit conversion to float when necessary
///
/// \endcode
///
////////////////////////////////////////////////////////////////////////////
class AtomicFloat
{
	public :

	static constexpr float scaling = 1000 * 1000;

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Converts `float` value to `long long int` for storage
	///
	/// We're storing a `float` inside a `atomic<long long int>` so we need a bijection
	/// between `float` and `long long int`, this is the function that send `float` to
	/// `long long int`.
	///
	/// \param f `float` to be converted
	///
	/// \return `long long int` storing the `float` value f
	///
	////////////////////////////////////////////////////////////////////////////
	static constexpr long long int scale(float f)
	{
		return std::llround(f * scaling);
	}

	private :

	std::atomic<long long int> value;

	public :

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Construct from `float`, default value is `0.0f`.
	///
	/// \param value The value AtomicFloat is set to.
	///
	////////////////////////////////////////////////////////////////////////////
	AtomicFloat(float value = 0.0f);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Copy constructor.
	///
	/// \param model The AtomicFloat we're taking the value from.
	///
	////////////////////////////////////////////////////////////////////////////
	AtomicFloat(const AtomicFloat & model);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Restitution of the `float` value.
	///
	/// This is the function that restitutes the `float` value represented by
	/// the `std::atomic<long long int>` member.
	///
	/// \return The current value of this AtomicFloat instance.
	///
	/// \see scale
	///
	////////////////////////////////////////////////////////////////////////////
	operator float() const;

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Restitution of the `float` value.
	///
	/// \return The current value of this AtomicFloat instance.
	///
	////////////////////////////////////////////////////////////////////////////
	float load() const;

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Get-and-add operation.
	///
	/// \param increment Value to add.
	///
	/// \return Reference to self.
	///
	////////////////////////////////////////////////////////////////////////////
	AtomicFloat & operator+=(float increment);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Assign `float` value.
	///
	/// \param value to assgin.
	///
	/// \return Reference to self.
	///
	////////////////////////////////////////////////////////////////////////////
	AtomicFloat & operator=(float value);

	/////////////////////////////////////////////////////////////////////////////
	/// \brief Copy assignment operator.
	///
	/// \param value to assign.
	///
	/// \return Reference to self.
	///
	////////////////////////////////////////////////////////////////////////////
	AtomicFloat & operator=(const AtomicFloat & model);
};

#endif
