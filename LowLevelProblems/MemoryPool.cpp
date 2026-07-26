/*
Try implementing a fixed-size buffer that supports construct-in-place and explicit-destroy operations using
placement new, and confirm with a class that has non-trivial constructor/destructor side effects (e.g., printing)
that lifetime is managed correctly.
*/
#include <bits/stdc++.h>
#include <stdlib.h>
template<typename T>
class FixedMemoryPool {
    public:
        typedef struct Node {
            Node* next;
        } Node;
        FixedMemoryPool(unsigned int numOfObjects) {
            int slotSize = std::max(sizeof(Node), sizeof(T));   // possible maximum size of the slot
            int slotAlign = std::max(alignof(Node), alignof(T)); // memory address should be divisible of this number
            int alignedSlotSize = (slotSize + slotAlign - 1) & ~(slotAlign-1);   // (x << a) >> a  --> x & ~(a-1)

            pool = ::operator new(alignedSlotSize* numOfObjects, std::align_val_t(slotAlign));  // args are each total size and start address alignment
            freeList = nullptr;
            Node* temp = static_cast<Node*>(pool);
            for(int i = 1; i <= numOfObjects; i++) {
                std::cout << "Start Addr of each Obj " << temp << " \n";
                temp->next = static_cast<Node*>(freeList);
                freeList = static_cast<void*>(temp);
                temp = reinterpret_cast<Node*>(reinterpret_cast<char*>(temp) + alignedSlotSize);
            }
            std::cout << "Created Pool of size " << numOfObjects << "\n";
        }

        void* GetFromPool() {
            if(freeList == nullptr) { 
                std::cout << "Pool Exhausted \n";
                throw std::bad_alloc();
            }  // pool exhausted
            std::cout << "FreeList addr " << freeList << "\n";
            auto temp = freeList;
            freeList = static_cast<void*>((static_cast<Node*>(freeList))->next);
            return temp;
        }

        void ReturnToPool(void* ptr) {
            if(ptr == nullptr) {return; }
            Node* node = static_cast<Node*>(ptr);
            node->next = static_cast<Node*>(freeList);
            freeList = static_cast<void*>(node);
        }

        ~FixedMemoryPool() {
            if(pool) {
                std::free(pool);
                pool = nullptr;
                freeList = nullptr;
            }
        }

    private:
        void* freeList;
        void* pool;
};

class Object {
    public:
        Object(int input):data(input) {
            std::cout << "Object Constructed \n";
        }
        ~Object() {
            std::cout << "Object Destructed \n";
        }
    private:
        int data;
};

template<typename T>
class PoolCustomDeleter {
    public:
    explicit PoolCustomDeleter(FixedMemoryPool<T>* p) : pool(p) {};
    void operator()(T* ptr) {
        if(ptr) {
            ptr->~T();   // destruct the object
            pool->ReturnToPool(ptr); // return to pool;
        }
    }

    private:
        FixedMemoryPool<T>* pool = nullptr;
};

template<typename T>
std::unique_ptr<T, PoolCustomDeleter<T>> createObject(FixedMemoryPool<T>& pool, int someInput) {
    T* ptr = new (pool.GetFromPool()) T(someInput);  // we can add try catch, to give back to pool, if ibj creation throws
    std::cout << "Object got pool add " << ptr << "\n";
    return std::unique_ptr<T, PoolCustomDeleter<T>>(ptr, PoolCustomDeleter<T>(&pool));
}

int main() {
    FixedMemoryPool<Object> fixedMemPool(3);
    auto obj1 = createObject<Object>(fixedMemPool, 1);
    auto obj2 = createObject<Object>(fixedMemPool, 2);
    auto obj3 = createObject<Object>(fixedMemPool, 3);
    // auto obj4 = createObject<Object>(fixedMemPool, 4);
}