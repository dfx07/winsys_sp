#pragma once
////////////////////////////////////////////////////////////////////////////////////
// File: GLWinFont.cpp   -- Create and render font use system font window           
// Copyright (C) 2020-2022 Thuong.NV   Created : 10/08/2022                         
// For conditions of distribution and use, see copyright notice in readme.txt       
////////////////////////////////////////////////////////////////////////////////////



/* =================================================================================
Display 2D text use system font . not support zoom
Feature:
+ Support bitmap vietnamese
+ Support special characters
Bitmap fonts offer a simple way to display 2D text on the screen.
Information about the characters in a bitmap font is stored as bitmap images.

[*] Advantage to bitmap fonts is that they provide a high performance method for render
image text to the screen
[*] If you not use Unicode , please reduce RANG_BASE_LIST = asscii
====================================================================================*/
#include "GL/glew.h"
#include <Windows.h>
#include <iostream>
#include <map>

#pragma comment (lib,"Glu32.lib")

#ifdef _UNICODE
#define RANGE_DATA 9000
#else
#define RANGE_DATA 128
#endif // UNICODE



class GLWinFontRender
{
	enum { RANG_BASE_LIST = RANGE_DATA };

public:
	enum FontType
	{
		Normal,
		Bold,
		Thin
	};

private:
	// Render information
	GLuint  m_textbase;
	GLuint  m_renlist;
	HDC     m_hdc;
	HFONT   m_hfont;

	// View information
	int     m_width;
	int     m_height;

	bool    m_bInitOK;

	std::map<short, unsigned int> m_character_map_list;

public:
	//==================================================================================
	// Get text bound size (width , height)                                             
	//==================================================================================
	void GetSizeText(const char* text, int& width, int& height)
	{
		SelectObject(m_hdc, m_hfont);
		SIZE _size;
		BOOL rel = GetTextExtentPointA(m_hdc, text, (int)strlen(text), &_size);

		if (rel) // it OK
		{
			width  = (int)_size.cx;
			height = (int)_size.cy;
		}
		else
		{
			width = height = 0;
		}
	}

	void GetSizeText(const wchar_t* text, int& width, int& height)
	{
		SelectObject(m_hdc, m_hfont);
		SIZE _size;
		BOOL rel = GetTextExtentPointW(m_hdc, text, (int)wcslen(text), &_size);

		if (rel) // it OK
		{
			width = (int)_size.cx;
			height = (int)_size.cy;
		}
		else
		{
			width = height = 0;
		}
	}

private:
	//==================================================================================
	// Load font system data                                                            
	//==================================================================================
	bool CreateBitmapFont(const char* fontname, unsigned int fontsize, DWORD fonttype)
	{
		if (m_hdc == NULL) return false;

		// Create list all charactor vietnamese
		m_textbase = glGenLists(RANG_BASE_LIST);

		if (m_textbase == 0) return false;

		m_hfont = CreateFontA(fontsize, 0, 0, 0, fonttype,
			FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
			FF_DONTCARE | DEFAULT_PITCH, fontname);

		// Verify font creation
		if (!m_hfont)
			return false;

		// Select a device context for the font
		SelectObject(m_hdc, m_hfont);

		wglUseFontBitmapsW(m_hdc, 32, RANG_BASE_LIST, m_textbase);

		return true;
	}

	//==================================================================================
	// Load character by character -> store to map character data                       
	// Defect    : Slow program                                                         
	//==================================================================================
	unsigned int GetWchar(const wchar_t& ch)
	{
		if (m_character_map_list.find(ch) != m_character_map_list.end())
		{
			return m_character_map_list[ch];
		}

		unsigned int id_list_char = glGenLists(1);
		SelectObject(m_hdc, m_hfont);

		BOOL rel = wglUseFontBitmaps(m_hdc, (DWORD)ch, 1, id_list_char);
		rel = (!rel) ? wglUseFontBitmapsW(m_hdc, (DWORD)ch, 1, id_list_char) : rel;

		if (!rel)
		{
			glDeleteLists(id_list_char, 1);
			return 0;
		}

		// Add list id to map character
		m_character_map_list[ch] = id_list_char;

		return id_list_char;
	}

public:
	GLWinFontRender()
	{
		m_textbase = 0;
		m_hdc = NULL;
		m_width = 0;
		m_height = 0;
		m_renlist = 0;
	}

	~GLWinFontRender()
	{
		DeleteObject(m_hfont);

		glDeleteLists(m_textbase, RANG_BASE_LIST);
		glDeleteLists(m_renlist, 1);
		for (int i = 0; i < m_character_map_list.size(); i++)
		{
			glDeleteLists(m_character_map_list[i], 1);
		}
	}

	void Init(HDC hdc, int width, int height)
	{
		m_hdc = hdc;
		m_width = width;
		m_height = height;
	}

	//==================================================================================
	// Update screen view information                                                   
	//==================================================================================
	void UpdateView(int width, int height)
	{
		m_width = width;
		m_height = height;
	}

	//==================================================================================
	// Create font                                                                      
	//==================================================================================
	void LoadFont(const char* fontname, unsigned int fontsize, FontType fonttype = Normal)
	{
		// Create font system
		DWORD sfonttype = FW_REGULAR;
		switch (fonttype)
		{
		case FontType::Bold:
			sfonttype = FW_BOLD;
			break;
		case FontType::Thin:
			sfonttype = FW_THIN;
			break;
		default:
			break;
		}

		if (!CreateBitmapFont(fontname, fontsize, sfonttype))
		{
			std::cout << "Create bitmap font failed" << std::endl;
			m_bInitOK = false;
		}

		m_bInitOK = true;
	}

	//==================================================================================
	// Use before render text to device context                                         
	//==================================================================================
	void Use()
	{
		if (!m_bInitOK) return;

		// Only initialized once
		if (!m_renlist)
		{
			m_renlist = glGenLists(1);

			glNewList(m_renlist, GL_COMPILE);
			{
				glListBase(m_textbase - 32);

				// Push information matrix
				glPushAttrib(GL_LIST_BIT);

				// Load model view matrix
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();

				// Load projection matrix + can use glm;
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDisable(GL_DEPTH_TEST);
			}
			glEndList();
		}

		glCallList(m_renlist);
		gluOrtho2D(0.0, m_width, m_height, 0.0);
	}

	//==================================================================================
	// Use after render text to device context                                          
	//==================================================================================
	void DontUse()
	{
		if (!m_bInitOK) return;

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		// Restore matrices
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		glListBase(0);
		// Pop information matrix
		glPopAttrib();
	}

	//==================================================================================
	// Render text Assci to device context                                              
	//==================================================================================
	void Write(int x, int y, const char* text, float r = 1.0f, float g = 1.0f, // Text out  [input]
		float b = 1.0f, float a = 1.0f)
	{
		if (!m_bInitOK || m_textbase == 0 || !text)
			return;

		glColor4f(r, g, b, a);
		glRasterPos2i(x, y);
		glCallLists((int)strlen(text), GL_UNSIGNED_BYTE, text);
	}

	//==================================================================================
	// Render text Unicode to device context - Each element                             
	// [*] it slooww -> Use for special characters                                      
	//==================================================================================
	void WriteEach(int x, int y, const wchar_t* text, float r = 1.0f, float g = 1.0f, // Text out  [input]
		float b = 1.0f, float a = 1.0f)
	{
		if (!m_bInitOK || m_textbase == 0 || !text)
			return;

		glColor4f(r, g, b, a);
		glRasterPos2i(x, y);

		GLint id = 0;
		for (int i = 0; i < wcslen(text); i++)
		{
			if (short(text[i]) > RANG_BASE_LIST)
			{
				id = GetWchar(text[i]);
			}
			else
			{
				// 31 because 32 list not init + 1 list complie
				id = text[i] - 31;
			}

			if (id <= 0) continue;
			glCallList((GLuint)id);
		}
	}

	//==================================================================================
	// Render text Unicode to device context  - All                                     
	// In case if you can't display the UNICODE character please Use: WriteEach         
	//==================================================================================
	void Write(int x, int y, const wchar_t* text, float r = 1.0f, float g = 1.0f, // Text out  [input]
		float b = 1.0f, float a = 1.0f)
	{
		if (!m_bInitOK || m_textbase == 0 || !text)
			return;

		//for (int i = 0; i < wcslen(text); i++)
		//{
		//    int a = text[i];
		//    int c = 10; // test
		//}

		glColor4f(r, g, b, a);
		glRasterPos2i(x, y);
		glCallLists((int)wcslen(text), GL_UNSIGNED_SHORT, text);
	}
};