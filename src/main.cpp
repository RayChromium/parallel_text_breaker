#include "text_processor.hpp"
#include "utils.hpp"
#include <iostream>
#include <string>
#include <stdexcept>
#include <omp.h>

void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " <wiki_titles_file> <input_text_file> <output_json_file> [--sequential]" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  --sequential    Run in sequential mode (default is parallel)" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        // Check command line arguments
        if (argc < 4 || argc > 5) {
            printUsage(argv[0]);
            return 1;
        }

        std::string wikiTitlesFile = argv[1];
        std::string inputTextFile = argv[2];
        std::string outputJsonFile = argv[3];
        
        // Check if sequential mode is specified
        bool useParallel = true;
        if (argc == 5 && std::string(argv[4]) == "--sequential") {
            useParallel = false;
        }

        // Set number of threads based on available hardware
        int maxThreads = omp_get_max_threads();
        std::cout << "Using up to " << maxThreads << " threads for text processing." << std::endl;

        // Initialize text processor
        std::cout << "Initializing text processor..." << std::endl;
        text_breaker::TextProcessor processor;
        
        // Load Wikipedia titles
        std::cout << "Loading Wikipedia titles from " << wikiTitlesFile << "..." << std::endl;
        double startTime = omp_get_wtime();
        processor.loadWikiTitles(wikiTitlesFile);
        double endTime = omp_get_wtime();
        std::cout << "Wiki titles loaded successfully in " << (endTime - startTime) * 1000 << " ms." << std::endl;
        
        // Process the input file
        std::cout << "Processing text file: " << inputTextFile << "..." << std::endl;
        startTime = omp_get_wtime();
        nlohmann::json result = processor.processFile(inputTextFile, useParallel);
        endTime = omp_get_wtime();
        std::cout << "Text processing complete in " << (endTime - startTime) * 1000 << " ms." << std::endl;
        
        // Write results to output file
        std::cout << "Writing results to " << outputJsonFile << "..." << std::endl;
        text_breaker::utils::writeFile(outputJsonFile, result.dump(2)); // Pretty print with 2-space indentation
        
        // Report statistics
        if (result.contains("references")) {
            std::cout << "Found " << result["references"].size() << " references in the text." << std::endl;
        }
        
        std::cout << "Processing complete." << std::endl;
        return 0;
    } 
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 