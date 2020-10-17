#include <stdint.h>
#include <list>
#include <iostream>

template<size_t Size>
class Chunk {
 public:
    Chunk() {
        free_ = data_ = new uint8_t[Size];
    }

    Chunk(const Chunk& ) = delete;
    Chunk operator=(const Chunk&) = delete;

    Chunk(Chunk&& other): data_(nullptr), free_(nullptr) {

        std::swap(data_, other.data_);
        std::swap(free_, other.free_);
    }

    Chunk& operator=(Chunk&& other) {
        if (this == &other) return *this;

        std::swap(data_, other.data_);
        std::swap(free_, other.free_);

        return *this;
    }

    uint8_t* allocate(size_t size) {
        if (Size - (free_ - data_) < size) return nullptr;
        free_ += size;
        return free_;
    }

    ~Chunk() {
        delete [] data_;
    }
 private:
    uint8_t* data_, *free_;
};

template <class T>
class MyAllocator {
    static constexpr int CHUNK_SIZE = 1'000'000;
    using ChunkContainer = std::list<Chunk<CHUNK_SIZE>>;
 public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference =T&;
    using const_reference = const T&;

    MyAllocator(): control_block_(new ControlBlock{})  {
    }

    MyAllocator(const MyAllocator& other): control_block_(other.control_block_) {
        control_block_->ref_counter += 1;
    }

    MyAllocator& operator=(const MyAllocator& other) {
        if (this == &other) return *this;

        if (--(control_block_->ref_counter_) == 0) {
            control_block_->chunks_.clear();
            delete control_block_;
        }

        control_block_ = other.control_block_;
        control_block_->ref_counter += 1;

        return *this;
    }

    ~MyAllocator() {
        if (--(control_block_->ref_counter) == 0) {
            control_block_->chunks.clear();
            delete control_block_;
        }
    }

    T* allocate( std::size_t n ) {
        auto to_allocate = n * sizeof(T);
        if (to_allocate > CHUNK_SIZE) throw std::bad_alloc();

        for (auto& chunk: control_block_->chunks) {
            if (auto res = chunk.allocate(to_allocate); res)
                return (T*)res;
        }

        control_block_->chunks.emplace_back();
        return (T*) control_block_->chunks.back().allocate(to_allocate);
    }

    void deallocate( T* p, std::size_t n ) {
        // do nothing
    }

    template<class... Args >
    void construct( T* p, Args&&... args ) {
        ::new((void *)p) T(std::forward<Args>(args)...);
    }

    void destroy(T* p) {
        p->~T();
    }

 private:
    struct ControlBlock {
      int ref_counter = 1;
      ChunkContainer chunks;
    };

    ControlBlock* control_block_;
};

#include <vector>
int main() {
    std::vector<int, MyAllocator<int>> t = {1};
    t.push_back(4);
    for (auto& i: t) std::cout<<i<<std::endl;
    return 0;
}
