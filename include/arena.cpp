#include <iostream>
#include <exception>
#include <algorithm>
#include <cstdlib>


//destructor list
//cross-arena copy

class arena{
private:
    char *begin, *offset, *end;
    bool owns_memory = false;
public:
    char* getBegin() const {
        return begin;
    }
    size_t size() const {
        return end - begin;
    }
    size_t used() const {
        return offset - begin;
    }
    bool is_owner() const {
        return owns_memory;
    }
    bool is_valid() const {
        return begin != nullptr;
    }
    size_t remaining_size() const {
        return end - offset;
    }

    arena(const  arena&) = delete;

    arena& operator=(const arena&) = delete;

    arena(size_t size){
        void* buf = malloc(size);
        if (buf) {
            std::cout << "allocation succeeded, size: " << size << '\n';
            owns_memory = true;
            offset = begin = (char*)buf; 
            end = begin + size;
        } else {
            std::cout << "allocation failed\n";
            owns_memory = false;
            begin = end = offset = nullptr;
            throw std::runtime_error("buffer is null");
        }
    }
    arena(void* buf, size_t size){ //does not own memory, so can't free it in dtor
        owns_memory = false;
        if (buf) {
            std::cout << "allocation succeeded, size: " << size << std::endl;
            offset = begin = (char*) buf;
            end = begin + size;
        } else {
            std::cout << "allocation failed" << std::endl;
            begin = end = offset = nullptr;
            throw std::invalid_argument("null buffer");
        }
    }
    arena (arena&& other) noexcept {
        begin = other.begin;
        end = other.end;
        offset = other.offset;
        owns_memory = other.owns_memory;
        other.begin = other.end = other.offset = nullptr;
        other.owns_memory = false;
    }
    arena& operator=(arena&& other) noexcept {
        if(this == &other) return *this;

        if(this->owns_memory)  this->reset();

        begin = other.begin;
        end = other.end;
        offset = other.offset;
        owns_memory = other.owns_memory;

        other.begin = other.end = other.offset = nullptr;
        other.owns_memory = false;

        return *this;
    }
    ~arena(){
        if(owns_memory && begin) {
            std::cout << "arena dtor" << std::endl;
            free(begin);
        }
    }
    char* align(char* _offset, size_t alignment){
        size_t addr = (size_t)_offset;
        addr = (addr + alignment - 1) & ~(alignment - 1);
        _offset = (char*)addr;
        return _offset;
    }
    void* allocate(size_t size, size_t alignment) {
        char* aligned_address = align(offset, alignment);
        if(aligned_address + size > end) {
            throw std::runtime_error("not enough space");
        }
        char* res = aligned_address;
        offset = aligned_address + size;
        return res;
    }
    void reset() {
        offset = begin;
    }
    void release() {
        if(!owns_memory) throw std::runtime_error("arena doesn't own memory");
        if(begin) {
            free(begin);
            begin = end = offset = nullptr;
            owns_memory = false;
        }
    }
};


//https://habr.com/ru/companies/otus/articles/988086/
//https://protobuf.dev/reference/cpp/arenas/