# Huffman File Compressor & Decompressor

This project implements **Huffman Coding** in C++ to compress and decompress files.  
It reduces file size by encoding frequently occurring characters with shorter bit sequences.

---

## 🚀 Features
- 📂 Compress any text file into a `.huf` format  
- 🔄 Decompress `.huf` files back to original  
- ⚡ Fast and memory-efficient  
- ✅ Command-line based, simple to use  

---

## 📂 Project Structure
├── compressor.cpp # Main source code
├── decompressor.cpp # Decompression logic
├── huffman.h # Header file for Huffman classes/functions
├── sample.txt # Example input file
├── README.md # Documentation

---

## ⚙️ How to Compile & Run

### Compile

g++ compressor.cpp -o compressor
g++ decompressor.cpp -o decompressor
Run Compressor

./compressor input.txt compressed.huf
Run Decompressor

./decompressor compressed.huf output.txt
📖 Example

./compressor sample.txt sample.huf
./decompressor sample.huf recovered.txt
sample.txt → Original text file

sample.huf → Compressed binary file

recovered.txt → Decompressed file (same as original)

📊 Algorithm Used
Huffman Coding (Greedy Algorithm)

Priority Queue (Min-Heap) for building Huffman Tree

Frequency Map for character counts

🖼️ Huffman Coding Workflow
Add this diagram image (huffman_workflow.png) in your repo and link it here:


Workflow Steps:

Input Text → Frequency Map → Huffman Tree → Huffman Codes → Compressed File
🤝 Contributing
Pull requests are welcome. If you’d like to add new features (like GUI, image compression, etc.), feel free to fork the repo and open a PR.
