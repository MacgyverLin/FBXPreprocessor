#ifndef _BSP_h_
#define _BSP_h_

#include "Plane.h"
#include <queue>

class BSP
{
public:
	BSP(int id_ = -1)
		: splitPlane()
		, id(id_)
		, left(nullptr)
		, right(nullptr)
	{
	}

	~BSP()
	{
		if (left)
		{
			delete left;
			left = nullptr;
		}

		if (right)
		{
			delete right;
			right = nullptr;
		}
	}

	int GetID() const
	{
		return id;
	}
	
	const Plane& GetPlane() const
	{
		return splitPlane;
	}

	const BSP* GetLeft() const
	{
		return left;
	}

	const BSP* GetRight() const
	{
		return right;
	}

	void Split(const Plane& splitPlane_)
	{
		if (!left && !right)
		{
			splitPlane = splitPlane_;
			left = new BSP(2 * id + 1);
			right = new BSP(2 * id + 2);
		}
		else
		{
			if (left)
				left->Split(splitPlane_);

			if (right)
				right->Split(splitPlane_);
		}
	}

	void BFS(std::function<void(const BSP*)> cb)
	{
		std::queue<BSP* > queue;

		queue.push(this);
		while (!queue.empty())
		{
			BSP* current = queue.front();
			cb(current);
			queue.pop();

			if (current->left)
			{
				queue.push(current->left);
			}

			if (current->right)
			{
				queue.push(current->right);
			}
		}
	}

	void DFS(std::function<void(const BSP*)> cb)
	{
		cb(this);

		if (left)
			left->DFS(cb);

		if (right)
			right->DFS(cb);
	}
private:
	int id;
	Plane splitPlane;
	BSP* left;
	BSP* right;
};

#endif