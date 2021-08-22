#ifndef _DataOptimizer_h_
#define _DataOptimizer_h_

#include "Common.h"
#include <vector>
#include <map>
#include <assert.h>

template<class T>
class DataOptimizer
{
public:
	DataOptimizer(bool enabledOptimize_ = true)
	: enabledOptimize(enabledOptimize_)
	, datasMap()
	, datas()
	, indices()
	{
	}

	~DataOptimizer()
	{
	}

	DataOptimizer(const DataOptimizer& other)
	{
		this->enabledOptimize = other.enabledOptimize;

		this->datasMap = other.datasMap;

		this->datas = other.datas;
		this->indices = other.indices;
	}

	DataOptimizer& operator = (const DataOptimizer& other)
	{
		this->enabledOptimize = other.enabledOptimize;

		this->datasMap = other.datasMap;

		this->datas = other.datas;
		this->indices = other.indices;

		return *this;
	}

	int Add(const T& data)
	{
		if (enabledOptimize)
		{
			std::map<T, int>::iterator itr = datasMap.find(data);

			int idx;
			if (itr == datasMap.end())
			{
				idx = datas.size();

				datasMap[data] = idx;
				datas.push_back(data);

				indices.push_back(idx);
				return idx;
			}
			else
			{
				idx = itr->second;
				
				indices.push_back(idx);
				return idx;
			}
		}
		else
		{
			int idx = datas.size();

			datasMap[data] = idx;
			datas.push_back(data);
			
			indices.push_back(idx);

			return idx;
		}
	}

	void Clear()
	{
		datasMap.clear();

		datas.clear();
		indices.clear();
	}

	size_t GetDataCount() const
	{
		return datas.size();
	}

	const T& GetData(int i) const
	{
		return datas[i];
	}

	const std::vector<T>& GetDatas() const
	{
		return datas;
	}

	size_t GetIndicesCount() const
	{
		return indices.size();
	}

	int GetIndex(int i) const
	{
		return indices[i];
	}

	int GetIndex(const T& data) const
	{
		std::map<T, int>::iterator itr = datasMap.find(data);
		if (itr == datasMap.end())
			return -1;
		else
			return itr->second;
	}

	const std::vector<int>& GetIndices() const
	{
		return indices;
	}
private:
	bool enabledOptimize;

	std::map<T, int> datasMap;

	std::vector<T> datas;
	std::vector<int> indices;
};

#endif