#include "linq.hpp"
#include <cassert>
#include <vector>
#include <iostream>

//    for (auto i = res.begin(); i != res.end(); ++i) {
//        std::cout << *i << " " << std::endl;
//    }

using linq::from;

void from_to_vector() {
    std::vector<int> xs = { 1, 2, 3 };
    std::vector<int> res = from(xs.begin(), xs.end()).to_vector();
    assert(res == xs);
}

void from_drop() {
    std::vector<int> xs = { 1, 2, 3 };
    std::vector<int> res = from(xs.begin(), xs.end()).drop(2).to_vector();
    std::vector<int> expected = { 3 };
    assert(res == expected);
}

void from_take() {
    std::vector<int> xs = { 1, 2, 3 };
    std::vector<int> res = from(xs.begin(), xs.end()).take(2).to_vector();
    std::vector<int> expected = { 1, 2 };
    assert(res == expected);
}

void from_select() {
    std::vector<int> xs = { 1, 2, 3 };
    std::vector<long> res = from(xs.begin(), xs.end()).select<long>([](int x) { return long(x); }).to_vector();
    std::vector<long> expected = { 1, 2, 3 };
    assert(res == expected);
}

void from_until() {
    std::vector<int> xs = { 1, 2, 3, -1, 4, 5 };
    std::vector<int> res = from(xs.begin(), xs.end()).until_eq(-1).to_vector();
    std::vector<int> expected = { 1, 2, 3 };
    assert(res == expected);
}

void from_where() {
    std::vector<int> xs = { 1, 6, 2, 1, 1, 5, 1 };
    std::vector<int> res = from(xs.begin(), xs.end()).where_neq(1).to_vector();
    std::vector<int> expected = { 6, 2, 5 };
    assert(res == expected);
}

int main() {
    from_to_vector();
    from_drop();
    from_take();
    from_select();
    from_until();
    from_where();
    return 0;
}



