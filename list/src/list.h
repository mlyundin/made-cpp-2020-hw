#pragma once
#include <iterator>
#include <list>

namespace task {


template<class T, class Alloc = std::allocator<T>>
class list {
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

        iterator(Node* node): node_(node) {

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
            return node_->value;
        }

        pointer operator->() const {
            return &node_->value;
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
        list::Node* node_;
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

        const_iterator(Node* node): node_(node) {

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
            return node_->value;
        }

        pointer operator->() const {
            return &node_->value;
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
        list::Node* node_;
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

    template <class ... Args>
    Node* create_node(Args&&... args);

    void delete_node(Node* node);

    void insert_node(const_iterator pos, Node* new_node);

    struct Node {
      Node() = default;

//      Node(const T& value): value(value), prev(nullptr), next(nullptr) {
//
//      }

//      Node(T&& value): value(std::move(value)), prev(nullptr), next(nullptr) {
//
//      }

      template <class... Args>
      Node(Args&&... args): value(std::forward<Args>(args)...), prev(nullptr), next(nullptr) {

      }

      T value;
      Node* prev;
      Node* next;
    };

    size_t size_;
    Node *head_, *tail_;
    Alloc alloc_;
    typename Alloc::template rebind<Node>::other node_alloc_;
};

template<class T, class Alloc>
list<T, Alloc>::list(): size_(0), head_(nullptr), tail_(nullptr), alloc_(), node_alloc_() {

}

template<class T, class Alloc>
list<T, Alloc>::list(const Alloc& alloc):size_(0), head_(nullptr), tail_(nullptr),
                                         alloc_(alloc), node_alloc_(alloc) {

}

template<class T, class Alloc>
list<T, Alloc>::list(size_t count, const T& value, const Alloc& alloc): size_(count), head_(nullptr),
                                                                        tail_(nullptr), alloc_(alloc), node_alloc_(alloc) {
    for (size_t i = 0; i < count; ++i) {
        Node* node = create_node(value);

        if (tail_ == nullptr) {
            tail_ = head_ = node;
        }
        else {
            node->prev = tail_;
            tail_ = tail_->next = node;
        }
    }
}

template<class T, class Alloc>
list<T, Alloc>::list(size_t count, const Alloc& alloc): size_(count), head_(nullptr),
                                                        tail_(nullptr), alloc_(alloc), node_alloc_(alloc) {
    for (size_t i = 0; i < count; ++i) {
        Node* node = create_node();

        if (tail_ == nullptr) {
            tail_ = head_ = node;
        }
        else {
            node->prev = tail_;
            tail_ = tail_->next = node;
        }
    }
}

template<class T, class Alloc>
list<T, Alloc>::~list() {
    clear();
}

template<class T, class Alloc>
list<T, Alloc>::list(const list& other): size_(other.size_), head_(nullptr), tail_(nullptr),
alloc_(other.alloc_), node_alloc_(other.node_alloc_) {
    Node* prev_node = nullptr;
    for (auto node = other.head_; node; node = node->next) {
        auto new_node = create_node(node->value);
        if (prev_node) {
            prev_node->next = new_node;
            new_node->prev = prev_node;
        }
        else {
            head_ = new_node;
        }
        prev_node = new_node;
    }

    tail_ = prev_node;
}

template<class T, class Alloc>
list<T, Alloc>::list(list&& other): size_(other.size_), head_(other.head_), tail_(other.tail_),
                                    alloc_(std::move(other.alloc_)), node_alloc_(std::move(other.node_alloc_)) {
    other.head_ = nullptr;
    other.tail_ = nullptr;
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
    return alloc_;
}

template<class T, class Alloc>
T& list<T, Alloc>::front() {
    return head_->value;
}

template<class T, class Alloc>
const T& list<T, Alloc>::front() const{
    return head_->value;
}

template<class T, class Alloc>
T& list<T, Alloc>::back() {
    return tail_->value;
}

template<class T, class Alloc>
const T& list<T, Alloc>::back() const{
    return tail_->value;
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::begin() {
    return {head_};
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::end() {
    return {};
}

template<class T, class Alloc>
typename list<T, Alloc>::const_iterator list<T, Alloc>::cbegin() const{
    return {head_};
}

template<class T, class Alloc>
typename list<T, Alloc>::const_iterator list<T, Alloc>::cend() const{
    return {};
}

template<class T, class Alloc>
typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rbegin() {
    return {tail_};
}

template<class T, class Alloc>
typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rend() {
    return {};
}

template<class T, class Alloc>
typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crbegin() const{
    return reverse_iterator{tail_};
}

template<class T, class Alloc>
typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crend() const{
    return {};
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
    while (head_ != nullptr) {
        auto node = head_;
        head_ = head_->next;
        delete_node(node);
    }

    tail_ = nullptr;
    size_ = 0;
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(const_iterator pos, const T& value) {
    auto new_node = create_node(value);
    insert_node(pos, new_node);
    return {new_node};
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(const_iterator pos, T&& value) {
    auto new_node = create_node(std::move(value));
    insert_node(pos, new_node);
    return {new_node};
}

template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(const_iterator pos, size_t count, const T& value) {
    for (size_t i = 0; i < count; ++i) {
        insert(pos--, value);
    }

    return {pos.node_};
}


template<class T, class Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(const_iterator pos) {
    auto node = pos.node_;
    auto next = node->next, prev = node->prev;

    if (node == head_) {
        head_ = next;
    }

    if (node == tail_) {
        tail_ = prev;
    }

    if (next) {
        next->prev = prev;
    }

    if (prev) {
        prev->next = next;
    }

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
    erase({tail_});
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
    erase({head_});
}

template<class T, class Alloc>
template <class... Args>
typename list<T, Alloc>::iterator list<T, Alloc>::emplace(const_iterator pos, Args&&... args) {
    auto new_node = create_node(std::forward<Args>(args)...);
    insert_node(pos, new_node);
    return {new_node};
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
        for (size_t i = 0; i < count - size_; ++i) {
            auto new_node = create_node();
            if (tail_) {
                tail_->next = new_node;
                new_node->prev = tail_;
            }

            tail_ = new_node;
        }
    } else if (count < size_) {
        size_t i = 0;
        auto node = head_;
        while (++i != count) {
            node = node->next;
        }
        tail_ = node;

        node = node->next;
        while (node != nullptr) {
            auto to_delete = node;
            node = node->next;
            delete_node(to_delete);
        }
    }

    size_ = count;
}

template<class T, class Alloc>
void list<T, Alloc>::swap(list& other) {
    std::swap(size_, other.size_);
    std::swap(head_, other.head_);
    std::swap(tail_, other.tail_);
    std::swap(alloc_, other.alloc_);
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
template <class ... Args>
typename list<T, Alloc>::Node* list<T, Alloc>::create_node(Args&&... args) {
    Node* node = node_alloc_.allocate(1);
    node_alloc_.construct(node, std::forward<Args>(args)...);

    return node;
}

template<class T, class Alloc>
void list<T, Alloc>::delete_node(Node* node) {
    node_alloc_.destroy(node);
    node_alloc_.deallocate(node, 1);
}

template<class T, class Alloc>
void list<T, Alloc>::insert_node(const_iterator pos, Node* new_node) {
    auto node = pos.node_;
    new_node->next = node;
    if (node) {
        new_node->prev = node->prev;
        if (new_node->prev)
            new_node->prev->next = new_node;
        node->prev = new_node;
    } else {
        if (tail_) {
            new_node->prev = tail_;
            tail_->next = new_node;
        }

        tail_ = new_node;
    }

    if (node == head_) {
        head_ = new_node;
    }

    ++size_;
}
}  // namespace task
