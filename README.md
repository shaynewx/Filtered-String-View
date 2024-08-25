# Filtered String View

A C++ library for creating filtered views of strings, demonstrating the use of C++ Standard Library functionalities and custom predicate-based filtering.

## Overview

This library provides a back-end for creating filtered views of strings—a sequence of characters that can be dynamically filtered based on a user-provided predicate function. It provides an efficient way to handle and process strings without making copies, reducing memory overhead and increasing performance.

### What is a Filtered String View?

A `filtered_string_view` represents a non-owning view into a sequence of characters that can be filtered on-the-fly. For example, a filtered view of the string "example" with a predicate to exclude 'e' would look like:


The library ensures efficient access and manipulation of the filtered data without copying the original string.

### Features

- Utilizes C++ Standard Library functionalities such as `std::function` and `std::string`.
- Provides an interface for filtered views with both `const char*` and `std::string`.
- Supports custom predicates for dynamic data filtering.

## Installation

To include the Filtered String View library in your project:

1. Clone the repository:
    ```sh
    git clone git@github.com:yourusername/filtered_string_view.git
    ```
2. Include the library files in your project:
    - `src/filtered_string_view.h`
    - `src/filtered_string_view.cpp`

3. Compile your project using a C++ compiler that supports C++20 or later.

## Usage

Include the header file in your source file and use the provided functionalities to create filtered views:

```cpp
#include "filtered_string_view.h"

int main() {
    std::string data = "example";
    auto view = fsv::filtered_string_view(data, [](char c) { return c != 'e'; });

    for (char c : view) {
        std::cout << c;
    }
    std::cout << std::endl;

    return 0;
}
```

