#pragma once

#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>

#include "xsysdef.h"

___NAMESPACE_BEGIN___

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


class CTimer
{
	typedef std::chrono::steady_clock::time_point	TimePointer;

private:
	TimePointer m_tstart;

public:
	CTimer()
	{
		reset();
	}
public:
	static TimePointer now()
	{
		return std::chrono::high_resolution_clock::now();
	}

	// default format : "%Y-%m-%d %X"
	// ref : https://www.programiz.com/python-programming/datetime/strftime
	static std::string time_now(const char* format = "%Y-%m-%d %X")
	{
		auto now = std::chrono::system_clock::now();
		std::time_t end_time = std::chrono::system_clock::to_time_t(now);

		struct tm  tstruct;
		auto err = localtime_s(&tstruct, &end_time);
		char buffer[128];
		memset(buffer, 0, sizeof(buffer));

		strftime(buffer, sizeof(buffer), format , &tstruct);

		return std::string(buffer);
	}

	void reset()
	{
		m_tstart = std::chrono::high_resolution_clock::now();
	}
	
	double seconds_elapsed()
	{
		TimePointer tend = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = tend - m_tstart;
		m_tstart = tend;
		return elapsed.count();
	}

	double mili_elapsed()
	{
		auto elp = seconds_elapsed();
		return elp * 1000.0;
	}
};

class CStopwatch
{
	typedef std::chrono::steady_clock::time_point	time_pointer;
	typedef std::chrono::duration<double>			tduration;
private:
	time_pointer	m_start;
	time_pointer	m_tpre;

	double		m_tstop;

	double		m_dur;
	bool		m_bstop;

public:
	CStopwatch() : m_dur(0.0),
		m_tstop(0.0), m_bstop(true)
	{

	}

public:
	void start()
	{
		m_bstop  = false;
		m_start  = std::chrono::high_resolution_clock::now();
		m_tpre   = m_start;
		m_dur    = 0.0;
		m_tstop  = 0.0;
	}

	void stop()
	{
		auto tps = std::chrono::high_resolution_clock::now();
		tduration elapsed = tps - m_tpre;
		m_tstop = elapsed.count();
		m_bstop = true;
	}
	void resume()
	{
		m_tpre = std::chrono::high_resolution_clock::now();
		m_bstop = false;
	}

	double lap()
	{
		auto tps = std::chrono::high_resolution_clock::now();
		tduration elapsed = tps - m_tpre;
		m_dur = elapsed.count() + m_tstop;
		m_tstop = 0.0;
		m_tpre = tps;

		return m_dur;
	}

	bool is_stop()
	{
		return m_bstop;
	}

	double all_time()
	{
		tduration elapsed = std::chrono::high_resolution_clock::now() - m_start;
		return elapsed.count();
	}

	double seconds_elapsed()
	{
		return m_dur;
	}

	double mili_elapsed()
	{
		return m_dur*1000.0;
	}
};

class CFPSCouter
{
	typedef std::chrono::steady_clock::time_point	  time_pointer;
	typedef std::chrono::duration<double>			  tduration;

private:
	int				m_fps;
	int				m_frames;
	double			m_elapsed;

	time_pointer	m_last_frame;

	double			m_reset;

private:
	void reset()
	{
		m_frames = 0;
		m_reset  = 0.0;
	}
public:
	CFPSCouter() : m_fps(0), m_elapsed(0.0),
		m_reset(0.0), m_frames(0)
	{

	}

	void start()
	{
		m_last_frame = std::chrono::high_resolution_clock::now();
		this->reset();
	}

	void update()
	{
		time_pointer cur_frame = std::chrono::high_resolution_clock::now();
		m_elapsed = tduration(cur_frame - m_last_frame).count();
		m_reset  += m_elapsed;

		m_frames++;

		if (m_reset >= 1.0)
		{
			m_fps = (int)(m_frames / m_reset);
			this->reset();
		}

		m_last_frame = cur_frame;
	}

public:
	int fps()
	{
		return this->m_fps;
	}
	double elapsed() //miliseconds
	{
		return this->m_elapsed;
	}
};


___NAMESPACE_END___
// 