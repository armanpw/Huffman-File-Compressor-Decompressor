//compressor

#include <bits/stdc++.h>
using namespace std;

struct Node{
    char ch;
    Node* left;
    Node* right;
    int freq;
   Node(char chr){
         ch=chr;
         left=nullptr;
         right=nullptr;
         freq=0 ;
   }

};

void deleteTree(Node* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

Node* HuffmanTree( unordered_map<char, int>& f) {
      priority_queue<pair<int,Node*>, vector<pair<int,Node*>>, greater<pair<int,Node*>>> minH;

    //creating node of each char-freq
    for (auto pair : f) {
        Node* node = new Node(pair.first);//char 
        node->freq = pair.second;//freq
        minH.push({ node->freq,node});
    }
    //making tree and pushing nodes into the min heap

    while (minH.size() > 1) {
        Node* l = minH.top().second;
        minH.pop();
        Node* r = minH.top().second;
        minH.pop();
        Node* Pnode = new Node('\0'); //creating parent node 
        Pnode->left = l;
        Pnode->right = r;
        Pnode->freq = l->freq + r->freq;
        minH.push({ Pnode->freq,Pnode});// inserting again tree
    }
    return minH.top().second; //return huffman tree root
}
//collecting huffman codes of all individual char
void HuffmanCode(Node* root, string code,  unordered_map<char,string>& prefix) {
    if (root == nullptr) 
        return;

    if (root->left == nullptr && root->right == nullptr) {
        prefix[root->ch]=code.empty() ? "0" : code;
        return;
    }
    HuffmanCode(root->left, code + "0", prefix);
    HuffmanCode(root->right, code + "1", prefix);
}

//padding of bits and converting into binary bits
 vector<unsigned char> packing( string& bits, int& pad){

    // padding-> making stream in form of multiple of 8
    pad = (8 - (bits.size() % 8)) % 8;
    bits.append(pad, '0'); // pad with zeros

    vector<unsigned char> bytes;

   //pack bit_stream into bytes as binary code
    for (size_t i = 0; i < bits.size(); i += 8) {
        unsigned char byte = 0;
        for (int b = 0; b < 8; ++b) {
            if (bits[i + b] == '1')
                 byte |= (1 << (7 - b));
        }
        bytes.push_back(byte);
}
  return bytes;
}

// converting tree to bit-stream preorder,0=internal, 1+8bits=leaf
void serializeTree(Node* node, string& tree_bits) {
    if (!node) return;
    if (!node->left && !node->right) {
        tree_bits += '1';
        unsigned char c = static_cast<unsigned char>(node->ch);
       // Convert char to string (8 bits)
        for (int i = 7; i >= 0; --i) {
            tree_bits += ((c >> i) & 1) ? '1' : '0';
        }
        return;
    }
    tree_bits += '0';
    serializeTree(node->left, tree_bits);
    serializeTree(node->right, tree_bits);
}

// Write 64-bit little-endian unsigned
void write_uint64_le(ofstream& out, uint64_t v) {
    for (int i = 0; i < 8; ++i) {
        unsigned char byte = static_cast<unsigned char>((v >> (8 * i)) & 0xFF);
        out.write(reinterpret_cast<char*>(&byte), 1);
    }
}

// Write 32-bit little-endian unsigned
void write_uint32_le(ofstream& out, uint32_t v) {

    for (int i = 0; i < 4; ++i) {
        unsigned char byte = static_cast<unsigned char>((v >> (8 * i)) & 0xFF);
        out.write(reinterpret_cast<char*>(&byte), 1);
    }
}

//for checking binary code of bit stream of tree and data
// void printByteBinary(unsigned char byte) {
//     for (int i = 7; i >= 0; --i) {     // check bits from MSB to LSB
//         cout << ((byte >> i) & 1);
//     }
// }

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: compressor <input> <output>\n";
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];   // <- use this, not hardcoded name

   ifstream in_file(inputFile, ios::binary);
     if (!in_file.is_open()) {
        cout << "Error: Could not open the file!";
        return 1;
    }

    char ch;
    unordered_map<char,int>freq;
    string original;
    while (in_file.get(ch)) {
        freq[ch]++;
        original.push_back(ch);//save chararters
    }

    in_file.close();

   Node* root= HuffmanTree(freq);
    if (root == nullptr) {
        cout << "Error: Huffman Tree could not be created!";
        return 1;
    }

    // Generate prefix free code
    unordered_map<char,string> prefix;

    HuffmanCode(root, "", prefix);
    // for(auto it:prefix)
    // cout<<"char "<<it.first<<" code "<<it.second<<"\n";
    //save_map(prefix);
    
    //converting the entire text file to a binary string (bit_stream)
    //making a bit stream by all individual prefix code of char
    string bit_stream;

    for (auto& ch : original) 
        bit_stream += prefix[ch];
    
    //convert bitstream into actual bytes(data_bytes) and store in binary file
    int data_padding=0;
    vector<unsigned char>data_bytes =packing(bit_stream,data_padding);

    //print decimal form for debudgging
    // cout << "Bytes (decimal and binary):\n";
    // for (unsigned char byte : data_bytes) {
    //     cout << static_cast<int>(byte) << " -> ";
    //     printByteBinary(byte);   // show in binary
    //     cout << endl;
    // }

    // Serialize tree preorder,0=internal, 1+8bits=leaf
    string tree_bits;
    serializeTree(root,tree_bits);
    // cout<<"tree_bits "<<tree_bits<<"\n";
     
   //saving structure of tree for decoding
    int tree_padding;
    vector<unsigned char> tree_bytes=packing(tree_bits,tree_padding);
    
    //print decimal form for debudgging
    //cout<<"tree padding "<< typeid(tree_padding).name() <<endl;
    //print decimal and binary form of tree bytes
    //  cout << "Bytes (decimal and binary):\n";
    // for (unsigned char byte : tree_bytes) {
    //     cout << static_cast<int>(byte) << " -> ";
    //     printByteBinary(byte);   // show in binary
    //     cout << endl;
    // }

    std::ofstream out_file(outputFile, std::ios::binary);

    if (!out_file.is_open()){
        cerr << "Cannot open output\n";
        deleteTree(root);
        return 1;
    }
     // 1. Magic marker (4 bytes)
    out_file.write("HUF1", 4);
    
    // 2. Tree length (uint32)
    write_uint32_le(out_file, static_cast<uint32_t>(tree_bytes.size()));

    // 3. store Tree's padding (int to char in binary form)
    out_file.put(static_cast<char>(tree_padding));

    // 4. save tree (tree shape)
    out_file.write(reinterpret_cast<char*>(tree_bytes.data()), tree_bytes.size());

    // 5. store bit stream's padding (int to char in binary form)
    out_file.put(static_cast<char>(data_padding));

    // 6. Original length of text file
    float original_size=original.size();
    // compressed length of text file
    float compressed_size=data_bytes.size();
    // compressed length of tree
    float compressed_size1=tree_bytes.size();

    write_uint64_le(out_file, static_cast<uint64_t>(original_size));

    // 7. save data bytes
    out_file.write(reinterpret_cast<char*>(data_bytes.data()), data_bytes.size());

    out_file.close();

    deleteTree(root);

    cout << "Compression done. Original size: " << original_size
         << "\ncompressed tree bytes: " << tree_bytes.size()<< "\n"
         << "compressed file bytes: " << data_bytes.size() << "\n"
        << "compression ratio: " << (1-(compressed_size+compressed_size1)/original_size )*100<< "%\n";

   return 0;
}