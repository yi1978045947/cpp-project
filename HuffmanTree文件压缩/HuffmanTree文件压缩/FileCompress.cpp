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
		cout << "打开压缩文件失败!" << endl;
		return false;
	}

	//文件的大小不知道,需要循环获取源文件中的内容
	unsigned char readBuff[1024];

	while (1)
	{
		//实际读取的rdsize个字节
		size_t rdsize = fread(readBuff, 1, 1024, pf);
		if (rdsize == 0)
		{
			//读取到文件末尾
			break;
		}

		//进行统计
		for (size_t i = 0; i < rdsize; i++)
		{
			//利用到哈希里面直接定制法，以字符的ASCII的值作为数组的下标快速的统计
			fileByteInfo[readBuff[i]].appearCount++;
		}
	}

	//2.根据统计的结果创建HuffmanTree,次数作为权值
	HuffmanTree<ByteInfo> ht;

	//在创建haffman树时要剔除出现为0的字符
	ByteInfo invalid;
	ht.CreateHuffmanTree(fileByteInfo, 256,invalid);

	//3.借助Huffman树获取每个字节的编码
	GenerateHuffmanCode(ht.GetRoot());


	//4.解压缩需要用到的信息
	FILE* fOut = fopen("2.hzp", "wb");

	WriteHead(fOut, filePath);
	
	//5.使用字节的编码对源文件重新进行改写
	//注意,在读取pf文件时，需要将pf文件指针挪动到pf文件起始位置
	//因为刚开始在文件中统计字节出现次数时已经都取过文件，pf已经在文件末尾

	//fseek(pf,  0, SEEK_SET);//把文件指针移动到起始位置
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

		//用编码改写字符,把改写的字符放到压缩文件当中
		for (size_t i = 0; i < rdsize; i++)
		{
			//readBuff[i]----》'A'---"100"
			string& strCode = fileByteInfo[readBuff[i]].strCode;

			//只需把字符串格式的编码往字节中存放

			for (size_t j = 0; j < strCode.size(); j++)
			{
				ch = ch << 1;  //高位丢弃，低位补0
				if (strCode[j] == '1')
				{
					ch = ch | 1;
				}
				bitCount++;

				//比特位填充完8位后，将该字节写入压缩文件中
				if (bitCount == 8)
				{
					fputc(ch, fOut);
					bitCount = 0;
				}
			}

		}
	}

	//检测:ch不够8比特位，实际是没有写进去
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
	//1.获取源文件中的后缀
	string postFix = filePath.substr(filePath.rfind('.')); //起始位置,子串长度，从末尾找，找到.截取后面的子串
	postFix += "\n";
	fwrite(postFix.c_str(), 1, postFix.size(), fOut);


	//2.构造字节出现的频次信息以及统计有效字节的总的行数
	string chApperatCount;
	size_t lineCount = 0;  //出现的频次
	for (size_t i = 0; i < 256; i++)
	{
		if (fileByteInfo[i].appearCount>0)
		{
			chApperatCount += fileByteInfo[i].ch; //获取字符           //A:2 /n B:1
			chApperatCount += ':';                //获取冒号
			chApperatCount += to_string(fileByteInfo[i].appearCount); //获取出现的次数
			chApperatCount += "\n";
			lineCount++;
		}
	}

	//写总行数以及频次信息
	string totalLine = to_string(lineCount);
	totalLine += "\n";
	fwrite(totalLine.c_str(), 1, totalLine.size(), fOut);            //总行数
	fwrite(chApperatCount.c_str(), 1, chApperatCount.size(), fOut);  //出现的频次信息
}


bool FileCompress::UnCompressFile(const string& filePath)   //解压缩
{
	//1.从压缩文件读取解压缩需要用到的信息
	FILE* fIn = fopen(filePath.c_str(), "rb");
	if (nullptr == fIn)
	{
		cout << "打开文件失败"<<endl;
		return false;
	}


	//读取源文件后缀
	string postFix;
	GetLine(fIn, postFix);

	//读取频次信息总行数
	string strConent;
	GetLine(fIn, strConent);
	size_t lineCount = atoi(strConent.c_str());

	//循环读取lineCount行；获取字节的频次信息
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



	//2.恢复huffmanTree

	HuffmanTree<ByteInfo> ht;
	ByteInfo invalid;
	ht.CreateHuffmanTree(fileByteInfo, 256, invalid);


	//3.读取压缩信息,结合huffmanTree进行解压缩
	string fileName("3");
	fileName += postFix;
	FILE* fOut = fopen(fileName.c_str(), "wb");

	unsigned char readBuff[1024];
	unsigned char bitCount = 0;
	HuffmanTreeNode<ByteInfo>* cur = ht.GetRoot();

	int fileSize = cur->weight.appearCount;    //根节点的weight的大小就是源文件的大小
	int compressSize = 0;                     //解压缩的文件大小

	while (1)
	{
		size_t rdsize = fread(readBuff, 1, 1024, fIn);
		if (rdsize == 0)
		{
			break;
		}
		for (size_t i = 0; i < rdsize; i++)
		{
			//逐个字节的比特位进行解压缩
			unsigned char ch = readBuff[i];
			bitCount = 0;       //把bitCount变为0
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
					if (compressSize == fileSize)   //如果成功解压缩的文件的大小与源文件大小相同则解压结束
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
void FileCompress::GetLine(FILE* fIn,  string& strContent)   //读取每一行的字符
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

	//因为在huffman树当中，所有有效的权值在叶子节点的位置
	//当遍历到叶子节点初，该权值对应的编码就拿到了
	if (root->left == NULL&&root->right == NULL)  //在叶子节点从下往上开始  
	{
		HuffmanTreeNode<ByteInfo>* cur = root;             //root就是一个叶子
		HuffmanTreeNode<ByteInfo>* parent = cur->parent;

		string& strCode = fileByteInfo[cur->weight.ch].strCode;  //strCode是这个后面的别名

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
			cur = parent;          //继续向上走
			parent = cur->parent;
		}
		reverse(strCode.begin(), strCode.end());
	}

	GenerateHuffmanCode(root->left);
	GenerateHuffmanCode(root->right);
}