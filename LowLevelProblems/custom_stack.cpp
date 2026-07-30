// Implement a Lock-Free Stack — design a lock-free stack using atomic operations and compare-exchange.
#include<bits/stdc++.h>
#include <atomic>
/*
stack should supports push O(1), pop O(1) (FILO), front O(1), size O(1)
*/

template<typename T>
struct Node {
    Node():next(nullptr), data(T()){}
    
    Node(T&& input) {
        data = input;
        next = nullptr;
    }
    struct Node* next;
    T data;
};

template<typename T>
class custom_stack {
    public:
        custom_stack() : head(nullptr), count(0) {}

        custom_stack(const custom_stack& oth) : head(nullptr), count(0) {
            std::vector<T> temp;
            Node<T>* cur = oth.head;
            while (cur) {
                temp.push_back(cur->data);
                cur = cur->next;
            }
            for (auto it = temp.rbegin(); it != temp.rend(); ++it) {
                push(*it);
            }
        }

        custom_stack& operator=(const custom_stack& oth) {
            if (this != &oth) {
                cleanUpStack();
                std::vector<T> temp;
                Node<T>* cur = oth.head;
                while (cur) {
                    temp.push_back(cur->data);
                    cur = cur->next;
                }
                for (auto it = temp.rbegin(); it != temp.rend(); ++it) {
                    push(*it);
                }
            }
            return *this;
        }

        custom_stack(custom_stack&& oth): head(oth.head), count(oth.count) {
            oth.head = nullptr;
            oth.count = 0;
        }

        custom_stack& operator=(custom_stack&& oth) {
            if(this != &oth) {
                cleanUpStack();
                this->head = oth.head;
                this->count = oth.count;
                oth.head = nullptr;
                oth.count = 0;
            }
            return *this;
        }

        void push(T data) {
            struct Node<T>* node = new Node<T>(std::move(data));
            node->next = head;
            head = node;
            count++;
        };

        void pop() {
            if(count == 0) {
                throw std::out_of_range("pop called on empty stack");
            }
            auto temp = head;
            head = head->next;
            delete temp;
            temp = nullptr;
            count--;
        };

        
        T front() {
            if(count == 0) {
                throw std::out_of_range("front() called on empty stack");
            }

            return head->data;
        };

        size_t size() {
            return count;
        };

        bool empty() {
            return (count == 0);
        };

        void cleanUpStack() {
            while(count) {
                pop();
            }
        }

        ~custom_stack() {
            cleanUpStack();
        }
    private:
        struct Node<T>* head; 
        size_t count;
};

int main() {
    custom_stack<int> customStack;
    for (auto i = 0; i < 10; i++) { customStack.push(i);}
    std::cout << "Stack size " << customStack.size() << std::endl;

    for (auto i = 0; i < 10; i++) { customStack.pop();}

    try {
        customStack.pop();
    }
    catch (std::out_of_range& e) {
        std::cout << "Error " << e.what() << "\n";
    }

}