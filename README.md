# Text Reference Breaker

A parallel text processing tool that identifies and annotates Wikipedia references in text using the Aho-Corasick algorithm, implemented in C++11 with OpenMP.

## Project Description

This tool takes a large text file, splits it into 1KB blocks, and automatically identifies semantically meaningful words and phrases by matching them against an offline Wikipedia titles database. It then annotates the text with reference numbers and compiles a "References" section, similar to academic paper citations.

## Features

- Parallel text processing using the Aho-Corasick algorithm
- OpenMP-based parallelization
- Offline Wikipedia titles database matching
- JSON output format with reference annotations
- Efficient memory usage with immutable trie structure

## Requirements

- C++11 compatible compiler (GCC 4.8+ or Clang 3.3+)
- OpenMP support
- CMake 3.10+
- nlohmann/json library (included as submodule)

## Building the Project

```bash
# Clone the repository with submodules
git clone --recursive https://github.com/yourusername/text_reference_breaker.git
cd text_reference_breaker

# Create a build directory
mkdir build
cd build

# Configure and build
cmake ..
make

# Run tests (optional)
make test
```

## Usage

```bash
# Basic usage
./text_reference_breaker <wiki_titles_file> <input_text_file> <output_json_file>

# Example
./text_reference_breaker data/wiki_titles.txt data/sample_text.txt output.json
```

### Input Format

- `wiki_titles_file`: A text file with one Wikipedia title per line
- `input_text_file`: Any text file that you want to process

### Output Format

The output is a JSON file with the following structure:

```json
{
  "text": "ColdPlay had a tour concert in Helsinki in August, 2024.",
  "references": [
    { "start": 0, "end": 9, "titleIndex": 301, "title": "Coldplay" },
    { "start": 28, "end": 36, "titleIndex": 2105, "title": "Helsinki" },
    { "start": 40, "end": 46, "titleIndex": 984, "title": "August" }
  ]
}
```

## Dependencies

- [Aho-Corasick](https://github.com/cjgdev/aho_corasick): Header-only C++ implementation of the Aho-Corasick algorithm
- [nlohmann/json](https://github.com/nlohmann/json): JSON for Modern C++

## License

MIT License 