#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../include/doctest.h"
#include "../include/MyContainer.hpp"
#include <sstream>
#include <cmath>

using namespace myns;

// ========================= CUSTOM TYPE =========================

// A custom struct to test generic container with complex types
struct Point {
    int x, y;
    int sum() const { return x + y; }  // Helper function for testing access

    // Define equality and ordering operators
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
    bool operator<(const Point& other) const { return (x < other.x) || (x == other.x && y < other.y); }

    // Stream output for test messages
    friend std::ostream& operator<<(std::ostream& os, const Point& p) {
        return os << "(" << p.x << "," << p.y << ")";
    }
};

// ========================= ELEMENT ACCESS TESTS =========================

TEST_CASE("Element access with MyContainer<std::string>") {
    MyContainer<std::string> c;
    c.add("alpha");
    c.add("beta");
    c.add("gamma");

    // Check indexing access (non-const)
    CHECK(c[0] == "alpha");
    CHECK(c[1] == "beta");

    // Modify using indexing
    c[2] = "delta";
    CHECK(c[2] == "delta");

    // Use at() for bounds-checked access
    CHECK(c.at(0) == "alpha");
    CHECK(c.at(2) == "delta");

    // Ensure exception is thrown when accessing out-of-range
    CHECK_THROWS_AS(c.at(5), std::out_of_range);
}

TEST_CASE("Element access with MyContainer<Point>") {
    MyContainer<Point> c;
    c.add({1, 2});
    c.add({3, 4});
    c.add({5, 6});

    // Access members directly
    CHECK(c[0].x == 1);
    CHECK(c[1].y == 4);
    CHECK(c.at(2).x == 5);

    // Modify through access
    c[1].x = 99;
    CHECK(c[1].x == 99);

    // Check bounds violation
    CHECK_THROWS_AS(c.at(10), std::out_of_range);
}

TEST_CASE("Pointer-based access using MyContainer<Point*> and operator->") {
    Point* p1 = new Point{10, 20};
    Point* p2 = new Point{5, 5};

    MyContainer<Point*> c;
    c.add(p1);
    c.add(p2);

    // Use -> to access fields of pointer types
    CHECK(c[0]->x == 10);
    CHECK(c[1]->y == 5);
    CHECK(c[0]->sum() == 30);
    CHECK(c[1]->sum() == 10);

    // Modify data through pointer
    c[0]->x = 42;
    CHECK(c[0]->x == 42);
    CHECK(p1->x == 42);
    CHECK(c[0]->sum() == 62);
    CHECK(p1->sum() == 62);

    // Manual memory cleanup
    delete p1;
    delete p2;
}

TEST_CASE("Modifying pointed-to objects via iterators when T = Point*") {
    // Allocate Points dynamically
    Point* p1 = new Point{1, 2};
    Point* p2 = new Point{5, 4};
    Point* p3 = new Point{3, 6};

    MyContainer<Point*> c;
    c.add(p1);
    c.add(p2);
    c.add(p3);

    // Modify all x values using an iterator
    for (auto ptr : c.ascending_order()) {
        ptr->x += 100;  // Should modify the actual objects, because ptr is a pointer
    }

    // Verify that original pointers reflect the changes
    CHECK(p1->x == 101);
    CHECK(p2->x == 105);
    CHECK(p3->x == 103);

    // Clean up
    delete p1;
    delete p2;
    delete p3;
}


// ========================= BASIC OPERATIONS =========================

TEST_CASE("Add elements and check size") {
    MyContainer<int> c;
    CHECK(c.size() == 0);

    c.add(5);
    c.add(10);
    c.add(5);  // Allow duplicates
    CHECK(c.size() == 3);
}

TEST_CASE("Remove elements - existing and non-existing") {
    MyContainer<int> c;
    c.add(1);
    c.add(2);
    c.add(1);
    c.remove(1);  // should remove all 1s
    CHECK(c.size() == 1);
    CHECK(c.get_data()[0] == 2);
    CHECK_THROWS_AS(c.remove(3), std::runtime_error);
}

TEST_CASE("Remove from empty container") {
    MyContainer<int> c;
    CHECK_THROWS(c.remove(100));
}

TEST_CASE("Container with strings") {
    MyContainer<std::string> c;
    c.add("hello");
    c.add("world");
    CHECK(c.size() == 2);

    std::ostringstream oss;
    oss << c;  // Test operator<< overload
    CHECK(oss.str() == "{hello, world}");
}

// ========================= ITERATORS =========================

// Test ascending order traversal of integers
TEST_CASE("AscendingOrder iterator") {
    MyContainer<int> c;
    for (int x : {7, 3, 9, 1, 5}) c.add(x);
    std::vector<int> expected = {1, 3, 5, 7, 9};  // Expected sorted result
    std::vector<int> actual;
    for (auto it : c.ascending_order()) actual.push_back(it);
    CHECK(actual == expected);
}

// Test descending order traversal of integers
TEST_CASE("DescendingOrder iterator") {
    MyContainer<int> c;
    for (int x : {2, 4, 1, 3}) c.add(x);
    std::vector<int> expected = {4, 3, 2, 1};  // Descending sort
    std::vector<int> actual;
    for (auto it : c.descending_order()) actual.push_back(it);
    CHECK(actual == expected);
}

// Test alternating traversal from both ends (side-cross)
TEST_CASE("SideCrossOrder with odd number of elements") {
    MyContainer<int> c;
    for (int x : {1, 3, 5, 7, 9}) c.add(x);
    std::vector<int> expected = {1, 9, 3, 7, 5};  // Alternating from sides inward
    std::vector<int> actual;
    for (auto it : c.sidecross_order()) actual.push_back(it);
    CHECK(actual == expected);
}

// Test reverse order and insertion order traversal
TEST_CASE("ReverseOrder and insertion order") {
    MyContainer<int> c;
    for (int x : {10, 20, 30}) c.add(x);
    std::vector<int> rev = {30, 20, 10};  // Reverse of insertion
    std::vector<int> ord = {10, 20, 30};  // Original insertion order

    std::vector<int> actual_rev, actual_ord;
    for (auto it : c.reverse_order()) actual_rev.push_back(it);
    for (auto it : c.order()) actual_ord.push_back(it);

    CHECK(actual_rev == rev);
    CHECK(actual_ord == ord);
}

// Test middle-out traversal (from center to outer elements)
TEST_CASE("MiddleOutOrder - odd and even") {
    {
        MyContainer<int> c;
        for (int x : {1, 2, 3, 4, 5}) c.add(x);
        std::vector<int> expected = {3, 2, 4, 1, 5};  // Middle element first
        std::vector<int> actual;
        for (auto it : c.middle_out_order()) actual.push_back(it);
        CHECK(actual == expected);
    }
    {
        MyContainer<int> c;
        for (int x : {10, 20, 30, 40}) c.add(x);
        std::vector<int> expected = {30, 20, 40, 10};  // Middle two first
        std::vector<int> actual;
        for (auto it : c.middle_out_order()) actual.push_back(it);
        CHECK(actual == expected);
    }
}

// === EMPTY CONTAINER CASE ===

// All iterators on an empty container should have begin() == end()
TEST_CASE("All iterators on empty container") {
    MyContainer<int> c;
    CHECK(c.ascending_order().begin() == c.ascending_order().end());
    CHECK(c.descending_order().begin() == c.descending_order().end());
    CHECK(c.sidecross_order().begin() == c.sidecross_order().end());
    CHECK(c.reverse_order().begin() == c.reverse_order().end());
    CHECK(c.order().begin() == c.order().end());
    CHECK(c.middle_out_order().begin() == c.middle_out_order().end());
}

// Same as above but with a string container
TEST_CASE("Begin equals end on empty for all iterators") {
    MyContainer<std::string> c;
    CHECK(c.ascending_order().begin() == c.ascending_order().end());
    CHECK(c.descending_order().begin() == c.descending_order().end());
    CHECK(c.sidecross_order().begin() == c.sidecross_order().end());
    CHECK(c.reverse_order().begin() == c.reverse_order().end());
    CHECK(c.order().begin() == c.order().end());
    CHECK(c.middle_out_order().begin() == c.middle_out_order().end());
}

// === STRESS TEST ===

// Stress test with large number of elements to check iterator stability and performance
TEST_CASE("Stress test: 1000 elements") {
    MyContainer<int> c;
    for (int i = 0; i < 1000; ++i) c.add(i);
    size_t count = 0;
    for (auto x : c.ascending_order()) {
        (void)x;  // suppress unused warning
        ++count;
    }
    CHECK(count == 1000);
}

// === DOUBLE TYPE TESTS ===

// Test container behavior with double values and ascending iterator
TEST_CASE("Container with doubles") {
    MyContainer<double> c;
    c.add(1.1);
    c.add(3.14);
    c.add(-2.0);
    std::vector<double> expected = {-2.0, 1.1, 3.14};  // Expected sorted order
    std::vector<double> actual;
    for (auto it : c.ascending_order()) actual.push_back(it);
    CHECK(actual == expected);
}

// === CHAR TYPE TESTS ===

// Test container behavior with characters and ascending iterator
TEST_CASE("Container with chars") {
    MyContainer<char> c;
    for (char ch : {'z', 'b', 'd', 'a'}) c.add(ch);
    std::vector<char> expected = {'a', 'b', 'd', 'z'};  // Sorted alphabetically
    std::vector<char> actual;
    for (auto it : c.ascending_order()) actual.push_back(it);
    CHECK(actual == expected);
}

// === CUSTOM TYPE TESTS ===

// Test container with user-defined Point type and ascending order
TEST_CASE("Container with custom Point type") {
    MyContainer<Point> c;
    c.add({1, 2});
    c.add({0, 0});
    c.add({1, 1});
    std::vector<Point> expected = { {0,0}, {1,1}, {1,2} };  // Sorted by x then y
    std::vector<Point> actual;
    for (auto it : c.ascending_order()) actual.push_back(it);
    CHECK(actual == expected);
}

// Ensure points with same x but different y are sorted correctly
TEST_CASE("Points with equal x but different y") {
    MyContainer<Point> c;
    c.add({1, 5});
    c.add({1, 3});
    c.add({1, 4});

    std::vector<Point> expected = {{1,3}, {1,4}, {1,5}};  // Sort by y since x is equal
    std::vector<Point> actual;
    for (auto x : c.ascending_order()) actual.push_back(x);

    CHECK(actual == expected);
}

// === STRINGS TESTS - ADD, REMOVE AND ITERATORS ===

// Comprehensive test: add/remove strings, handle duplicates and use all iterators
TEST_CASE("String container - Comprehensive iterator and edge case test") {
    MyContainer<std::string> c;

    // Add several strings including duplicates and an empty string
    c.add("delta");
    c.add("alpha");
    c.add("echo");
    c.add("");         // empty string
    c.add("bravo");
    c.add("alpha");    // duplicate
    c.add("charlie");

    // Remove all occurrences of "alpha"
    c.remove("alpha");

    // After removal, the container should contain: delta, echo, "", bravo, charlie
    CHECK(c.size() == 5);

    // Collect iterator results into separate vectors for comparison
    std::vector<std::string> asc, desc, cross, rev, ord, mid;

    for (auto x : c.ascending_order())     asc.push_back(x);
    for (auto x : c.descending_order())    desc.push_back(x);
    for (auto x : c.sidecross_order())     cross.push_back(x);
    for (auto x : c.reverse_order())       rev.push_back(x);
    for (auto x : c.order())               ord.push_back(x);
    for (auto x : c.middle_out_order())    mid.push_back(x);

    // Define expected values based on logic of each iterator
    std::vector<std::string> expected_order = {
        "delta", "echo", "", "bravo", "charlie"
    };

    std::vector<std::string> expected_reverse = {
        "charlie", "bravo", "", "echo", "delta"
    };

    std::vector<std::string> expected_asc = {
        "", "bravo", "charlie", "delta", "echo"
    };

    std::vector<std::string> expected_desc = {
        "echo", "delta", "charlie", "bravo", ""
    };

    std::vector<std::string> expected_cross = {
        "", "echo", "bravo", "delta", "charlie"
    };

    std::vector<std::string> expected_middle = {
        "", "echo", "bravo", "delta", "charlie"
    };

    // Verify that each iterator produces the expected result
    CHECK(ord   == expected_order);
    CHECK(rev   == expected_reverse);
    CHECK(asc   == expected_asc);
    CHECK(desc  == expected_desc);
    CHECK(cross == expected_cross);
    CHECK(mid   == expected_middle);

    // Check that none of the iterators throw when iterated
    CHECK_NOTHROW(for (auto x : c.ascending_order()) (void)x;);
    CHECK_NOTHROW(for (auto x : c.descending_order()) (void)x;);
    CHECK_NOTHROW(for (auto x : c.sidecross_order()) (void)x;);
    CHECK_NOTHROW(for (auto x : c.reverse_order()) (void)x;);
    CHECK_NOTHROW(for (auto x : c.order()) (void)x;);
    CHECK_NOTHROW(for (auto x : c.middle_out_order()) (void)x;);
}

// Test string sorting with case sensitivity (based on ASCII)
// ASCII order: uppercase letters < lowercase letters
TEST_CASE("String iterator behavior with case sensitivity") {
    MyContainer<std::string> c;
    c.add("apple");
    c.add("Banana");
    c.add("APPLE");
    c.add("banana");

    std::vector<std::string> expected_asc = {"APPLE", "Banana", "apple", "banana"};
    std::vector<std::string> actual;
    for (auto x : c.ascending_order()) actual.push_back(x);

    CHECK(actual == expected_asc);
}

// Test edge cases with empty strings, spaces, and special characters
// Ensures remove() works and sorting behaves as expected
TEST_CASE("Strings with empty and special characters") {
    MyContainer<std::string> c;
    c.add("");
    c.add(" ");
    c.add("@@@");
    c.add("hello");
    c.add("world");
    c.add(" ");
    c.add("");

    CHECK(c.size() == 7);

    // Remove all occurrences of space string
    c.remove(" ");
    CHECK(c.size() == 5);

    // Test stream output
    std::ostringstream oss;
    oss << c;
    CHECK(oss.str() == "{, @@@, hello, world, }");

    // Check ascending iterator result
    std::vector<std::string> expected = {"", "", "@@@", "hello", "world"};
    std::vector<std::string> actual;
    for (auto x : c.ascending_order()) actual.push_back(x);
    CHECK(actual == expected);
}

// Verify that all iterators work properly with a single string
TEST_CASE("Iterators with a single string element") {
    MyContainer<std::string> c;
    c.add("single");

    std::vector<std::string> expected = {"single"};
    std::vector<std::string> actual;

    // Ascending
    for (auto x : c.ascending_order()) actual.push_back(x);
    CHECK(actual == expected);

    // Descending
    actual.clear();
    for (auto x : c.descending_order()) actual.push_back(x);
    CHECK(actual == expected);

    // Side-cross
    actual.clear();
    for (auto x : c.sidecross_order()) actual.push_back(x);
    CHECK(actual == expected);

    // Reverse
    actual.clear();
    for (auto x : c.reverse_order()) actual.push_back(x);
    CHECK(actual == expected);

    // Insertion order
    actual.clear();
    for (auto x : c.order()) actual.push_back(x);
    CHECK(actual == expected);

    // Middle-out
    actual.clear();
    for (auto x : c.middle_out_order()) actual.push_back(x);
    CHECK(actual == expected);
}
