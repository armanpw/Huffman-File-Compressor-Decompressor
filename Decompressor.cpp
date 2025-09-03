
#include<bits/stdc++.h>
using namespace std;
struct Node{
        char ch;
        Node* left;
        Node* right;
        Node(char chr){
                ch=chr;
                left=nullptr;
                right=nullptr;
   }
};
void deleteTree(Node* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}
// Read Tree length
uint32_t read_uint32_le(ifstream& in) {
    uint32_t v = 0;
    for (int i = 0; i < 4; ++i) {
        unsigned char byte;
        in.read(reinterpret_cast<char*>(&byte), 1);
        v |= (static_cast<uint32_t>(byte) << (8 * i));
    }
    return v;
}
// Read original length of text file
uint64_t read_uint64_le(ifstream& in) {
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) {
        unsigned char byte;
        in.read(reinterpret_cast<char*>(&byte), 1);
        v |= (static_cast<uint64_t>(byte) << (8 * i));
    }
    return v;
}
// Read bytes from packed bytes (tree code and text code) and convert back into bit-stream in string
string unpack_bits(const vector<unsigned char>& bytes, int padding) {
    string bits;
    for (unsigned char byte : bytes) {
        for (int b = 7; b >= 0; --b) {
            bits.push_back((byte & (1 << b)) ? '1' : '0');// Convert packed bytes into a string of '0' and '1' bits
        }
    }
    // remove padding bits from the end
    if (padding > 0) {
        bits.erase(bits.end() - padding, bits.end());//Removing padded bits
    }
    return bits;
}
// Rebuilds Huffman tree from serialized bit string
Node* deserializeTree(const string& tree_bits, size_t& index) {
    if (index >= tree_bits.size()) return nullptr;
    
    if (tree_bits[index] == '1') {
        index++;
        unsigned char c = 0;
        for (int i = 0; i < 8; ++i) {
           c = c << 1;

        // Convert char '0'/'1' into integer 0/1 by subtracting '0'
        // '0' → 48, '1' → 49 in ASCII
        // ('1' - '0') = 1, ('0' - '0') = 0
        int bit = tree_bits[index++] - '0';

        // OR with current value → sets the last bit if needed
        // Example: 00000010 | 1 = 00000011
        c = c | bit;
        }//after completion of loop c = 97 (a)
        return new Node(static_cast<char>(c));
    }
 
    else{ // Internal node
        index++;
        Node* left = deserializeTree(tree_bits, index);
        Node* right = deserializeTree(tree_bits, index);
        Node* node = new Node('\0');
        node->left = left;
        node->right = right;
        return node;
    } 
}

//  Walks through bits in encoded_bits, moving through the Huffman tree until it reaches a leaf node, then adds that character to the result
// Function to decode Huffman-encoded bit string
string decodeHuffman( string& data_bits, Node* root, uint32_t original_size) {
    string output;
    output.reserve(original_size); // Avoid repeated allocations of memory enhance speed
    Node* node = root;
    for (char bit : data_bits){
        node = (bit == '0') ? node->left : node->right;
        // Leaf node found save char and again traverse from root
        if (node->left == nullptr && node->right == nullptr) {
            output.push_back(node->ch);
            node = root; // Reset to root for next symbol
            // Stop if we've decoded all original characters
            if (output.size() == original_size) break;
        }
    }
    return output;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: decompressor <input_file> <output_file>" << endl;
        return 1;
    }

    string input_file = argv[1];   // e.g. "uploads/abc123.huf"
    string output_file = argv[2];  // e.g. "uploads/abc123_decompressed.txt"

    ifstream in_file(input_file, ios::binary);

    if (!in_file.is_open()) {
        cerr << "Cannot open file.\n";
        return 1;
    }
    // 1. Read magic- verify huffman file or not
    char magic[4];
    in_file.read(magic, 4);
    
    if (strncmp(magic, "HUF1", 4) != 0) {
        cerr << "Invalid file format\n";
        return 1;
    }
    // 2. Tree length
    uint32_t tree_len = read_uint32_le(in_file);
    // 3. Tree padding
    unsigned char tree_padding;
    in_file.read(reinterpret_cast<char*>(&tree_padding), 1);
    // 4. Tree bytes
    vector<unsigned char> tree_bytes(tree_len);
    in_file.read(reinterpret_cast<char*>(tree_bytes.data()), tree_len);
    // 5. Data padding
    unsigned char data_padding;
    in_file.read(reinterpret_cast<char*>(&data_padding), 1);
    // 6. Original size
    uint64_t original_size = read_uint64_le(in_file);
    // 7. Data bytes
    vector<unsigned char> data_bytes((istreambuf_iterator<char>(in_file)), {});
    in_file.close();
    // Unpack tree bits
    string tree_bits = unpack_bits(tree_bytes, tree_padding);
    size_t idx = 0;
   //reconstruct tree
    Node* root = deserializeTree(tree_bits, idx);
    // Unpack data bits
    string data_bits = unpack_bits(data_bytes, data_padding);
    // Decode using Huffman tree (regain original text file)
    string decoded_text = decodeHuffman(data_bits, root, original_size);
    ofstream out_file(output_file, ios::binary);
     if (!out_file) {
        cerr << "Error: could not create output file: " << output_file << endl;
        return 1;
    }
    out_file << decoded_text;
    out_file.close();
    deleteTree(root);
    cout << "Decompression done. Output saved to decompressed.txt\n";
    return 0;
}
