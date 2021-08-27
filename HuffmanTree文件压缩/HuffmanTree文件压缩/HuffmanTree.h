#pragma once
#include<iostream>
#include<queue>
#include<vector>
//#include<functional>
using namespace std;

template<class W>
struct HuffmanTreeNode
{
	HuffmanTreeNode<W>* left;
	HuffmanTreeNode<W>* right;
	HuffmanTreeNode<W>* parent;
	W weight;  //Ȩֵ

	HuffmanTreeNode(const W& w=W())
		:left(nullptr)
		, right(nullptr)
		, parent(nullptr)
		, weight(w)
	{}
};

template<class W>
struct Com
{
	typedef HuffmanTreeNode<W> Node;
	bool operator()(const Node* left,const Node* right)
	{
		return left->weight > right->weight;
	}
};

template<class W>
class HuffmanTree
{
	typedef HuffmanTreeNode<W> Node;
public:
	HuffmanTree()
		:root(nullptr)
	{}
	~HuffmanTree()
	{
		Destroy(root);
	}

	void CreateHuffmanTree(const W array[], size_t size,const W& invalid)  //Ȩֵ��Ԫ�ظ���
	{
		//���ȼ�����Ĭ���Ǵ��,ֻ��Ҫ�޸���ȽϹ���,�º���

		//std::priority_queue<Node*,vector<Node*>,greater<Node*>> q;  ע����沩����ʾ����
		//����д����ǰ���ַ�Ĵ�С�Ƚ�,��Ϊ����ȥ���ǽڵ��ָ��

		std::priority_queue<Node*, vector<Node*>, Com<W>> q;

		//1.��ʹ��������Ȩֵ����ֻ�и��ڵ��������ɭ��
		for (size_t i = 0; i < size; i++)
		{
			if (array[i] != invalid)
			{
				q.push(new Node(array[i]));
			}
		}

		//2.ѭ������һ�²��裬ֱ��������ɭ��ֻʣ��һ�Ŷ�����Ϊֹ

		while (q.size()>1)
		{
			//�Ӷ�����ɭ����ȥȨֵ��С�����Ŷ�����
			Node* left = q.top();
			q.pop();

			Node* right = q.top();
			q.pop();

			//��left��right��Ϊĳ���½ڵ����������������һ���µĶ�����
			//���¶��������ڵ��Ȩֵ����left��right����������Ȩֵ֮��

			Node* parent = new Node(left->weight + right->weight);
			parent->left = left;
			parent->right = right;
			left->parent = parent;
			right->parent = parent;
			//������µĶ��������뵽������ɭ����
			q.push(parent);
		}
		
		//���root����HuffmanTree���ĸ��ڵ�
		root = q.top();  
	}

	Node* GetRoot()
	{
		return root;
	}

	void Destroy(Node* & root)
	{
		if (root)
		{
			Destroy(root->left);
			Destroy(root->right);
			delete root;
			root = nullptr;
		}
	}
private:
	Node* root;
};

//void TestHuffmanTree()
//{
//	int array[] = { 3, 1, 7, 5 };
//	HuffmanTree<int> h;
//	h.CreateHuffmanTree(array, sizeof(array) / sizeof(array[0]));
//}