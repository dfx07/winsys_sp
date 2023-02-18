/*!*********************************************************************************
* @Copyright (C) 2021-2022 thuong.nv <thuong.nv.mta@gmail.com>
*            All rights reserved.
************************************************************************************
* @file     xsystype.h
* @create   Nov 15, 2022
* @brief    Defines the data types used in the library
* @note     For conditions of distribution and use, see copyright notice in readme.txt
************************************************************************************/
#ifndef XSYSTYPE_H
#define XSYSTYPE_H

#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>


//#if defined(_WIN32) || defined(_WIN64)
//
//#endif

#include "xsysdef.h"

___BEGIN_NAMESPACE___

/// ////////////////////////////////////////////////////////////////////////////////
/// name  : MonitorInfo class
/// brief : Provides a container for content encoded using multipart/form-data MIME type
/// ////////////////////////////////////////////////////////////////////////////////
struct MonitorInfo
{
	DWORD		  WIDTH;
	DWORD		  HEIGHT;
	DWORD		  DISFREQ;
	DWORD		  VERSION;
	std::string   NAME;
};

#if defined(_WIN32) || defined(_WIN64)
/// ////////////////////////////////////////////////////////////////////////////////
/// GDIplusToken struct - WINDOW
/// Provides a container GDI data
/// ////////////////////////////////////////////////////////////////////////////////
//struct GDIplusToken
//{
//	ULONG_PTR						 m_id;
//	Gdiplus::GdiplusStartupInput     m_prop;
//};
#endif

/// ////////////////////////////////////////////////////////////////////////////////
/// name  : xBuffer class
/// brief : Provides a container byte data
/// ////////////////////////////////////////////////////////////////////////////////
class Dllexport xBuffer
{
protected:

	/***********************************************************************************
	*! @brief  : reserve memory data [use Buffer]
	*! @author : thuong.nv - [CreateDate] : 18/02/2023
	*! @note   : These functions only handle the inside and don't expose it to the outside
	************************************************************************************/
	void reserve(const size_t nsize)
	{
		alloc(nsize, true);
	}

	/***********************************************************************************
	*! @brief  : Clear information data (not free data) [Buffer + const Buffer]
	*! @author : thuong.nv - [CreateDate] : 18/02/2023
	*! @note   : These functions only handle the inside and don't expose it to the outside
	************************************************************************************/
	void clear_data()
	{
		m_data		= NULL;
		m_capacity  = 0;
		m_length	= 0;
	}

	/***********************************************************************************
	*! @brief  : delete memory and clear information data [Buffer]
	*! @author : thuong.nv - [CreateDate] : 18/02/2023
	*! @note   : These functions only handle the inside and don't expose it to the 
	************************************************************************************/
	void free_data()
	{
		delete[] m_data;
		this->clear_data();
	}

	/***********************************************************************************
	*! @brief  : not delete and set null memory [Buffer]
	*! @author : thuong.nv - [CreateDate] : 18/02/2023
	*! @note   : These functions only handle the inside and don't expose it to the outside
	************************************************************************************/
	void reset_data(int val = 0)
	{
		if (!m_data) return;
		memset(m_data, val, m_capacity);
		m_length = 0;
	}

	/***********************************************************************************
	*! @brief  : allocate memory data [Buffer]
	*! @return : size_t : allocated memory size
	*! @param  : [in]	cp : copy old data (false)
	*! @author : thuong.nv - [CreateDate] : 18/02/2023
	*! @note   : These functions only handle the inside and don't expose it to the outside
	************************************************************************************/
	size_t alloc(size_t nsize, bool cp = false)
	{
		// Trường hợp đã cấp phát và cấp phát nhỏ hơn
		if (nsize <= m_capacity)
		{
			if (!cp)
			{
				memset(m_data, 0, m_capacity);
				m_length = 0;
			}
			return nsize;
		}

		// Trường hợp cấp phát mới hoặc thêm
		try
		{
			auto old_data = m_data;
			m_capacity = nsize + 2;

			m_data = new char[m_capacity];
			memset(m_data, 0, m_capacity);

			if (cp && old_data)
			{
				memcpy_s(m_data, m_length, old_data, m_length);
			}
			else
			{
				m_length = 0;
			}

			delete[] old_data;
		}
		catch (std::exception& ex)
		{
			SAFE_DELETE_ARRAY(m_data);
			m_capacity = m_length = 0;
			return 0;
		}

		return nsize;
	}

	/***********************************************************************************
	*! @brief  : append allocate memory data - use [Buffer]
	*! @return : size_t : allocated memory size append
	*! @author : thuong.nv - [CreateDate] : 18/02/2023
	*! @note   : These functions only handle the inside and don't expose it to the outside
	************************************************************************************/
	size_t alloc_append(size_t nsize_append)
	{
		auto newsize = nsize_append + m_length;

		if (newsize <= m_capacity)
			return nsize_append;

		// Trường hợp không đủ dữ liệu
		return alloc(newsize, true);
	}

	/***********************************************************************************
	*! @brief  : allocate and copy data form the source data [Buffer]
	*! @return : void
	*! @author : thuong.nv - [CreateDate] : 18/02/2023
	*! @note   : These functions only handle the inside and don't expose it to the outside
	************************************************************************************/
	void copy_data(const void* data, const size_t nsize)
	{
		if (alloc(nsize) > 0)
		{
			memcpy_s(m_data, nsize, data, nsize);
			m_length = nsize;
		}
	}

	/***********************************************************************************
	*! @brief  : delete and move data from the source data [Buffer]
	*! @return : void
	*! @author : thuong.nv - [CreateDate] : 18/02/2023
	*! @note   : These functions only handle the inside and don't expose it to the outside
	************************************************************************************/
	void move_data(void* data, const size_t capacity, const size_t length, bool bfree_old = true)
	{
		if (bfree_old)
			this->free_data();

		m_data	   = data;
		m_length   = length;
		m_capacity = capacity;
	}

	/***********************************************************************************
	*! @brief  : append data from source data [Buffer]
	*! @return : void
	*! @author : thuong.nv - [CreateDate] : 18/02/2023
	*! @note   : These functions only handle the inside and don't expose it to the outside
	************************************************************************************/
	void append_data(const void* data, const size_t nsize)
	{
		if (alloc_append(nsize) > 0)
		{
			this->set_data(data, nsize, m_length);
		}
	}

	/***********************************************************************************
	*! @brief  : set data use offset [Buffer]
	*! @return : void
	*! @author : thuong.nv - [CreateDate] : 18/02/2023
	*! @note   : These functions only handle the inside and don't expose it to the outside
	************************************************************************************/
	void set_data(const void* data, const size_t nsize, size_t offset)
	{
		size_t newsize = offset + nsize;

		if (alloc(newsize, true) > 0)
		{
			memcpy_s((char*)(m_data)+offset, nsize, data, nsize);
		}
	}

public:
	xBuffer() : m_data(NULL)
		, m_length(0) , m_capacity(0)
	{

	}

	virtual const size_t length() const { return m_length; };

protected:
	// data struct union use buffer || ref
	union
	{
		void*		m_data;
		const void* m_const_data;
	};
	
	size_t		 m_length;
	size_t		 m_capacity;
};

/// ////////////////////////////////////////////////////////////////////////////////
/// name  : CBuffer class
/// brief : Provides a container byte data
/// ////////////////////////////////////////////////////////////////////////////////
class Dllexport CBuffer : public xBuffer
{
	// Disable : Function base class 
private:

	// Enable : Function base class -> public
public:
	using xBuffer::reserve;

public:
	virtual void* data() const { return m_data; };

public:
	CBuffer(): xBuffer()
	{

	}

	CBuffer(const void* data, const int nsize)
		:xBuffer()
	{
		this->set(data, nsize);
	}

	~CBuffer() 
	{
		this->free_data();
	}

// Use old function base -> public
public:
	

public:
	bool set(const void* data, const size_t nsize)
	{
		this->copy_data(data, nsize);
		return true;
	}

	const CBuffer& operator+=(const CBuffer& buff)
	{
		this->append_data(buff.data(), buff.length());
		return *this;
	}

	CBuffer& operator+=(CBuffer&& buff_move) noexcept
	{
		this->append_data(buff_move.data(), buff_move.length());
		buff_move.free_data();
		return *this;
	}

	const CBuffer& operator=(const CBuffer& buff)
	{
		this->reset_data();
		this->copy_data(buff.data(), buff.length());
	}

	CBuffer& operator=(CBuffer&& buff_move) noexcept
	{
		this->move_data(buff_move.data(), buff_move.length(), buff_move.m_capacity, true);
	}
};

/// ////////////////////////////////////////////////////////////////////////////////
/// name  : CBufferRef class
/// brief : Provides a container byte data not free data 
/// note  : Contains only information without changing data
/// ////////////////////////////////////////////////////////////////////////////////
class Dllexport CBufferRef : public xBuffer
{
	// Disable : Function base class 
private:
	using xBuffer::reserve;

	// Enable : Function base class -> public
public:


public:
	virtual const void* data() const { return m_const_data; };

public:
	CBufferRef()
	{
		m_const_data = NULL;
		m_length = m_capacity = 0;
	}

	CBufferRef(const void* data, const size_t nsize)
	{
		m_const_data = NULL;
		this->set(data, nsize);
	}

	~CBufferRef()
	{
		// don't free data
		this->clear_data();
	}

public:
	bool set(const void* data, const size_t nsize)
	{
		m_const_data = data;
		m_length = nsize;

		return true;
	}

	const CBufferRef& operator=(const CBufferRef& buff)
	{
		m_const_data = buff.m_const_data;
		m_length     = buff.m_length;
	}

	CBufferRef& operator=(CBufferRef&& buff_move) noexcept
	{
		m_const_data = buff_move.m_data;
		m_length     = buff_move.m_length;
	}
};

/// ////////////////////////////////////////////////////////////////////////////////
/// CBuffer class
/// Provides a container byte data
/// ////////////////////////////////////////////////////////////////////////////////
//class CFileBuffer
//{
//private:
//	std::vector<unsigned char> m_data;
//
//public:
//	CFileBuffer(const int& nsize = 0)
//	{
//		resize(nsize);
//	}
//public:
//	void resize(const int& nsize)
//	{
//		m_data.resize(nsize);
//	}
//
//	void set(void* data, const int& nsize)
//	{
//		// set data for vector data have null
//		resize(nsize + 1);
//		reset(nsize + 1, 0);
//		memcpy_s(&m_data[0], nsize, data, nsize);
//		// reset length vector data
//		resize(nsize);
//	}
//
//	void reset()
//	{
//		m_data.clear();
//	}
//
//	void reset(const int nsize, const int& val = 0)
//	{
//		memset(&m_data[0], val, nsize);
//	}
//public:
//	void* get() { return !m_data.empty() ? &m_data[0] : NULL; }
//	int   size(){ return (int)m_data.size(); }
//};
//
//class CTimer
//{
//	typedef std::chrono::steady_clock::time_point	TimePointer;
//
//private:
//	TimePointer m_tstart;
//
//public:
//	CTimer()
//	{
//		reset();
//	}
//public:
//	static TimePointer now()
//	{
//		return std::chrono::high_resolution_clock::now();
//	}
//
//	// default format : "%Y-%m-%d %X"
//	// ref : https://www.programiz.com/python-programming/datetime/strftime
//	static std::string time_now(const char* format = "%Y-%m-%d %X")
//	{
//		auto now = std::chrono::system_clock::now();
//		std::time_t end_time = std::chrono::system_clock::to_time_t(now);
//
//		struct tm  tstruct;
//		auto err = localtime_s(&tstruct, &end_time);
//		char buffer[128];
//		memset(buffer, 0, sizeof(buffer));
//
//		strftime(buffer, sizeof(buffer), format , &tstruct);
//
//		return std::string(buffer);
//	}
//
//	void reset()
//	{
//		m_tstart = std::chrono::high_resolution_clock::now();
//	}
//	
//	double seconds_elapsed()
//	{
//		TimePointer tend = std::chrono::high_resolution_clock::now();
//		std::chrono::duration<double> elapsed = tend - m_tstart;
//		m_tstart = tend;
//		return elapsed.count();
//	}
//
//	double mili_elapsed()
//	{
//		auto elp = seconds_elapsed();
//		return elp * 1000.0;
//	}
//};
//
//class CStopwatch
//{
//	typedef std::chrono::steady_clock::time_point	time_pointer;
//	typedef std::chrono::duration<double>			tduration;
//private:
//	time_pointer	m_start;
//	time_pointer	m_tpre;
//
//	double		m_tstop;
//
//	double		m_dur;
//	bool		m_bstop;
//
//public:
//	CStopwatch() : m_dur(0.0),
//		m_tstop(0.0), m_bstop(true)
//	{
//
//	}
//
//public:
//	void start()
//	{
//		m_bstop  = false;
//		m_start  = std::chrono::high_resolution_clock::now();
//		m_tpre   = m_start;
//		m_dur    = 0.0;
//		m_tstop  = 0.0;
//	}
//
//	void stop()
//	{
//		if (m_bstop)
//			return;
//
//		auto tps = std::chrono::high_resolution_clock::now();
//		tduration elapsed = tps - m_tpre;
//		m_tstop = elapsed.count();
//		m_bstop = true;
//	}
//	void resume()
//	{
//		if (!m_bstop)
//			return;
//		m_tpre = std::chrono::high_resolution_clock::now();
//		m_bstop = false;
//	}
//
//	double lap()
//	{
//		if (m_bstop)
//			return 0.0;
//
//		auto tps = std::chrono::high_resolution_clock::now();
//		tduration elapsed = tps - m_tpre;
//		m_dur = elapsed.count() + m_tstop;
//		m_tstop = 0.0;
//		m_tpre = tps;
//
//		return m_dur;
//	}
//
//	bool is_stop()
//	{
//		return m_bstop;
//	}
//
//	double all_time()
//	{
//		tduration elapsed = std::chrono::high_resolution_clock::now() - m_start;
//		return elapsed.count();
//	}
//
//	double seconds_elapsed()
//	{
//		return m_dur;
//	}
//
//	double mili_elapsed()
//	{
//		return m_dur*1000.0;
//	}
//};
//
//class CFPSCounter
//{
//	typedef std::chrono::steady_clock::time_point	  time_pointer;
//	typedef std::chrono::duration<double>			  tduration;
//
//private:
//	int				m_fps;
//	int				m_frames;
//	double			m_elapsed;
//
//	time_pointer	m_last_frame;
//
//	double			m_reset;
//
//private:
//	void reset()
//	{
//		m_frames = 0;
//		m_reset  = 0.0;
//	}
//public:
//	CFPSCounter() : m_fps(0), m_elapsed(0.0),
//		m_reset(0.0), m_frames(0)
//	{
//
//	}
//
//	void start()
//	{
//		m_last_frame = std::chrono::high_resolution_clock::now();
//		this->reset();
//	}
//
//	void update()
//	{
//		time_pointer cur_frame = std::chrono::high_resolution_clock::now();
//		m_elapsed = tduration(cur_frame - m_last_frame).count();
//		m_reset  += m_elapsed;
//
//		m_frames++;
//
//		if (m_reset >= 1.0)
//		{
//			m_fps = (int)(m_frames / m_reset);
//			this->reset();
//		}
//
//		m_last_frame = cur_frame;
//	}
//
//public:
//	int fps()
//	{
//		return this->m_fps;
//	}
//	double frametime() //miliseconds
//	{
//		return this->m_elapsed;
//	}
//};

____END_NAMESPACE____

#endif // !XSYSTYPE_H