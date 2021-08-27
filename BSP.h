#ifndef _BSP_h_
#define _BSP_h_

#include "Plane.h"
#include <queue>

class BSP
{
public:
	BSP()
		: splitPlane()
	{
		left = nullptr;
		right = nullptr;
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

	const Plane& GetPlane() const
	{
		return splitPlane;
	}

	void Split(const Plane& splitPlane_)
	{
		if (!left && !right)
		{
			splitPlane = splitPlane_;
			left = new BSP();
			right = new BSP();
		}
		else
		{
			if (left)
				left->Split(splitPlane_);

			if (right)
				right->Split(splitPlane_);
		}
	}

	void BFS(std::function<void(BSP*)> cb)
	{
		std::queue<BSP*> queue;

		queue.push(this);
		while (!queue.empty())
		{
			cb(queue.front());
			queue.pop();

			if (left)
			{
				queue.push(left);
			}

			if (right)
			{
				queue.push(right);
			}
		}
	}

	void DFS(std::function<void(BSP*)> cb)
	{
		cb(this);

		if (left)
			left->DFS(cb);

		if (right)
			right->DFS(cb);
	}
private:
	Plane splitPlane;
	BSP* left;
	BSP* right;
};

#endif