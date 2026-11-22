// BetterVector/BetterVector.hpp

#pragma once

#ifndef MMYMYS_TOOLS_BETTER_VECTOR_HPP
#define MMYMYS_TOOLS_BETTER_VECTOR_HPP

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <chrono>

namespace tool {
    template <typename T>
    class BetterVector {
    private:
        T* data;
        size_t len;
        size_t cap;

        // 内部扩容逻辑
        void resize(size_t newCap) {
            T* newData = (T*)std::realloc(data, newCap * sizeof(T));
            if (!newData) throw std::bad_alloc();
            data = newData;
            cap = newCap;
            // std::cout << "扩容成功！\n";
            // std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

    public:
        BetterVector() : data(nullptr), len(0), cap(0) {}

        // 析构函数：RAII 自动释放
        ~BetterVector() {
            if (data) std::free(data);
        }

        // 禁止拷贝
        BetterVector(const BetterVector&) = delete;
        BetterVector& operator=(const BetterVector&) = delete;

        // 支持移动
        BetterVector(BetterVector&& other) noexcept
            : data(other.data), len(other.len), cap(other.cap) {
            other.data = nullptr;
            other.len = 0;
            other.cap = 0;
        }

        void reserve(size_t newCap) {
            if (newCap <= cap) return; // 已经够大了就不管
            T* newData = (T*)std::realloc(data, newCap * sizeof(T));
            if (!newData) throw std::bad_alloc();
            data = newData;
            cap = newCap;
        }

        void push_back(const T& val) {
            if (len >= cap) {
                size_t newCap = cap == 0 ? 4 : cap + (cap >> 1);
                resize(newCap);
            }
            data[len++] = val;
        }

        void pop_back() {
            if (len > 0) {
                len--;
                if (cap > 4 && len <= cap / 4) {
                    resize(cap / 2);
                }
            }
        }

        T& operator[](size_t index) {
            if (index >= len) {
                std::cerr << "\033[31m[BetterVector 你犯法了！]\033[0m 越界访问 Index: " << index << " (Size: " << len << ")\n";
                std::abort(); // 直接扬了，不惯着
            }
            return data[index];
        }

        size_t size() const { return len; }
        size_t capacity() const { return cap; }
        bool empty() const { return len == 0; }
    };

} // namespace mytools

#endif // MMYMYS_TOOLS_BETTER_VECTOR_HPP