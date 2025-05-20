#include "text_processor.hpp"
#include "utils.hpp"
#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

// Create a temporary file with the given content
std::string createTempFile(const std::string& content) {
    std::string filename = "/tmp/test_" + std::to_string(rand()) + ".txt";
    std::ofstream file(filename);
    file << content;
    file.close();
    return filename;
}

class TextProcessorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary file with some wiki titles
        wikiTitlesFile = createTempFile("Coldplay\nHelsinki\nAugust");
        
        // Initialize the text processor and load titles
        processor.loadWikiTitles(wikiTitlesFile);
    }
    
    void TearDown() override {
        // Remove temporary files
        remove(wikiTitlesFile.c_str());
    }
    
    text_breaker::TextProcessor processor;
    std::string wikiTitlesFile;
};

TEST_F(TextProcessorTest, ProcessSimpleText) {
    // Create a temporary file with test content
    std::string testContent = "Coldplay had a tour concert in Helsinki in August, 2024.";
    std::string inputFile = createTempFile(testContent);
    
    // Process the file
    nlohmann::json result = processor.processFile(inputFile);
    
    // Clean up
    remove(inputFile.c_str());
    
    // Verify results
    ASSERT_TRUE(result.contains("text"));
    ASSERT_TRUE(result.contains("references"));
    ASSERT_EQ(result["text"], testContent);
    
    // We should have found references to "Coldplay", "Helsinki", and "August"
    ASSERT_EQ(result["references"].size(), 3);
    
    // Check the first reference (Coldplay)
    ASSERT_EQ(result["references"][0]["start"], 0);
    ASSERT_EQ(result["references"][0]["end"], 8);
    ASSERT_EQ(result["references"][0]["title"], "Coldplay");
    
    // Check the second reference (Helsinki)
    ASSERT_EQ(result["references"][1]["start"], 28);
    ASSERT_EQ(result["references"][1]["end"], 36);
    ASSERT_EQ(result["references"][1]["title"], "Helsinki");
    
    // Check the third reference (August)
    ASSERT_EQ(result["references"][2]["start"], 40);
    ASSERT_EQ(result["references"][2]["end"], 46);
    ASSERT_EQ(result["references"][2]["title"], "August");
}

TEST_F(TextProcessorTest, ProcessLargeText) {
    // Create a larger text that should be split into multiple blocks
    std::string largeText;
    for (int i = 0; i < 10; i++) {
        largeText += "This is paragraph " + std::to_string(i) + " which mentions Coldplay. ";
        largeText += "It also mentions Helsinki and August in the same paragraph. ";
        // Add more text to make the paragraph longer
        largeText += "Lorem ipsum dolor sit amet, consectetur adipiscing elit. ";
        largeText += "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ";
    }
    
    std::string inputFile = createTempFile(largeText);
    
    // Process the file
    nlohmann::json result = processor.processFile(inputFile);
    
    // Clean up
    remove(inputFile.c_str());
    
    // Verify results
    ASSERT_TRUE(result.contains("text"));
    ASSERT_TRUE(result.contains("references"));
    ASSERT_EQ(result["text"], largeText);
    
    // We should have found multiple references to Coldplay, Helsinki, and August
    ASSERT_GE(result["references"].size(), 20); // At least 2 references per term x 10 paragraphs
    
    // Check that references are sorted by start position
    for (size_t i = 1; i < result["references"].size(); i++) {
        ASSERT_LE(result["references"][i-1]["start"], result["references"][i]["start"]);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 