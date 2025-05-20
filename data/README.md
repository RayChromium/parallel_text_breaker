# Sample Data for Text Reference Breaker

This directory contains sample data for testing the Text Reference Breaker tool.

## Files

- `wiki_titles.txt`: A sample list of Wikipedia titles (one per line) that the tool will use for matching references in text
- `sample_text.txt`: A sample text file that contains several references to the titles in `wiki_titles.txt`

## Usage

You can use these sample files to test the Text Reference Breaker tool:

```bash
# From the build directory after compiling
./text_reference_breaker ../data/wiki_titles.txt ../data/sample_text.txt output.json
```

This will process the sample text, identify all references to titles in the wiki_titles.txt file, and output the results to output.json.

## Expected Results

The output JSON file should contain the original text and a list of references with their start and end positions, title indices, and the actual titles.

The sample text contains references to many terms in the titles list, including "Technology", "Internet", "Artificial intelligence", "Computer science", "Apple", "Google", "Microsoft", "Amazon", "Algorithm", "Science", "Physics", "Albert Einstein", "Chemistry", "Biology", "Mathematics", "COVID-19", "University", "Python", "Java", "C++", "Linux", "Windows", "Soccer", "Basketball", "Tennis", "Chess", "Music", "Movie", "Book", "Art", "New York City", "Tokyo", "London", "Paris", "Berlin", "Rome", "United States", "Canada", "Finland", "Europe", "Helsinki", "August", and "World War II". 