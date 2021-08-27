#define _CRT_SECURE_NO_WARNINGS   1
#pragma once

#include"FileCompress.h"

int main()
{
	//TestHuffmanTree();
	//int array[] = { 3, 1, 7, 5 };
	//HuffmanTree<int> h;
	//h.CreateHuffmanTree(array, sizeof(array) / sizeof(array[0]));


	FileCompress fc;
	fc.CompressFile("C++Ë¼Î¬µ¼Í¼.png");
	fc.UnCompressFile("2.hzp");
	return 0;
}