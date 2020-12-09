#include "../3rd_party/aids-patched.hpp"

struct Control_block {
    size_t ref_cnt{};
    size_t weak_cnt{};
};

template <typename T>
struct Shared_ptr {
    T *ptr{nullptr};
    Control_block *cb{nullptr};

    // https://stackoverflow.com/questions/49173654/why-deleted-copy-constructor-doesnt-let-to-use-other-constructor-with-polymorph
    // Shared_ptr<T>(const Shared_ptr<T>&) = delete;

    ~Shared_ptr() {
        cb->ref_cnt -= 1;
        if (cb->ref_cnt == 0) {
            free(cb);
            aids::destroy(ptr);

            cb = nullptr;
            ptr = nullptr;
        }
    }

    T *get() {
        return ptr;
    }

    size_t use_count() const {
        return cb->ref_cnt;
    }
};

template <typename T>
Shared_ptr<T> make_shared(T *dumb_ptr) {
    Shared_ptr<T> result{};

    result.cb = (Control_block*)malloc(sizeof(Control_block));

    result.ptr = dumb_ptr;
    result.cb->ref_cnt = 1;

    return result;
}

template <typename T>
Shared_ptr<T> copy_shared_ptr(Shared_ptr<T> &sp) {
    Shared_ptr<T> result{};

    result.ptr = sp.ptr;    
    result.cb = sp.cb;    
    result.cb->ref_cnt += 1;

    return result;
}