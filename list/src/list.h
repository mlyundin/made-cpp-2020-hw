#pragma once
#include <iterator>
#include <list>

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

    void init_ends();

    template <class ... Args>
    std::pair<Node*, Node*> create_nodes(size_t count, Args&&... args);

    void delete_node(BaseNode* base_node);

    void insert_nodes(const_iterator pos, size_t count, BaseNode* first, BaseNode* last);

    struct BaseNode {
      BaseNode(): prev(nullptr), next(nullptr) {};
      BaseNode(BaseNode* prev, BaseNode* next): prev(prev), next(next) {}
      BaseNode* prev;
      BaseNode* next;
    };

    struct Node: public BaseNode {
      Node() = default;

      Node(const T& value):BaseNode(), value(value) {

      }

//      Node(T&& value): value(std::move(value)), prev(nullptr), next(nullptr) {
//
//      }

      template <class... Args>
      Node(Args&&... args): BaseNode(), value(std::forward<Args>(args)...) {

      }

      T value;
    };

    size_t size_;
    BaseNode *head_, *tail_;
    typename Alloc::template rebind<BaseNode>::other base_node_alloc_;
    typename Alloc::template rebind<Node>::other node_alloc_;
};

template<class T, class Alloc>
list<T, Alloc>::list(): size_(0), head_(nullptr), tail_(nullptr), base_node_alloc_(), node_alloc_() {
    init_ends();
}

template<class T, class Alloc>
list<T, Alloc>::list(const Alloc& alloc):size_(0), head_(nullptr), tail_(nullptr),
                                         base_node_alloc_(alloc), node_alloc_(alloc) {
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
    base_node_alloc_.deallocate(head_, 2);
}

template<class T, class Alloc>
list<T, Alloc>::list(const list& other): size_(0), head_(nullptr), tail_(nullptr),
                                         base_node_alloc_(other.base_node_alloc_), node_alloc_(other.node_alloc_) {

    init_ends();

    Node* prev_node = nullptr;
    Node* first = nullptr, *last = nullptr;
    for (auto node = other.head_->next; node != other.tail_; node = node->next) {
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
list<T, Alloc>::list(list&& other): size_(other.size_), head_(nullptr), tail_(nullptr),
                                    base_node_alloc_(std::move(other.base_node_alloc_)), node_alloc_(std::move(other.node_alloc_)) {

    init_ends();
    std::swap(head_, other.head_);
    std::swap(tail_, other.tail_);
    other.size_ = 0;
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

//    size_ = other.size_;
//    head_ = other.head_;
//    tail_ = other.tail_;
//    alloc_ = std::move(other.alloc_);
//    node_alloc_ = std::move(other.node_alloc_);

//    other.head_ = nullptr;
//    other.tail_ = nullptr;
//    other.size_ = 0;

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
    return *(--end());
}

template<class T, class Alloc>
const T& list<T, Alloc>::back() const{
    return *(--cend());
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::begin() {
    return {head_->next};
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::end() {
    return {tail_};
}

template<class T, class Alloc>
typename list<T, Alloc>::const_iterator list<T, Alloc>::cbegin() const{
    return {head_->next};
}

template<class T, class Alloc>
typename list<T, Alloc>::const_iterator list<T, Alloc>::cend() const{
    return {tail_};
}

template<class T, class Alloc>
typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rbegin() {
    return {tail_->prev};
}

template<class T, class Alloc>
typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rend() {
    return {head_};
}

template<class T, class Alloc>
typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crbegin() const{
    return reverse_iterator{tail_->prev};
}

template<class T, class Alloc>
typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crend() const{
    return reverse_iterator{head_};
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
    auto node = head_->next;
    while (node != tail_) {
        node = node->next;
        delete_node(node->prev);
    }

    head_->next = tail_;
    tail_->prev = head_;
    size_ = 0;
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
        auto node = head_;
        for (size_t i = 0; i < count + 1; ++i) {
            node = node->next;
        }

        while (node != tail_) {
            node = node->next;
            delete_node(node->prev);
        }
    }

    size_ = count;
}

template<class T, class Alloc>
void list<T, Alloc>::swap(list& other) {
    std::swap(size_, other.size_);
    std::swap(head_, other.head_);
    std::swap(tail_, other.tail_);
    std::swap(base_node_alloc_, other.base_node_alloc_);
    std::swap(node_alloc_, other.node_alloc_);
}

template<class T, class Alloc>
void list<T, Alloc>::merge(list& other) {

}

template<class T, class Alloc>
void list<T, Alloc>::splice(const_iterator pos, list& other) {

}

template<class T, class Alloc>
void list<T, Alloc>::remove(const T& value) {

}

template<class T, class Alloc>
void list<T, Alloc>::reverse() {

}
template<class T, class Alloc>
void list<T, Alloc>::unique() {

}

template<class T, class Alloc>
void list<T, Alloc>::sort() {

}


template<class T, class Alloc>
void list<T, Alloc>::init_ends() {
    head_ = base_node_alloc_.allocate(2);
    tail_ = head_ + 1;
    node_alloc_.construct(head_, nullptr, tail_);
    node_alloc_.construct(tail_, head_, nullptr);
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
            prev->next = node;
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
    auto node = pos.node_;
    last->next = node;
    first->prev = node->prev;
    node->prev = last;
    first->prev->next = first;

    size_ += count;
}
}  // namespace task
