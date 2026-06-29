#pragma once
#include <stdexcept>

// ============================================================
// Custom Queue (circular array) — used by BFS
// ============================================================

template <typename T>
class Queue {
private:
    static const int INITIAL_CAP = 64;
    T*   data;
    int  front_, rear_, size_, cap;

    void resize() {
        int newCap = cap * 2;
        T*  newData = new T[newCap];
        for (int i = 0; i < size_; i++)
            newData[i] = data[(front_ + i) % cap];
        delete[] data;
        data   = newData;
        front_ = 0;
        rear_  = size_;
        cap    = newCap;
    }

public:
    Queue() : front_(0), rear_(0), size_(0), cap(INITIAL_CAP) {
        data = new T[cap];
    }
    ~Queue() { delete[] data; }

    void enqueue(const T& val) {
        if (size_ == cap) resize();
        data[rear_] = val;
        rear_ = (rear_ + 1) % cap;
        size_++;
    }

    T dequeue() {
        if (isEmpty()) throw std::underflow_error("Queue is empty");
        T val = data[front_];
        front_ = (front_ + 1) % cap;
        size_--;
        return val;
    }

    T peek() const {
        if (isEmpty()) throw std::underflow_error("Queue is empty");
        return data[front_];
    }

    bool isEmpty() const { return size_ == 0; }
    int  getSize() const { return size_; }

    void clear() { front_ = rear_ = size_ = 0; }
};
