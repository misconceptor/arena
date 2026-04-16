#include <iostream>
#include <exception>
#include <algorithm>
#include <cstdlib>
//delete copy ctor and copy =
class arena{
private:
    char *begin, *offset, *end;
    bool owns_memory = false;
public:
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
    char* align(char* _offset, size_t alignment){
        size_t addr = (size_t)_offset;
        addr = (addr + alignment - 1) & ~(alignment - 1);
        _offset = (char*)addr;
        return _offset;
    }
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
    ~arena(){
        if(owns_memory && begin) {
            std::cout << "dtor" << std::endl;
            free(begin);
        }
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