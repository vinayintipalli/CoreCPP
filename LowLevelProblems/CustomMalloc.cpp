/**1. Implement a Fixed-Size Memory Pool Allocator — pre-allocate a large chunk of memory and serve fixed-size object allocations in O(n) time. The allocator must avoid fragmentation and support deallocation. Discuss cache locality and alignment concerns.
 */

/**
Assumption is first lets try to target to allocate the memory and deallocate memory using static memory
Considering the linkedlist as a big memory chunk as it grows, split the block and allocate. While deallocating merge based on free blocks
**/
#include <iostream>
#include <utility>
#include <memory>
#include <stdexcept>
#include <cstdint>
#include <initializer_list>

#define ALIGN_SIZE 16
#define ALIGNED(N) ((N + (ALIGN_SIZE -1)) & ~(ALIGN_SIZE-1))

uintptr_t poolStart, poolEnd;

typedef struct alignas(ALIGN_SIZE) Node {
    Node* next;
    Node* prev;
    size_t size;
    bool isFree;
} Node;

Node* freeList = nullptr;
Node* head = nullptr;

#define META_HEADER_SIZE sizeof(Node)

void* allocate(size_t n) {
    if(n == 0) {
        return nullptr;
    }
    if(head == nullptr) {
        return nullptr; // pool not initialized
    }
    if(n > (poolEnd - poolStart)) {
        return nullptr; // can never fit; also avoids ALIGNED() overflow for huge n
    }
    if(freeList == nullptr) {
        freeList = head; // last search hint was fully consumed; fall back to a full scan from head
    }
    auto* endPtr = freeList;
    auto temp = freeList;
    do {
        if(temp->isFree && temp->size >= (ALIGNED(n))) { // Verify that current block have enough usable memorsize
            if(temp->size > ((ALIGNED(n) + META_HEADER_SIZE + ALIGN_SIZE))) { // deciding whether to split or assign entire block
                Node* newBlockPtr = (Node*)((char*)temp +  (META_HEADER_SIZE + ALIGNED(n)));
                newBlockPtr->prev = temp;
                newBlockPtr->next = temp->next;
                if(temp->next) {
                    temp->next->prev = newBlockPtr;
                }
                temp->next = newBlockPtr;
                newBlockPtr->isFree = true;
                temp->isFree = false;
                newBlockPtr->size = (temp->size - (ALIGNED(n) + META_HEADER_SIZE));
                temp->size = ALIGNED(n);
                freeList = newBlockPtr;
                return (void*)((char*)(temp) + META_HEADER_SIZE);
            } else { // no split, not enough size to split
                temp->isFree = false;
                if(freeList == temp) {// checking that free list still points to current temp
                    freeList = temp->next;
                } // else case don't do, because there might be small size block exist at freeList start
                return (void*)((char*)(temp) + META_HEADER_SIZE);
            }
        }
        temp = temp->next ? temp->next : head;
    } while(temp && temp != endPtr);
    return nullptr;
}

void deallocate(void* ptr) {
    if(ptr == nullptr) {
        return;
    }

    auto blockPtr = (Node*)((char*)(ptr)-META_HEADER_SIZE);

    if ((uintptr_t)blockPtr < poolStart || (uintptr_t)blockPtr >= poolEnd) {
        throw std::invalid_argument("Pointer not from pool");
    }

    if (blockPtr->isFree) {
        throw std::runtime_error("Double free detected");
    }
    auto prevPtr = blockPtr->prev;
    auto finalBlockPtr = blockPtr;
    if(prevPtr && prevPtr->isFree) {
        auto nextPtr = blockPtr->next;
        if(nextPtr) {
            nextPtr->prev = prevPtr;
        }
        prevPtr->next = nextPtr;
        prevPtr->size += (blockPtr->size + META_HEADER_SIZE);
        finalBlockPtr = prevPtr;
    }
    auto nextPtr = finalBlockPtr->next;
    if(nextPtr && nextPtr->isFree) {
        auto nextNextPtr = nextPtr->next;
        if(nextNextPtr) {
            nextNextPtr->prev = finalBlockPtr;
        }
        finalBlockPtr->next = nextPtr->next;
        finalBlockPtr->size += (nextPtr->size+ META_HEADER_SIZE);
    }

    if (freeList == blockPtr || freeList == prevPtr || freeList == nextPtr) {
        freeList = finalBlockPtr;
    }
    finalBlockPtr->isFree = true;
    return;
}


void initPool(void* buffer, size_t size) {
    poolStart = (uintptr_t)buffer;
    poolEnd = poolStart + size;
    head = (Node*)buffer;
    head->isFree = true;
    head->next = nullptr;
    head->prev = nullptr;
    head->size = size - META_HEADER_SIZE;
    freeList = head;
}

int main() {
    alignas(META_HEADER_SIZE) static char arr[10000000]; // 10 MB
    initPool(arr, sizeof(arr));
    std::cout << "Pool Memory address start " << poolStart << " and End Addr " << poolEnd << std::endl;

    auto ptr = allocate(100);
    auto metaPtr = (void*)((char*)ptr-META_HEADER_SIZE);
    std::cout << " Head Size " << head->size << std::endl;
    std::cout << "Allocated Ptr Addr " << ptr << ", head Ptr Addr " << head << ", FreeListPtr Addr  " << freeList << ", MetaSection Ptr Addr " << metaPtr << std::endl; 

    deallocate(ptr);
    std::cout << " Head Size " << head->size << std::endl;

    auto ptr1 = allocate(10000000);
    if(ptr1 == nullptr) {
        std::cout << "Not able to allocate" << std::endl;
    }

    auto ptr2 = allocate(500);
    std::cout << "Allocated Ptr Addr " << ptr2 << " Expected Ptr is " << (void*)((char*)head+ALIGNED(500)+2*META_HEADER_SIZE) <<  std::endl;
    auto ptr3 = allocate(400);
    auto ptr4 = allocate(1200);
    auto ptr5 = allocate(100);

    deallocate(ptr3);
    deallocate(ptr4);

    auto temp = head;
    std::cout << "Test 1 \n" ;
    while (temp != nullptr) {
        std::cout << "Block details isFree " << temp->isFree << " Size " << temp->size << std::endl;
        temp = temp->next;
    }

    deallocate(ptr2);
    std::cout << "Test 2 \n" ;
    temp = head;
    while (temp != nullptr) {
        std::cout << "Block details isFree " << temp->isFree << " Size " << temp->size << std::endl;
        temp = temp->next;
    }

    deallocate(ptr5);
    std::cout << "Test 3 \n" ;
    temp = head;
    while (temp != nullptr) {
        std::cout << "Block details isFree " << temp->isFree << " Size " << temp->size << std::endl;
        temp = temp->next;
    }
}
