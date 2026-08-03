// Implement custome vector

/*
Discussion:
custom_vec supports 
push_back(), pop_back(), insert, erase, operator[], size(), empty(), capacity_t(), at(), data(), back(), front()
emplace_back(), reserve

Contiguous memory block
*/
#include <bits/stdc++.h>

template<typename T>
class custom_vec {
    public:
        custom_vec() : capacity_t(0), count(0), head(nullptr) {}
        custom_vec(size_t capacity_t) : capacity_t(capacity_t), count(0) {
            head = static_cast<T*>(::operator new(capacity_t*sizeof(T), std::align_val_t(alignof(T))));
        }

        custom_vec(const custom_vec& oth) {
            head = static_cast<T*>(::operator new(oth.capacity_t*sizeof(T), std::align_val_t(alignof(T))));
            auto curr = head;
            this->capacity_t = oth.capacity_t;
            this->count = oth.size();
            auto temp = oth.head;
            auto end = temp + oth.size();
            while(temp != end) {
                new (curr++) T(std::move(*temp++));
            }
        }

        custom_vec(custom_vec&& oth) noexcept {
            this->count = oth.count;
            this->head = oth.head;
            this->capacity_t = oth.capacity_t;
            oth.head = nullptr;
            oth.count = 0;
            oth.capacity_t = 0;
        }

        void swap(custom_vec& c1, custom_vec& c2) {
            using std::swap;
            swap(c1.capacity_t, c2.capacity_t);
            swap(c1.head, c2.head);
            swap(c1.count, c2.count);
        }
        
        custom_vec& operator=(custom_vec oth) {
            swap(*this, oth);
            return *this;
        }

        void reallocate(size_t newCapacity_t) {
            assert(newCapacity_t >= count);
            T* newHead = static_cast<T*>(::operator new(newCapacity_t*sizeof(T), std::align_val_t(alignof(T))));
            auto newCurr = newHead;
            auto temp = head;
            auto end = temp + this->count;
            while(temp != end) {
                new (newCurr++) T(*temp++);
            }
            cleanUp();
            head = newHead;
            count = newCurr - newHead;
            capacity_t = newCapacity_t;
        }

        void push_back(const T& data) {
            if(count == capacity_t) {
                auto newCapacity_t = (capacity_t == 0) ?  1 : 2 * capacity_t;
                reallocate(newCapacity_t);
            }
            new (head+count) T(data);
            count++;
        }
        
        void pop_back() {
            auto temp = head + count - 1;
            temp->~T();
            count--;
        }
        void reserve(size_t cap) {
            if (cap > capacity_t) reallocate(cap);
        }

        size_t size() const { return count; }

        size_t capacity() const { return capacity_t; }

        T& operator[](size_t index) {
            return *(head + index);
        }
        
        void cleanUp() {
            T* curr = head + count;
            while (curr != head) (--curr)->~T();
            ::operator delete(head, std::align_val_t(alignof(T)));
            head = nullptr;
            count = 0;
            capacity_t = 0;
        }
        
        // An iterator is an object/type that provides a pointer-like interface for traversing elements in a range/container.
        using iterator = T*;
        void insert(iterator pos, const T& data) {
            auto index = pos - head;
            if(capacity <  count+1) {
                auto newCapacity_t = (capacity_t == 0) ?  1 : 2 * capacity_t;
                reallocate(newCapacity_t);
            }
            if(index == count) {
                new (head+count) T(data);
            } else {
                new (head+count) T(head[count-1]);
                auto currentIndex = count;
                while(index != --currentIndex) {
                    head[currentIndex] = std::move(head[currentIndex-1]);
                }
                head[currentIndex] = data;
            }
            count++;
            return head + index;
        }

        iterator begin() const { return head; }
        iterator end() const { return head+count; }
        
        ~custom_vec() {
            cleanUp();
        }
    private:
        size_t capacity_t;
        size_t count;
        T* head;
};
int main() {
    
}