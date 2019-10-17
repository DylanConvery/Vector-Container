#ifndef VECTOR_H
#define VECTOR_H

#include <initializer_list>

template<typename T, typename A = std::allocator<T>>
class vector {
public:
	//default constructor
	vector();

	//construct a vector object using the argmuents passed and allocates 'size' amount of elements
	//which are then default initialized to 0
	explicit vector(int size);

	//construct a vector object using the elements provided within the initializer list argument
	vector(std::initializer_list<T> elements);

	//copy initialization constructor. Construct a new vector by copying the components of the 
	//argument passed
	vector(const vector& copy);

	//overloaded assignment operator to avoid chance of memory leak
	vector& operator = (const vector& copy);

	//move constructor, allows us to avoid 'n' amount of operations and instead steal an objects 
	//data meaning its pointers, size, etc, unlike the copy initialization constructor
	vector(vector&& move);

	//overloaded move assignment operator to allow us to avoid memory leaks and needless
	//operations like stated in the move constructor. This allows us to steal an rvalues
	//data
	vector& operator = (vector&& move);

	//subscript operator, allows for the getting and setting of elements
	T& operator[] (int element);

	//subscript operator, allows for the getting elements of a constant vectors
	const T& operator[] (int element) const;

	//destroys vector object and deallocates all elements allocated on free store
	~vector();

	//returns amount of elements in vector
	int size() const;

	//returns the space available to our vector
	int capacity() const;

	//reserves new space for a vector to fill with data.
	void reserve(int new_allocation);

	//resizes a vector.
	void resize(int new_size, T value = T());

	//push a value to the end of our vector
	void push_back(const T& d);

private:
	int _size;			//holds the number of elements available to index in vector
	T* _elements;		//points to our elements
	A _allocator;		//used to allocate memory of size T
	int _space;			//number of elements + free memory available to vector
};

#endif // !VECTOR_H

template<typename T, typename A>
vector<T, A>::vector() : _size{ 0 }, _elements{ nullptr }, _space{ 0 } {}

template<typename T, typename A>
vector<T, A>::vector(int size) : _size{ size }, _elements{ new T[size] }, _space{ size } {
	for (int i = 0; i < size; ++i) {
		_elements[i] = 0;
	}
}

template<typename T, typename A>
vector<T, A>::vector(std::initializer_list<T> elements) : _size{ static_cast<int>(elements.size()) }, _elements{ new T[_size] }, _space{ static_cast<int>(elements.size()) } {
	for (int i = 0; i < static_cast<int>(elements.size()); ++i) {
		_elements[i] = *(elements.begin() + i);
	}
}

template<typename T, typename A>
vector<T, A>::vector(const vector& copy) : _size{ copy._size }, _elements{ new T[_size] }, _space{ copy._size } {
	for (int i = 0; i < copy._size; ++i) {
		_elements[i] = copy._elements[i];
	}
}

//checks whether you're trying to assign a vector to itself, then checks whether the size of 
//the right operand is less than or equal to the left hand operand, if so we copy over its 
//values, overwriting the elements in our vector with the new ones provided by the right hand
//operand. If that fails, we first allocate enough space to exactly copy over the values of 
//the right hand operand and then do so. We then erase all our current member variables 
//values, and then copy the right hand operands member variables values. 
template<typename T, typename A>
vector<T, A>& vector<T, A>::operator = (const vector& copy) {
	if (this == &copy) { return *this; }

	if (copy._size <= _space) {
		for (int i = 0; i < copy._size; ++i) {
			_elements[i] = copy._elements[i];
		}
		//the left over elements now don't matter and will be overwritten if need be
		_size = copy._size; 
		return *this;
	}

	T* elements = new T[copy._size];
	for (int i = 0; i < copy._size; ++i) {
		elements[i] = copy._elements[i];
	}

	delete[] _elements;
	_size = copy._size;
	_space = copy._size;
	_elements = elements;
	return *this;
}

//the '&&' notation is used for "rvalue references"
template<typename T, typename A>
vector<T, A>::vector(vector&& move) : _size{ move._size }, _elements{ move._elements }, _space{ move._size } {
	move._size = 0;
	move._space = 0;
	move._elements = nullptr;
}

template<typename T, typename A>
vector<T, A>& vector<T, A>::operator = (vector&& move) {
	delete[] _elements;

	_size = move._size;
	_space = move._size;
	_elements = move._elements;

	move._size = 0;
	move._space = 0;
	move._elements = nullptr;

	return *this;
}

template<typename T, typename A>
T& vector<T, A>::operator[](int element) {
	return _elements[element];
}

template<typename T, typename A>
const T& vector<T, A>::operator[](int element) const {
	return _elements[element];
}

template<typename T, typename A>
vector<T, A>::~vector() {
	delete[] _elements;
}

template<typename T, typename A>
int vector<T, A>::size() const {
	return _size;
}

template<typename T, typename A>
int vector<T, A>::capacity() const {
	return _space;
}

//checks if newly requested space is needed, if so, copies our current data into a new array which
//contains the newly requested space available for use. Does not initialize newly allocation space.
template<typename T, typename A>
void vector<T, A>::reserve(int new_allocation) {
	if (new_allocation <= _space) { return; }

	T* elements = _allocator.allocate(new_allocation);
	for (int i = 0; i < _size; ++i) {
		_allocator.construct(&elements[i], _elements[i]);
	}
	for (int i = 0; i < _size; ++i) {
		_allocator.destroy(&_elements[i]);
	}
	_allocator.deallocate(_elements, _space);

	_elements = elements;
	_space = new_allocation;
}

//requests new memory to be allocated for our vector to fill, initializes all that new memory and 
//assigns the new size of our vector to our _size member variable as to reflect its new size.
template<typename T, typename A>
void vector<T, A>::resize(int new_size, T value) {
	reserve(new_size);
	for (int i = _size; i < new_size; ++i) {
		_allocator.construct(&_elements[i], value);
	}

	for (int i = new_size; i < _size; ++i) {
		_allocator.destroy(&_elements[i]);
	}
	_size = new_size;
}

//checks to make sure we have the space available to us to insert a new value into our vector, if
//we do, we just go ahead and do so and update our _size variable. If not, we then allocate twice 
//the amount of memory we currently have available to us and then insert our value and update 
//_size. If we have no memory available whatsoever, we allocate 8 bytes, insert our value and 
//update size
template<typename T, typename A>
void vector<T, A>::push_back(const T& value) {
	if (_space == 0) {
		reserve(8);
	} else if (_size == _space) {
		reserve(_space * 2);
	}

	_allocator.construct(&_elements[_size], value);
	++_size;
}