#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>
#include <new>
#include <functional>

namespace sjtu {

template<typename T>
class list {
protected:
    class node {
    public:
        node *prev;
        node *next;
        T *data;
        
        node() : prev(nullptr), next(nullptr), data(nullptr) {}
        
        node(const T &value) : prev(nullptr), next(nullptr) {
            data = new T(value);
        }
        
        ~node() {
            if (data != nullptr) {
                delete data;
                data = nullptr;
            }
        }
    };

    node *head;
    node *tail;
    size_t num_elements;

    void init() {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;
        num_elements = 0;
    }

    void destroy() {
        clear();
        delete head;
        delete tail;
    }

protected:
    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        pos->prev->next = cur;
        pos->prev = cur;
        num_elements++;
        return cur;
    }

    node *erase(node *pos) {
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        num_elements--;
        return pos;
    }

public:
    class const_iterator;
    class iterator {
    private:
        list *lst;
        node *ptr;
        
        iterator(list *l, node *p) : lst(l), ptr(p) {}

    public:
        friend class list;
        friend class const_iterator;

        iterator() : lst(nullptr), ptr(nullptr) {}
        
        iterator(const iterator &other) : lst(other.lst), ptr(other.ptr) {}

        iterator operator++(int) {
            if (ptr == nullptr || ptr->next == nullptr) throw invalid_iterator();
            iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }
        
        iterator &operator++() {
            if (ptr == nullptr || ptr->next == nullptr) throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }
        
        iterator operator--(int) {
            if (ptr == nullptr || ptr->prev == lst->head) throw invalid_iterator();
            iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }
        
        iterator &operator--() {
            if (ptr == nullptr || ptr->prev == lst->head) throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }
        
        T &operator*() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return *(ptr->data);
        }
        
        T *operator->() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return ptr->data;
        }
        
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        
        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        
        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }
        
        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
    };

    class const_iterator {
    private:
        const list *lst;
        const node *ptr;
        
        const_iterator(const list *l, const node *p) : lst(l), ptr(p) {}

    public:
        friend class list;
        friend class iterator;

        const_iterator() : lst(nullptr), ptr(nullptr) {}
        
        const_iterator(const const_iterator &other) : lst(other.lst), ptr(other.ptr) {}
        
        const_iterator(const iterator &other) : lst(other.lst), ptr(other.ptr) {}

        const_iterator operator++(int) {
            if (ptr == nullptr || ptr->next == nullptr) throw invalid_iterator();
            const_iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }
        
        const_iterator &operator++() {
            if (ptr == nullptr || ptr->next == nullptr) throw invalid_iterator();
            ptr = ptr->next;
            return *this;
        }
        
        const_iterator operator--(int) {
            if (ptr == nullptr || ptr->prev == lst->head) throw invalid_iterator();
            const_iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }
        
        const_iterator &operator--() {
            if (ptr == nullptr || ptr->prev == lst->head) throw invalid_iterator();
            ptr = ptr->prev;
            return *this;
        }
        
        const T &operator*() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return *(ptr->data);
        }
        
        const T *operator->() const {
            if (ptr == nullptr || ptr->data == nullptr) {
                throw invalid_iterator();
            }
            return ptr->data;
        }
        
        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        
        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }
        
        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }
    };

    list() {
        init();
    }
    
    list(const list &other) {
        init();
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
    }
    
    virtual ~list() {
        destroy();
    }
    
    list &operator=(const list &other) {
        if (this == &other) return *this;
        clear();
        for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
            push_back(*it);
        }
        return *this;
    }

    const T &front() const {
        if (empty()) throw container_is_empty();
        return *(head->next->data);
    }
    
    const T &back() const {
        if (empty()) throw container_is_empty();
        return *(tail->prev->data);
    }

    iterator begin() {
        return iterator(this, head->next);
    }
    
    const_iterator cbegin() const {
        return const_iterator(this, head->next);
    }

    iterator end() {
        return iterator(this, tail);
    }
    
    const_iterator cend() const {
        return const_iterator(this, tail);
    }

    virtual bool empty() const {
        return num_elements == 0;
    }

    virtual size_t size() const {
        return num_elements;
    }

    virtual void clear() {
        while (!empty()) {
            pop_front();
        }
    }

    virtual iterator insert(iterator pos, const T &value) {
        if (pos.lst != this) throw invalid_iterator();
        node *newNode = new node(value);
        insert(pos.ptr, newNode);
        return iterator(this, newNode);
    }

    virtual iterator erase(iterator pos) {
        if (empty()) throw container_is_empty();
        if (pos.lst != this) throw invalid_iterator();
        if (pos.ptr == tail) throw invalid_iterator();
        node *toErase = pos.ptr;
        node *nextNode = toErase->next;
        erase(toErase);
        delete toErase;
        return iterator(this, nextNode);
    }

    void push_back(const T &value) {
        insert(tail, new node(value));
    }

    void pop_back() {
        if (empty()) throw container_is_empty();
        node *toErase = tail->prev;
        erase(toErase);
        delete toErase;
    }

    void push_front(const T &value) {
        insert(head->next, new node(value));
    }

    void pop_front() {
        if (empty()) throw container_is_empty();
        node *toErase = head->next;
        erase(toErase);
        delete toErase;
    }

    static bool node_compare(node *a, node *b) {
        return *(a->data) < *(b->data);
    }

    void sort() {
        if (num_elements <= 1) return;
        
        node **arr = new node*[num_elements];
        size_t i = 0;
        for (node *cur = head->next; cur != tail; cur = cur->next) {
            arr[i++] = cur;
        }
        
        std::function<bool(node* const&, node* const&)> cmp = node_compare;
        sjtu::sort<node*>(arr, arr + num_elements, cmp);
        
        head->next = arr[0];
        arr[0]->prev = head;
        for (size_t j = 1; j < num_elements; j++) {
            arr[j-1]->next = arr[j];
            arr[j]->prev = arr[j-1];
        }
        arr[num_elements-1]->next = tail;
        tail->prev = arr[num_elements-1];
        
        delete[] arr;
    }

    void merge(list &other) {
        if (this == &other) return;
        
        node *c1 = head->next;
        node *c2 = other.head->next;
        
        head->next = tail;
        tail->prev = head;
        size_t total = num_elements + other.num_elements;
        num_elements = 0;
        
        while (c1 != tail && c2 != other.tail) {
            if (*(c2->data) < *(c1->data)) {
                node *nextC2 = c2->next;
                insert(tail, c2);
                c2 = nextC2;
            } else {
                node *nextC1 = c1->next;
                insert(tail, c1);
                c1 = nextC1;
            }
        }
        
        while (c1 != tail) {
            node *nextC1 = c1->next;
            insert(tail, c1);
            c1 = nextC1;
        }
        
        while (c2 != other.tail) {
            node *nextC2 = c2->next;
            insert(tail, c2);
            c2 = nextC2;
        }
        
        num_elements = total;
        
        other.head->next = other.tail;
        other.tail->prev = other.head;
        other.num_elements = 0;
    }

    void reverse() {
        if (num_elements <= 1) return;
        
        // First reverse all the prev/next pointers for all nodes including sentinels
        node *cur = head;
        node *last = nullptr;
        while (cur != nullptr) {
            node *tmp = cur->next;
            cur->next = cur->prev;
            cur->prev = tmp;
            last = cur;
            cur = tmp;
        }
        
        // Swap head and tail
        node *tmp = head;
        head = tail;
        tail = tmp;
    }

    void unique() {
        if (num_elements <= 1) return;
        
        node *cur = head->next;
        while (cur->next != tail) {
            if (*(cur->data) == *(cur->next->data)) {
                node *toErase = cur->next;
                erase(toErase);
                delete toErase;
            } else {
                cur = cur->next;
            }
        }
    }
};

}

#endif