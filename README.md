# Filtered String View Library

The Filtered String View provides a C++ implementation for viewing strings through a filter without modifying the underlying string. It allows dynamic filtering based on provided predicates, making it useful for scenarios where non-destructive, conditional access to string data is required.

## Overview
The Filtered String View project provides a flexible and efficient way to handle string views that can be dynamically filtered based on custom criteria. This C++ library allows users to apply function-based filters to control the visibility and access of characters in the string, making it ideal for handling data where specific filtering of content is needed.

## Features
- **Dynamic Filtering**: Apply customizable filter functions to string views.
- **Standard Compatibility**: Uses modern C++ features, fully compatible with C++17 standards and forward.
- **Efficient Memory Use**: Operates directly on the original string without copying, ensuring efficient memory usage.
- **Iterator Support**: Includes bidirectional iterators that respect the applied filters, compliant with C++ bidirectional iterator requirements.

## Detailed Functionality
### Constructors
- **Default Constructor**: Initializes an empty view.
- **Implicit String Constructor**: Takes a standard string and views it without any filters.
- **String Constructor with Predicate**: Allows initialization from a string with a specific filtering predicate.
- **Implicit Null-Terminated String Constructor**: Views a C-style string without filters.
- **Null-Terminated String with Predicate Constructor**: Views a C-style string with a specified filter.
- **Copy Constructor**: Supports copying from another `filtered_string_view`.
- **Move Constructor**: Supports efficient move operations.

### Member Functions
- **`original_size()`**: Returns the length of the original unfiltered string.
- **`size()`**: Returns the size of the filtered view.
- **`empty()`**: Checks if the filtered view is empty.
- **`data()`**: Provides access to the underlying character data.
- **`predicate()`**: Returns the currently applied filter predicate.

### Iterator Functionality
- **Bidirectional Iterators**: Allows iteration over the filtered view forwards and backwards.
- **Range-based Iteration**: Supports `begin()`, `end()`, `rbegin()`, and `rend()` for range-based loops.

### Non-Member Functions
- **Relational Operators**: Defines equality and three-way comparison for filtered views.
- **Stream Output Operator**: Allows printing the filtered view directly to an output stream.
- **Utility Functions**: Includes functions like `compose` to combine multiple filters, `split` to divide the view based on a delimiter, and `substr` to get a substring view.


## Installation
1. Clone the repository:
    ```sh
    git clone git@github.com:shaynewx/Filtered-String-View.git
    ```
2. Include the library files from the `src` directory in your project:
    - `filtered_string_view.h`
    - `filtered_string_view.cpp`

3. Compile your project using a C++ compiler that supports C++17 or later.

## Usage
Here is a simple example of how to use the filtered_string_view:
```c++
#include "filtered_string_view.h"

int main() {
    std::string data = "Example Data";
    fsv::filter my_filter = [](const char& c) { return c != ' '; };
    fsv::filtered_string_view fsv(data, my_filter);

    std::cout << "Filtered view: " << static_cast<std::string>(fsv) << std::endl;
    return 0;
}
```

## Example Operations
### Equality and Comparison:
```c++
auto const lo = fsv::filtered_string_view{"aaa"};
auto const hi = fsv::filtered_string_view{"zzz"};

std::cout << std::boolalpha 
          << (lo == hi) << ' '
          << (lo != hi) << std::endl;
// Output: false true
```

### Relational Comparisons Using Spaceship Operator:
```c++
std::cout << std::boolalpha
<< (lo < hi) << ' '
<< (lo <= hi) << ' '
<< (lo > hi) << ' '
<< (lo >= hi) << ' '
<< (lo <=> hi == std::strong_ordering::less) << std::endl;
// Output: true true false false true
```

## Testing
Unit tests are provided in `src/filtered_string_view.test.cpp` to ensure the correctness and efficiency of the library. To run the tests:

1. Configure your project with CMake in the root directory:
    ```sh
    cmake -B build
    ```
2. Build the project:
    ```sh
    cd build
    make
    ```
3. Run the tests:
    ```sh
    ./filtered_string_view_test
    ```


## Contribution

Contributions are welcome! Please fork the repository and submit a pull request for any improvements or bug fixes.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Authors
- **[shaynewx](https://github.com/shaynewx)**
