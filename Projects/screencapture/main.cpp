#define _CRT_SECURE_NO_WARNINGS
//http://resources.infosecinstitute.com/using-setwindowshookex-for-dll-injection-on-windows/
#undef UNICODE
#include <windows.h>
#include <hftimer.h>
#include <iostream>
#include <fstream>

// http://stackoverflow.com/questions/15914012/encoding-a-screenshot-into-a-video-using-ffmpeg
// http://ffmpeg.zeranoe.com/builds/
// http://gopalakrishna.palem.in/createmovie.html

#include <libavcodec\avcodec.h>
#include <libavutil\imgutils.h>
#include <libswscale\swscale.h>

int STREAM_FRAME_RATE=10;
int STREAM_DURATION=10;
void screenCap() {}
void* pPixels;
int screenHeight=480;
int screenWidth=640;

void encode()
{
	AVCodec* codec;
	AVCodecContext* c = NULL;
	uint8_t* outbuf;
	int i, out_size, outbuf_size;

	avcodec_register_all();                                 // mandatory to register ffmpeg functions

	printf("Video encoding\n");

	codec = avcodec_find_encoder(CODEC_ID_H264);            // finding the H264 encoder
	if (!codec)
	{
		fprintf(stderr, "Codec not found\n");
		exit(1);
	}
	else printf("H264 codec found\n");

	c = avcodec_alloc_context3(codec);
	c->bit_rate = 400000;
	c->width = 1280;                                        // resolution must be a multiple of two (1280x720),(1900x1080),(720x480)
	c->height = 720;
	c->time_base.num = 1;                                   // framerate numerator
	c->time_base.den = 25;                                  // framerate denominator
	c->gop_size = 10;                                       // emit one intra frame every ten frames
	c->max_b_frames = 1;                                    // maximum number of b-frames between non b-frames
	c->keyint_min = 1;                                      // minimum GOP size
	c->i_quant_factor = (float)0.71;                        // qscale factor between P and I frames
	c->b_frame_strategy = 20;                               ///// find out exactly what this does
	c->qcompress = (float)0.6;                              ///// find out exactly what this does
	c->qmin = 20;                                           // minimum quantizer
	c->qmax = 51;                                           // maximum quantizer
	c->max_qdiff = 4;                                       // maximum quantizer difference between frames
	c->refs = 4;                                            // number of reference frames
	c->trellis = 1;                                         // trellis RD Quantization
	c->pix_fmt = PIX_FMT_YUV420P;                           // universal pixel format for video encoding
	c->codec_id = CODEC_ID_H264;
	c->codec_type = AVMEDIA_TYPE_VIDEO;

	if (avcodec_open2(c, codec,NULL) < 0)
	{
		fprintf(stderr, "Could not open codec\n");          // opening the codec
		exit(1);
	}
	else printf("H264 codec opened\n");

	outbuf_size = 100000 + c->width*c->height*(32>>3);      // allocate output buffer
	outbuf = static_cast<uint8_t *>(malloc(outbuf_size));
	printf("Setting buffer size to: %d\n",outbuf_size);

	FILE* f = fopen("example.mpg","wb");                    // opening video file for writing
	if(!f) printf("x  -  Cannot open video file for writing\n");
	else printf("Opened video file for writing\n");

	// encode video
	for(i=0;i<STREAM_FRAME_RATE*STREAM_DURATION;i++)
	{
		fflush(stdout);

		screenCap();                                                                                                // taking screenshot

		int nbytes = avpicture_get_size(PIX_FMT_YUV420P, c->width, c->height);                                      // allocating outbuffer
		uint8_t* outbuffer = (uint8_t*)av_malloc(nbytes*sizeof(uint8_t));


//		AVFrame* inpic = avcodec_alloc_frame();                                                                     // mandatory frame allocation
//		AVFrame* outpic = avcodec_alloc_frame();
		AVFrame* inpic = av_frame_alloc();                                                                     // mandatory frame allocation
		AVFrame* outpic = av_frame_alloc();

		outpic->pts = (int64_t)((float)i * (1000.0/((float)(c->time_base.den))) * 90);                              // setting frame pts
		avpicture_fill((AVPicture*)inpic, (uint8_t*)pPixels, PIX_FMT_RGB32, c->width, c->height);                   // fill image with input screenshot
		avpicture_fill((AVPicture*)outpic, outbuffer, PIX_FMT_YUV420P, c->width, c->height);                        // clear output picture for buffer copy
		av_image_alloc(outpic->data, outpic->linesize, c->width, c->height, c->pix_fmt, 1); 

		inpic->data[0] += inpic->linesize[0]*(screenHeight-1);                                                      // flipping frame
		inpic->linesize[0] = -inpic->linesize[0];                                                                   // flipping frame

		struct SwsContext* fooContext = sws_getContext(screenWidth, screenHeight, PIX_FMT_RGB32, c->width, c->height, PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
		sws_scale(fooContext, inpic->data, inpic->linesize, 0, c->height, outpic->data, outpic->linesize);          // converting frame size and format

		out_size = avcodec_encode_video(c, outbuf, outbuf_size, outpic);                                            // encoding video
//		out_size = avcodec_encode_video2(c, outbuf, outbuf_size, outpic);                                            // encoding video

		printf("Encoding frame %3d (size=%5d)\n", i, out_size);
		fwrite(outbuf, 1, out_size, f);

		delete[] pPixels;                                                                                          // freeing memory
		av_free(outbuffer);     
		av_free(inpic);
		av_free(outpic);
	}

	for(; out_size; i++)
	{
		fflush(stdout);

		out_size = avcodec_encode_video(c, outbuf, outbuf_size, NULL);                                              // encode the delayed frames
		printf("Writing frame %3d (size=%5d)\n", i, out_size);
		fwrite(outbuf, 1, out_size, f);
	}

	outbuf[0] = 0x00;
	outbuf[1] = 0x00;                                                                                               // add sequence end code to have a real mpeg file
	outbuf[2] = 0x01;
	outbuf[3] = 0xb7;
	fwrite(outbuf, 1, 4, f);
	fclose(f);

	avcodec_close(c);                                                                                               // freeing memory
	free(outbuf);
	av_free(c);
	printf("Closed codec and Freed\n");
}


int CaptureBMP(char* szFile, int x0, int y0, int nWidth, int nHeight)
{
	// Source[1]
	HDC hdcScr, hdcMem;
	HBITMAP hbmScr;
	BITMAP bmp;
	int iXRes, iYRes;

	// Create a normal DC and a memory DC for the entire screen. The normal DC provides a "snapshot" of the screen contents.
	// The memory DC keeps a copy of this "snapshot" in the associated  bitmap.
	hdcScr = CreateDC("DISPLAY", NULL, NULL, NULL);
	hdcMem = CreateCompatibleDC(hdcScr);

	iXRes = GetDeviceCaps(hdcScr, HORZRES);
	iYRes = GetDeviceCaps(hdcScr, VERTRES);
	if (x0<0 || y0<0 || x0+nWidth>iXRes || y0+nHeight>iYRes || nHeight<0 || nWidth<0) return 0; // Invalid capture region

	// Create a compatible bitmap for hdcScreen.
	hbmScr = CreateCompatibleBitmap(hdcScr, nWidth, nHeight);
	if (hbmScr == 0) return 0;

	// Select the bitmaps into the compatible DC.
	if (!SelectObject(hdcMem, hbmScr)) return 0;

	// Copy color data for the entire display into a bitmap that is selected into a compatible DC.
	if (!StretchBlt(hdcMem, 0, 0, nWidth, nHeight, hdcScr,x0, y0, nWidth, nHeight, SRCCOPY)) return 0;

	// Source[2]
	PBITMAPINFO pbmi;
	WORD cClrBits;

	// Retrieve the bitmap's color format, width, and height.
	if (!GetObject(hbmScr, sizeof(BITMAP), (LPSTR) &bmp)) return 0;

	// Convert the color format to a count of bits.
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;

	// Allocate memory for the BITMAPINFO structure. (This structure contains a BITMAPINFOHEADER structure and an array of RGBQUAD data structures.)
	if (cClrBits != 24)
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << cClrBits));

	// There is no RGBQUAD array for the 24-bit-per-pixel format.
	else
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

	// Initialize the fields in the BITMAPINFO structure.
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

	// If the bitmap is not compressed, set the BI_RGB flag.
	pbmi->bmiHeader.biCompression = BI_RGB;

	// Compute the number of bytes in the array of color indices and store the result in biSizeImage.
	pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) / 8 * pbmi->bmiHeader.biHeight * cClrBits;

	// Set biClrImportant to 0, indicating that all of the device colors are important.
	pbmi->bmiHeader.biClrImportant = 0;

	HANDLE hf;                  // file handle
	BITMAPFILEHEADER hdr;       // bitmap file-header
	PBITMAPINFOHEADER pbih;     // bitmap info-header
	LPBYTE lpBits;              // memory pointer
	DWORD dwTotal;              // total count of bytes
	DWORD cb;                   // incremental count of bytes
	BYTE *hp;                   // byte pointer
	DWORD dwTmp;

	pbih = (PBITMAPINFOHEADER) pbmi;
	lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits) return 0;

	// Retrieve the color table (RGBQUAD array) and the bits
	// (array of palette indices) from the DIB.
	if (!GetDIBits(hdcMem, hbmScr, 0, (WORD) pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS)) return 0;

	// Create the .BMP file.
	hf = CreateFile(szFile, GENERIC_READ | GENERIC_WRITE, (DWORD) 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
	if (hf == INVALID_HANDLE_VALUE) return 0;

	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"

	// Compute the size of the entire file.
	hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the offset to the array of color indices.
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof (RGBQUAD);

	// Copy the BITMAPFILEHEADER into the .BMP file.
	if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp, NULL)) return 0;

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
	if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD), (LPDWORD) &dwTmp, NULL))
		return 0;

	// Copy the array of color indices into the .BMP file.
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp, NULL)) return 0;

	// Close the .BMP file.
	if (!CloseHandle(hf)) return 0;

	// Free memory.
	GlobalFree((HGLOBAL)lpBits);
	ReleaseDC(0, hdcScr);
	ReleaseDC(0, hdcMem);

	return 1;
}// int CaptureBMP(char* szFile, int x0, int y0, int nWidth, int nHeight)

HANDLE timerEvent, terminateEvent, mutex, threadTerminatedEvent;
bool runCapture;

VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	SetEvent(timerEvent);
}


// http://msdn.microsoft.com/en-us/library/windows/desktop/ms682516(v=vs.85).aspx
DWORD WINAPI MyThreadFunction(LPVOID lpParam) 
{ 
	DWORD status = WAIT_OBJECT_0;
	HANDLE lpHandles[2];
	lpHandles[0] = timerEvent;
	lpHandles[1] = terminateEvent;
	bool running = true;
	while (running)
	{
		status = WaitForMultipleObjects(2, lpHandles, false, INFINITE);
		switch (status) 
		{
			case WAIT_OBJECT_0: // timerEvent
				WaitForSingleObject(mutex,INFINITE);
				if (runCapture)
				{
					//running = true;
					CaptureBMP("baba.bmp", 220, 120, 250, 400);
				}
				ReleaseMutex(mutex);
				ResetEvent(timerEvent);
				break;
			case WAIT_OBJECT_0+1: // terminateEvent
				running = false;
				break;
			default:
				running = false; // this must be some error
		} // switch (status) 
	} // while (running)
	SetEvent(threadTerminatedEvent);
	return 0;
} // DWORD WINAPI MyThreadFunction(LPVOID lpParam) 

//The function that implements the key logging functionality
LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam)
{
	char pressedKey;
	bool altdown = 0;
	// Declare a pointer to the KBDLLHOOKSTRUCTdsad
	KBDLLHOOKSTRUCT *pKeyBoard = (KBDLLHOOKSTRUCT *)lParam;
	pressedKey = 0;
	switch( wParam )
	{
	//case WM_SYSKEYDOWN:
	//	if (pKeyBoard->vkCode==164) // alt
	//		altdown = true;
	//	break;
	//case WM_SYSKEYUP:
	//	altdown = false;
	//	break;
	//case WM_KEYDOWN: // When the key has been pressed and released
	//		if (pKeyBoard->vkCode==20) altdown = true;
	//		//get the key code
	//		pressedKey = (char)pKeyBoard->vkCode;
	//	break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
			pressedKey = (char)pKeyBoard->vkCode;
		break;
		default:
			return CallNextHookEx( NULL, nCode, wParam, lParam );
		break;
	}

	//do something with the pressed key here
	altdown = ((DWORD)pKeyBoard->flags & LLKHF_ALTDOWN);
	if (altdown && pressedKey==81) // ALT+Q
	{
		Beep(440, 500); 
		runCapture = ! runCapture;
	}

	//according to winapi all functions which implement a hook must return by calling next hook
	return CallNextHookEx( NULL, nCode, wParam, lParam);
} // LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam)


LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)	// handle the messages
	{
		case WM_DESTROY:
			PostQuitMessage (0); // send a WM_QUIT to the message queue
			break;
		default: // for messages that we don't deal with
			return DefWindowProc (hwnd, message, wParam, lParam);
	}
	return 0;
}

char szClassName[ ] = "WindowsApp";

DWORD WINAPI createWindowThread(LPVOID lpParam) 
{
	HWND hwnd;  // This is the handle for our window
	MSG messages;  // Here messages to the application are saved
	WNDCLASSEX wincl;  // Data structure for the windowclass
	HINSTANCE instance = *(HINSTANCE*)lpParam;

	// The Window structure
	wincl.hInstance = instance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;      // This function is called by windows 
	wincl.style = CS_DBLCLKS;                 // Catch double-clicks
	wincl.cbSize = sizeof (WNDCLASSEX);

	// Use default icon and mouse-pointer
	wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;                 // No menu 
	wincl.cbClsExtra = 0;                      // No extra bytes after the window class
	wincl.cbWndExtra = 0;                      // structure or the window instance
	// Use Windows's default color as the background of the window
	wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

	// Register the window class, and if it fails quit the program
	if (!RegisterClassEx (&wincl))
		return 0;

	// The class is registered, let's create the program
	hwnd = CreateWindowEx (
			0,                   // Extended possibilites for variation
			szClassName,         // Classname
			"Windows App",       // Title Text
			WS_OVERLAPPEDWINDOW, // default window
			CW_USEDEFAULT,       // Windows decides the position
			CW_USEDEFAULT,       // where the window ends up on the screen
			544,                 // The programs width
			375,                 // and height in pixels
			HWND_DESKTOP,        // The window is a child-window to desktop
			NULL,                // No menu
			instance,       // Program Instance handler
			NULL                 // No Window Creation data
			);

	// Make the window visible on the screen
	ShowWindow (hwnd, SW_SHOW);
//	ShowWindow (hwnd, SW_HIDE);

	// Run the message loop. It will run until GetMessage() returns 0 
	while (GetMessage (&messages, NULL, 0, 0))
	{
		//Translate virtual-key messages into character messages
		TranslateMessage(&messages);
		// Send message to WindowProcedure
		DispatchMessage(&messages);
	}

	// The program return-value is 0 - The value that PostQuitMessage() gave
	return messages.wParam;
} // DWORD WINAPI createWindowThread()



int main()
{

	encode();

	// read also http://msdn.microsoft.com/en-us/library/dd183402(v=VS.85).aspx
	//Retrieve the applications instance
	HINSTANCE instance = GetModuleHandle(NULL);
	//HANDLE wThread = CreateThread( 
	//	NULL,            // default security attributes
	//	0,               // use default stack size  
	//	createWindowThread,// thread function name
	//	&instance,          // argument to thread function 
	//	0,               // run the thred immediately after creation
	//	NULL);			 // we do not wan the thread identifier 

	 //Set a global Windows Hook to capture keystrokes using the function declared above
	 HHOOK test1 = NULL;
	test1 = SetWindowsHookEx( WH_KEYBOARD_LL, LowLevelKeyboardProc, instance,0);
	
	 if (test1)
	{
		std::cout<<"Keyboard hook sucessfully installed. "<< std::endl;
	} else
	{
		std::cout<<"Error installing hook: "<< GetLastError()<<std::endl;
	}

	timerEvent = CreateEvent(NULL, TRUE, FALSE, "timerEvent");
	if (NULL == timerEvent)
	{
		std::cout<<"CreateEvent failed : "<< GetLastError()<<std::endl;
		return 1;
	}

	terminateEvent = CreateEvent(NULL, TRUE, FALSE, "terminate");
	if (NULL == terminateEvent)
	{
		std::cout<<"CreateEvent failed : "<< GetLastError()<<std::endl;
		return 1;
	}

	threadTerminatedEvent = CreateEvent(NULL, TRUE, FALSE, "terminate");
	if (NULL == terminateEvent)
	{
		std::cout<<"CreateEvent failed : "<< GetLastError()<<std::endl;
		return 1;
	}

	mutex = CreateMutex(NULL,false,"ScreenCaptureCriticalSection");
	if (NULL == mutex)
	{
		std::cout<<"CreateMutex failed : "<< GetLastError()<<std::endl;
		return 1;
	}

	int framerate = 10;
	DWORD timebetweenframes = 1000/framerate; // in msec
	runCapture = false;

	HANDLE hTimer = NULL;
	HANDLE hTimerQueue = NULL;

	// Create the timer queue.
	hTimerQueue = CreateTimerQueue();
	if (NULL == hTimerQueue)
	{
		printf("CreateTimerQueue failed (%d)\n", GetLastError());
		return 1;
	}

	// Set a timer to call the timer routine in 10 seconds.
//	if (!CreateTimerQueueTimer( &hTimer, hTimerQueue, (WAITORTIMERCALLBACK)TimerRoutine, NULL, 0, timebetweenframes, WT_EXECUTEINTIMERTHREAD))
	if (!CreateTimerQueueTimer( &hTimer, hTimerQueue, (WAITORTIMERCALLBACK)TimerRoutine, NULL, 0, timebetweenframes, 0))
	{
		printf("CreateTimerQueueTimer failed (%d)\n", GetLastError());
		return 1;
	}



	HANDLE hThread = CreateThread( 
		NULL,            // default security attributes
		0,               // use default stack size  
		MyThreadFunction,// thread function name
		NULL,          // argument to thread function 
		0,               // run the thred immediately after creation
		NULL);			 // we do not want the thread identifier 

	// Close all thread handles and free memory allocations.

	createWindowThread(instance);

	SetEvent(terminateEvent);
	WaitForSingleObject(threadTerminatedEvent,INFINITE);
	CloseHandle(hThread);

	CloseHandle(timerEvent);
	CloseHandle(terminateEvent);
	CloseHandle(threadTerminatedEvent);

	DeleteTimerQueueTimer(hTimerQueue, hTimer, NULL);
	DeleteTimerQueue(hTimerQueue);

	if (UnhookWindowsHookEx(test1) !=0)
	{
		std::cout<<"Hook sucessfully removed. "<< std::endl;
	} else
	{
		std::cout<<"Error removing hook: "<< GetLastError()<<std::endl;
	}
	std::cout<<"All done."<<std::endl;;
} // int main()
