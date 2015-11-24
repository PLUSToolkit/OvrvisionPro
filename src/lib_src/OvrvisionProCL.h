// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OVRVISIONPRODLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OVRVISIONPRODLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef _OVRVISION_EXPORTS
#define OVRVISIONPRODLL_API __declspec(dllexport)
#else
#define OVRVISIONPRODLL_API __declspec(dllimport)
#endif


#include <opencv2/core/core.hpp>

// OpenCL header
#include <CL/opencl.h>// OpenCL and its extensions

#ifdef _WIN32
#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <GL/gl.h> 
#include <CL/cl_d3d11_ext.h>	// for OpenCL and Direct3D11 interoperability (NV and KHR are equivalent)
#endif

//ovrvision setting
#include "ovrvision_setting.h"

using namespace cv;

namespace OVR
{
	// OpenCL Sharing mode 
	enum SHARING_MODE {
		NONE = 0,
		OPENGL,
		D3D11,
		//D3D10
	};

	// Scaling
	enum SCALING {
		HALF,	// 1/2
		FOURTH,	// 1/4
		EIGHTH	// 1/8
	};

	// Extension vendor 
	enum VENDOR {
		KHRONOS,	// Khronos準拠の機能拡張
		INTEL,		// Intelの機能拡張
		AMD,		// AMDの機能拡張
		NVIDIA		// NVIDIAの機能拡張
	};

	// OpenCLの機能拡張情報を返すコールバック関数
	typedef int(*EXTENSION_CALLBACK)(void *pItem, const char *extensions);

	// OpenCL version
	class OVRVISIONPRODLL_API OvrvisionProOpenCL {
		public:
			OvrvisionProOpenCL(int width, int height, enum SHARING_MODE mode = NONE);
			~OvrvisionProOpenCL();

			// Load camera parameters
			bool LoadCameraParams(const char *filename);
			bool LoadCameraParams(OvrvisionSetting* ovrset);

			// Demosaicing
			void Demosaic(const ushort* src, cl_event *execute = NULL);	// OpenGL/D3D連携で、CPUへの転送を行わない
			void Demosaic(const ushort* src, cl_mem left, cl_mem right, cl_event *execute = NULL);
			void Demosaic(const ushort* src, uchar *left, uchar *right);
			void Demosaic(const ushort* src, Mat &left, Mat &right);
			void Demosaic(const Mat src, Mat &left, Mat &right);

			// Demosaic and Remap
			void DemosaicRemap(const ushort* src, cl_event *execute = NULL);	// OpenGL/D3D連携で、CPUへの転送を行わない
			void DemosaicRemap(const ushort* src, cl_mem left, cl_mem right, cl_event *execute = NULL);
			void DemosaicRemap(const ushort* src, uchar *left, uchar *right);
			void DemosaicRemap(const ushort* src, Mat &left, Mat &right);
			void DemosaicRemap(const Mat src, Mat &left, Mat &right);

			cl_device_id SelectGPU(const char *platform, const char *version);
			
			// TODO: OpenGL連携用のテクスチャーを生成
			cl_mem CreateGLTexture2D(GLuint texture, int width, int height, GLenum pixelFormat = GL_RGBA, GLenum dataType = GL_UNSIGNED_BYTE);
#ifdef _WIN32
			// TODO: Direct3D連携用のテクスチャーを生成
			cl_mem CreateD3DTexture2D(int width, int height, ID3D11Texture2D *texture);
#endif
			// TODO: 縮小したグレースケール画像を取得
			void Grayscale(uchar *left, uchar *right, enum SCALING scale);	

			// Enumerate OpenCL extensions
			int EnumExtensions(EXTENSION_CALLBACK callback = NULL, void *item = NULL);

			void createProgram(const char *filename, bool binary = false);
			int saveBinary(const char *filename);
			//bool SaveSettings(const char *filename);
			//void Remap(Cameye eye, const Mat src, Mat &dst);

		private:
			bool CreateProgram();
			bool Prepare4Sharing();		// OpenGL/D3D連携準備

#ifdef _WIN32
			enum VENDOR _vendorD3D11;	// D3D11の機能拡張

			// Extension functions for NVIDIA 
			clGetDeviceIDsFromD3D11NV_fn        clGetDeviceIDsFromD3D11NV = NULL;
			clCreateFromD3D11BufferNV_fn		clCreateFromD3D11BufferNV = NULL;
			clCreateFromD3D11Texture2DNV_fn		clCreateFromD3D11Texture2DNV = NULL;
			clCreateFromD3D11Texture3DNV_fn     clCreateFromD3D11Texture3DNV = NULL;
			clEnqueueAcquireD3D11ObjectsNV_fn	clEnqueueAcquireD3D11ObjectsNV = NULL;
			clEnqueueReleaseD3D11ObjectsNV_fn	clEnqueueReleaseD3D11ObjectsNV = NULL;

			// Extension functions for Khronos
			clGetDeviceIDsFromD3D11KHR_fn       clGetDeviceIDsFromD3D11KHR = NULL;
			clCreateFromD3D11BufferKHR_fn		clCreateFromD3D11BufferKHR = NULL;
			clCreateFromD3D11Texture2DKHR_fn	clCreateFromD3D11Texture2DKHR = NULL;
			clCreateFromD3D11Texture3DKHR_fn    clCreateFromD3D11Texture3DKHR = NULL;
			clEnqueueAcquireD3D11ObjectsKHR_fn	clEnqueueAcquireD3D11ObjectsKHR = NULL;
			clEnqueueReleaseD3D11ObjectsKHR_fn	clEnqueueReleaseD3D11ObjectsKHR = NULL;
#endif
			char *_deviceExtensions;
			int _width, _height;
			Mat *mapX[2], *mapY[2]; // camera parameter
			enum SHARING_MODE _sharing;	// Sharing with OpenGL or Direct3D11 

		protected:
			// OpenCL variables
			cl_platform_id	_platformId;
			cl_device_id	_deviceId;
			cl_context		_context;
			cl_program		_program;
			cl_kernel		_demosaic;
			cl_kernel		_remap;
			cl_kernel		_grayscale;
			cl_kernel		_skincolor;
			cl_command_queue _commandQueue;
			cl_image_format	_format16UC1;
			cl_image_format	_format8UC4;
			cl_image_format _format8UC1;
			cl_image_format _formatMap;
			cl_int			_errorCode;

		private:
			cl_event _execute;
			cl_mem	_src;
			cl_mem	_l, _r, _L, _R;
			cl_mem	_grayL, _grayR;
			cl_mem	_mx[2], _my[2]; // map for remap in GPU
			bool	_remapAvailable;
		};

	/*
	typedef int(*PENUMDEVICE)(void *pItem,
	const char *deviceName,
	const char *opencl_version,
	const char *deviceExtension,
	const int majorVersion,
	const int minorVersion);

	OVRVISIONPRODLL_API int EnumerateGPU(PENUMDEVICE callback = NULL, void *pItem = NULL);
	*/
}
