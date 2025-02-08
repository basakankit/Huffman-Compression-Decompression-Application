#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cstdlib>
#include <vector>
#include <bitset>

using namespace std;

#define MAX_TREE_HT 50

// Count character frequencies from file input
typedef map<char, int> Map;
//Map to store huffman codes
typedef map<char, vector<bool>> HuffmanCodeMap;

// Represents node in Huffman tree
struct MinHNode {
    int freq;// Frequency of char
    char item;// Character itself
    struct MinHNode *left, *right;
};

//Min Heap structure
struct MinH {
    int size;//current no. of elements/nodes present in the heap
    int capacity;//max no. of nodes heap can accomodate
    struct MinHNode **array;
};

// Creating Huffman tree node
struct MinHNode *newNode(char item, int freq) {
    //creates and allocates memory for the entire heap structure.
    struct MinHNode *temp = (struct MinHNode *)malloc(sizeof(struct MinHNode));
    temp->left = temp->right = NULL;
    temp->item = item;
    temp->freq = freq;
    return temp;
}

// Create min heap using given capacity
struct MinH *createMinH(int capacity) {
    //creates and allocates memory for the entire heap structure.
    struct MinH *minHeap = (struct MinH *)malloc(sizeof(struct MinH));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    //allocates memory for an array of pointers to MinHNode elements (nodes), where each pointer 
    //will store the address of an actual node in the heap.
    minHeap->array = (struct MinHNode **)malloc(minHeap->capacity * sizeof(struct MinHNode *));
    return minHeap;
}

//Swap function
void swapMinHNode(struct MinHNode **a, struct MinHNode **b) {
    struct MinHNode *t = *a;
    *a = *b;
    *b = t;
}

// Heapify
void minHeapify(struct MinH *minHeap, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq) {
        smallest = left;
    }
    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq) {
        smallest = right;
    }
    if (smallest != i) {
        swapMinHNode(&minHeap->array[smallest], &minHeap->array[i]);
        minHeapify(minHeap, smallest);
    }
}

//Check if size is 1
int checkSizeOne(struct MinH *minHeap) {
    return (minHeap->size == 1);
}

// Extract the min
struct MinHNode *extractMin(struct MinH *minHeap) {
    struct MinHNode *temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

//Insertion
void insertMinHeap(struct MinH *minHeap, struct MinHNode *minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

//Build min heap
void buildMinHeap(struct MinH *minHeap) {
    int n = minHeap->size - 1;

    for (int i = (n - 1) / 2; i >= 0; i--) {
        minHeapify(minHeap, i);
    }
}

//checks if a node does not have any children
int isLeaf(struct MinHNode *root) {
    return !(root->left) && !(root->right);
}

struct MinH *createAndBuildMinHeap(char item[], int freq[], int size) {
    struct MinH *minHeap = createMinH(size);

    for (int i = 0; i < size; i++) {
        minHeap->array[i] = newNode(item[i], freq[i]);
    }

    minHeap->size = size;
    buildMinHeap(minHeap);
    return minHeap;
}

struct MinHNode *buildHfTree(char item[], int freq[], int size) {
    struct MinHNode *left, *right, *top;
    struct MinH *minHeap = createAndBuildMinHeap(item, freq, size);
    while (!checkSizeOne(minHeap)) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

void storeHCode(struct MinHNode *root, vector<bool> &arr, int top, HuffmanCodeMap &huffmanCodes) {
    if (root->left) {
        arr[top] = false;
        storeHCode(root->left, arr, top + 1, huffmanCodes);
    }
    if (root->right) {
        arr[top] = true;
        storeHCode(root->right, arr, top + 1, huffmanCodes);
    }
    if (isLeaf(root)) {
        vector<bool> code(arr.begin(), arr.begin() + top);
        huffmanCodes[root->item] = code;
    }
}

void HuffmanCodes(char item[], int freq[], int size, HuffmanCodeMap &huffmanCodes) {
    struct MinHNode *root = buildHfTree(item, freq, size);
    vector<bool> arr(MAX_TREE_HT);
    int top = 0;
    storeHCode(root, arr, top, huffmanCodes);
}

void count_char(istream &in, Map &chars) {
    char text;
    while (in.get(text)) {  // Use in.get() to read every single character, including spaces
        ++chars[text];
    }
}


char toByte(const vector<bool> &bits) {
    char byte = 0;
    for (size_t i = 0; i < bits.size(); ++i) {
        byte |= bits[i] << i;
    }
    return byte;
}

void encodeText(istream &in, const HuffmanCodeMap &huffmanCodes, vector<bool> &encodedData) {
    char text;
    while (in.get(text)) {  // Use in.get() to read every single character, including spaces
        if (huffmanCodes.find(text) != huffmanCodes.end()) {
            const vector<bool> &code = huffmanCodes.at(text);
            encodedData.insert(encodedData.end(), code.begin(), code.end());
        } else {
            cout << "Warning: Character '" << text << "' not found in Huffman codes map." << endl;
        }
    }
}


int main() {
    ifstream in("Hello.txt");

    if (!in.is_open()) {
        cout << "Error! Could not open file." << endl;
        return 1;
    }
    Map char_map;
    count_char(in, char_map);

    int size = char_map.size();
    char *arr = new char[size];
    int *freq = new int[size];

    int i = 0;
    for (auto it = char_map.begin(); it != char_map.end(); it++) {
        arr[i] = it->first;
        freq[i] = it->second;
        i++;
    }

    HuffmanCodeMap huffmanCodes;
    HuffmanCodes(arr, freq, size, huffmanCodes);

    vector<bool> encodedData;
    in.clear();
    in.seekg(0, ios::beg);  // Reset the input file stream to the beginning
    encodeText(in, huffmanCodes, encodedData);
    in.close();

    ofstream out("encoded.bin", ios::binary);
    if (!out.is_open()) {
        cout << "Error! Could not open output file." << endl;
        return 1;
    }

    for (size_t i = 0; i < encodedData.size(); i += 8) {
        vector<bool> byteBits(encodedData.begin() + i, encodedData.begin() + i + 8);
        while (byteBits.size() < 8) {
            byteBits.push_back(false);
        }
        char byte = toByte(byteBits);
        out.write(&byte, sizeof(byte));
    }
    out.close();

    cout << "Char  |  Huffman Code \n";
    for (auto it = huffmanCodes.begin(); it != huffmanCodes.end(); it++) {
        cout << it->first << "  |  ";
        for (bool bit : it->second) {
            cout << bit;
        }
        cout << endl;
    }

    delete[] arr;
    delete[] freq;

    return 0;
}
