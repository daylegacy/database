#ifndef VECTOR_H
#define VECTOR_H

#define VEC_REALLOC 10
#define BIG_ENOUGH 200
#define MULTIPLIER 1.5
#include <cstdlib>

template <typename T>
struct vector {
private:
	T * ptr = nullptr;
	int size = 0;
	int capacity = 0;
public:
	vector() {}
	vector(int sz, int cp)
		:ptr(new T[cp]), size(sz), capacity(cp)
	{
		memset(ptr, 0, capacity * sizeof(T));
	}
	vector(const vector<T> & other)
		:vector(other.size, other.capacity)
	{
		//printf("COPY cons\n");
		//printf("oth.size =%d cp = %d\n", other.size, other.capacity);
		memcpy(ptr, other.ptr, size * sizeof(T));
	}
	vector(vector<T> && other)
		:ptr(other.ptr), size(other.size), capacity(other.capacity)
	{
		//printD("MOVE cons\n");
		other.ptr = nullptr;
		other.size = 0;
		other.capacity = 0;
	}
	void push_back(T a) {
		//printD("PUsh back>\n");
		if (size >= capacity) {
			reallocate(size+1);
		}
		ptr[size++] = a;
		//printD("PUsh back<\n");
	}
	void pop() {
		if (size) size--;
	}
	void reallocate(int new_size) {
		int delta = is_big_enough() ? (int)(capacity * MULTIPLIER) : VEC_REALLOC;
		delta = new_size-capacity > delta ? new_size - capacity : delta;
		int new_capacity = capacity + delta;

		auto new_ptr = new T[new_capacity];
		memset(new_ptr, 0, new_capacity * sizeof(T));
		memcpy(new_ptr, ptr, capacity * sizeof(T));
		delete[] ptr;

		//ptr = (T *)realloc(ptr, new_capacity*sizeof(T));
		
		capacity = new_capacity;
		ptr = new_ptr;
	}
	T& operator[](int i) const {
		return *(ptr + i);
	}
	operator bool() const {
		return ptr;
	}
	operator T*() const {
		return ptr;
	}

	vector<T> & operator=(const vector<T> & other) {
		if (&other != this) {
			delete[] ptr;
			capacity = other.capacity;
			size = other.size;
			ptr = new T[capacity];
			memcpy(ptr, other.ptr, size * sizeof(T));
		}
		return *this;
	}
	vector<T> & operator=(const vector<T> && other) {
		if (&other != this) {
			ptr = other.ptr;
			other.ptr = nullptr;
			size = other.size;
			other.size = 0;
			capacity = other.capacity;
			other.capacity = 0;
		}
		return *this;
	}
	int is_big_enough() const {
		return capacity > BIG_ENOUGH ? 1 : 0;
	}
	int getsize() const {
		return size;
	}
	int getcapacity()const {
		return capacity;
	}

	void setsize(int size) {
		this->size = size;
	}
	T* gp() const {
		return ptr;
	}
	void release() { 
		//printf("release %d, %d\n", size, capacity);
		delete[] ptr; size = capacity = 0; 
		ptr = nullptr;
	}
	~vector() {
		printD("its size and capacity were %d, %d\n", size, capacity);
		delete[] ptr;
	}

};

#endif