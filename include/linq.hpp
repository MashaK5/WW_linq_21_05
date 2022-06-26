#pragma once

#include <utility>
#include <vector>
#include <functional>


namespace linq {
namespace impl {

template<typename T, typename Iter>
class range_enumerator;
template<typename T>
class drop_enumerator;
template<typename T>
class take_enumerator;
template<typename T, typename U, typename F>
class select_enumerator;
template<typename T, typename F>
class until_enumerator;
template<typename T, typename F>
class where_enumerator;


template<typename T>
class enumerator {
public:
    enumerator() = default;
    virtual ~enumerator() = default;

    virtual const T& operator*() = 0; // Получает текущий элемент.
    virtual void operator++() = 0;  // Переход к следующему элементу
    virtual explicit operator bool() = 0;  // Возвращает true, если есть текущий элемент

    auto drop(int count) {
        return drop_enumerator<T>(*this, count);
    }

    auto take(int count) {
        return take_enumerator<T>(*this, count);
    }

    template<typename U = T, typename F>
    auto select(F func) {
        return select_enumerator<T, U, F>(*this, func);
    }

    template<typename F>
    auto until(F func) {
        return until_enumerator<T, F>(*this, func);
    }

    auto until_eq(const T& ref) {
        auto func = std::function([ref](T elem) { return ref == elem; });
        return until_enumerator<T, decltype(func)>(*this, func);
    }

    template<typename F>
    auto where(F func) {
        return where_enumerator<T, F>(*this, func);
    }

    auto where_neq(const T& ref) {
        auto func = std::function([ref](T elem) { return ref != elem; });
        return where_enumerator<T, decltype(func)>(*this, func);
    }

    std::vector<T> to_vector() {
        std::vector<T> res;
        while (operator bool()) {
            res.push_back(operator*());
            operator++();
        }
        return res;
    }

    template<typename Iter>
    void copy_to(Iter it) {
        while (operator bool()) {
            *it = operator*();
            operator++();
            ++it;
        }
    }
};


template<typename T, typename Iter>
class range_enumerator: public enumerator<T> {
public:
    range_enumerator(Iter begin, Iter end) : _begin(begin), _end(end) {}

    const T& operator*() override {
        return *_begin;
    }

    void operator++() override {
        ++_begin;
    }

    explicit operator bool() override {
        return _begin != _end;
    }

private:
    Iter _begin, _end;
};


template<typename T>
class drop_enumerator: public enumerator<T> {
public:
    drop_enumerator(enumerator<T>& parent, int count) : _parent(parent), _count_leave(count) {}

    const T& operator*() override {
        do_skip();
        return *_parent;
    }

    void operator++() override {
        do_skip();
        ++_parent;
    }

    explicit operator bool() override {
        do_skip();
        return static_cast<bool>(_parent);
    }

private:
    void do_skip() {
        while (_count_leave > 0 && _parent) {
            --_count_leave;
            ++_parent;
        }
    }

private:
    enumerator<T>& _parent;
    int _count_leave;
};


template<typename T>
class take_enumerator: public enumerator<T> {
public:
    take_enumerator(enumerator<T>& parent, int count) : _parent(parent), _count_leave(count) {}

    const T& operator*() override {
        return *_parent;
    }

    void operator++() override {
        --_count_leave;
        ++_parent;
    }

    explicit operator bool() override {
        return static_cast<bool>(_parent) && _count_leave > 0;
    }

private:
    enumerator<T>& _parent;
    int _count_leave;
};


template<typename T, typename U, typename F>
class select_enumerator: public enumerator<U> {
public:
    select_enumerator(enumerator<T>& parent, F func) : _parent(parent), _func(std::move(func)) {}

    const U& operator*() override {
        _new_value = _func(*_parent);
        return _new_value;
    }

    void operator++() override {
        ++_parent;
    }

    explicit operator bool() override {
        return static_cast<bool>(_parent);
    }

private:
    enumerator<T>& _parent;
    F _func;
    U _new_value;
};


template<typename T, typename F>
class until_enumerator: public enumerator<T> {
public:
    until_enumerator(enumerator<T> &parent, F predicate) : _parent(parent), _pred(std::move(predicate)) {}

    const T& operator*() override {
        return *_parent;
    }

    void operator++() override {
        ++_parent;
    }

    explicit operator bool() override {
        return static_cast<bool>(_parent) && !_pred(*_parent);
    }

private:
    enumerator<T>& _parent;
    F _pred;
};


template<typename T, typename F>
class where_enumerator: public enumerator<T> {
public:
    where_enumerator(enumerator<T> &parent, F predicate) : _parent(parent), _pred(std::move(predicate)) {}

    const T& operator*() override {
        return *_parent;
    }

    void operator++() override {
        ++_parent;
    }

    explicit operator bool() override {
        do_skip();
        return static_cast<bool>(_parent) && _pred(*_parent);
    }

private:
    void do_skip() {
        while (_parent && !_pred(*_parent)) {
            ++_parent;
        }
    }

private:
    enumerator<T>& _parent;
    F _pred;
};

} // namespace impl


template<typename Iter>
auto from(Iter begin, Iter end) {
    using value_type = typename std::iterator_traits<Iter>::value_type;
    return impl::range_enumerator<value_type, Iter>(begin, end);
}

} // namespace linq

