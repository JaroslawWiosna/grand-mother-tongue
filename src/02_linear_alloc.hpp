#pragma once

#include "../3rd_party/aids-patched.hpp"

constexpr unsigned long long operator ""_Kb (unsigned long long s)
{
    return s * 1024;
}
constexpr unsigned long long operator ""_Mb (unsigned long long s)
{
    return s * 1024_Kb;
}
constexpr unsigned long long operator ""_Gb (unsigned long long s)
{
    return s * 1024_Mb;
}

template <size_t Capacity = 640_Kb>
struct Region
{
    size_t size = 0;
    uint8_t data[Capacity];
};

template <size_t Capacity>
void *alloc(Region<Capacity> *region, size_t size)
{
    if (not (size + region->size <= Capacity)) {
        aids::panic(" Not enough memory allocated upfront. \n",
            " Allocated: ", Capacity, '\n',
            " In use:    ", region->size,
            "...so allocating additional ", size, " is not possible");
    }    
    void *result = region->data + region->size;
    region->size += size;
    return result;
}

// Region region{};

/// 

// NOTE: Stands for dynamic region
struct DynRegion
{
    size_t capacity{640_Kb};
    size_t size = 0;
    uint8_t *data;

    DynRegion() {
        data = new uint8_t[capacity];
    }
    DynRegion(size_t cap) {
        capacity = cap;
        data = new uint8_t[capacity];
    }
    ~DynRegion() {
        delete[] data;
    }
};

void *alloc(DynRegion *region, size_t size)
{
    if (not (size + region->size <= region->capacity)) {
        aids::panic(" Not enough memory allocated in DynRegion. \n",
            " Allocated: ", region->capacity, '\n',
            " In use:    ", region->size,
            "...so allocating additional ", size, " is not possible");
    }
    void *result = region->data + region->size;
    region->size += size;
    return result;
}

DynRegion region{1_Gb};
