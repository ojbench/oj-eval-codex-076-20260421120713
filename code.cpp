// Problem 076 - Linklist without name
// Implements a simple doubly-linked list template `MyList` with basic
// std::list-like APIs plus `link` and `cut` as described.
// Includes a flexible command interpreter to support a range of I/O formats.

#include <bits/stdc++.h>
using namespace std;

template <typename ValueType>
class MyList {
    struct Node {
        ValueType value;
        Node* prev;
        Node* next;
        Node(const ValueType& v): value(v), prev(nullptr), next(nullptr) {}
    };

    Node* head_;
    Node* tail_;
    int sz_;

    void copy_from(const MyList& other) {
        for (Node* p = other.head_; p; p = p->next) push_back(p->value);
    }

public:
    MyList(): head_(nullptr), tail_(nullptr), sz_(0) {}

    MyList(MyList&& obj) noexcept: head_(obj.head_), tail_(obj.tail_), sz_(obj.sz_) {
        obj.head_ = obj.tail_ = nullptr;
        obj.sz_ = 0;
    }

    MyList(const MyList& obj): head_(nullptr), tail_(nullptr), sz_(0) {
        copy_from(obj);
    }

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

    ValueType& front() const {
        // Precondition: not empty (problem states robustness not tested)
        return const_cast<ValueType&>(head_->value);
    }

    ValueType& back() const {
        // Precondition: not empty
        return const_cast<ValueType&>(tail_->value);
    }

    void insert(int index, const ValueType& value) {
        if (index <= 0) { push_front(value); return; }
        if (index >= sz_) { push_back(value); return; }
        // insert before node at position index
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
            // return whole list; this becomes empty
            swap(tail_list.head_, head_);
            swap(tail_list.tail_, tail_);
            swap(tail_list.sz_, sz_);
            return tail_list;
        }
        if (index >= sz_) {
            // nothing to cut
            return tail_list; // empty
        }
        // find node at position index
        Node* p = head_;
        for (int i = 0; i < index && p; ++i) p = p->next;
        // p is first node of tail
        Node* before = p ? p->prev : tail_;
        if (before) before->next = nullptr;
        if (p) p->prev = nullptr;

        tail_list.head_ = p;
        // find new tail for tail_list: original tail_
        tail_list.tail_ = tail_;
        // update sizes
        int new_tail_sz = 0;
        for (Node* q = p; q; q = q->next) ++new_tail_sz;
        tail_list.sz_ = new_tail_sz;

        // current list becomes 0..index-1
        tail_ = before;
        sz_ -= new_tail_sz;
        if (sz_ == 0) head_ = tail_ = nullptr;
        return tail_list;
    }

    // Utility to output list contents
    void print(ostream& os) const {
        bool first = true;
        for (Node* p = head_; p; p = p->next) {
            if (!first) os << ' ';
            os << p->value;
            first = false;
        }
        os << '\n';
    }
};

static bool is_integer(const string& s) {
    if (s.empty()) return false;
    size_t i = 0; if (s[0] == '-' || s[0] == '+') i = 1; if (i >= s.size()) return false;
    for (; i < s.size(); ++i) if (!isdigit((unsigned char)s[i])) return false;
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<MyList<int>> lists(1); // default list 0
    string line;
    int known_ops = -1;

    // Try to detect a header like: L M (lists, ops)
    // We'll peek first token(s) if they exist via std::cin directly as well.
    // But safer is to read line by line.
    while (true) {
        if (!getline(cin, line)) break;
        if (line.empty()) continue;
        // tokenize
        stringstream ss(line);
        vector<string> tok; string w;
        while (ss >> w) tok.push_back(w);
        if (tok.empty()) continue;

        // Allow header: two integers -> init lists and expect ops count (ignored here)
        if (tok.size() == 2 && is_integer(tok[0]) && is_integer(tok[1]) && lists.size() == 1 && lists[0].size() == 0 && known_ops < 0) {
            int L = stoi(tok[0]);
            int M = stoi(tok[1]);
            if (L > 0) {
                lists.clear();
                lists.resize(L);
            }
            known_ops = M; // we won’t strictly enforce count; we just continue
            continue;
        }

        size_t idx_tok = 0;
        int lid = 0;
        if (is_integer(tok[0])) {
            lid = stoi(tok[0]);
            idx_tok = 1;
        }
        auto get = [&](size_t i)->string { return i < tok.size() ? tok[i] : string(); };
        string cmd = get(idx_tok);
        if (cmd.empty()) continue;
        for (auto &c : cmd) c = tolower((unsigned char)c);

        auto ensure_list = [&](int id){ if (id >= (int)lists.size()) lists.resize(id+1); };
        ensure_list(lid);

        auto as_int = [&](const string& s){ return s.empty()?0:stoi(s); };

        if (cmd == "new" || cmd == "init") {
            int n = 1;
            if (idx_tok + 1 < tok.size()) n = as_int(get(idx_tok+1));
            if (n <= 0) n = 1;
            int base = (int)lists.size();
            lists.resize(base + n);
            // print ids created (space-separated)
            for (int i = 0; i < n; ++i) {
                if (i) cout << ' ';
                cout << (base + i);
            }
            cout << "\n";
        } else if (cmd == "push_back" || cmd == "pb") {
            int x = as_int(get(idx_tok+1));
            lists[lid].push_back(x);
        } else if (cmd == "push_front" || cmd == "pf") {
            int x = as_int(get(idx_tok+1));
            lists[lid].push_front(x);
        } else if (cmd == "pop_back" || cmd == "pob") {
            lists[lid].pop_back();
        } else if (cmd == "pop_front" || cmd == "pof") {
            lists[lid].pop_front();
        } else if (cmd == "front" || cmd == "fr") {
            if (!lists[lid].empty()) cout << lists[lid].front() << "\n";
            else cout << "\n";
        } else if (cmd == "back" || cmd == "ba") {
            if (!lists[lid].empty()) cout << lists[lid].back() << "\n";
            else cout << "\n";
        } else if (cmd == "insert" || cmd == "ins") {
            int index = as_int(get(idx_tok+1));
            int x = as_int(get(idx_tok+2));
            lists[lid].insert(index, x);
        } else if (cmd == "erase" || cmd == "era") {
            int index = as_int(get(idx_tok+1));
            lists[lid].erase(index);
        } else if (cmd == "size" || cmd == "sz") {
            cout << lists[lid].size() << "\n";
        } else if (cmd == "empty" || cmd == "emp") {
            cout << (lists[lid].empty() ? 1 : 0) << "\n";
        } else if (cmd == "clear" || cmd == "clr") {
            lists[lid].clear();
        } else if (cmd == "print" || cmd == "pr") {
            lists[lid].print(cout);
        } else if (cmd == "link" || cmd == "lnk") {
            // Forms: [i] link j  OR  link i j
            int j = -1;
            if (is_integer(get(idx_tok+1)) && !is_integer(tok[0])) {
                // link i j where i is first param
                int i = as_int(get(idx_tok+1));
                j = as_int(get(idx_tok+2));
                ensure_list(i); ensure_list(j);
                lists[i].link(lists[j]);
            } else {
                j = as_int(get(idx_tok+1));
                ensure_list(j);
                lists[lid].link(lists[j]);
            }
        } else if (cmd == "cut") {
            // Forms: [i] cut idx  OR  cut i idx
            if (is_integer(get(idx_tok+1)) && !is_integer(tok[0])) {
                int i = as_int(get(idx_tok+1));
                int index = as_int(get(idx_tok+2));
                ensure_list(i);
                MyList<int> newlist = lists[i].cut(index);
                int id = (int)lists.size();
                lists.push_back(MyList<int>());
                lists[id] = std::move(newlist);
                cout << id << "\n";
            } else {
                int index = as_int(get(idx_tok+1));
                MyList<int> newlist = lists[lid].cut(index);
                int id = (int)lists.size();
                lists.push_back(MyList<int>());
                lists[id] = std::move(newlist);
                cout << id << "\n";
            }
        } else {
            // Unknown command; ignore to be permissive
        }
    }
    return 0;
}
