#pragma once 
#include<string>
using namespace std;
#include"HuffmanTree.h"

//typedef unsigned char uchar;
struct ByteInfo
{
	unsigned char ch;
	int appearCount; //ch�ַ����ֵĴ���
	string strCode;  //ch�ַ���Ӧ�������Ϣ

	ByteInfo(int count = 0)
		:appearCount(count)
	{}
	ByteInfo operator+(const ByteInfo& b)  const       //�Զ������Ͳ������,Ҫ����+ 
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
	void GetLine(FILE* fIn, string& strContent); //��ȡÿһ�е��ַ�
	void GenerateHuffmanCode(HuffmanTreeNode<ByteInfo>* root);


	//�ļ��ڴ����������ֽڵķ�ʽ�洢
	//�ֽڵ�״̬һ����256��
	void WriteHead(FILE* fOut, const string& filePath);

	//ֻ���һ������256��ByteInfo���͵����鱣���ֽڳ��ֵ�Ƶ����Ϣ
	ByteInfo fileByteInfo[256];
};