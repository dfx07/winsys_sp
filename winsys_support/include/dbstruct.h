#pragma once

#include <iostream>
#include <memory>
#include <map>
#include <assert.h>

/*
	1/ data value
*/

class CDataValue;
class CDataRow;
class CDataTable;


typedef std::shared_ptr<CDataRow>   CDataRowPtr;
typedef std::shared_ptr<CDataTable> CDataTablePtr;

//==========================================================================
// Class CDataValue
// Support data : int, double, array, object, bool
//==========================================================================
class CDataValue
{
private:
	union
	{
		int			m_ivalue;
		double		m_dvalue;
		void*		m_vvalue;
		void*		m_ovalue;
		bool		m_bvalue;
	};
	int             m_type = -1;
	int             m_nsize=  0;

private:
	void SetInt(const int& value)
	{
		m_ivalue = value;
		m_type = 0;
	}
	void SetBool(const bool& value)
	{
		m_bvalue = value;
		m_type = 1;
	}
	void SetDouble(const double& value)
	{
		m_dvalue = value;
		m_type = 2;
	}
	void SetBlob(const void* value, const int& nsize)
	{
		auto alloc = [](const int& _size)
		{
			auto blob = new char[_size];
			memset(blob, 0, _size);
			return blob;
		};

		if (m_nsize > 0)
			delete[] m_vvalue;

		// nsize + 2 using for wchar_t null
		m_vvalue = alloc(nsize + 2);
		memcpy_s(m_vvalue, nsize, value, nsize);
		m_nsize = nsize;
		m_type = 3;
	}
	void SetObject(void* value)
	{
		m_ovalue = value;
		m_type = 4;
	}

private:
	void Free()
	{
		switch (m_type)
		{
		case 0:
		case 1:
		case 2:
			std::cout << "free arithmetic " << std::endl;
			break;
		case 3:
			std::cout << "free blob " << std::endl;
			delete[] m_vvalue;
			break;
		case 4:
			std::cout << "free object " << std::endl;
			delete m_ovalue;
			break;
		default:
			break;
		}
	}

public:
	template<typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
	CDataValue(const T& value)
	{
		if (std::is_same<T, bool>::value)
		{
			SetBool(value);
		}
		else if (std::is_floating_point<T>::value)
		{
			SetDouble(value);
		}
		else if (std::is_integral<T>::value)
		{
			SetInt((int)value);
		}
		else
		{
			assert(0);
		}
	}

	template<typename T, typename std::enable_if<std::is_class<T>::value, T>::type* = nullptr>
	CDataValue(const T& value)
	{
		size_t s = (!value.empty()) ? sizeof(value[0]) : 1;
		SetBlob(value.c_str(), value.length() * sizeof(value[0]));
	}

	template<typename T, typename std::enable_if<std::is_pointer<T>::value, T>::type = nullptr>
	CDataValue(const T value, const int& nsize)
	{
		SetBlob(value, nsize);
	}

	template<typename T, typename std::enable_if<std::is_same<T, void*>::value, T>::type = nullptr>
	CDataValue(const T object)
	{
		SetObject(object);
	}

	~CDataValue()
	{
		Free();
	}
public:
	int    GetType()   const  { return m_type;   }

	template<typename T>
	int is_type() const 
	{
		if (std::is_integral<T>::value && m_type == 0)
			return true;
		if (std::is_same<T, bool>::value && m_type == 1)
			return true;
		if (std::is_floating_point<T>::value && m_type == 2)
			return true;
		if (std::is_class<T>::value && m_type == 3)
			return true;
		if (std::is_pointer<T>::value && m_type == 4)
			return true;
		return false;
	}


	int    GetInt()    const  { return m_ivalue; }
	bool   GetBool()   const  { return m_bvalue; }
	double GetDouble() const  { return m_dvalue; }
	void*  GetValue()  const
	{ 
		if (m_type == 3)
			return (char*)m_vvalue;
		else if (m_type == 4)
			return (char*)m_ovalue;
		return NULL;
	}
	int   GetSize()    const { return m_nsize;  }
	int   GetBlobSize()const { return m_nsize;  }
};

//==========================================================================
// Class CDataRow
// Support : index cell + data cell
//==========================================================================
class CDataRow
{
	std::map<const std::string, std::shared_ptr<CDataValue>> m_data;
	std::vector<std::shared_ptr<CDataValue>> m_data_index;

private:
	bool _Add(const std::string& key, std::shared_ptr<CDataValue> ptr)
	{
		if (m_data.find(key) != m_data.end())
		{
			_Replace(key, ptr);
			assert("Key is already");
			return true;
		}
		m_data.insert(std::make_pair(key, ptr));
		m_data_index.push_back(ptr);
		return true;
	}

	void _Replace(const std::string& key, std::shared_ptr<CDataValue> ptr)
	{
		auto found = m_data.find(key);
		if (found != m_data.end())
		{
			m_data.insert(std::make_pair(key, ptr));

			// reference avoids copying element
			auto iter = std::find(m_data_index.begin(), m_data_index.end(), found->second);

			if (iter != m_data_index.end())
			{
				*iter = ptr;
			}
		}
	}
public:
	void Remove(const std::string& key)
	{
		auto found = m_data.find(key);
		if (found != m_data.end())
		{
			auto a =std::remove(m_data_index.begin(), m_data_index.end(), found->second);
			m_data.erase(key);
		}
	}

	void Remove(const int& index)
	{
		if (index < 0 || index > m_data_index.size())
			return;
		auto condi = m_data_index[index];

		for (auto iter = m_data.begin(); iter != m_data.end();)
		{
			if (iter->second == condi)
			{
				iter = m_data.erase(iter);
			}
			else {
				++iter;
			}
		}
	}
public:

	template<typename T, typename std::enable_if<!std::is_pointer<T>::value, T>::type* = nullptr>
	void Add(const std::string& key, const T& value)
	{
		std::shared_ptr<CDataValue> ptr = std::make_shared<CDataValue>(value);
		_Add(key, ptr);
	}

	void Add(const std::string& key, const char* value, const int& nsize = -1)
	{
		size_t _size = (nsize <= 0) ? strlen(value) : nsize;
		std::shared_ptr<CDataValue> ptr = std::make_shared<CDataValue>(value, (int)_size);
		_Add(key, ptr);
	}

	void Add(const std::string& key, const wchar_t* value, const int& nsize = -1)
	{
		size_t _size = (nsize <= 0) ? wcslen(value)*sizeof(wchar_t) : nsize;
		std::shared_ptr<CDataValue> ptr = std::make_shared<CDataValue>(value, (int)_size);
		_Add(key, ptr);
	}

	void Add(const std::string& key, void* object)
	{
		std::shared_ptr<CDataValue> ptr = std::make_shared<CDataValue>(object);
		_Add(key, ptr);
	}

public:
	CDataValue* operator[](const std::string& key) const
	{
		if (m_data.find(key) == m_data.end())
			return NULL;
		return m_data.at(key).get();
	}

	CDataValue* operator[](const int& i) const
	{
		if (i > 0 && i < m_data_index.size())
			return NULL;
		return m_data_index[i].get();
	}

	int Size() const
	{
		return (int)m_data.size();
	}

	static CDataRowPtr CreateInstance()
	{
		return std::make_shared<CDataRow>();
	}
};

//==========================================================================
// Class CDataTable
// Contain data : string , wstring , object, arithmetic type
//==========================================================================
class CDataTable
{
	std::vector<CDataRowPtr> m_data;

public:
	void AddRow(CDataRowPtr row)
	{
		m_data.push_back(row);
	}
	int Size() const
	{
		return (int)m_data.size();
	}

	CDataRow* operator[](const int& i) const
	{
		if (i <0 || i> m_data.size())
			return nullptr;
		return m_data[i].get();
	}

	CDataValue* FirstValue()
	{
		if (m_data.empty())
			return NULL;
		CDataRow* _row = m_data[0].get();
		if (!_row || _row->Size() <= 0)
			return NULL;

		return _row[0][0];
	}

	template<typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
	T SingleOrDefault(const T& defvalue)
	{
		T value = defvalue;
		CDataValue* val = FirstValue();
		if (!val)
			return value;

		if (std::is_integral<T>::value && val->is_type<int>())
			value = static_cast<T>(val->GetInt());
		else if (std::is_floating_point<T>::value && val->is_type<double>())
			value = static_cast<T>(val->GetDouble());
		else if (std::is_same<T, bool>::value && val->is_type<bool>())
			value = static_cast<T>(val->GetBool());
		return value;
	};
	
	template<typename T, typename std::enable_if<std::is_class<T>::value, T>::type* = nullptr>
	T* SingleOrDefault(T* defvalue =nullptr)
	{
		T* value = defvalue;
		CDataValue* val = FirstValue();
		if (!val)
			return value;
		auto raw_value = val->GetValue();
		if (raw_value && val->is_type<T*>())
		{
			value = (T*)raw_value;
		}

		return value;
	}

	template<typename T, typename std::enable_if<std::is_pointer<T>::value, T>::type* = nullptr>
	T SingleOrDefault(T defvalue = nullptr)
	{
		T value = defvalue;
		CDataValue* val = FirstValue();
		if (!val)
			return value;
		auto raw_value = val->GetValue();
		if (raw_value)
			value = (T)raw_value;

		return value;
	}

	template<typename T, typename std::enable_if<std::is_base_of<std::wstring, T>::value, T>::type* = nullptr>
	std::wstring SingleOrDefault(const std::wstring& defvalue = L"")
	{
		T value = defvalue;
		CDataValue* val = FirstValue();
		if (!val)
			return value;

		auto raw_value = val->GetValue();
		if (raw_value && val->is_type<T>())
		{
			auto isize = val->GetSize();
			value.assign((wchar_t*)raw_value, isize);
		}
		return value;
	}

	template<typename T, typename std::enable_if<std::is_base_of<std::string, T>::value, T>::type* = nullptr>
	std::string SingleOrDefault(const std::string& defvalue = "")
	{
		T value = defvalue;
		CDataValue* val = FirstValue();
		if (!val)
			return value;

		auto raw_value = val->GetValue();
		if (raw_value && val->is_type<T>())
		{
			auto isize = val->GetSize();
			value.assign((char*)raw_value, isize);
		}
		return value;
	}
};



