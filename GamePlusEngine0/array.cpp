#include "array.h"


#include <algorithm>    // For std::sort, std::random_shuffle
#include <iostream>     // For std::cout, std::endl
#include <stdexcept>    // For std::out_of_range
#include <unordered_set> // For std::unordered_set

namespace IceEngine {


	template <typename ForwardIt1, typename ForwardIt2>
	void my_iter_swap(ForwardIt1 a, ForwardIt2 b) {
		auto temp = *a;
		*a = *b;
		*b = temp;
	}

		
	struct Basic_Random_Generator {
		unsigned int state;

		Basic_Random_Generator(unsigned int seed) {
			state = seed;
		}

		// Linear Congruential Generator parameters
		static constexpr unsigned int LCG_A = 1664525;
		static constexpr unsigned int LCG_C = 1013904223;

		unsigned int next();
		
	};

	unsigned int Basic_Random_Generator::next() {
		state = LCG_A * state + LCG_C;
		return state;
	}
	

	

	// Calculate the distance between two iterators
	template <typename Iterator>
	std::size_t my_distance(Iterator first, Iterator last) {
		std::size_t distance = 0;
		while (first != last) {
			++first;
			++distance;
		}
		return distance;
	}

	template <typename T>
	void my_random_shuffle(T begin, T end) {
		// Seed for the custom random number generator
		Basic_Random_Generator rng(42); 

		// Perform Fisher-Yates shuffle
		for (T it = end - 1; it > begin; --it) {
			std::size_t dist = rng.next() % my_distance(begin, it);
			my_iter_swap(*it, *(begin + dist));
		}
	}


	// *iter
	template<typename T>
	inline T Array_Iterator<T>::operator*() const {
		return array->data[index];
	}

	// ++iter
	template<typename T>
	const Array_Iterator<T> &Array_Iterator<T>::operator++() {
		++index;
		return *this;
	}

	// iter != iter
	template<typename T>
	bool Array_Iterator<T>::operator!=(const Array_Iterator &other) const {
		return index != other.index;
	}

	// --iter
	template<typename T>
	Array_Iterator<T> &Array_Iterator<T>::operator--() {
		--index;
		return *this;
	}

	// iter+= n
	template<typename T>
	Array_Iterator<T> &Array_Iterator<T>::operator+=(int n) {
		index += n;
		return *this;
	}

	// iter + n
	template<typename T>
	Array_Iterator<T> Array_Iterator<T>::operator+(int n) const {
		return Array_Iterator(array, index + n);
	}

	// iter -= n
	template<typename T>
	Array_Iterator<T> &Array_Iterator<T>::operator-=(int n) {
		index -= n;
		return *this;
	}

	// iter - n
	template<typename T>
	Array_Iterator<T> Array_Iterator<T>::operator-(int n) const {
		return Array_Iterator(array, index - n);
	}

	// int index = iter - iter ?
	template<typename T>
	int Array_Iterator<T>::operator-(const Array_Iterator &other) const {
		return index - other.index;
	}

	// iter[n]
	template<typename T>
	T &Array_Iterator<T>::operator[](int n) const {
		return array->data[index + n];
	}

	// iter == iter
	template<typename T>
	bool Array_Iterator<T>::operator==(const Array_Iterator &other) const {
		return index == other.index;

	}

	// iter->
	template<typename T>
	T *Array_Iterator<T>::operator->() const {
		return  &(array->data[index]);
	}

	// Array<T> t(n);
	template<typename T>
	Array<T>::Array(int start_size) {
		items = 0;
		allocated_items = start_size > 0 ? start_size : 0;

		// allocate memory for the array
		data = new T[allocated_items];
	}

	template<typename T>
	Array<T>::~Array() {
		// Release the memory used by the array
		if (data != nullptr) {
			delete[] data;
			data = nullptr;  // Set to nullptr after deletion to avoid double deletion
		}
	}

	// t.add(n);
	template<typename T>
	void Array<T>::add(T item) {
		// check if there is enough space in the array
		if (items == allocated_items) {
			int newCapacity = (allocated_items == 0) ? 1 : 2 * allocated_items; // Double the capacity
			resize(newCapacity);
		}
		// add the new element to the end of the array
		data[items] = item;
		items++;
	}

	// t.add_unique(n);
	template<typename T>
	void Array<T>::add_unique(T item) {

		int index = 0;

		if (!find(item, &index)) return; 
		add(item);
	}

	template<typename T>
	void Array<T>::insert_at(int index, T item) {
		// check valid index
		if (index < 0 || index >= items) throw std::out_of_range("Index out of bounds");;

		// check if we need to resize
		if (items == allocated_items) {
			int newCapacity = (allocated_items == 0) ? 0 : 2 * allocated_items;
			resize(newCapacity);
		}

		// Add by shifting the elements
		for (int i = count(); i >= index; i--) {
			data[i + 1] = data[i];
		}

		data[index] = item;
		items++;
	}

	template<typename T>
	int Array<T>::count() const {
		return items;
	} 

	template<typename T>
	void Array<T>::reset() {
		items = 0;
	}

	template<typename T>
	bool Array<T>::remove(T item) {

		for (int i = 0; i < count(); i++) {

			if (data[i] == item) {
				
				for (int j = i; j < count() - 1; j++) {
					data[j] = data[j + 1];
				}

				items--;
				return true;

			}
		}

		return false;
	}

	// Searches for the first occurrence of the specified item in the array
	template<typename T>
	bool Array<T>::find(T item, int *index) const {
		for (int i = 0; i < count(); i++) {
			if (data[i] == item) {
				*index = i;
				return true;
			}
		}
		*index = -1;
		return false;
	}


	template<typename T>
	T Array<T>::remove_nth(int index) {

		if (index < 0 || index >= items) {
			throw std::out_of_range("Index out of bounds");
		}

		T removedElement = data[index];

		data[index] = data[items - 1];

		--items;

		return removedElement;
	}


	template<typename T>
	void Array<T>::remove_nth_elemente_while_keeping_order(int n) {
	
		if (n < 0 || n >= items) {
			throw std::out_of_range("Index out of bounds");
		}

		for (int i = n; n < count() - 1; i++) {
			data[i] = data[i + 1];
		}
		--items;
	}

	template<typename T>
	void Array<T>::reserve(int size) {
		if (size > allocated_items) {

			T *newData = new T[size];
			for (int i = 0; i < items; i++) {
				newData[i] = data[i];
			}

			delete[] data;

			data = newData;
			allocated_items = size;
		}
	}

	// Changes the number of elements stored
	template<typename T>
	void Array<T>::resize(int size) {
		reserve(size);
	}

	// Remove the last element
	template<typename T>
	T Array<T>::pop() {

		if (empty()) throw std::out_of_range("Array is empty");

		T temp = data[items-1];
		--items;
		return temp;
	}

	template<typename T>
	T Array<T>::peek() {

		if (empty()) throw std::out_of_range("Array is empty");

		return T(data[items-1]);
	}
	
	template<typename T>
	void Array<T>::reverse() {
		for (int i = 0; i < items / 2; ++i) {
			std::swap(data[i], data[items - i - 1]);
		}
	}

	template<typename T>
	void Array<T>::sort() {
		std::sort(data, data + items);
	}

	template<typename T>
	T Array<T>::front() const {
		if (empty()) {
			throw std::out_of_range("Array is empty");
		}
		return T(data[0]);
	}

	template<typename T>
	T Array<T>::back() const {
		if (empty()) {
			throw std::out_of_range("Array is empty");
		}
		return T(data[items-1]);
	}

	template<typename T>
	bool Array<T>::empty() const {
		return items == 0;
	}

	template<typename T>
	int Array<T>::capacity() const {
		return allocated_items;
	}

	template<typename T>
	void Array<T>::fill(const T &value) {
		for (int i = 0; i < count(); i++) {
			data[i] = value;
		}
	}



	// TODO(mo) replace with own swap
	// Swaps the contents
	template<typename T>
	void Array<T>::swap(Array &other) {
		std::swap(data, other.data);
		std::swap(items, other.items);
		std::swap(allocated_items, other.allocated_items);
	}


	// TOOD(mo) replace copy with own version
	template<typename T>
	void Array<T>::concatenate(const Array &other) {
		reserve(items + other.items);
		std::copy(other.data, other.data + other.items, data + items);
		items += other.items;
	}

	// TODO(mo): Replace unordered_set with one version
	template<typename T>
	void Array<T>::unique() {
		// remove duplicate elements while preserving the order
		std::unordered_set<T> uniqueSet;
		int index = 0;
		for (int i = 0; i < items; ++i) {
			if (uniqueSet.insert(data[i]).second) {
				data[index++] = data[i];
			}
		}
		items = index;
	}


	// TODO(mo): Replace unordered_set with one version
	template<typename T>
	void Array<T>::unique_unsorted() {
		// remove duplicate elements without preserving the order
		std::unordered_set<T> uniqueSet;
		int index = 0;
		for (int i = 0; i < items; ++i) {
			if (uniqueSet.insert(data[i]).second) {
				std::swap(data[index++], data[i]);
			}
		}
		items = index;
	}

	template<typename T>
	void Array<T>::shuffle() {
		// Shuffle the elements in the array
		
		my_random_shuffle(data, data + items);
	}

	template<typename T>
	void Array<T>::print() const {
		for (int i = 0; i < count(); i++) {
			std::cout << data[i] << " ";
		}
		std::cout << "\n";
	}

	template<typename T>
	const T &Array<T>::operator[](const int i) const {
		if (i < 0 || i >= items) throw std::out_of_range("Index out of bounds");
		return data[i];
	}

	template<typename T>
	T &Array<T>::operator[](const int i) {
		if (i < 0 || i >= items) throw std::out_of_range("Index out of bounds");

		return data[i];
	}

	template<typename T>
	Array_Iterator<T> Array<T>::begin() {
		return Array_Iterator<T>(this, 0);
	}

	template<typename T>
	Array_Iterator<T> Array<T>::end() {
		return Array_Iterator<T>(this, items);
	}

	template<typename T>
	Array_Iterator<T> Array<T>::rbegin() {
		return Array_Iterator<T>(this, items);
	}

	template<typename T>
	Array_Iterator<T> Array<T>::rend() {
		return Array_Iterator<T>(this, 0);
	}

	template<typename T>
	const Array_Iterator<T> Array<T>::begin() const {
		return Array_Iterator<T>(this, 0);
	}

	template<typename T>
	const Array_Iterator<T> Array<T>::end() const {
		return Array_Iterator<T>(this, items);
	}

	template<typename T>
	const Array_Iterator<T> Array<T>::rbegin() const {
		return Array_Iterator<T>(this, items);
	}

	template<typename T>
	const Array_Iterator<T> Array<T>::rend() const {
		return Array_Iterator<T>(this, 0);
	}

	template<typename T>
	Array_Iterator<T> Array<T>::erase(Array_Iterator<T> pos) {
		if (pos.index < 0 || pos.index >= items) {
			throw std::out_of_range("Index out of bounds");
		}

		for (int i = pos.index; i < count() - 1; i++) {
			data[i] = data[i + 1];
		}

		--items;

		// Return iterator to the next element
		return Array_Iterator<T>(this, pos.index);
	}

	template<typename T>
	Array_Iterator<T> Array<T>::erase(Array_Iterator<T> first, Array_Iterator<T> last) {
		if (first.index < 0 || first.index >= items || last.index < 0 || last.index > items || first.index > last.index) {
			throw std::out_of_range("Invalid range");
		}

		int rangeSize = last.index - first.index + 1;

		for (int i = first.index; i < items - rangeSize; ++i) {
			data[i] = data[i + rangeSize];
		}

		items -= rangeSize;

		// Return iterator to the next element after the erased range
		return Array_Iterator<T>(this, first.index);
	}

	void TestArray() {

		Array<int> myArray;

		myArray.add(10);
		myArray.add(20);
		myArray.add(30);

		std::cout << "Count: " << myArray.count() << std::endl; // Should print 3

		// Test iteration using range-based for loop
		for (const int &item : myArray) {
			std::cout << item << " ";
		}
		std::cout << std::endl; // Should print: 10 20 30

		// Test reverse
		myArray.reverse();
		for (const int &item : myArray) {
			std::cout << item << " ";
		}
		std::cout << std::endl; // Should print: 30 20 10

		// Test iterators
		Array_Iterator<int> it = myArray.begin();
		std::cout << "First element: " << *it << std::endl; // Should print 30

		// Test increment
		++it;
		std::cout << "Second element: " << *it << std::endl; // Should print 20

		// Test random access
		it += 1;
		std::cout << "Third element: " << *it << std::endl; // Should print 10

		// Test comparison
		Array_Iterator<int> endIt = myArray.end() - 1;
		std::cout << "Are iterators equal? " << (it == endIt) << std::endl; // Should print 1 (true)

		// Test erase
		it = myArray.erase(it);
		std::cout << "Second element after erase: " << *it << std::endl; // Should print 10
		for (const int &item : myArray) {
			std::cout << item << " ";
		}
		std::cout << std::endl; // Should print: 30 20 10
		
		myArray.insert_at(0, 0x02);

		for (auto Iterator = myArray.begin(); Iterator != myArray.end(); ++Iterator) {
			std::cout << *Iterator << std::endl;
		}
	}
}
