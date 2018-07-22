#pragma once
#include <string>
#include <iostream>
#include <queue>

#include "opencv2/core.hpp"
#include "opencv2/core/directx.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"

#include "WinApp.h"

#define SAFE_RELEASE(p) if (p) { p->Release(); p = NULL; }



class D3DBase : public WinApp
{
public:
	enum MODE
	{
		MODE_CPU,
		MODE_GPU_RGBA,
		MODE_GPU_NV12
	};

	D3DBase(int width, int height, std::string& window_name) :
		WinApp(width, height, window_name)
	{
		m_shutdown = false;
		m_mode = MODE_CPU;
		m_modeStr[0] = cv::String("Processing on CPU");
		m_modeStr[1] = cv::String("Processing on GPU RGBA");
		m_modeStr[2] = cv::String("Processing on GPU NV12");
		m_demo_processing = false;
	}

	~D3DBase() {}

	virtual int create() { return WinApp::create(); }
	virtual int render() = 0;
	virtual int cleanup()
	{
		m_shutdown = true;
		return WinApp::cleanup();
	}

protected:
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CHAR:
			if (wParam == '1')
			{
				m_mode = MODE_CPU;
				return 0;
			}
			if (wParam == '2')
			{
				m_mode = MODE_GPU_RGBA;
				return 0;
			}
			if (wParam == '3')
			{
				m_mode = MODE_GPU_NV12;
				return 0;
			}
			else if (wParam == VK_SPACE)
			{
				m_demo_processing = !m_demo_processing;
				return 0;
			}
			else if (wParam == VK_ESCAPE)
			{
				return cleanup();
			}
			break;

		case WM_CLOSE:
			return cleanup();

		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		}

		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}

	// do render at idle
	virtual int idle() { return render(); }

protected:
	bool               m_shutdown;
	bool               m_demo_processing;
	MODE               m_mode;
	cv::String         m_modeStr[3];
	cv::Mat            m_frame_bgr;
	cv::Mat            m_frame_rgba;
};


static void help()
{
	printf(
		"\nSample demonstrating interoperability of DirectX and OpenCL with OpenCV.\n"
		"Hot keys: \n"
		"  SPACE - turn processing on/off\n"
		"    1   - process DX surface through OpenCV on CPU\n"
		"    2   - process DX RGBA surface through OpenCV on GPU (via OpenCL)\n"
		"    3   - process DX NV12 surface through OpenCV on GPU (via OpenCL)\n"
		"  ESC   - exit\n\n");
}


static const char* keys =
{
	"{c camera | true  | use camera or not}"
	"{f file   |       | movie file name  }"
	"{h help   |       | print help info  }"
};
