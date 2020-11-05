namespace task {

template<class T>
UniquePtr<T>::UniquePtr(typename UniquePtr<T>::pointer ptr): data_(ptr) {

}

template<class T>
UniquePtr<T>::UniquePtr(UniquePtr&& other): data_(other.data_) {
    other.data_ = nullptr;
}

template<class T>
UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr&& other) {
    if (this == &other) return *this;
    release();
    std::swap(data_, other.data_);
}

template<class T>
UniquePtr<T>::~UniquePtr() {
    release();
}

template<class T>
typename std::add_lvalue_reference<typename UniquePtr<T>::element_type>::type
UniquePtr<T>::operator*() const {
    return *data_;
}

template<class T>
typename UniquePtr<T>::pointer UniquePtr<T>::operator->() const {
    return data_;
}
template<class T>
typename UniquePtr<T>::pointer UniquePtr<T>::get() const {
    return data_;
};

template<class T>
typename UniquePtr<T>::pointer UniquePtr<T>::release() {
    reset(nullptr);
}

template<class T>
void UniquePtr<T>::reset(UniquePtr<T>::pointer ptr) {
    auto to_delete = data_;
    data_ = ptr;
    delete to_delete;
}

template<class T>
void UniquePtr<T>::swap(UniquePtr& other) {
    std::swap(data_, other.data_);
}

template <class T> struct ControlBlock {
    T data;
    std::atomic_size_t shared_count;
    std::atomic_size_t weak_count;
    ControlBlock(): data(nullptr), shared_count(0), weak_count(0) {};
};


template<class T>
SharedPtr<T>::SharedPtr(SharedPtr<T>::pointer ptr): ctrl_block_(nullptr){
    if (ptr) {
        ctrl_block_ = new ControlBlock<decltype(ptr)>();
        ctrl_block_->data = ptr;
        ++ctrl_block_->shared_count;
    }
}

template<class T>
SharedPtr<T>::SharedPtr(SharedPtr&& other): ctrl_block_(other.ctrl_block_) {
    other.ctrl_block_ = nullptr;
}

template<class T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) {
    if (this == &other) return *this;
    reset();
    other.swap(*this);
    return *this;
}

template<class T>
SharedPtr<T>::SharedPtr(const SharedPtr& other): ctrl_block_(other.ctrl_block_) {
    if (ctrl_block_) ++ctrl_block_->shared_count;
}

template<class T>
SharedPtr<T>::SharedPtr(const WeakPtr<T>& other): ctrl_block_(other.ctrl_block_) {
    if (ctrl_block_) ++ctrl_block_->shared_count;
}

template<class T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other) {
    if (this == &other) return *this;
    SharedPtr temp(other);
    temp.swap(*this);
    return *this;
}

template<class T>
SharedPtr<T>::~SharedPtr() {
    reset();
}

template<class T>
typename std::add_lvalue_reference<typename SharedPtr<T>::element_type>::type SharedPtr<T>::operator*() const {
    return *(ctrl_block_->data);
}
template<class T>
typename SharedPtr<T>::pointer SharedPtr<T>::operator->() const {
    return ctrl_block_->data;
}

template<class T>
typename SharedPtr<T>::pointer SharedPtr<T>::get() const {
    return ctrl_block_ ? ctrl_block_->data : nullptr;
}

template<class T>
std::size_t SharedPtr<T>::use_count() const {
    return ctrl_block_->shared_count;
}

template<class T>
void SharedPtr<T>::reset(SharedPtr<T>::pointer ptr ) {
    if (ptr) {
        if (ctrl_block_) {
            delete ctrl_block_->data;
            ctrl_block_->data = ptr;
        }
        else {
            SharedPtr temp(ptr);
            temp.swap(*this);
        }
    } else if (ctrl_block_) {
        if (--ctrl_block_->shared_count == 0) {
            delete ctrl_block_->data;
            ctrl_block_->data = nullptr;

            if (ctrl_block_->weak_count == 0) {
                delete ctrl_block_;
            }
        }

        ctrl_block_ = nullptr;
    }
}

template<class T>
void SharedPtr<T>::swap(SharedPtr& other) {
    if (this == &other) return;

    std::swap(ctrl_block_, other.ctrl_block_);
}


template<class T>
WeakPtr<T>::WeakPtr(const SharedPtr<T>& other): ctrl_block_(other.ctrl_block_) {
    if (ctrl_block_) ++ctrl_block_->weak_count;
}

template<class T>
WeakPtr<T>::WeakPtr(WeakPtr&& other): ctrl_block_(other.ctrl_block_) {
    other.ctrl_block_ = nullptr;
}

template<class T>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr&& other) {
    if (this == &other) return *this;
    reset();
    other.swap(*this);
    return *this;
}

template<class T>
WeakPtr<T>::WeakPtr(const WeakPtr& other): ctrl_block_(other.ctrl_block_) {
    if (ctrl_block_) ++ctrl_block_->weak_count;
}

template<class T>
WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr& other) {
    if (this == &other) return *this;
    WeakPtr temp(other);
    temp.swap(*this);
    return *this;
}

template<class T>
WeakPtr<T>::~WeakPtr() {
    reset();
}

template<class T>
SharedPtr<T> WeakPtr<T>::lock() const {
    return SharedPtr<T>(*this);
}

template<class T>
bool WeakPtr<T>::expired() const {
    return use_count() == 0;
}

template<class T>
std::size_t WeakPtr<T>::use_count() const {
    if (ctrl_block_) return ctrl_block_->shared_count;
    return 0;
}

template<class T>
void WeakPtr<T>::reset() {
    if (ctrl_block_) {
        if (--ctrl_block_->weak_count == 0 and ctrl_block_->shared_count == 0) {
            delete ctrl_block_;
        }

        ctrl_block_ = nullptr;
    }
}

template<class T>
void WeakPtr<T>::swap(WeakPtr& other) {
    if (this == &other) return;

    std::swap(ctrl_block_, other.ctrl_block_);
}

}
