#include <iostream>
#include<vector>
#include <bits/stdc++.h>

template<typename T>
class ObjectPool {
    public:
        template<typename... Args>
        ObjectPool(unsigned int numOfObjects, const Args&... args) {
            ptr = ::operator new(numOfObjects* sizeof(T), std::align_val_t(alignof(T)));
            unsigned int i = 0;;
            numOfObjs = numOfObjects;
        try
        {
            for (; i < numOfObjects; i++) {
                auto memptr = static_cast<void*>(reinterpret_cast<char*>(ptr) + i *sizeof(T));
                pool.push_back(new (memptr) T(args...));
            }
        }
        catch(...) {
            for (int j =0;j<i; j++) {
                reinterpret_cast<T*>(reinterpret_cast<char*>(ptr) + j *sizeof(T))->~T();
            }
            ::operator delete(ptr, std::align_val_t(alignof(T)));
            throw;
        }
        }

        T* acquire() {
            if(pool.empty()) {
                throw std::bad_alloc();
            } else {
                auto ptr = pool.back();
                pool.pop_back();
                return ptr;
            }
        }

        void release(T* ptr) {
            if(!ptr) {return;}
            pool.push_back(ptr);
        }
        
        ObjectPool(const ObjectPool&)            = delete;   // owns raw memory: no copying
        ObjectPool& operator=(const ObjectPool&) = delete;
        ObjectPool(ObjectPool&&)                 = delete;   // (implementable, omitted for simplicity)
        ObjectPool& operator=(ObjectPool&&)      = delete;


        ~ObjectPool() {
            for (int j =0;j<numOfObjs; j++) {
                reinterpret_cast<T*>(reinterpret_cast<char*>(ptr) + j *sizeof(T))->~T();
            }
            ::operator delete(ptr, std::align_val_t(alignof(T)));
            ptr = nullptr;
        }

    private:
        std::vector<T*> pool;
        void* ptr;
        unsigned int numOfObjs;
};

class ArenaAllocator {
    public:
        ArenaAllocator(size_t size) {
            ptr = new char[size];
            offset = 0;
            totalSize = size;
        }

        void* acquire(size_t objSize, unsigned int objAlignSize = alignof(std::max_align_t)) {
            auto alignedOffset = (offset + objAlignSize - 1) & ~(objAlignSize-1); // aligned the offset with objAlignSize (for start addr) 
            // New type of objects creation from areans will lead to waste of memory due to alignment
            if(alignedOffset+ objSize > totalSize) {return nullptr;}
            void* buff = static_cast<void*>(ptr + alignedOffset);
            offset = alignedOffset + objSize;
            return buff;
        }

        void reset() {
            offset = 0;  // invalidates all allocations. Any required destructors must be called by the user before reset().
        }
    private:
        char* ptr;
        unsigned int offset;
        unsigned int totalSize;
};

class Object {
public:
    Object(int a, int b) : a(a), b(b) {}
    ~Object() {}
private:
    int a;
    int b;
};

int main() {
    ObjectPool<int> objPool = ObjectPool<int>(5);
    ObjectPool<Object> objPool1 = ObjectPool<Object>(5, 1, 2);
}