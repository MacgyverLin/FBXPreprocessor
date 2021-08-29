#ifndef _DataOptimizer_h_
#define _DataOptimizer_h_

#include "Common.h"

template<class T>
class DataOptimizer
{
public:
	DataOptimizer(bool enabledOptimize_ = true)
	: enabledOptimize(enabledOptimize_)
	, maxIndex(-1)
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
		this->maxIndex = other.maxIndex;
		this->datasMap = other.datasMap;

		this->datas = other.datas;
		this->indices = other.indices;
	}

	DataOptimizer& operator = (const DataOptimizer& other)
	{
		this->enabledOptimize = other.enabledOptimize;
		this->maxIndex = other.maxIndex;
		this->datasMap = other.datasMap;

		this->datas = other.datas;
		this->indices = other.indices;

		return *this;
	}

	int Add(const T& data)
	{
		int idx;
		if (enabledOptimize)
		{
			std::map<T, int>::iterator itr = datasMap.find(data);

			if (itr == datasMap.end())
			{
				idx = datas.size();

				datasMap[data] = idx;
				datas.push_back(data);
			}
			else
			{
				idx = itr->second;
			}
		}
		else
		{
			idx = datas.size();

			datasMap[data] = idx;
			datas.push_back(data);
		}

		if (maxIndex < idx)
			maxIndex = idx;
		indices.push_back(idx);
		return idx;
	}

	void Clear()
	{
		maxIndex = -1;
		datasMap.clear();

		datas.clear();
		indices.clear();
	}

	void EnabledOptimize()
	{
		enabledOptimize = true;
	}

	void DisabledOptimize()
	{
		enabledOptimize = false;
	}

	bool IsEnabledOptimize() const
	{
		return enabledOptimize;
	}

	int GetMaxIndex() const
	{
		return maxIndex;
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
		std::map<T, int>::const_iterator itr = datasMap.find(data);
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
	
	int maxIndex;
	std::map<T, int> datasMap;

	std::vector<T> datas;
	std::vector<int> indices;
};

#endif