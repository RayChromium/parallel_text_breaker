#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <aho_corasick/aho_corasick.hpp>

namespace text_breaker {

struct Reference {
    size_t start;
    size_t end;
    size_t titleIndex;
    std::string title;
};

struct ProcessedBlock {
    std::string text;
    std::vector<Reference> references;
};

class TextProcessor {
public:
    TextProcessor();
    ~TextProcessor() = default;

    // Load Wikipedia titles into the Aho-Corasick trie
    void loadWikiTitles(const std::string& wikiTitlesFile);

    // Process a text file and return JSON result (chooses between parallel and sequential based on useParallel flag)
    nlohmann::json processFile(const std::string& inputFile, bool useParallel = true);

    // Process a text file in parallel and return JSON result
    nlohmann::json processFileParallel(const std::string& inputFile);
    
    // Process a text file sequentially and return JSON result
    nlohmann::json processFileSequential(const std::string& inputFile);

private:
    // Split text into blocks of approximately blockSize bytes
    std::vector<std::string> splitIntoBlocks(const std::string& text, size_t blockSize = 1024);

    // Process a single block of text
    ProcessedBlock processBlock(const std::string& block, size_t blockOffset);

    // Convert processed blocks to JSON format
    nlohmann::json blocksToJson(const std::vector<ProcessedBlock>& blocks);

    // Aho-Corasick trie for pattern matching
    std::unique_ptr<aho_corasick::trie> trie_;
    
    // Map to store Wikipedia titles and their indices
    std::unordered_map<std::string, size_t> titleIndices_;
};

} // namespace text_breaker 