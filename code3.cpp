#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <bitset>
#include <algorithm>
#include <iomanip>

using namespace std;

// Huffman Tree Node
struct HuffmanNode {
    char data;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;
    
    HuffmanNode(char d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
    HuffmanNode(int f) : data(0), freq(f), left(nullptr), right(nullptr) {}
};

// Comparator for priority queue
struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

// LZ77 Match structure
struct LZ77Match {
    int offset;
    int length;
    char nextChar;
    
    LZ77Match(int o, int l, char c) : offset(o), length(l), nextChar(c) {}
};

class TextCompressor {
private:
    // Huffman coding variables
    unordered_map<char, string> huffmanCodes;
    HuffmanNode* huffmanRoot;
    
    // Statistics
    size_t originalSize;
    size_t compressedSize;
    
public:
    TextCompressor() : huffmanRoot(nullptr), originalSize(0), compressedSize(0) {}
    
    ~TextCompressor() {
        deleteHuffmanTree(huffmanRoot);
    }
    
    void deleteHuffmanTree(HuffmanNode* node) {
        if (node) {
            deleteHuffmanTree(node->left);
            deleteHuffmanTree(node->right);
            delete node;
        }
    }
    
    // Build Huffman Tree
    void buildHuffmanTree(const string& text) {
        unordered_map<char, int> freq;
        for (char c : text) {
            freq[c]++;
        }
        
        priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;
        
        for (auto& p : freq) {
            pq.push(new HuffmanNode(p.first, p.second));
        }
        
        while (pq.size() > 1) {
            HuffmanNode* left = pq.top(); pq.pop();
            HuffmanNode* right = pq.top(); pq.pop();
            
            HuffmanNode* merged = new HuffmanNode(left->freq + right->freq);
            merged->left = left;
            merged->right = right;
            
            pq.push(merged);
        }
        
        huffmanRoot = pq.top();
    }
    
    // Generate Huffman codes
    void generateHuffmanCodes(HuffmanNode* node, string code) {
        if (!node) return;
        
        if (!node->left && !node->right) {
            huffmanCodes[node->data] = code.empty() ? "0" : code;
            return;
        }
        
        generateHuffmanCodes(node->left, code + "0");
        generateHuffmanCodes(node->right, code + "1");
    }
    
    // LZ77 Compression
    vector<LZ77Match> lz77Compress(const string& text) {
        vector<LZ77Match> matches;
        int windowSize = 4096;
        int bufferSize = 18;
        
        for (int i = 0; i < text.length(); ) {
            int bestOffset = 0;
            int bestLength = 0;
            
            int searchStart = max(0, i - windowSize);
            int searchEnd = i;
            
            // Find the longest match
            for (int j = searchStart; j < searchEnd; j++) {
                int length = 0;
                while (j + length < searchEnd && 
                       i + length < text.length() && 
                       text[j + length] == text[i + length] && 
                       length < bufferSize) {
                    length++;
                }
                
                if (length > bestLength) {
                    bestLength = length;
                    bestOffset = i - j;
                }
            }
            
            char nextChar = (i + bestLength < text.length()) ? text[i + bestLength] : '\0';
            matches.push_back(LZ77Match(bestOffset, bestLength, nextChar));
            i += bestLength + 1;
        }
        
        return matches;
    }
    
    // LZ77 Decompression
    string lz77Decompress(const vector<LZ77Match>& matches) {
        string result;
        
        for (const auto& match : matches) {
            if (match.length > 0) {
                int start = result.length() - match.offset;
                for (int i = 0; i < match.length; i++) {
                    result += result[start + i];
                }
            }
            if (match.nextChar != '\0') {
                result += match.nextChar;
            }
        }
        
        return result;
    }
    
  
    string huffmanEncode(const string& text) {
        string encoded;
        for (char c : text) {
            encoded += huffmanCodes[c];
        }
        return encoded;
    }
    
    
    string huffmanDecode(const string& encoded) {
        string decoded;
        HuffmanNode* current = huffmanRoot;
        
        for (char bit : encoded) {
            if (bit == '0') {
                current = current->left;
            } else {
                current = current->right;
            }
            
            if (!current->left && !current->right) {
                decoded += current->data;
                current = huffmanRoot;
            }
        }
        
        return decoded;
    }
    
    // Convert binary string to bytes
    vector<unsigned char> binaryStringToBytes(const string& binary) {
        vector<unsigned char> bytes;
        string paddedBinary = binary;
        
        // Pad to make it divisible by 8
        while (paddedBinary.length() % 8 != 0) {
            paddedBinary += "0";
        }
        
        for (size_t i = 0; i < paddedBinary.length(); i += 8) {
            string byte = paddedBinary.substr(i, 8);
            unsigned char byteValue = 0;
            for (int j = 0; j < 8; j++) {
                if (byte[j] == '1') {
                    byteValue |= (1 << (7 - j));
                }
            }
            bytes.push_back(byteValue);
        }
        
        return bytes;
    }
    
    // Convert bytes to binary string
    string bytesToBinaryString(const vector<unsigned char>& bytes, int validBits) {
        string binary;
        for (size_t i = 0; i < bytes.size(); i++) {
            bitset<8> bits(bytes[i]);
            binary += bits.to_string();
        }
        
        // Trim to valid bits
        if (validBits > 0 && validBits < binary.length()) {
            binary = binary.substr(0, validBits);
        }
        
        return binary;
    }
    
    // Main compression function
    bool compressFile(const string& inputFile, const string& outputFile) {
        ifstream file(inputFile);
        if (!file.is_open()) {
            cout << "Error: Could not open input file!" << endl;
            return false;
        }
        
        string text((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        file.close();
        
        if (text.empty()) {
            cout << "Error: Input file is empty!" << endl;
            return false;
        }
        
        originalSize = text.length();
        
        // Step 1: LZ77 Compression
        vector<LZ77Match> lz77Matches = lz77Compress(text);
        
        // Convert LZ77 matches to string for Huffman coding
        string lz77String;
        for (const auto& match : lz77Matches) {
            lz77String += to_string(match.offset) + "," + to_string(match.length) + "," + match.nextChar + ";";
        }
        
        // Step 2: Huffman Coding
        buildHuffmanTree(lz77String);
        generateHuffmanCodes(huffmanRoot, "");
        string huffmanEncoded = huffmanEncode(lz77String);
        
        // Convert to bytes
        vector<unsigned char> compressedBytes = binaryStringToBytes(huffmanEncoded);
        compressedSize = compressedBytes.size();
        
        // Save compressed data
        ofstream outFile(outputFile, ios::binary);
        if (!outFile.is_open()) {
            cout << "Error: Could not create output file!" << endl;
            return false;
        }
        
        // Write metadata
        int validBits = huffmanEncoded.length();
        outFile.write(reinterpret_cast<const char*>(&validBits), sizeof(validBits));
        
        // Write Huffman codes
        int codeCount = huffmanCodes.size();
        outFile.write(reinterpret_cast<const char*>(&codeCount), sizeof(codeCount));
        for (const auto& pair : huffmanCodes) {
            outFile.write(&pair.first, sizeof(char));
            int codeLength = pair.second.length();
            outFile.write(reinterpret_cast<const char*>(&codeLength), sizeof(codeLength));
            outFile.write(pair.second.c_str(), codeLength);
        }
        
        // Write compressed data
        int dataSize = compressedBytes.size();
        outFile.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
        outFile.write(reinterpret_cast<const char*>(compressedBytes.data()), compressedBytes.size());
        
        outFile.close();
        
        // Update compressed size to include metadata
        compressedSize = sizeof(validBits) + sizeof(codeCount);
        for (const auto& pair : huffmanCodes) {
            compressedSize += sizeof(char) + sizeof(int) + pair.second.length();
        }
        compressedSize += sizeof(dataSize) + compressedBytes.size();
        
        return true;
    }
    
    // Main decompression function
    bool decompressFile(const string& inputFile, const string& outputFile) {
        ifstream file(inputFile, ios::binary);
        if (!file.is_open()) {
            cout << "Error: Could not open compressed file!" << endl;
            return false;
        }
        
        // Read metadata
        int validBits;
        file.read(reinterpret_cast<char*>(&validBits), sizeof(validBits));
        
        // Read Huffman codes
        int codeCount;
        file.read(reinterpret_cast<char*>(&codeCount), sizeof(codeCount));
        
        huffmanCodes.clear();
        for (int i = 0; i < codeCount; i++) {
            char ch;
            file.read(&ch, sizeof(char));
            int codeLength;
            file.read(reinterpret_cast<char*>(&codeLength), sizeof(codeLength));
            string code(codeLength, ' ');
            file.read(&code[0], codeLength);
            huffmanCodes[ch] = code;
        }
        
        // Rebuild Huffman tree from codes
        huffmanRoot = new HuffmanNode(0);
        for (const auto& pair : huffmanCodes) {
            HuffmanNode* current = huffmanRoot;
            for (char bit : pair.second) {
                if (bit == '0') {
                    if (!current->left) current->left = new HuffmanNode(0);
                    current = current->left;
                } else {
                    if (!current->right) current->right = new HuffmanNode(0);
                    current = current->right;
                }
            }
            current->data = pair.first;
        }
        
        // Read compressed data
        int dataSize;
        file.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
        vector<unsigned char> compressedBytes(dataSize);
        file.read(reinterpret_cast<char*>(compressedBytes.data()), dataSize);
        
        file.close();
        
        // Convert bytes back to binary string
        string binaryString = bytesToBinaryString(compressedBytes, validBits);
        
        // Huffman decode
        string lz77String = huffmanDecode(binaryString);
        
        // Parse LZ77 matches
        vector<LZ77Match> lz77Matches;
        stringstream ss(lz77String);
        string token;
        
        while (getline(ss, token, ';')) {
            if (token.empty()) continue;
            
            size_t pos1 = token.find(',');
            size_t pos2 = token.find(',', pos1 + 1);
            
            if (pos1 != string::npos && pos2 != string::npos) {
                int offset = stoi(token.substr(0, pos1));
                int length = stoi(token.substr(pos1 + 1, pos2 - pos1 - 1));
                char nextChar = (pos2 + 1 < token.length()) ? token[pos2 + 1] : '\0';
                
                lz77Matches.push_back(LZ77Match(offset, length, nextChar));
            }
        }
        
        // LZ77 decompress
        string decompressedText = lz77Decompress(lz77Matches);
        
        // Write decompressed text
        ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            cout << "Error: Could not create decompressed file!" << endl;
            return false;
        }
        
        outFile << decompressedText;
        outFile.close();
        
        return true;
    }
    
    // Print compression statistics
    void printStatistics() {
        cout << "\n=== COMPRESSION STATISTICS ===" << endl;
        cout << "Original Size: " << originalSize << " bytes" << endl;
        cout << "Compressed Size: " << compressedSize << " bytes" << endl;
        
        if (originalSize > 0) {
            double ratio = (double)compressedSize / originalSize;
            double compressionPercent = (1.0 - ratio) * 100.0;
            
            cout << "Compression Ratio: " << fixed << setprecision(4) << ratio << endl;
            cout << "Space Saved: " << fixed << setprecision(2) << compressionPercent << "%" << endl;
        }
        cout << "==============================" << endl;
    }
};

int main() {
    TextCompressor compressor;
    string inputFile, compressedFile, decompressedFile;
    int choice;
    
    cout << "Text File Compression using Huffman + LZ77" << endl;
    cout << "==========================================" << endl;
    cout << "1. Compress a file" << endl;
    cout << "2. Decompress a file" << endl;
    cout << "Enter your choice (1-2): ";
    cin >> choice;
    
    switch (choice) {
        case 1:
            cout << "Enter input file name: ";
            cin >> inputFile;
            cout << "Enter compressed file name: ";
            cin >> compressedFile;
            
            if (compressor.compressFile(inputFile, compressedFile)) {
                cout << "File compressed successfully!" << endl;
                compressor.printStatistics();
            } else {
                cout << "Compression failed!" << endl;
            }
            break;
            
        case 2:
            cout << "Enter compressed file name: ";
            cin >> compressedFile;
            cout << "Enter output file name: ";
            cin >> decompressedFile;
            
            if (compressor.decompressFile(compressedFile, decompressedFile)) {
                cout << "File decompressed successfully!" << endl;
                cout << "Check '" << decompressedFile << "' for the readable content." << endl;
            } else {
                cout << "Decompression failed!" << endl;
            }
            break;
            
        default:
            cout << "Invalid choice! Please enter 1 or 2." << endl;
            break;
    }
    
    return 0;
}