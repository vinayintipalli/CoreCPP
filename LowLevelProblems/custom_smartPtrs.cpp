#include <iostream>
#include <utility>
// unique_ptr

template<typename T>
class custom_unique_ptr {
    public:
        custom_unique_ptr(): dataPtr(nullptr) {}   // default
        explicit custom_unique_ptr(T* ptr): dataPtr(ptr) {} // parameterized

        custom_unique_ptr(custom_unique_ptr&& ptr) noexcept {  // move constructor
            dataPtr = ptr->dataPtr;
            ptr->dataPtr = nullptr;
        }
        custom_unique_ptr(const custom_unique_ptr&)=delete;   // deleted copy constructor

        custom_unique_ptr& operator=(custom_unique_ptr&& ptr) noexcept { // move assignment operator
            if(this != &ptr) {
                delete dataPtr;  // first delete otherwise memory leak
                this->dataPtr = ptr->dataPtr;
                ptr->dataPtr = nullptr;
            }
            return *this;
        }
        custom_unique_ptr& operator=(const custom_unique_ptr&)=delete; // delete copy assignment operator

        T& operator*() const noexcept {
            return *dataPtr;
        } 
        
        T* operator->() const noexcept {
            return dataPtr;
        }

        explicit operator bool() const noexcept {  // conversion operators signature: operator <type>() 
            return dataPtr != nullptr;
        }

        T* getPtr() const noexcept {
            return dataPtr;
        }

        T* release() noexcept {    // provide the raw ptr and set the datPtr to null
            auto temp = dataPtr;
            dataPtr = nullptr;
            return temp;
        }

        void reset(T* ptr =nullptr) noexcept {
            if(this->dataPtr != ptr) {
                delete dataPtr;
                dataPtr = ptr;
            }
        }

        void swap(custom_unique_ptr& ptr) noexcept {
            std::swap(dataPtr, ptr->dataPtr);
        }

        ~custom_unique_ptr() noexcept {
            delete dataPtr;
        }

    private:
        T* dataPtr;
};

template<typename T, typename... Args>
custom_unique_ptr<T> make_unique(Args&&... args) {
    return custom_unique_ptr<T>(new T(std::forward<Args>(args)...));  // in modern c++, RVO triggers direct object is constructed once in constructor. (no move constructor triggered)
}
// new T(std::forward<Args>(args)...) -> creating the T object, by forwading all the arguments 
#include <atomic>

template<typename T>
struct controlBlock {
    template<typename... Args>
    explicit controlBlock(Args&&... args)
        : strongCount(1), weakCount(0),
          data(new T(std::forward<Args>(args)...))
    {}

    ~controlBlock() {}

    controlBlock(const controlBlock&) = delete;
    controlBlock& operator=(const controlBlock&) = delete;

    std::atomic<unsigned int> strongCount;
    std::atomic<unsigned int> weakCount;
    T* data;
};

template<typename T>
class custom_shared_ptr {
    public:
        custom_shared_ptr():ctrlBlock(nullptr){}
        explicit custom_shared_ptr(controlBlock<T>* ptr): ctrlBlock(ptr) noexcept {}
        custom_shared_ptr(const custom_shared_ptr& ptr) noexcept {
            this->ctrlBlock = ptr.ctrlBlock;
            if(this->ctrlBlock) {
                this->ctrlBlock->strongCount++;
            }
        }

        template<typename>
        friend class custom_weak_ptr;

        custom_shared_ptr(custom_shared_ptr&& ptr) noexcept {
            this->ctrlBlock = ptr.ctrlBlock;
            ptr.ctrlBlock = nullptr;
        }

        custom_shared_ptr& operator=(const custom_shared_ptr& other) noexcept {
            if(this != &other) {
                release();
                if (other.ctrlBlock) {
                    other.ctrlBlock->strongCount++;
                }
                this->ctrlBlock = other.ctrlBlock;
            }
            return *this;
        }

        custom_shared_ptr& operator=(custom_shared_ptr&& other) noexcept {
            if(this != &other) {
                release();
                this->ctrlBlock = other.ctrlBlock;
                other.ctrlBlock = nullptr;
            }
            return *this;
        }

        explicit operator bool() const {
            return (this->ctrlBlock) ? (this->ctrlBlock->data != nullptr) : false;
        }   

        T* getPtr() const {
            return (this->ctrlBlock) ? ctrlBlock->data : nullptr;
        }

        T& operator*() const {
            return *ctrlBlock->data;
        }

        T* operator->() const {
            return ctrlBlock->data;
        }

        size_t use_count() const {
            return ctrlBlock ? ctrlBlock->strongCount.load() : 0;
        }

        void reset() {
            release();
        }

        ~custom_shared_ptr() noexcept {
            release();
        }

    private:
        controlBlock<T>* ctrlBlock;

        void release() {
            if(!ctrlBlock) {
                return;
            }
            if(--ctrlBlock->strongCount == 0) {
                delete ctrlBlock->data;
                ctrlBlock->data = nullptr;
            }

            if(ctrlBlock->strongCount == 0 && ctrlBlock->weakCount == 0) {
                delete ctrlBlock;
                ctrlBlock = nullptr;
            }

            ctrlBlock = nullptr;
        }
};

template<typename T, typename... Args>
custom_shared_ptr<T> make_shared(Args&&...args) {
    return custom_shared_ptr<T>(new controlBlock<T>(std::forward<Args>(args...)));
};


template<typename T>
class custom_weak_ptr {
    public:
        custom_weak_ptr():ctrlBlock(nullptr){}
        custom_weak_ptr(const custom_shared_ptr<T>& cust_shared_ptr) {
            this->ctrlBlock = cust_shared_ptr.ctrlBlock;
            if(this->ctrlBlock) {this->ctrlBlock->weakCount++;}
        }

        custom_weak_ptr(custom_weak_ptr<T>&& ptr) {
            this->ctrlBlock = ptr.ctrlBlock;
            ptr.ctrlBlock = nullptr;
        }

        custom_weak_ptr(const custom_weak_ptr<T>& ptr) {
            this->ctrlBlock = ptr.ctrlBlock;
            if(this->ctrlBlock) {
                this->ctrlBlock->weakCount++;
            }
        }

        custom_weak_ptr<T>& operator=(const custom_weak_ptr<T>& ptr) {
            if (this != &ptr) { 
                release();
                this->ctrlBlock = ptr.ctrlBlock;
                if (this->ctrlBlock) {
                    this->ctrlBlock->weakCount++;
                }
            }
            return *this;
        }

        custom_weak_ptr<T>& operator=(custom_weak_ptr<T>&& ptr) {
            if (this != &ptr) {
                release();
                this->ctrlBlock = ptr.ctrlBlock;
                ptr.ctrlBlock = nullptr;
            }
            return *this;
        }

        custom_shared_ptr<T> lock() {
            if(this->ctrlBlock && this->ctrlBlock->strongCount > 0) {
                this->ctrlBlock->strongCount++;
                return custom_shared_ptr<T>(this->ctrlBlock);
            }

            return custom_shared_ptr<T>();
        }

        bool expired() const {
            return (!ctrlBlock || this->ctrlBlock->strongCount == 0);
        }
 
        ~custom_weak_ptr() {
            release();
        }
    private:
        void release() {
            if(this->ctrlBlock && --this->ctrlBlock->weakCount == 0 && ctrlBlock->strongCount == 0) {
                delete ctrlBlock;
                ctrlBlock = nullptr;
            }
            ctrlBlock = nullptr;
        }
        controlBlock<T>* ctrlBlock;
};