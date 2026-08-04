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
                new (curr++) T(*temp++);
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
            this->swap(*this, oth);
            return *this;
        }

        void push_back(const T& data) {
            if(count == capacity_t) {
                auto newCapacity_t = (capacity_t == 0) ?  1 : 2 * capacity_t;
                reallocate(newCapacity_t);
            }
            new (head+count) T(data);
            count++;
        }
        
        void push_back(T&& data) {
            if(count == capacity_t) {
                auto newCapacity_t = (capacity_t == 0) ?  1 : 2 * capacity_t;
                reallocate(newCapacity_t);
            }
            new (head+count) T(std::move(data));
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

        bool empty() const {return (count == 0);}

        T* data() {
            return head;
        }

        const T* data() const {
            return head;
        }

        T& front() {
            return head[0];
        }

        T& back() {
            return head[count-1];
        }

        const T& front() const {
            return head[0];
        }

        const T& back() const {
            return head[count-1];
        }


        T& operator[](size_t index) {
            return *(head + index);
        }

        const T& operator[] (size_t index) const {
            return *(head + index);
        }
        
        // An iterator is an object/type that provides a pointer-like interface for traversing elements in a range/container.
        using iterator = T*;
        iterator insert(iterator pos, const T& data) {
            auto index = pos - head;
            if(capacity_t <  count+1) {
                auto newCapacity_t = (capacity_t == 0) ?  1 : 2 * capacity_t;
                reallocate(newCapacity_t);
            }
            if(index == count) {
                new (head+count) T(data);
            } else {
                new (head+count) T(std::move(head[count-1]));
                auto currentIndex = count;
                while(index != --currentIndex) {
                    head[currentIndex] = std::move(head[currentIndex-1]);
                }
                head[currentIndex] = data;
            }
            count++;
            return head + index;
        }

        iterator erase(iterator pos) {
            auto index = pos-head;
            auto temp = index;
            if(index >= count) {
                return end();
            }
            while(index < count-1) {
                head[index] = std::move(head[index+1]);
                index++;
            }
            head[index].~T();
            count--;
            return head + temp;
        }

        template<typename... Args>
        T& emplace_back(Args&&... args) {
            if(count == capacity_t) {
                auto newCapacity_t = (capacity_t == 0) ?  1 : 2 * capacity_t;
                reallocate(newCapacity_t);
            }
            auto temp = new (head+count) T(std::forward<Args>(args)...);
            count++;
            return head[count-1];
        }

        iterator begin() const { return head; }
        iterator end() const { return head+count; }
        
        T& at(size_t index) {
            if(index >= count) { throw std::out_of_range("custom_vec.at()");}
            return head[index];
        }

        const T& at(size_t index) const {
            if(index >= count) { throw std::out_of_range("custom_vec.at()");}
            return head[index];
        }

        void clear() {
            T* curr = head + count;
            while (curr != head) (--curr)->~T();
            count = 0;
        }

        ~custom_vec() {
            cleanUp();
        }
    private:
        size_t capacity_t;
        size_t count;
        T* head;

        void reallocate(size_t newCapacity_t) {
            assert(newCapacity_t >= count);
            T* newHead = static_cast<T*>(::operator new(newCapacity_t*sizeof(T), std::align_val_t(alignof(T))));
            auto newCurr = newHead;
            auto temp = head;
            auto end = temp + this->count;
            while(temp != end) {
                new (newCurr++) T(std::move(*temp++));
            }
            cleanUp();
            head = newHead;
            count = newCurr - newHead;
            capacity_t = newCapacity_t;
        }

        void cleanUp() {
            T* curr = head + count;
            while (curr != head) (--curr)->~T();
            ::operator delete(head, std::align_val_t(alignof(T)));
            head = nullptr;
            count = 0;
            capacity_t = 0;
        }
};

#define CHECK(c) do{ if(!(c)){ std::cout<<"FAIL line "<<__LINE__<<": "#c<<"\n"; ok=false;} }while(0)
bool ok = true;
 
struct Counted {
    static int live;
    std::string s;
    Counted(std::string v=""):s(std::move(v)){++live;}
    Counted(const Counted& o):s(o.s){++live;}
    Counted(Counted&& o) noexcept :s(std::move(o.s)){++live;}
    Counted& operator=(const Counted&)=default;
    Counted& operator=(Counted&&) noexcept =default;
    ~Counted(){--live;}
};
int Counted::live = 0;
 
int main(){
    { // copy independence + assignment + self-assign
        custom_vec<std::string> a;
        for(int i=0;i<5;++i) a.push_back("s"+std::to_string(i));
        auto b = a; a.pop_back();
        CHECK(a.size()==4 && b.size()==5 && b[4]=="s4" && b[0]=="s0");
        custom_vec<std::string> c; c = b;
        CHECK(c.size()==5 && b[0]=="s0");
        b = b;  CHECK(b.size()==5 && b[0]=="s0");
        auto d = std::move(b);
        CHECK(d.size()==5 && b.size()==0);
    }
    { // insert at front / middle / end / empty
        custom_vec<std::string> v;
        v.push_back("A"); v.push_back("B"); v.push_back("C");
        auto it = v.insert(v.begin()+1,"X"); CHECK(*it=="X");
        v.insert(v.begin(),"F"); v.insert(v.end(),"Z");
        std::string got; for(auto& s:v) got+="["+s+"]";
        CHECK(got=="[F][A][X][B][C][Z]");
        custom_vec<std::string> e; e.insert(e.begin(),"solo"); CHECK(e[0]=="solo"&&e.size()==1);
    }
    { // erase returns next position
        custom_vec<std::string> v;
        for(const char* p:{"A","B","C","D"}) v.push_back(p);
        auto it=v.erase(v.begin()+1); CHECK(*it=="C");
        v.erase(v.end()-1); v.erase(v.begin());
        CHECK(v.size()==1 && v[0]=="C");
    }
    { // erase-while-iterating idiom
        custom_vec<int> v; for(int i=0;i<10;++i) v.push_back(i);
        for(auto it=v.begin(); it!=v.end(); )
            if(*it%2==0) it=v.erase(it); else ++it;
        CHECK(v.size()==5 && v[0]==1 && v[4]==9);
    }
    { // no leaks: every ctor paired with a dtor
        CHECK(Counted::live==0);
        { custom_vec<Counted> v;
          for(int i=0;i<20;++i) v.emplace_back("x"+std::to_string(i)); // forces reallocs
          v.insert(v.begin(),Counted("front"));
          v.erase(v.begin()+3);
          v.pop_back();
          auto c=v; c.clear();
          CHECK(c.size()==0 && c.capacity()>0); }
        CHECK(Counted::live==0);
    }
    { // const access
        custom_vec<int> v; v.push_back(7); v.push_back(8);
        const custom_vec<int>& r = v;
        CHECK(r[0]==7 && r.at(1)==8 && r.front()==7 && r.back()==8 && *r.begin()==7);
        CHECK(r.data()[1]==8);
    }
    { // move-only type
        custom_vec<std::unique_ptr<int>> v;
        for(int i=0;i<8;++i) v.emplace_back(new int(i));
        CHECK(*v[7]==7 && v.size()==8);
        v.push_back(std::make_unique<int>(99));
        CHECK(*v.back()==99);
    }
    { // works with std algorithms
        custom_vec<int> v; for(int i:{5,3,9,1}) v.push_back(i);
        std::sort(v.begin(),v.end());
        CHECK(v[0]==1&&v[3]==9);
        CHECK(std::find(v.begin(),v.end(),9)!=v.end());
    }
    { // reserve semantics
        custom_vec<std::string> v; for(int i=0;i<5;++i) v.push_back("q");
        size_t n=v.size(); v.reserve(2);
        CHECK(v.size()==n);                 // reserve must not change size
        v.reserve(64); CHECK(v.capacity()>=64 && v.size()==n);
    }
    { // at() throws
        custom_vec<int> v; v.push_back(1);
        bool threw=false;
        try{ v.at(5);}catch(const std::out_of_range&){threw=true;}
        CHECK(threw);
    }
    { custom_vec<std::string> e; }  // empty destructs cleanly
    { // matches std::vector on a random op sequence
        custom_vec<int> a; std::vector<int> b;
        for(int i=0;i<200;++i){
            int op=i%5;
            if(op<3){ a.push_back(i); b.push_back(i); }
            else if(op==3 && !b.empty()){ size_t p=i%b.size();
                a.insert(a.begin()+p,i); b.insert(b.begin()+p,i); }
            else if(!b.empty()){ size_t p=i%b.size();
                a.erase(a.begin()+p); b.erase(b.begin()+p); }
        }
        CHECK(a.size()==b.size());
        CHECK(std::equal(a.begin(),a.end(),b.begin()));
    }
    std::cout << (ok ? "ALL TESTS PASSED\n" : "SOME TESTS FAILED\n");
    return ok?0:1;
}