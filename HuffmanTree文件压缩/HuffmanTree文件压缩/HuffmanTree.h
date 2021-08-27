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
	W weight;  //权值

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

	void CreateHuffmanTree(const W array[], size_t size,const W& invalid)  //权值和元素个数
	{
		//优先级队列默认是大堆,只需要修改其比较规则,仿函数

		//std::priority_queue<Node*,vector<Node*>,greater<Node*>> q;  注意后面博客显示出来
		//这里写最后是按地址的大小比较,因为传过去的是节点的指针

		std::priority_queue<Node*, vector<Node*>, Com<W>> q;

		//1.先使用所给的权值创建只有根节点二叉树的森林
		for (size_t i = 0; i < size; i++)
		{
			if (array[i] != invalid)
			{
				q.push(new Node(array[i]));
			}
		}

		//2.循环进行一下步骤，直到二叉树森林只剩下一颗二叉树为止

		while (q.size()>1)
		{
			//从二叉树森林中去权值最小的两颗二叉树
			Node* left = q.top();
			q.pop();

			Node* right = q.top();
			q.pop();

			//将left和right作为某个新节点的左右子树，构造一个新的二叉树
			//将新二叉树根节点的权值就是left和right两颗子树的权值之和

			Node* parent = new Node(left->weight + right->weight);
			parent->left = left;
			parent->right = right;
			left->parent = parent;
			right->parent = parent;
			//将这个新的二叉树插入到二叉树森林中
			q.push(parent);
		}
		
		//这个root就是HuffmanTree树的根节点
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