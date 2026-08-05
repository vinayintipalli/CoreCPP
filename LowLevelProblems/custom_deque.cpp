#include <bits/stdc++.h>

/*
custom deque should support push_back, push_front, back, front, insert, erase, [], pop_front, pop_back
*/

template<typename T>
struct Node {
    T data;
    Node* prev;
    Node* next;

    Node(T&& dat, Node* prevPtr, Node* nextPtr) : data(std::move(dat)), prev(prevPtr), next(nextPtr) {}
    Node(const T& dat, Node* prevPtr, Node* nextPtr) : data(dat), prev(prevPtr), next(nextPtr) {}
};

template<typename T>
class custom_deque {
    public:
        custom_deque() : head(nullptr), tail(nullptr) {}
        custom_deque(const custom_deque& oth) {
            auto othHead = oth.head;
            auto othTail = oth.tail;
            if(othHead == nullptr) {
                this->head = nullptr;
                this->tail = nullptr;
                return;
            }
            struct Node<T>* currNode = nullptr;
            auto newHead = new Node<T>(othHead->data, nullptr, nullptr);
            auto prevNode = newHead;
            othHead = othHead->next;
            while(othHead != nullptr) {
                currNode = new Node<T>(othHead->data, prevNode, nullptr);
                if(prevNode) {
                    prevNode->next = currNode;
                }
                prevNode = currNode;
                othHead = othHead->next;
            }
            this->tail = prevNode;
            this->head = newHead;
        }

        custom_deque(custom_deque&& oth) noexcept {
            if (this != &oth) {
                this->head = oth.head;
                this->tail = oth.tail;
                oth.head = nullptr;
                oth.tail = nullptr;
            }
        }
        
        custom_deque& operator=(const custom_deque& oth) {
            if(this != &oth) {
                destroy_deque();
                auto othHead = oth.head;
                auto othTail = oth.tail;
                if(othHead == nullptr) {
                    return *this;
                }
                struct Node<T>* currNode = nullptr;
                auto newHead = new Node<T>(othHead->data, nullptr, nullptr);
                auto prevNode = newHead;
                othHead = othHead->next;
                while(othHead != nullptr) {
                    currNode = new Node<T>(othHead->data, prevNode, nullptr);
                    if(prevNode) {
                        prevNode->next = currNode;
                    }
                    prevNode = currNode;
                    othHead = othHead->next;
                }
                this->tail = prevNode;
                this->head = newHead;
            }
            return *this;
        }

        custom_deque& operator=(custom_deque&& oth) noexcept {
            if (this != &oth) {
                destroy_deque();
                this->head = oth.head;
                this->tail = oth.tail;
                oth.head = nullptr;
                oth.tail = nullptr;
            }
            return *this;
        }

        ~custom_deque() {
            destroy_deque();
        }

        void push_back(const T& data) {
            auto node = new Node<T>(data, tail, nullptr);
            if(tail) tail->next  = node;
            this->tail = node;
            if(head == nullptr) {this->head  = tail; }
        }

        void push_back(T&& data) {
            auto node = new Node<T>(std::move(data), tail, nullptr);
            if (tail) tail->next  = node;
            this->tail = node;
            if(head == nullptr) {this->head  = tail; }
        }

        void push_front(const T& data) {
            auto node = new Node<T>(data, nullptr, head);
            if(head) head->prev = node;
            this->head = node;
            if(tail == nullptr) {
                this->tail = head;
            }
        }

        void push_front(T&& data) {
            auto node = new Node<T>(std::move(data), nullptr, head);
            if(head) head->prev = node;
            this->head = node;
            if(tail == nullptr) {
                this->tail = head;
            }
        }

        void pop_back() { 
            struct Node<T>* prev = nullptr;
            // std::cout << "Popb Tail Addr " << tail << " And Head addr " << head << "\n";
            if(tail) {
                prev = tail->prev;
                if(head == tail) {
                    head = nullptr;
                }
                delete this->tail;
                if(prev) prev->next = nullptr;
                this->tail = prev;
            // std::cout << " End Popb Tail Addr " << tail << " And Head addr " << head << "\n";
            }
        }

        void pop_front() {
            struct Node<T>* next = nullptr;
            // std::cout << "Popf Tail Addr " << tail << " And Head addr " << head << "\n";
            if(head) {
                next = head->next;
                if(head == tail) {
                    tail = nullptr;
                }
                delete head;
                if(next) next->prev = nullptr;
                head = next;
            }
            // std::cout << "End Popf Tail Addr " << tail << " And Head addr " << head << "\n";
        }

        T back() const {
            return (tail->data);
        }

        T front() const {
            return (head->data);
        }

        T& operator[](size_t pos) {
            auto temp = head;
            while(temp && pos--) {
                temp = temp->next;
            }
            if(temp == nullptr) {
                throw std::out_of_range("custom_deque: invalid index access");
            }
            return temp->data;
        }

        const T& operator[] (size_t pos) const {
            auto temp = head;
            while(temp && pos--) {
                temp = temp->next;
            }
            if(temp == nullptr) {
                throw std::out_of_range("custom_deque: invalid index access");
            }
            return temp->data;
        }

        using iterator = struct Node<T>*;

        iterator begin() {
            return head;
        }

        iterator end() {
            return nullptr;
        }

        iterator insert(iterator it, const T& data) {
            auto prevNode = (it == end()) ? tail : it->prev;
            auto currNode = it;
            auto newNode = new Node<T>(data, prevNode, currNode);
            if (prevNode) {
                prevNode->next = newNode;
            } else {
                head = newNode;
            }

            if(currNode) {
                currNode->prev = newNode;
            } else {
                tail = newNode;
            }
            return newNode;
        }

        iterator insert(iterator it, T&& data) {
            auto prevNode = (it == end()) ? tail : it->prev;
            auto currNode = it;
            auto newNode = new Node<T>(std::move(data), prevNode, currNode);
            if (prevNode) {
                prevNode->next = newNode;
            } else {
                head = newNode;
            }

            if(currNode) {
                currNode->prev = newNode;
            } else {
                tail = newNode;
            }
            return newNode;
        }

        iterator erase(iterator it) {
            if(it == end()) {
                return tail;
            }
            auto prevNode = it->prev;
            auto nextNode = it->next;
            if(prevNode) {
                prevNode->next = nextNode;
            } else {
                head = nextNode;
            }
            if(nextNode) {
                nextNode->prev = prevNode;
            } else {
                tail = prevNode;
            }
            delete it;
            return nextNode;
        }


    private:
        struct Node<T>* head;
        struct Node<T>* tail;

        void destroy_deque() {
            auto temp = head;
            struct Node<T>* next = nullptr;
            while(temp != nullptr) {
                next =  temp->next;
                delete temp;
                temp = next;
            }
            head = nullptr;
            tail = nullptr;
        }
};

#define CHECK(cond) do { \
    if (!(cond)) { \
        std::cerr << "FAILED: " << #cond << " (line " << __LINE__ << ")" << std::endl; \
        std::exit(1); \
    } \
} while (0)

void test_push_and_access() {
    custom_deque<int> dq;
    dq.push_back(1);
    dq.push_front(2);
    dq.push_back(3);
    dq.push_front(4);
    // deque is now: 4 2 1 3
    CHECK(dq.front() == 4);
    CHECK(dq.back() == 3);
    CHECK(dq[0] == 4);
    CHECK(dq[1] == 2);
    CHECK(dq[2] == 1);
    CHECK(dq[3] == 3);
    std::cout << "test_push_and_access passed" << std::endl;
}

void test_pop() {
    custom_deque<int> dq;
    dq.push_back(1);
    dq.push_back(2);
    dq.push_back(3);
    dq.pop_back();
    CHECK(dq.back() == 2);
    dq.pop_front();
    CHECK(dq.front() == 2);
    dq.pop_front();
    // deque is now empty; pop_front/pop_back on empty should be no-ops
    dq.pop_front();
    dq.pop_back();
    std::cout << "test_pop passed" << std::endl;
}

void test_operator_index_out_of_range() {
    custom_deque<int> dq;
    dq.push_back(1);
    dq.push_back(2);
    bool threw = false;
    try {
        dq[5];
    } catch (const std::out_of_range&) {
        threw = true;
    }
    CHECK(threw);
    std::cout << "test_operator_index_out_of_range passed" << std::endl;
}

void test_mutation_through_index() {
    custom_deque<int> dq;
    dq.push_back(1);
    dq.push_back(2);
    dq.push_back(3);
    dq[1] = 42;
    CHECK(dq[1] == 42);
    std::cout << "test_mutation_through_index passed" << std::endl;
}

void test_copy_constructor() {
    custom_deque<int> original;
    original.push_back(1);
    original.push_back(2);
    original.push_back(3);

    custom_deque<int> copy(original);
    CHECK(copy[0] == 1);
    CHECK(copy[1] == 2);
    CHECK(copy[2] == 3);

    // mutate the copy and ensure the original is unaffected (deep copy check)
    copy[0] = 99;
    CHECK(original[0] == 1);
    std::cout << "test_copy_constructor passed" << std::endl;
}

void test_move_constructor() {
    custom_deque<int> original;
    original.push_back(1);
    original.push_back(2);

    custom_deque<int> moved(std::move(original));
    CHECK(moved[0] == 1);
    CHECK(moved[1] == 2);
    std::cout << "test_move_constructor passed" << std::endl;
}

void test_copy_assignment() {
    custom_deque<int> a;
    a.push_back(1);
    a.push_back(2);

    custom_deque<int> b;
    b.push_back(100);
    b = a;
    CHECK(b[0] == 1);
    CHECK(b[1] == 2);

    b[0] = 55;
    CHECK(a[0] == 1);
    std::cout << "test_copy_assignment passed" << std::endl;
}

void test_move_assignment() {
    custom_deque<int> a;
    a.push_back(1);
    a.push_back(2);

    custom_deque<int> b;
    b.push_back(100);
    b = std::move(a);
    CHECK(b[0] == 1);
    CHECK(b[1] == 2);
    std::cout << "test_move_assignment passed" << std::endl;
}

void test_string_type() {
    custom_deque<std::string> dq;
    dq.push_back(std::string("hello"));
    dq.push_front(std::string("world"));
    CHECK(dq.front() == "world");
    CHECK(dq.back() == "hello");
    std::cout << "test_string_type passed" << std::endl;
}

void test_insert() {
    // middle: 1 2 4 -- insert 3 before the node holding 4
    {
        custom_deque<int> dq;
        dq.push_back(1);
        dq.push_back(2);
        dq.push_back(4);
        auto it = dq.begin()->next->next;
        auto inserted = dq.insert(it, 3);
        CHECK(inserted->data == 3);
        CHECK(dq[0] == 1);
        CHECK(dq[1] == 2);
        CHECK(dq[2] == 3);
        CHECK(dq[3] == 4);
    }

    // head: 2 3 -- insert 1 before begin()
    {
        custom_deque<int> dq;
        dq.push_back(2);
        dq.push_back(3);
        dq.insert(dq.begin(), 1);
        CHECK(dq.front() == 1);
        CHECK(dq[0] == 1);
        CHECK(dq[1] == 2);
        CHECK(dq[2] == 3);
    }

    // tail: 1 2 -- insert 3 before end()
    {
        custom_deque<int> dq;
        dq.push_back(1);
        dq.push_back(2);
        dq.insert(dq.end(), 3);
        CHECK(dq.back() == 3);
        CHECK(dq[2] == 3);
    }

    // empty deque: insert 42 before end()
    {
        custom_deque<int> dq;
        dq.insert(dq.end(), 42);
        CHECK(dq.front() == 42);
        CHECK(dq.back() == 42);
    }

    std::cout << "test_insert passed" << std::endl;
}

void test_erase() {
    // middle: 1 2 3 -- erase the node holding 2
    {
        custom_deque<int> dq;
        dq.push_back(1);
        dq.push_back(2);
        dq.push_back(3);
        auto it = dq.begin()->next;
        auto next = dq.erase(it);
        CHECK(dq[0] == 1);
        CHECK(dq[1] == 3);
        CHECK(next != nullptr && next->data == 3);
    }

    // head: 1 2 3 -- erase begin()
    {
        custom_deque<int> dq;
        dq.push_back(1);
        dq.push_back(2);
        dq.push_back(3);
        dq.erase(dq.begin());
        CHECK(dq.front() == 2);
        CHECK(dq[0] == 2);
        CHECK(dq[1] == 3);
    }

    // tail: 1 2 3 -- erase the last node
    {
        custom_deque<int> dq;
        dq.push_back(1);
        dq.push_back(2);
        dq.push_back(3);
        auto it = dq.begin();
        while (it->next) it = it->next;
        auto ret = dq.erase(it);
        CHECK(dq.back() == 2);
        CHECK(ret == nullptr);
    }

    // erase down to empty, then reuse the deque
    {
        custom_deque<int> dq;
        dq.push_back(42);
        dq.erase(dq.begin());
        CHECK(dq.begin() == dq.end());
        dq.push_back(7);
        CHECK(dq.front() == 7);
        CHECK(dq.back() == 7);
    }

    std::cout << "test_erase passed" << std::endl;
}



int main() {
    test_push_and_access();
    test_pop();
    test_operator_index_out_of_range();
    test_mutation_through_index();
    test_copy_constructor();
    test_move_constructor();
    test_copy_assignment();
    test_move_assignment();
    test_string_type();
    test_insert();
    test_erase();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}