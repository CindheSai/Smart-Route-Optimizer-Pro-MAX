#pragma once
#include <stdexcept>

// ============================================================
// Custom Stack (dynamic array) — used by DFS
// ============================================================

template <typename T>
class Stack {
private:
    T*  data;
    int top_, cap;

    void resize() {
        int newCap = cap * 2;
        T*  newData = new T[newCap];
        for (int i = 0; i <= top_; i++) newData[i] = data[i];
        delete[] data;
        data = newData;
        cap  = newCap;
    }

public:
    Stack(int initCap = 64) : top_(-1), cap(initCap) {
        data = new T[cap];
    }
    ~Stack() { delete[] data; }

    void push(const T& val) {
        if (top_ + 1 == cap) resize();
        data[++top_] = val;
    }

    T pop() {
        if (isEmpty()) throw std::underflow_error("Stack is empty");
        return data[top_--];
    }

    T peek() const {
        if (isEmpty()) throw std::underflow_error("Stack is empty");
        return data[top_];
    }

    bool isEmpty() const { return top_ == -1; }
    int  getSize() const { return top_ + 1; }

    void clear() { top_ = -1; }
};
