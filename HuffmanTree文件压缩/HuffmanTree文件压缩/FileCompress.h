#pragma once 
#include<string>
using namespace std;
#include"HuffmanTree.h"

//typedef unsigned char uchar;
struct ByteInfo
{
	unsigned char ch;
	int appearCount; //ch字符出现的次数
	string strCode;  //ch字符对应编码的信息

	ByteInfo(int count = 0)
		:appearCount(count)
	{}
	ByteInfo operator+(const ByteInfo& b)  const       //自定义类型不能相加,要重载+ 
	{
		ByteInfo temp;
		temp.appearCount = appearCount + b.appearCount;
		return temp;
	}

	bool operator>(const ByteInfo& b) const
	{
		return appearCount > b.appearCount;
	}

	bool operator!=(const ByteInfo& b) const
	{
		return appearCount != b.appearCount;
	}

	bool operator==(const ByteInfo& b) const
	{
		return appearCount == b.appearCount;
	}
};
class FileCompress
{
public:
	FileCompress();
	bool CompressFile(const string& filePath);

	bool UnCompressFile(const string& filePath);

private:
	void GetLine(FILE* fIn, string& strContent); //读取每一行的字符
	void GenerateHuffmanCode(HuffmanTreeNode<ByteInfo>* root);


	//文件在磁盘上是以字节的方式存储
	//字节的状态一共有256种
	void WriteHead(FILE* fOut, const string& filePath);

	//只需给一个包含256个ByteInfo类型的数组保存字节出现的频次信息
	ByteInfo fileByteInfo[256];
};