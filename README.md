# DataSqueeze - File Compression & Decompression App

### GUI Interface
![GUI Screenshot](images/img1.png)

### Compression Output (CMD)
![Compression Output](images/img2.png)

## Introduction
This project implements a **Text File Compression and Decompression System** using a combination of **LZ77** and **Huffman Coding** algorithms.  
The system reduces file size by first eliminating repeated patterns using LZ77 and then applying Huffman coding for optimal prefix-based encoding.  
The project supports both:
- **Command Line Interface (CLI)** using C++
- **Graphical User Interface (GUI)** using Java Swing
[Live Demo]

---

## Features
- Compress text files using LZ77 + Huffman coding
- Decompress compressed files back to original form
- Menu-driven CLI interface
- GUI-based file selection and execution
- Supports relative file paths (inputs/outputs folders)
- Efficient lossless compression

---

## Concepts Used
- Data Compression Techniques
- LZ77 Algorithm
- Huffman Coding
- File Handling
- Command Line Processing
- Java–C++ Integration

---

## Tech Stack
- Backend: C++
- GUI: Java (Swing)
- Algorithms: LZ77, Huffman Coding
- Platform: Windows
- Version Control: Git & GitHub

---

## Project Structure
```
DataSqueeze/
│
├── code3.cpp                 # C++ implementation (LZ77 + Huffman)
├── CompressionApp.java       # Java Swing GUI
├── README.md                 # Project documentation
│
├── inputs/                   # Sample input files
│ └── input.txt
│
└── outputs/                   # Generated output files (ignored in Git)
│   └── output.txt
│ 
└── images/                     # Project screenshots / diagrams
    ├── img1.png
    └── img2.png
```
---

## Installation & Execution

### 1. Clone the Repository
```sh
git clone https://github.com/Bronika04/Text-File-Compression.git
cd Text-File-Compression
```
### 2. Compile and Run C++ Program (CLI Mode)
```sh
Copy code
g++ code3.cpp -o compress
compress
```
### 3. Compile and Run Java GUI
```sh
Copy code
javac --release 17 CompressionApp.java
java CompressionApp
```
Ensure compress.exe and CompressionApp.class are in the same directory.

## How It Works
1. User selects Compress or Decompress
2. Input file is processed using LZ77 to remove redundancy
3. Huffman coding generates optimal prefix codes
4. Compressed file is generated
5. Decompression reverses the process to restore original content

## Output
- Compressed file: *.compressed
- Decompressed file: output.txt
- Output files are generated in the outputs folder

Contact
For any queries, reach out at:
📧 [bronika2005@gmail.com](mailto:bronika2005@gmail.com)

