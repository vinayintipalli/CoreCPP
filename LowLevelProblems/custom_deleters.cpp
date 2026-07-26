#include <iostream>
#include <bits/stdc++.h>
/**
Smart pointers should know how to delete when they go out of scope.
If memory allocated
from pool --> release back to pool
new --> delete
malloc --> free
FILE* fp --> fclose(fp)
Deleter is a simple struct with a function call operator.
**/

/*
Effect on sizeof(unique_ptr<T, Deleter>):
a stateless deleter (empty class/lambda with no captures) typically adds zero size overhead via empty base optimization, so sizeof stays pointer-sized;
a stateful deleter (holds data, e.g., a pool pointer) increases the size by however much state it stores.

Effect on shared_ptr<T> size: (unaffected) because the deleter is type-erased and stored inside the separately-allocated control block, not inline in the shared_ptr handle itself — the handle always stays two pointers (object pointer + control block pointer) regardless of deleter complexity.
*/

// Empty Base Optimization
template<typename T>
class Deleter {
public:                             // was private-by-default -> unique_ptr couldn't call it
    void operator()(T* ptr) {
        delete ptr;
    }
};

template<typename T>
class unique_ptr : private Deleter<T> {   // needed the <T> argument
public:
    Deleter<T>& get_deleter() { return *this; }
    ~unique_ptr() {
        if (ptr) get_deleter()(ptr);
    }
private:
    T* ptr;
};
// Above one: sizeof(unique_ptr<T>) = 8

// General one (sizeof(unique_ptr<T>) = 16 (8+1 with padding -> 16))
template<typename T, class Deleter = std::default_delete<T>>
class unique_ptr {
public:                             // ctor was private-by-default -> unusable
    unique_ptr(T* ptr, Deleter d) : ptr(ptr), d(std::move(d)) {}
    ~unique_ptr() {
        d(ptr);
    }
private:
    T* ptr;
    Deleter d;
};

// For unique_ptr we cannot write a factory that decides at runtime, but shared_ptr can solve that

struct Disposer {
    virtual void dispose(void* p) = 0;
    virtual ~Disposer() {}
};
// pure virtual function: when someone calls dispose through Disposer*, the actual func is decided at runtime based on the derived obj's implementation (vptr)

// concreteDis knows the proper deleter
template<typename T, class D = std::default_delete<T>>
class concreteDis : public Disposer {     // must be public: derived*->Disposer* conversion needs it
    D d;
public:                                    // ctor must be reachable from shared_ptr_c
    concreteDis(D d) : d(std::move(d)) {}
    void dispose(void* p) override { d(static_cast<T*>(p)); }
};

class shared_ptr_c {
    void* ptr;
    Disposer* d;
public:
    template<typename T, typename D>
    shared_ptr_c(T* data, D deleter) {
        ptr = static_cast<void*>(data);
        d = new concreteDis<T, D>(deleter);
    }
    ~shared_ptr_c() {
        if (ptr) d->dispose(ptr);
        delete d;                          // was leaking the Disposer/control-block object
    }
};
/*
If we observe closely, once the constructor returns the deleter is not known — it's kept in a heap location, hidden behind a vptr.
This is called type erasure.

Real shared_ptr does precisely this, except the disposer object is bundled together with the reference counts into a single heap object called the control block.
*/

// Note: unique_ptr decides everything at compile time whereas shared_ptr decides at runtime

/*
struct Base { ~Base() {} };                    // note: NOT virtual
struct Derived : Base { std::string s; };

std::shared_ptr<Base> p(new Derived);          // ~Derived runs. Correct.
std::unique_ptr<Base> q(new Derived);          // broken: ~Base runs, Derived's members leak (UB)

shared_ptr bakes its cleanup code once, at construction, into the control block. So construction is the only moment that needs the complete type; destruction just calls a stored function through a vptr and needs nothing. One requirement, and it's early — in the .cpp.

unique_ptr stores no cleanup code. It reconstructs cleanup from the type every time it destroys. So destruction is a demanding moment too — and destruction is the one the compiler loves to auto-generate in the header.
*/