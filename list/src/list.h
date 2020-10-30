#pragma once
#include <iterator>
#include <list>

#include <vector>
#include <algorithm>

namespace task {


template<class T, class Alloc = std::allocator<T>>
class list {
    struct BaseNode;
    struct Node;

public:

    class const_iterator;
    class iterator {
    public:
        using difference_type = ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator(): node_(nullptr) {

        }

        iterator(const iterator& other): node_(other.node_) {

        }

        iterator(BaseNode* node): node_(node) {

        }

         operator const_iterator() const {
             return {node_};
        }

        iterator& operator=(const iterator& other) {
            if (this != &other) {
                node_ = other.node_;
            }

            return *this;
        }

        iterator& operator++() {
            node_ = node_->next;
            return *this;
        }

        iterator operator++(int) {
            iterator res(*this);
            node_ = node_->next;
            return res;
        }

        reference operator*() const {
            return static_cast<Node*>(node_)->value;
        }

        pointer operator->() const {
            return &static_cast<Node*>(node_)->value;
        }

        iterator& operator--() {
            node_ = node_->prev;
            return *this;
        }

        iterator operator--(int) {
            iterator res(*this);
            node_ = node_->prev;
            return res;
        }

        bool operator==(iterator other) const  {
            return node_ == other.node_;
        }

        bool operator!=(iterator other) const {
            return node_ != other.node_;
        }

    private:
        friend class list;
        list::BaseNode* node_;
    };

    class const_iterator {
     public:
        using difference_type = ptrdiff_t;
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;
        using iterator_category = std::bidirectional_iterator_tag;

        const_iterator(): node_(nullptr) {

        }

        const_iterator(const const_iterator& other): node_(other.node_) {

        }

        const_iterator(BaseNode* node): node_(node) {

        }


        const_iterator& operator=(const const_iterator& other) {
            if (this != &other) {
                node_ = other.node_;
            }

            return *this;
        }

        const_iterator& operator++() {
            node_ = node_->next;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator res(*this);
            node_ = node_->next;
            return res;
        }

        reference operator*() const {
            return static_cast<Node*>(node_)->value;
        }

        pointer operator->() const {
            return &static_cast<Node*>(node_)->value;
        }

        const_iterator& operator--() {
            node_ = node_->prev;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator res(*this);
            node_ = node_->prev;
            return res;
        }

        bool operator==(const_iterator other) const  {
            return node_ == other.node_;
        }

        bool operator!=(const_iterator other) const {
            return node_ != other.node_;
        }

     private:
        friend class list;
        list::BaseNode* node_;
    };

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;


    list();
    explicit list(const Alloc& alloc);
    list(size_t count, const T& value, const Alloc& alloc = Alloc());
    explicit list(size_t count, const Alloc& alloc = Alloc());

    ~list();

    list(const list& other);
    list(list&& other);
    list& operator=(const list& other);
    list& operator=(list&& other);

    Alloc get_allocator() const;


    T& front();
    const T& front() const;

    T& back();
    const T& back() const;


    iterator begin();
    iterator end();

    const_iterator cbegin() const;
    const_iterator cend() const;

    reverse_iterator rbegin();
    reverse_iterator rend();

    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;


    bool empty() const;
    size_t size() const;
    size_t max_size() const;
    void clear();

    iterator insert(const_iterator pos, const T& value);
    iterator insert(const_iterator pos, T&& value);
    iterator insert(const_iterator pos, size_t count, const T& value);

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);


    void push_back(const T& value);
    void push_back(T&& value);
    void pop_back();

    void push_front(const T& value);
    void push_front(T&& value);
    void pop_front();

    template <class... Args>
    iterator emplace(const_iterator pos, Args&&... args);

    template <class... Args>
    void emplace_back(Args&&... args);

    template <class... Args>
    void emplace_front(Args&&... args);

    void resize(size_t count);
    void swap(list& other);


    void merge(list& other);
    void splice(const_iterator pos, list& other);
    void remove(const T& value);
    void reverse();
    void unique();
    void sort();

    // Your code goes here?..

private:

    void init_ends(size_t new_size=0);

    template <class ... Args>
    std::pair<Node*, Node*> create_nodes(size_t count, Args&&... args);

    void delete_node(BaseNode* base_node);

    void insert_nodes(const_iterator pos, size_t count, BaseNode* first, BaseNode* last);

    struct BaseNode {
      BaseNode(): prev(nullptr), next(nullptr) {};

      void connect(BaseNode* other) {
        next = other;
        other->prev = this;
      }

      BaseNode* prev;
      BaseNode* next;
    };

    struct Node: public BaseNode {
      Node() = default;

      template <class... Args>
      Node(Args&&... args): BaseNode(), value(std::forward<Args>(args)...) {

      }

      T value;
    };

    size_t size_;
    BaseNode head_, tail_;
    typename Alloc::template rebind<Node>::other node_alloc_;
};

template<class T, class Alloc>
list<T, Alloc>::list(): size_(0), head_(), tail_(),  node_alloc_() {
    init_ends();
}

template<class T, class Alloc>
list<T, Alloc>::list(const Alloc& alloc):size_(0), head_(), tail_(), node_alloc_(alloc) {
    init_ends();

}

template<class T, class Alloc>
list<T, Alloc>::list(size_t count, const T& value, const Alloc& alloc): list(alloc) {
    auto[first, last] = create_nodes(count, value);
    insert_nodes(cend(), count, first, last);
}

template<class T, class Alloc>
list<T, Alloc>::list(size_t count, const Alloc& alloc): list(alloc) {
    auto[first, last] = create_nodes(count);
    insert_nodes(cend(), count, first, last);
}

template<class T, class Alloc>
list<T, Alloc>::~list() {
    clear();
}

template<class T, class Alloc>
list<T, Alloc>::list(const list& other): size_(0), head_(), tail_(), node_alloc_(other.node_alloc_) {

    init_ends();

    Node* prev_node = nullptr;
    Node* first = nullptr, *last = nullptr;
    for (auto node = other.head_.next; node != &other.tail_; node = node->next) {
        Node* new_node = node_alloc_.allocate(1);
        node_alloc_.construct(new_node, static_cast<Node*>(node)->value);
        new_node->prev = prev_node;
        if (prev_node) {
            prev_node->next = new_node;
        }
        else {
            first = new_node;
        }
        prev_node = new_node;
    }

    insert_nodes(cend(), other.size(), first, prev_node);
}

template<class T, class Alloc>
list<T, Alloc>::list(list&& other): size_(0), head_(), tail_(), node_alloc_(std::move(other.node_alloc_)) {

    init_ends(other.size_);
    other.head_.next->prev = &head_;
    head_.next = other.head_.next;
    other.tail_.prev->next = &tail_;
    tail_.prev = other.tail_.prev;

    other.init_ends();
}

template<class T, class Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(const list& other) {
    if (this == &other) return *this;

    list temp(other);
    temp.swap(*this);

    return *this;
}

template<class T, class Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(list&& other) {
    if (this == &other) return *this;

    clear();
    other.swap(*this);

    return *this;
}

template<class T, class Alloc>
Alloc list<T, Alloc>::get_allocator() const {
    return Alloc();
}

template<class T, class Alloc>
T& list<T, Alloc>::front() {
    return *begin();
}

template<class T, class Alloc>
const T& list<T, Alloc>::front() const{
    return *cbegin();
}

template<class T, class Alloc>
T& list<T, Alloc>::back() {
    auto it = end();
    return *(--it);
}

template<class T, class Alloc>
const T& list<T, Alloc>::back() const{
    return *(--cend());
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::begin() {
    return {head_.next};
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::end() {
    return {&tail_};
}

template<class T, class Alloc>
typename list<T, Alloc>::const_iterator list<T, Alloc>::cbegin() const{
    return {head_.next};
}

template<class T, class Alloc>
typename list<T, Alloc>::const_iterator list<T, Alloc>::cend() const{
    return {const_cast<BaseNode*>(&tail_)};
}

template<class T, class Alloc>
typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rbegin() {
    return reverse_iterator(end());
}

template<class T, class Alloc>
typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rend() {
    return reverse_iterator(begin());
}

template<class T, class Alloc>
typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crbegin() const{
    return const_reverse_iterator(cend());
}

template<class T, class Alloc>
typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crend() const{
    return const_reverse_iterator(cbegin());
}

template<class T, class Alloc>
bool list<T, Alloc>::empty() const {
    return size_ == 0;
}

template<class T, class Alloc>
size_t list<T, Alloc>::size() const {
    return size_;
}

template<class T, class Alloc>
size_t list<T, Alloc>::max_size() const {
    return size_;
}

template<class T, class Alloc>
void list<T, Alloc>::clear() {
    auto node = head_.next;
    while (node != &tail_) {
        node = node->next;
        delete_node(node->prev);
    }

    init_ends();
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(const_iterator pos, const T& value) {
    auto [first, last] = create_nodes(1, value);
    insert_nodes(pos, 1, first, last);

    return {first};
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(const_iterator pos, T&& value) {
    auto [first, last] = create_nodes(1, std::move(value));
    insert_nodes(pos, 1, first, last);

    return {first};
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(const_iterator pos, size_t count, const T& value) {
    auto [first, last] = create_nodes(count, value);
    insert_nodes(pos, count, first, last);

    return {first};
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(const_iterator pos) {
    auto node = pos.node_;
    auto next = node->next, prev = node->prev;
    next->prev = prev;
    prev->next = next;

    delete_node(node);
    --size_;

    return {next};
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(const_iterator first, const_iterator last) {
    while (first != last) {
        erase(first++);
    }

    return {last.node_};
}

template<class T, class Alloc>
void list<T, Alloc>::push_back(const T& value) {
    insert(cend(), value);
}

template<class T, class Alloc>
void list<T, Alloc>::push_back(T&& value) {
    insert(cend(), std::move(value));
}

template<class T, class Alloc>
void list<T, Alloc>::pop_back() {
    erase(--cend());
}

template<class T, class Alloc>
void list<T, Alloc>::push_front(const T& value) {
    insert(cbegin(), value);
}

template<class T, class Alloc>
void list<T, Alloc>::push_front(T&& value) {
    insert(cbegin(), std::move(value));
}

template<class T, class Alloc>
void list<T, Alloc>::pop_front() {
    erase(cbegin());
}

template<class T, class Alloc>
template <class... Args>
typename list<T, Alloc>::iterator list<T, Alloc>::emplace(const_iterator pos, Args&&... args) {
    auto [first, last] = create_nodes(1, std::forward<Args>(args)...);
    insert_nodes(pos, 1, first, last);
    return {first};
}

template<class T, class Alloc>
template <class... Args>
void list<T, Alloc>::emplace_back(Args&&... args) {
    emplace(cend(), std::forward<Args>(args)...);
}

template<class T, class Alloc>
template <class... Args>
void list<T, Alloc>::emplace_front(Args&&... args) {
    emplace(cbegin(), std::forward<Args>(args)...);
}

template<class T, class Alloc>
void list<T, Alloc>::resize(size_t count) {
    if (count > size_) {
        auto [first, last] = create_nodes(count - size_);
        insert_nodes(cend(), count - size_, first, last);
    } else if (count < size_) {
        auto node = &head_;
        for (size_t i = 0; i < count + 1; ++i) {
            node = node->next;
        }
        auto node_last = node->prev;

        while (node != &tail_) {
            node = node->next;
            delete_node(node->prev);
        }

        node_last->connect(&tail_);
    }

    size_ = count;
}

template<class T, class Alloc>
void list<T, Alloc>::swap(list& other) {
    if (size_ != 0 || other.size_ != 0) {
        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);

        if (size_ == 0) {
            other.init_ends();
        } else {
            other.head_.connect(other.head_.next);
            other.tail_.prev->connect(&other.tail_);
        }

        if (other.size_ == 0) {
            init_ends();
        } else {
            head_.connect(head_.next);
            tail_.prev->connect(&tail_);
        }
    }

    std::swap(size_, other.size_);
    std::swap(node_alloc_, other.node_alloc_);
}

template<class T, class Alloc>
void list<T, Alloc>::merge(list& other) {
    if (this == &other) return;

    BaseNode* prev = nullptr;
    BaseNode* begin = nullptr;

    auto it1 = this->begin();
    auto it2 = other.begin();
    auto end1 = this->end();
    auto end2 = other.end();

    while (it1 != end1 or it2 != end2) {
        BaseNode* node = nullptr;
        if (it1 != end1 and it2 != end2) {
            node =  static_cast<Node*>(it2.node_)->value < static_cast<Node*>(it1.node_)->value ?
                    (it2++).node_ : (it1++).node_;
        } else if (it1 != end1) {
            node = (it1++).node_;
        } else if (it2 != end2) {
            node = (it2++).node_;
        }

        node->prev = prev;
        if (prev == nullptr) {
            begin = node;
        } else {
            prev->next = node;
        }

        prev = node;
    }

    auto new_size = size_ + other.size_;

    init_ends();
    other.init_ends();

    insert_nodes(cend(), new_size, begin, prev);
}

template<class T, class Alloc>
void list<T, Alloc>::splice(const_iterator pos, list& other) {
    if (other.empty()) return;
    insert_nodes(pos, other.size_, other.head_.next, other.tail_.prev);
    other.init_ends();
}

template<class T, class Alloc>
void list<T, Alloc>::remove(const T& value) {
    for (auto it = cbegin(); it != cend(); ) {
        if (*it == value)
            it = erase(it);
        else
            ++it;
    }
}

template<class T, class Alloc>
void list<T, Alloc>::reverse() {
    if (size_ < 2) return;

    for (auto node = head_.next; node != &tail_; node = node->prev) {
        std::swap(node->next, node->prev);
    }

    auto new_first = tail_.prev;
    auto new_last = head_.next;
    head_.connect(new_first);
    new_last->connect(&tail_);
}

template<class T, class Alloc>
void list<T, Alloc>::unique() {
    if (size_ < 2) return;

    auto prev_it = cbegin();
    for (auto it = ++cbegin(); it != cend();) {
        if (*it == *prev_it) {
            it = erase(it);
        }
        else {
            prev_it = it++;
        }
    }
}


template<class T, class Alloc>
void list<T, Alloc>::sort() {
    if (size_ < 2)
        return;

    list temp1, temp2;

    auto half_size = size_ / 2;

    auto it = begin();
    std::advance(it, half_size - 1);

    temp2.insert_nodes(temp2.cend(), size_ - half_size, it.node_->next, tail_.prev);
    temp1.insert_nodes(temp1.cend(), half_size, head_.next, it.node_);
    init_ends();

    temp1.sort();
    temp2.sort();

    splice(cend(), temp1);
    merge(temp2);
}

template<class T, class Alloc>
void list<T, Alloc>::init_ends(size_t new_size) {
    tail_.prev = &head_;
    head_.next = &tail_;
    size_ = new_size;
}

template<class T, class Alloc>
template <class ... Args>
std::pair<typename list<T, Alloc>::Node* , typename list<T, Alloc>::Node* >
list<T, Alloc>::create_nodes(size_t count, Args&&... args) {
    Node* prev = nullptr;
    Node* first = nullptr;
    for (size_t i = 0; i < count; ++i) {
        Node* node = node_alloc_.allocate(1);
        node_alloc_.construct(node, std::forward<Args>(args)...);
        node->prev = prev;
        if (prev) {
            prev->connect(node);
        }
        else {
            first = node;
        }

        prev = node;
    }

    return std::make_pair(first, prev);
}

template<class T, class Alloc>
void list<T, Alloc>::delete_node(BaseNode* base_node) {
    auto node = static_cast<Node*>(base_node);
    node_alloc_.destroy(node);
    node_alloc_.deallocate(node, 1);
}

template<class T, class Alloc>
void list<T, Alloc>::insert_nodes(const_iterator pos, size_t count, BaseNode* first, BaseNode* last) {
    auto tail_node = pos.node_;
    auto head_node = tail_node->prev;

    head_node->connect(first);
    last->connect(tail_node);

    size_ += count;
}

}  // namespace task
