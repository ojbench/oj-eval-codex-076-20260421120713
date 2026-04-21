// Header submission for ACMOJ 1363 - Linklist without name
// Implements template MyList with required APIs, plus link/cut.

#pragma once
#include <cstddef>
#include <utility>

template <typename ValueType>
class MyList {
    struct Node {
        ValueType value;
        Node* prev;
        Node* next;
        explicit Node(const ValueType& v) : value(v), prev(nullptr), next(nullptr) {}
    };

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    int sz_ = 0;

    void copy_from(const MyList& other) {
        for (Node* p = other.head_; p; p = p->next) push_back(p->value);
    }

public:
    MyList() = default;

    MyList(MyList&& obj) noexcept : head_(obj.head_), tail_(obj.tail_), sz_(obj.sz_) {
        obj.head_ = obj.tail_ = nullptr;
        obj.sz_ = 0;
    }

    MyList(const MyList& obj) : head_(nullptr), tail_(nullptr), sz_(0) { copy_from(obj); }

    MyList& operator=(MyList&& obj) noexcept {
        if (this != &obj) {
            clear();
            head_ = obj.head_;
            tail_ = obj.tail_;
            sz_ = obj.sz_;
            obj.head_ = obj.tail_ = nullptr;
            obj.sz_ = 0;
        }
        return *this;
    }

    MyList& operator=(const MyList& obj) {
        if (this != &obj) {
            clear();
            copy_from(obj);
        }
        return *this;
    }

    ~MyList() { clear(); }

    void push_back(const ValueType& value) {
        Node* n = new Node(value);
        if (!tail_) {
            head_ = tail_ = n;
        } else {
            tail_->next = n;
            n->prev = tail_;
            tail_ = n;
        }
        ++sz_;
    }

    void pop_back() {
        if (!tail_) return;
        Node* t = tail_;
        tail_ = tail_->prev;
        if (tail_) tail_->next = nullptr; else head_ = nullptr;
        delete t;
        --sz_;
    }

    void push_front(const ValueType& value) {
        Node* n = new Node(value);
        if (!head_) {
            head_ = tail_ = n;
        } else {
            n->next = head_;
            head_->prev = n;
            head_ = n;
        }
        ++sz_;
    }

    void pop_front() {
        if (!head_) return;
        Node* h = head_;
        head_ = head_->next;
        if (head_) head_->prev = nullptr; else tail_ = nullptr;
        delete h;
        --sz_;
    }

    ValueType& front() const { return const_cast<ValueType&>(head_->value); }
    ValueType& back() const { return const_cast<ValueType&>(tail_->value); }

    void insert(int index, const ValueType& value) {
        if (index <= 0) { push_front(value); return; }
        if (index >= sz_) { push_back(value); return; }
        Node* p = head_;
        for (int i = 0; i < index && p; ++i) p = p->next;
        if (!p) { push_back(value); return; }
        Node* n = new Node(value);
        Node* prev = p->prev;
        n->next = p;
        n->prev = prev;
        p->prev = n;
        if (prev) prev->next = n; else head_ = n;
        ++sz_;
    }

    void erase(int index) {
        if (index < 0 || index >= sz_) return;
        Node* p = head_;
        for (int i = 0; i < index && p; ++i) p = p->next;
        if (!p) return;
        Node* prev = p->prev;
        Node* next = p->next;
        if (prev) prev->next = next; else head_ = next;
        if (next) next->prev = prev; else tail_ = prev;
        delete p;
        --sz_;
    }

    int size() const { return sz_; }
    bool empty() const { return sz_ == 0; }

    void clear() {
        Node* p = head_;
        while (p) {
            Node* n = p->next;
            delete p;
            p = n;
        }
        head_ = tail_ = nullptr;
        sz_ = 0;
    }

    void link(const MyList& obj) {
        for (Node* p = obj.head_; p; p = p->next) push_back(p->value);
    }

    MyList cut(int index) {
        MyList tail_list;
        if (index <= 0) {
            using std::swap;
            swap(tail_list.head_, head_);
            swap(tail_list.tail_, tail_);
            swap(tail_list.sz_, sz_);
            return tail_list;
        }
        if (index >= sz_) return tail_list;

        Node* p = head_;
        for (int i = 0; i < index && p; ++i) p = p->next;
        Node* before = p ? p->prev : tail_;
        if (before) before->next = nullptr;
        if (p) p->prev = nullptr;

        tail_list.head_ = p;
        tail_list.tail_ = tail_;

        int new_tail_sz = 0;
        for (Node* q = p; q; q = q->next) ++new_tail_sz;
        tail_list.sz_ = new_tail_sz;

        tail_ = before;
        sz_ -= new_tail_sz;
        if (sz_ == 0) head_ = tail_ = nullptr;
        return tail_list;
    }
};

