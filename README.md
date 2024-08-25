# Filtered String View Library

A C++ library for generating word ladders, demonstrating the use of C++ Standard Library containers and algorithms.

## Overview
The Filtered String View project provides a flexible and efficient way to handle string views that can be dynamically filtered based on custom criteria. This C++ library allows users to apply function-based filters to control the visibility and access of characters in the string, making it ideal for handling data where specific filtering of content is needed.

## Features
- **Dynamic Filtering**: Apply customizable filter functions to string views.
- **Standard Compatibility**: Uses modern C++ features, fully compatible with C++17 standards and forward.
- **Efficient Memory Use**: Operates directly on the original string without copying, ensuring efficient memory usage.
- **Iterator Support**: Includes bidirectional iterators that respect the applied filters, compliant with C++ bidirectional iterator requirements.


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


## Contribution

Contributions are welcome! Please fork the repository and submit a pull request for any improvements or bug fixes.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Authors
- **[shaynewx](https://github.com/shaynewx)**
