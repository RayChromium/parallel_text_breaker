#include "text_processor.hpp"
#include "utils.hpp"
#include <fstream>
#include <algorithm>
#include <omp.h>
#include <iostream>

namespace text_breaker {

TextProcessor::TextProcessor() : trie_(new aho_corasick::trie()) {
    std::cout << "TextProcessor constructed." << std::endl;
}

void TextProcessor::loadWikiTitles(const std::string& wikiTitlesFile) {
    std::ifstream file(wikiTitlesFile);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open wiki titles file: " + wikiTitlesFile);
    }

    std::cout << "File opened successfully." << std::endl;
    std::string line;
    size_t index = 0;
    while (std::getline(file, line)) {
        line = utils::trim(line);
        if (!line.empty()) {
            trie_->insert(line);
            titleIndices_[line] = index++;
            if (index % 10 == 0) {
                std::cout << "Inserted " << index << " titles..." << std::endl;
            }
        }
    }
    std::cout << "Inserted all " << index << " titles." << std::endl;
}

nlohmann::json TextProcessor::processFile(const std::string& inputFile, bool useParallel) {
    if (useParallel) {
        std::cout << "Using parallel processing mode..." << std::endl;
        return processFileParallel(inputFile);
    } else {
        std::cout << "Using sequential processing mode..." << std::endl;
        return processFileSequential(inputFile);
    }
}

nlohmann::json TextProcessor::processFileParallel(const std::string& inputFile) {
    // Read the entire file
    std::cout << "Reading file: " << inputFile << std::endl;
    std::string content = utils::readFile(inputFile);
    std::cout << "File content read, size: " << content.size() << " bytes." << std::endl;
    
    // Split into blocks
    std::cout << "Splitting into blocks..." << std::endl;
    std::vector<std::string> blocks = splitIntoBlocks(content);
    std::cout << "Split into " << blocks.size() << " blocks." << std::endl;
    std::vector<ProcessedBlock> processedBlocks(blocks.size());
    
    // Pre-calculate block offsets to avoid race conditions
    std::vector<size_t> blockOffsets(blocks.size());
    size_t currentOffset = 0;
    for (size_t i = 0; i < blocks.size(); ++i) {
        blockOffsets[i] = currentOffset;
        currentOffset += blocks[i].length();
    }
    
    // Process blocks in parallel
    std::cout << "Processing blocks in parallel..." << std::endl;
    #pragma omp parallel for
    for (size_t i = 0; i < blocks.size(); ++i) {
        std::cout << "Processing block " << i << "..." << std::endl;
        processedBlocks[i] = processBlock(blocks[i], blockOffsets[i]);
        std::cout << "Block " << i << " processed." << std::endl;
    }
    
    // Convert to JSON
    std::cout << "Converting to JSON..." << std::endl;
    return blocksToJson(processedBlocks);
}

nlohmann::json TextProcessor::processFileSequential(const std::string& inputFile) {
    // Read the entire file
    std::cout << "Reading file: " << inputFile << std::endl;
    std::string content = utils::readFile(inputFile);
    std::cout << "File content read, size: " << content.size() << " bytes." << std::endl;
    
    // Split into blocks
    std::cout << "Splitting into blocks..." << std::endl;
    std::vector<std::string> blocks = splitIntoBlocks(content);
    std::cout << "Split into " << blocks.size() << " blocks." << std::endl;
    std::vector<ProcessedBlock> processedBlocks(blocks.size());
    
    // Pre-calculate block offsets
    std::vector<size_t> blockOffsets(blocks.size());
    size_t currentOffset = 0;
    for (size_t i = 0; i < blocks.size(); ++i) {
        blockOffsets[i] = currentOffset;
        currentOffset += blocks[i].length();
    }
    
    // Process blocks sequentially
    std::cout << "Processing blocks sequentially..." << std::endl;
    for (size_t i = 0; i < blocks.size(); ++i) {
        std::cout << "Processing block " << i << "..." << std::endl;
        processedBlocks[i] = processBlock(blocks[i], blockOffsets[i]);
        std::cout << "Block " << i << " processed." << std::endl;
    }
    
    // Convert to JSON
    std::cout << "Converting to JSON..." << std::endl;
    return blocksToJson(processedBlocks);
}

std::vector<std::string> TextProcessor::splitIntoBlocks(const std::string& text, size_t blockSize) {
    std::vector<std::string> blocks;
    
    size_t textLength = text.length();
    size_t position = 0;
    
    while (position < textLength) {
        size_t remainingLength = textLength - position;
        size_t currentBlockSize = std::min(blockSize, remainingLength);
        
        // If we're not at the end of the text, try to split at a sentence boundary
        if (position + currentBlockSize < textLength) {
            size_t splitPos = position + currentBlockSize;
            
            // Look for a sentence boundary (., !, ?) within the last 100 characters of the block
            size_t lookbackLimit = std::min(currentBlockSize, size_t(100));
            size_t sentenceEndPos = text.find_last_of(".!?", splitPos, lookbackLimit);
            
            if (sentenceEndPos != std::string::npos && sentenceEndPos > position) {
                // Split after the sentence end (include the punctuation)
                splitPos = sentenceEndPos + 1;
                
                // Also include any whitespace after the punctuation
                while (splitPos < textLength && std::isspace(text[splitPos])) {
                    splitPos++;
                }
                
                currentBlockSize = splitPos - position;
            }
        }
        
        blocks.push_back(text.substr(position, currentBlockSize));
        position += currentBlockSize;
    }
    
    return blocks;
}

ProcessedBlock TextProcessor::processBlock(const std::string& block, size_t blockOffset) {
    ProcessedBlock result;
    result.text = block;
    
    std::cout << "About to parse text of size " << block.size() << "..." << std::endl;
    
    try {
        // Use a safer approach with explicit looping instead of relying on the library's parse_text
        std::cout << "Searching for matches..." << std::endl;
        std::vector<Reference> refs;
        
        // For each possible starting position in the text
        for (size_t i = 0; i < block.size(); ++i) {
            // For each Wiki title
            for (const auto& entry : titleIndices_) {
                const std::string& title = entry.first;
                size_t titleIndex = entry.second;
                
                // Check if there's a match at this position
                if (i + title.size() <= block.size() && 
                    block.substr(i, title.size()) == title) {
                    Reference ref;
                    ref.start = blockOffset + i;
                    ref.end = blockOffset + i + title.size();
                    ref.title = title;
                    ref.titleIndex = titleIndex;
                    refs.push_back(ref);
                }
            }
        }
        
        result.references = std::move(refs);
        std::cout << "Found " << result.references.size() << " references in block." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error during text parsing: " << e.what() << std::endl;
        throw;
    } catch (...) {
        std::cerr << "Unknown error during text parsing" << std::endl;
        throw;
    }
    
    return result;
}

nlohmann::json TextProcessor::blocksToJson(const std::vector<ProcessedBlock>& blocks) {
    nlohmann::json result;
    
    // Combine all blocks into a single text
    std::string fullText;
    std::vector<Reference> allReferences;
    
    for (const auto& block : blocks) {
        fullText += block.text;
        allReferences.insert(allReferences.end(), block.references.begin(), block.references.end());
    }
    
    // Sort references by start position
    std::sort(allReferences.begin(), allReferences.end(), 
        [](const Reference& a, const Reference& b) { 
            return a.start < b.start; 
        }
    );
    
    // Convert to JSON
    result["text"] = fullText;
    
    nlohmann::json refsJson = nlohmann::json::array();
    for (const auto& ref : allReferences) {
        nlohmann::json refJson;
        refJson["start"] = ref.start;
        refJson["end"] = ref.end;
        refJson["titleIndex"] = ref.titleIndex;
        refJson["title"] = ref.title;
        refsJson.push_back(refJson);
    }
    
    result["references"] = refsJson;
    return result;
}

} // namespace text_breaker 