#pragma once



namespace IceEngine {

	// Forward declaration of Array class
	template <typename T>
	struct Array;

	template<typename T>
	struct Array_Iterator {
		T operator *() const;
		const Array_Iterator &operator++();

		bool operator!=(const Array_Iterator &other) const;

		// If you want to support bidirectional iterators:
		Array_Iterator &operator--();

		// If you want to support random access iterators:
		Array_Iterator &operator+=(int n);
		Array_Iterator operator+(int n) const;

		Array_Iterator &operator-=(int n);
		Array_Iterator operator-(int n) const;

		int operator-(const Array_Iterator &other) const;

		T &operator[](int n) const;

		// If you want to support comparison between iterators:
		bool operator==(const Array_Iterator &other) const;

		// If you want to support dereferencing with the arrow operator:
		T *operator->() const;

		const Array<T> *array = nullptr;
		int index = 0;

		// Constructor
		Array_Iterator(const Array<T> *arr, int i) : array(arr), index(i) {}
	};


	template <typename T>
	struct Array {
		Array(int start_size = 0);
		~Array();

		int items = 0;
		int allocated_items = 0;

		T *data = nullptr;

		void add(T item);
		void add_unique(T item);
		void insert_at(int index, T item);
		int count() const;
		void reset();
		bool remove(T item);
		bool find(T item, int *index = nullptr) const;
		T remove_nth(int index);
		void remove_nth_elemente_while_keeping_order(int n);
		void reserve(int size);
		void resize(int size);


		T pop();
		T peek();

		void reverse();

		// Sorting
		void sort();

		// Access and Information
		T front() const;
		T back() const;
		bool empty() const;
		int capacity() const;

		// Element Modification
		void fill(const T &value);
		void swap(Array &other);

		// Array Concatenation
		void concatenate(const Array &other);

		// Element Manipulation
		void unique();
		void unique_unsorted();
		void shuffle();

		// Printing
		void print() const;


		const T &operator [](const int i) const;
		T &operator [](const int i);

		//
		// C++11 stuff that lets us iterate:
		//

	

		Array_Iterator<T> begin();
		Array_Iterator<T> end();
		Array_Iterator<T> rbegin();
		Array_Iterator<T> rend();


		const Array_Iterator <T> begin() const;
		const Array_Iterator <T> end() const;
		const Array_Iterator <T> rbegin() const;
		const Array_Iterator <T> rend() const;

		// Iterators
		Array_Iterator<T> erase(Array_Iterator<T> pos);
		Array_Iterator<T> erase(Array_Iterator<T> first, Array_Iterator<T> last);
	};


	void TestArray();

}