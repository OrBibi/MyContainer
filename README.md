# MyContainer Project - C++ Generic Container with Custom Iterators

`MyContainer` is a generic C++ container that supports dynamic insertion, removal, indexed access, and six custom read-only iterators for various traversal orders. It works with any type `T` that supports `==` and `<`, and provides safe, const-based iteration along with standard access methods and stream output support.

---

## 📁 Project Structure

```
MyContainerProject/
├── include/                   # Header file MyContainer.hpp (fully self-contained)
├── test/                      # Unit tests using Doctest
├── main/                      # Demo program
├── Makefile                   # Build instructions
└── README.md                  # This documentation file
```

---

## 🛠️ Building & Running

```bash
make test        # Compile and run unit tests
make valgrind    # Run Valgrind to check for memory leaks
make Main        # Build demo executable
make clean       # Clean object and binary files
```

---

## ✅ Features

### MyContainer<T>

A generic container that supports:

* `add(const T&)` – Add an element
* `remove(const T&)` – Remove all occurrences (throws if not found)
* `size()` – Returns current size
* `get_data()` – Provides read-only access to internal vector
* `at(size_t)` / `operator[](size_t)` – Index-based access (const & non-const versions)

  * `at()` provides bounds-checked access and throws `std::out_of_range`
  * `operator[]` provides direct (unchecked) access, similar to `std::vector`
* `operator<<` – Prints container in `{a, b, c}` format

---

## 🔁 Iterators

Example usage:

```cpp
MyContainer<int> c;
c.add(4);
c.add(2);
c.add(7);
for (auto x : c.ascending_order()) {
    std::cout << x << " "; // Output: 2 4 7
}
```

| Iterator Name     | Description                                        |
| ----------------- | -------------------------------------------------- |
| `AscendingOrder`  | From smallest to largest                           |
| `DescendingOrder` | From largest to smallest                           |
| `SideCrossOrder`  | Smallest, largest, 2nd smallest, 2nd largest, etc. |
| `ReverseOrder`    | From last inserted to first                        |
| `Order`           | In original insertion order                        |
| `MiddleOutOrder`  | Start from middle, then alternate outward          |

Each iterator implements:

* `begin()`, `end()`
* `operator*`, `operator++`, `operator!=`

All iterators are **read-only** and return `const T&`.

⚠️ However, when `T` is a pointer type (e.g., `Point*`), it is possible to modify the object being pointed to via the iterator:

```cpp
for (auto ptr : container.ascending_order()) {
    ptr->x += 10;  // Allowed: modifies the object, not the pointer
}
```
This is because the iterator protects the pointer itself from being reassigned, but does not prevent modifying the data it points to.

---

## 🔐 Why are iterators read-only?

Iterators return `const T&` to:

* Prevent accidental mutation of container elements
* Ensure integrity when iterators are based on sorted/rearranged views
* Match assignment requirements and promote safe, predictable behavior

---

## ⚠️ Static Type Checks

The template enforces strict constraints on `T` at compile time using `static_assert`. These checks ensure that the type used in `MyContainer<T>` supports the necessary operations:

* `std::is_copy_constructible<T>`: ensures elements can be copied into internal storage
* `std::is_copy_assignable<T>`: ensures elements can be assigned
* `std::is_move_constructible<T>`: allows safe performance optimizations via move semantics
* `std::is_move_assignable<T>`: enables move assignment during container operations
* `operator==` returns `bool`: required for comparisons (e.g., in `remove()`)
* `operator<` returns `bool`: required for sorting in ordered iterators

These validations ensure that invalid types (e.g., `bool`, lambdas, non-comparable structs) are rejected at compile-time.

---

## 🧠 Memory Management Responsibility



`MyContainer<T>` supports storing values of any type `T`, including pointers (e.g., `T = Point*`). When using pointer types, **the container does not take ownership of the dynamically allocated memory**.

This means:

- The container will store and allow access to raw pointers, but will **not delete** them.
- It is the **user's responsibility** to properly `delete` any dynamically allocated objects before the container is destroyed.
- Failing to do so may result in **memory leaks**.

For example:

```cpp
MyContainer<Point*> c;
c.add(new Point(1, 2));
// ...
delete c[0]; // must be done manually before c is destroyed
```

## 🧭 Pointer Sorting Behavior

When storing raw pointers (e.g., `T = Point*`) inside the container, sorting-based iterators such as `AscendingOrder`, `DescendingOrder`, and `SideCrossOrder` will sort based on the **pointer addresses**, not on the values pointed to.

This is the expected behavior:
- Sorting follows the default semantics of `operator<` on pointers.
- Elements will be ordered according to their memory addresses.

This behavior was chosen intentionally to avoid dereferencing invalid or null pointers and to keep the container logic generic and type-safe.

---

## ✅ Testing

Testing is done using [doctest](https://github.com/doctest/doctest). Tests include:

* Basic add/remove operations
* All iterator traversal patterns
* Index access and exception behavior
* Exception handling
* Empty container scenarios
* Static type enforcement validation

Run tests with:

```bash
make test
make valgrind
```

---

## 👨‍💼 Author

Or Bibi
📧 [orbibiariel@gmail.com](mailto:orbibiariel@gmail.com)
