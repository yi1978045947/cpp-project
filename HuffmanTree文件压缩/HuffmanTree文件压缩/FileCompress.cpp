#define _CRT_SECURE_NO_WARNINGS   1

#include"FileCompress.h"

FileCompress::FileCompress()
{
	for (int i = 0; i < 256; i++)
	{
		fileByteInfo[i].ch = i;
		//fileByteInfo[i].appearCount = 0;
		
	}
}
bool FileCompress::CompressFile(const string& filePath)
{
	FILE* pf = fopen(filePath.c_str(), "rb");
	if (pf == NULL)
	{
		cout << "��ѹ���ļ�ʧ��!" << endl;
		return false;
	}

	//�ļ��Ĵ�С��֪��,��Ҫѭ����ȡԴ�ļ��е�����
	unsigned char readBuff[1024];

	while (1)
	{
		//ʵ�ʶ�ȡ��rdsize���ֽ�
		size_t rdsize = fread(readBuff, 1, 1024, pf);
		if (rdsize == 0)
		{
			//��ȡ���ļ�ĩβ
			break;
		}

		//����ͳ��
		for (size_t i = 0; i < rdsize; i++)
		{
			//���õ���ϣ����ֱ�Ӷ��Ʒ������ַ���ASCII��ֵ��Ϊ������±���ٵ�ͳ��
			fileByteInfo[readBuff[i]].appearCount++;
		}
	}

	//2.����ͳ�ƵĽ������HuffmanTree,������ΪȨֵ
	HuffmanTree<ByteInfo> ht;

	//�ڴ���haffman��ʱҪ�޳�����Ϊ0���ַ�
	ByteInfo invalid;
	ht.CreateHuffmanTree(fileByteInfo, 256,invalid);

	//3.����Huffman����ȡÿ���ֽڵı���
	GenerateHuffmanCode(ht.GetRoot());


	//4.��ѹ����Ҫ�õ�����Ϣ
	FILE* fOut = fopen("2.hzp", "wb");

	WriteHead(fOut, filePath);
	
	//5.ʹ���ֽڵı����Դ�ļ����½��и�д
	//ע��,�ڶ�ȡpf�ļ�ʱ����Ҫ��pf�ļ�ָ��Ų����pf�ļ���ʼλ��
	//��Ϊ�տ�ʼ���ļ���ͳ���ֽڳ��ִ���ʱ�Ѿ���ȡ���ļ���pf�Ѿ����ļ�ĩβ

	//fseek(pf,  0, SEEK_SET);//���ļ�ָ���ƶ�����ʼλ��
	rewind(pf);

	unsigned char ch = 0;
	unsigned char bitCount = 0;

	while (1)
	{
		size_t rdsize = fread(readBuff, 1, 1024, pf);
		if (rdsize == 0)
		{
			break;
		}

		//�ñ����д�ַ�,�Ѹ�д���ַ��ŵ�ѹ���ļ�����
		for (size_t i = 0; i < rdsize; i++)
		{
			//readBuff[i]----��'A'---"100"
			string& strCode = fileByteInfo[readBuff[i]].strCode;

			//ֻ����ַ�����ʽ�ı������ֽ��д��

			for (size_t j = 0; j < strCode.size(); j++)
			{
				ch = ch << 1;  //��λ��������λ��0
				if (strCode[j] == '1')
				{
					ch = ch | 1;
				}
				bitCount++;

				//����λ�����8λ�󣬽����ֽ�д��ѹ���ļ���
				if (bitCount == 8)
				{
					fputc(ch, fOut);
					bitCount = 0;
				}
			}

		}
	}

	//���:ch����8����λ��ʵ����û��д��ȥ
	if (bitCount > 0 && bitCount < 8)
	{
		ch <<= (8 - bitCount);
		fputc(ch, fOut);
	}
	fclose(pf);
	fclose(fOut);
	return true;
}

//1.txt 
void FileCompress::WriteHead(FILE* fOut, const string& filePath)
{
	//1.��ȡԴ�ļ��еĺ�׺
	string postFix = filePath.substr(filePath.rfind('.')); //��ʼλ��,�Ӵ����ȣ���ĩβ�ң��ҵ�.��ȡ������Ӵ�
	postFix += "\n";
	fwrite(postFix.c_str(), 1, postFix.size(), fOut);


	//2.�����ֽڳ��ֵ�Ƶ����Ϣ�Լ�ͳ����Ч�ֽڵ��ܵ�����
	string chApperatCount;
	size_t lineCount = 0;  //���ֵ�Ƶ��
	for (size_t i = 0; i < 256; i++)
	{
		if (fileByteInfo[i].appearCount>0)
		{
			chApperatCount += fileByteInfo[i].ch; //��ȡ�ַ�           //A:2 /n B:1
			chApperatCount += ':';                //��ȡð��
			chApperatCount += to_string(fileByteInfo[i].appearCount); //��ȡ���ֵĴ���
			chApperatCount += "\n";
			lineCount++;
		}
	}

	//д�������Լ�Ƶ����Ϣ
	string totalLine = to_string(lineCount);
	totalLine += "\n";
	fwrite(totalLine.c_str(), 1, totalLine.size(), fOut);            //������
	fwrite(chApperatCount.c_str(), 1, chApperatCount.size(), fOut);  //���ֵ�Ƶ����Ϣ
}


bool FileCompress::UnCompressFile(const string& filePath)   //��ѹ��
{
	//1.��ѹ���ļ���ȡ��ѹ����Ҫ�õ�����Ϣ
	FILE* fIn = fopen(filePath.c_str(), "rb");
	if (nullptr == fIn)
	{
		cout << "���ļ�ʧ��"<<endl;
		return false;
	}


	//��ȡԴ�ļ���׺
	string postFix;
	GetLine(fIn, postFix);

	//��ȡƵ����Ϣ������
	string strConent;
	GetLine(fIn, strConent);
	size_t lineCount = atoi(strConent.c_str());

	//ѭ����ȡlineCount�У���ȡ�ֽڵ�Ƶ����Ϣ
	strConent = "";
	for (size_t i = 0; i < lineCount; i++)
	{
		//strConent = "";
		GetLine(fIn, strConent);
		if (strConent == "")
		{
			strConent += "\n";
			GetLine(fIn, strConent);
		}
		//fileByteInfo[strConent[0]].ch = strConent[0];
		fileByteInfo[(unsigned char)strConent[0]].appearCount = atoi(strConent.c_str() + 2); //A:1 
		strConent = "";

	}



	//2.�ָ�huffmanTree

	HuffmanTree<ByteInfo> ht;
	ByteInfo invalid;
	ht.CreateHuffmanTree(fileByteInfo, 256, invalid);


	//3.��ȡѹ����Ϣ,���huffmanTree���н�ѹ��
	string fileName("3");
	fileName += postFix;
	FILE* fOut = fopen(fileName.c_str(), "wb");

	unsigned char readBuff[1024];
	unsigned char bitCount = 0;
	HuffmanTreeNode<ByteInfo>* cur = ht.GetRoot();

	int fileSize = cur->weight.appearCount;    //���ڵ��weight�Ĵ�С����Դ�ļ��Ĵ�С
	int compressSize = 0;                     //��ѹ�����ļ���С

	while (1)
	{
		size_t rdsize = fread(readBuff, 1, 1024, fIn);
		if (rdsize == 0)
		{
			break;
		}
		for (size_t i = 0; i < rdsize; i++)
		{
			//����ֽڵı���λ���н�ѹ��
			unsigned char ch = readBuff[i];
			bitCount = 0;       //��bitCount��Ϊ0
			while (bitCount < 8)
			{
				if (ch & 0x80)
				{
					cur = cur->right;
				}
				else
				{
					cur = cur->left;
				}

				if (cur->left == NULL&&cur->right == NULL)
				{
					fputc(cur->weight.ch, fOut);
					cur = ht.GetRoot();

					compressSize++;
					if (compressSize == fileSize)   //����ɹ���ѹ�����ļ��Ĵ�С��Դ�ļ���С��ͬ���ѹ����
					{
						break;
					}
				}

				bitCount++;
				ch <<= 1;
			}		
		}
	}

	fclose(fIn);
	fclose(fOut);
	return true;
}
void FileCompress::GetLine(FILE* fIn,  string& strContent)   //��ȡÿһ�е��ַ�
{
	unsigned char ch;
	while (!feof(fIn))
	{
		ch = fgetc(fIn);
		if (ch == '\n')
		{
			break;
		}
		strContent += ch;
	}
}

void FileCompress::GenerateHuffmanCode(HuffmanTreeNode<ByteInfo>* root)
{
	if (root == nullptr)
	{
		return;
	}

	//��Ϊ��huffman�����У�������Ч��Ȩֵ��Ҷ�ӽڵ��λ��
	//��������Ҷ�ӽڵ������Ȩֵ��Ӧ�ı�����õ���
	if (root->left == NULL&&root->right == NULL)  //��Ҷ�ӽڵ�������Ͽ�ʼ  
	{
		HuffmanTreeNode<ByteInfo>* cur = root;             //root����һ��Ҷ��
		HuffmanTreeNode<ByteInfo>* parent = cur->parent;

		string& strCode = fileByteInfo[cur->weight.ch].strCode;  //strCode���������ı���

		while (parent)
		{
			if (cur == parent->left)
			{
				strCode = strCode + '0';
			}
			else
			{
				strCode = strCode + '1';
			}
			cur = parent;          //����������
			parent = cur->parent;
		}
		reverse(strCode.begin(), strCode.end());
	}

	GenerateHuffmanCode(root->left);
	GenerateHuffmanCode(root->right);
}