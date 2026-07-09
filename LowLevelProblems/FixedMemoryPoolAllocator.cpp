/**1. Implement a Fixed-Size Memory Pool Allocator — pre-allocate a large chunk of memory and serve fixed-size object allocations in O(1) time. The allocator must avoid fragmentation and support deallocation. Discuss cache locality and alignment concerns.
 */

/**
Assumption is first lets try to target to allocate the memory in O(1)
Idea is too create the multiple pool of size 16KB, 32 KB, 64 kB, 128 KB, .... Based on request size, will provide the chunk to user and on deallocate will give it back to that pool. 
But as first step to limit the problem scope, targeting to create only 16KB pool, where big chunk is splitted into 16KB blocks and linked as linked list. On request, will provide the block and On deallocate, will give it back to pool and add to the tail

Pros:
I can ensure the alignment as the entire pool is aligned.

Cons:
Fragmentation issue is still exists as we can't ensure that adjacent blocks are always adjacent.
**/
#include <iostream>
#include <utility>
#include <memory>
#include <vector>

#define ALIGN_SIZE 16
#define ALIGNED(N) ((N + (ALIGN_SIZE -1)) & ~(ALIGN_SIZE-1))

struct FreeNode {
    FreeNode* next;
};

class FixedSizePool {
private:
    void* pool;
    size_t blockSize;
    size_t blockCount;
    FreeNode* freeList;

public:
    FixedSizePool(
        void* buffer,
        size_t totalSize,
        size_t blockSize)
        : pool(buffer),
          blockSize(ALIGNED(blockSize)),
          blockCount(totalSize / ALIGNED(blockSize)),
          freeList(nullptr)
    {
        auto* currentPtr = (char*)(pool);

        for (size_t i = 0;i < blockCount; ++i) {
            auto* node = (FreeNode*)(currentPtr);
            node->next = freeList;
            freeList = node;
            currentPtr += blockSize;
        }
    }

    void* allocate() {
        if (!freeList) {
            return nullptr;
        }

        FreeNode* node = freeList;
        freeList = freeList->next;

        return node;
    }

    void deallocate(void* ptr) {
        if (!ptr) {
            return;
        }

        auto* node = (FreeNode*)(ptr);
        node->next = freeList;
        freeList = node;
    }
};

int main() {
    alignas(16) static char arr[10000000];
    FixedSizePool fixedSizePool(&arr, sizeof(arr), 16000); // 16Kb pool
    std::vector<void*> allocatedPtrs;
    auto n = sizeof(arr) / ALIGNED(16000);
    while(n) { 
        auto ptr = fixedSizePool.allocate(); 
        allocatedPtrs.push_back(ptr);
        n--; 
    }
    auto ptr = fixedSizePool.allocate(); 
    if(ptr == nullptr) {
        std::cout << "Pool Exhausted\n";
    } 
    for (auto ptr2 : allocatedPtrs) { fixedSizePool.deallocate(ptr2); }
}