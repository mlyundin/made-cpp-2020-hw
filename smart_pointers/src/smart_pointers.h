#pragma once
#include <type_traits>
#include <atomic>

namespace task {

template <class T>
class UniquePtr {
public:
    using element_type = typename std::remove_reference<T>::type;
    using pointer = element_type*;

    UniquePtr() = default;
    UniquePtr(pointer);
    UniquePtr(UniquePtr&&);
    UniquePtr& operator=(UniquePtr&&);
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    ~UniquePtr();

    typename std::add_lvalue_reference<element_type>::type operator*() const;
    pointer operator->() const;
    pointer get() const;

    pointer release();
    void reset( pointer ptr = pointer() );
    void swap(UniquePtr& other);

private:
    pointer data_ = nullptr;
};

template <class T> struct ControlBlock;
template <class T> class WeakPtr;

template <class T>
class SharedPtr {
public:
    using element_type = T;
    using pointer = element_type*;

    SharedPtr() = default;
    SharedPtr(pointer);
    SharedPtr(SharedPtr&&);
    SharedPtr& operator=(SharedPtr&&);
    SharedPtr(const SharedPtr&);
    SharedPtr(const WeakPtr<T>&);
    SharedPtr& operator=(const SharedPtr&);

    ~SharedPtr();

    typename std::add_lvalue_reference<element_type>::type operator*() const;
    pointer operator->() const;
    pointer get() const;

    std::size_t use_count() const;

    void reset( pointer ptr = pointer() );

    void swap(SharedPtr&);

private:
    friend class WeakPtr<T>;
    ControlBlock<pointer>* ctrl_block_ = nullptr;

};

template <class T>
class WeakPtr {
public:
    using element_type = T;
    using pointer = element_type*;

    WeakPtr() = default;
    WeakPtr(const SharedPtr<T>&);
    WeakPtr(WeakPtr&&);
    WeakPtr& operator=(WeakPtr&&);
    WeakPtr(const WeakPtr&);
    WeakPtr& operator=(const WeakPtr&);

    ~WeakPtr();

    SharedPtr<T> lock() const;

    std::size_t use_count() const;
    bool expired() const;

    void reset();
    void swap(WeakPtr&);

private:
    friend class SharedPtr<T>;
    ControlBlock<pointer>* ctrl_block_ = nullptr;
};


}  // namespace task


#include "smart_pointers.tpp"
