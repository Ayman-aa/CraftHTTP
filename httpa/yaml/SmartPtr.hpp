/* -- SmartPtr.hpp --*/

#pragma once

#include <cstdlib> /* only for NULL HaHa */

template <typename T>
class SmartPtr {
	private:
		T* ptr;
		int* refCount;
	public:
		/* Constructor */
		SmartPtr(T* p = NULL) {
			ptr = p;
			refCount = new int(1);
		}
		/* Copy constructor */
		SmartPtr(const SmartPtr& sp) {
			ptr = sp.ptr;
			refCount = sp.refCount;
			(*refCount)++;
		}
		/* Destructor */
		~SmartPtr() {
			(*refCount)--;
			if (*refCount == 0) {
				delete ptr;
				delete refCount;
			}
		}

		T& operator*() { return *ptr; }
		T* operator->(){ return  ptr; }
};
