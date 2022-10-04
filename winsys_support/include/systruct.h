#pragma once

#include <iostream>
#include <vector>

// CBuffer 
class CBuffer
{
private:
	//@brief  free memory the buffer
	void free()
	{
		delete[] m_data;
		m_capacity = 0;
		m_length   = 0;
		m_data     = NULL;
	}

	//@brief  allocate memory for buffer
	bool alloc(const int& nsize, const bool bcp)
	{
		// not allocate memory if capacity bigger
		if (nsize <= m_capacity)
		{
			// set null memory if not copy data
			if (!bcp)
			{
				memset(m_data, 0, m_length);
				m_length = 0;
			}
			return true;
		}

		// allocate new buffer and set NULL
		char* newbuff = new char[nsize];
		memset(newbuff, 0, nsize);

		// copy old memory to new memory
		if (bcp)
			memcpy_s(newbuff, m_length, m_data, m_length);
		else
			m_length = 0;

		delete[] m_data;

		m_data = newbuff;
		m_capacity = nsize;
	}

public:
	CBuffer():
		m_data(NULL), m_capacity(0), m_length(0)
	{

	}

	CBuffer(const void* data, const int nsize)
	{
		set(data, nsize);
	}

	~CBuffer()
	{
		this->free();
	}

public:
	void reset()
	{
		this->free();
	}

	bool set(const void* data, const int nsize)
	{
		if (!alloc(nsize + 1, false)) 
			return false;

		errno_t err = memcpy_s(m_data, nsize, data, nsize);
		if (err) //error
		{
			std::cerr << "set data error" << std::endl;
			return false;
		}
		m_length = nsize;

		return true;
	}

	void reserve(const int nsize)
	{
		alloc(nsize, true);
	}
	
	void* get()
	{
		return (void*)m_data;
	}

	int size()
	{
		return m_length;
	}

private:
	char*	m_data;
	int		m_capacity;
	int		m_length;
};

class CFileBuffer
{
private:
	std::vector<unsigned char> m_data;

public:
	CFileBuffer(const int& nsize = 0)
	{
		resize(nsize);
	}
public:
	void resize(const int& nsize)
	{
		m_data.resize(nsize);
	}

	void set(void* data, const int& nsize)
	{
		// set data for vector data have null
		resize(nsize + 1);
		reset(nsize + 1, 0);
		memcpy_s(&m_data[0], nsize, data, nsize);
		// reset length vector data
		resize(nsize);
	}

	void reset()
	{
		m_data.clear();
	}

	void reset(const int nsize, const int& val = 0)
	{
		memset(&m_data[0], val, nsize);
	}
public:
	void* get() { return !m_data.empty() ? &m_data[0] : NULL; }
	int   size(){ return (int)m_data.size(); }
};


// 