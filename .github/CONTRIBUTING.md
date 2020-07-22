# How to contribute

You can contribute in many different ways, for example report a bug or come up with an idea for improvement.
If you are good at programming you could also provide a fix for bugs or Pull Requests with improvements.
If you want to help others using CLP you are welcome to extend the [Wiki on GitHub](https://github.com/Legion2/CorsairLightingProtocol/wiki).

## Finding information

General information can be found in the [Readme](https://github.com/Legion2/CorsairLightingProtocol) file at the root of the project.
Information about specific topics are written in the [Wiki](https://github.com/Legion2/CorsairLightingProtocol/wiki).
For developer there is also an [API documentation](https://legion2.github.io/CorsairLightingProtocol/) for all the public methods and types.

## Writing code

For writing code I recommend [VS Code](https://code.visualstudio.com/) with the [Clang-Format](https://marketplace.visualstudio.com/items?itemName=xaver.clang-format) and [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) extensions.
For uploading to Arduino boards I use the ArduinoIDE.

This project consists of two major parts, the CLP library itself, for which the source code can be found in `src/` directory and the example sketches in the `examples/` subdirectories.
The main logic for processing commands and creating the LED colors is part of the library source code.
The examples only show how to use the library and what can be configured.
The library is modular so communication with iCUE and the processing of commands is decoupled and can be extended by other developers.
Single parts of the library, such as the LEDController can also be used without iCUE, by just calling the public API of these classes.

The examples are used by most users as a template which they only modified slightly, so the basic examples should be simple and explicitly show the main configuration options.
Not all users are developers and may not understand the syntax or semantics of the code, they just alter some values and upload the sketches.
Examples can also be used to show how some additional or special features can be used, these sketches are not meant to be used by the users directly.
To verify that the library can be compiled on all supported platforms, all the examples are compiled in the CI pipeline for different boards.

## Coding conventions

Public methods must be documented with JavaDoc style comments.
When introducing new methods you should think about if the method should be part of the public API or only used internally.
Always use descriptive names for variables, only use acronyms if they are well known and frequently used.
The coding style is defined using ClangFormat and automatically checked when you submit a Pull Request.
