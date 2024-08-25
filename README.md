# Filtered String View Library

A C++ library for generating word ladders, demonstrating the use of C++ Standard Library containers and algorithms.

## Overview
The Filtered String View project provides a flexible and efficient way to handle string views that can be dynamically filtered based on custom criteria. This C++ library allows users to apply function-based filters to control the visibility and access of characters in the string, making it ideal for handling data where specific filtering of content is needed.

## Features
- **Dynamic Filtering**: Apply customizable filter functions to string views.
- **Standard Compatibility**: Uses modern C++ features, fully compatible with C++11 standards and forward.
- **Efficient Memory Use**: Operates directly on the original string without copying, ensuring efficient memory usage.
- **Iterator Support**: Includes bidirectional iterators that respect the applied filters, compliant with C++ bidirectional iterator requirements.


## Installation
1. Clone the repository:
    ```sh
    git clone git@github.com:shaynewx/Filtered-String-View.git
    ```
2. Navigate to the project directory:
    ```sh
    cd filtered_string_view
    ```

3. Build the project using CMake.
    ```sh
    cmake -B build
    cd build 
    ```


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






## Contribution

Contributions are welcome! Please fork the repository and submit a pull request for any improvements or bug fixes.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Authors
- **[shaynewx](https://github.com/shaynewx)**
