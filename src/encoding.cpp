/*
* CS 106X Huffman Encoding
* Huffman Encoding's functions impelementaion
*
* Author : Elyas Esmaeili
*/
#include "encoding.h"
#include "pqueue.h"
#include "filelib.h"

Map<int, int> buildFrequencyTable(istream& input) {  
	rewindStream(input);
	Map<int, int> freqTable;   
    int chr;

	while ((chr = input.get()) != -1 ) {
		freqTable[chr]++;
	}
	freqTable[PSEUDO_EOF] = 1;

    return freqTable;          
}

HuffmanNode* buildEncodingTree(const Map<int, int>& freqTable) {
    PriorityQueue<HuffmanNode*> encodingQueue;
	for (auto& chr : freqTable) {
        HuffmanNode* node = new HuffmanNode(chr, freqTable[chr]);
        encodingQueue.enqueue(node, node->count);
	}

    while(encodingQueue.size() != 1) {
        HuffmanNode* firstNode = encodingQueue.dequeue();
        HuffmanNode* secondNode = encodingQueue.dequeue();
        HuffmanNode* newNode = new HuffmanNode(NOT_A_CHAR, firstNode->count + secondNode->count,
                                           firstNode, secondNode);
        encodingQueue.enqueue(newNode, newNode->count);
    }

    HuffmanNode* root = encodingQueue.dequeue();
    return root;
}

Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    Map<int, string> encodingMap;
	string code;
    buildEncodingMapRecursive(encodingTree, code, encodingMap);

    return encodingMap;             
}

void buildEncodingMapRecursive(HuffmanNode *encodingTree, string code, Map<int, string>& encodingMap)
{
	if (encodingTree != NULL) {
		buildEncodingMapRecursive(encodingTree->zero, code + '0', encodingMap);
		if (encodingTree->character != NOT_A_CHAR) {
			encodingMap[encodingTree->character] = code;
		}
		buildEncodingMapRecursive(encodingTree->one, code + '1', encodingMap);
	}
}

void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {
	rewindStream(input);

    int chr;
	string chrCode;

    while ((chr = input.get()) != -1) {
		chrCode = encodingMap[chr];
		for (auto& bit : chrCode) {
			output.writeBit(bit - 48);
		}
	}
    chrCode = encodingMap[PSEUDO_EOF];
    for (auto& bit : chrCode) {
        output.writeBit(bit - 48);
    }
}

void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output)
{
    int chr;
	while (chr != PSEUDO_EOF) {
		chr = decodeChar(input, encodingTree);
		output.put(chr);
	}
}

int decodeChar(ibitstream& input, HuffmanNode* encodingTree)
{
	if (encodingTree->character != NOT_A_CHAR) {
		return encodingTree->character;
	}

	int bit = input.readBit();
	if (bit == 0) {
		return decodeChar(input, encodingTree->zero);
	}
	else {
		return decodeChar(input, encodingTree->one);
	}
}

void compress(istream& input, obitstream& output) {
	Map<int, int> freqTabel = buildFrequencyTable(input);
	HuffmanNode* encodingTree = buildEncodingTree(freqTabel);
	Map<int, string> encodingMap = buildEncodingMap(encodingTree);

	output << freqTabel;
	encodeData(input, encodingMap, output);
	freeTree(encodingTree);
}

void decompress(ibitstream& input, ostream& output) {
	Map<int, int> freqTabel;
    input >> freqTabel;
	HuffmanNode* encodingTree = buildEncodingTree(freqTabel);

    decodeData(input, encodingTree, output);
	freeTree(encodingTree);
}

void freeTree(HuffmanNode* node) {
	if (node != NULL) {
		HuffmanNode* zeroChild = node->zero;
        HuffmanNode* oneChild = node->one;
		delete node;

		freeTree(zeroChild);
		freeTree(oneChild);
	}
}
