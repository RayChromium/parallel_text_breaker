# Text Reference Breaker

A parallel text processing tool that identifies and annotates Wikipedia references in text using the Aho-Corasick algorithm, implemented in C++11 with OpenMP.

## Project Description

This tool takes a large text file, splits it into 1KB blocks, and automatically identifies semantically meaningful words and phrases by matching them against an offline Wikipedia titles database. It then annotates the text with reference numbers and compiles a "References" section, similar to academic paper citations.

For detailed background and motivation, see the [Project Proposal](RuiCai-Project-proposal.md).

## Features

- Parallel text processing using the Aho-Corasick algorithm
- OpenMP-based parallelization
- Offline Wikipedia titles database matching
- JSON output format with reference annotations
- Efficient memory usage with immutable trie structure

## Performance Benchmarks

The following tables show performance comparisons between parallel and sequential processing modes using different combinations of Wikipedia title databases and input text files.

**Note**: The tables below show **processing time only** (excluding the title loading phase) to properly compare the parallelized vs sequential performance. The loading phase is sequential in both modes and varies by database size:
- **Small database (51 titles)**: Loading time ~6-13ms
- **Large database (10,000 titles)**: Loading time ~66-79ms

### Small Wikipedia Database (51 titles)

| Input File | Mode | Processing Time | Speedup | References Found |
|------------|------|----------------|---------|------------------|
| sample_text.txt (2.2KB) | Sequential | 5.64ms | - | 52 |
| sample_text.txt (2.2KB) | Parallel | 13.95ms | 0.40x | 52 |
| book-war-and-peace.txt (3.1MB) | Sequential | 7863.76ms | - | 193 |
| book-war-and-peace.txt (3.1MB) | Parallel | 1296.74ms | **6.06x** | 193 |

### Large Wikipedia Database (10,000 titles)

| Input File | Mode | Processing Time | Speedup | References Found |
|------------|------|----------------|---------|------------------|
| sample_text.txt (2.2KB) | Sequential | 1252.71ms | - | 3743 |
| sample_text.txt (2.2KB) | Parallel | 671.75ms | **1.76x** | 3743 |
| book-war-and-peace.txt (3.1MB) | Sequential | 1766.43s | - | 5,084,135 |
| book-war-and-peace.txt (3.1MB) | Parallel | 285.80s | **6.18x** | 5,084,135 |

### Key Observations

- **Small text files**: Sequential processing is faster due to parallelization overhead
- **Large text files**: Parallel processing provides significant speedup (6x improvement consistently)
- **Loading time impact**: Large databases (10K titles) take ~12x longer to load than small databases (51 titles), but this is a one-time cost
- **Processing complexity**: Large databases require ~200x more processing time due to dramatically more pattern matches
- **Reference density**: Larger databases find dramatically more matches (10K database finds 72x more references than 51-title database in small text, and 26,000x more in large text)
- **Scalability**: Performance benefits increase with text size - the largest test showed 6.18x speedup (29.4 min vs 4.8 min for processing only)
- **Database impact**: 10K title database processes the same text ~200x slower than 51-title database due to more matches found

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
```

## Usage

```bash
# Basic usage (parallel mode - default)
./text_reference_breaker <wiki_titles_file> <input_text_file> <output_json_file>

# Sequential mode (for performance comparison)
./text_reference_breaker <wiki_titles_file> <input_text_file> <output_json_file> --sequential

# Example with sample data
./text_reference_breaker ../data/wiki_titles.txt ../data/sample_text.txt output.json
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

## Data Sources

- **War and Peace text**: Downloaded from [NYU Economics 370 course materials](https://github.com/mmcky/nyu-econ-370/blob/master/notebooks/data/book-war-and-peace.txt)
- **Google 10,000 English words**: Common English words list from [first20hours/google-10000-english](https://github.com/first20hours/google-10000-english) for testing large vocabulary scenarios
- **Sample Wikipedia titles**: Custom curated list of 51 popular Wikipedia titles for demonstration

## Acknowledgments

This project makes use of several excellent open-source libraries and data sources:

- **Aho-Corasick Algorithm Implementation**: Thanks to [cjgdev](https://github.com/cjgdev) for the header-only C++ implementation of the Aho-Corasick string matching algorithm ([aho_corasick](https://github.com/cjgdev/aho_corasick))
- **JSON Processing**: [nlohmann/json](https://github.com/nlohmann/json) by [Niels Lohmann](https://github.com/nlohmann) for modern C++ JSON processing
- **Test Data**: War and Peace text sourced from [NYU Economics 370 course repository](https://github.com/mmcky/nyu-econ-370) maintained by [mmcky](https://github.com/mmcky)

## License

MIT License 