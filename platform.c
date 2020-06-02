/********************************************************************************
 * MIT License
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Copyright (c) 2020 Sarayu Nhookeaw
 *
 ********************************************************************************/

#include "platform.h"
#include "header.h"

CM_BEGIN_EXTERN

#if defined(CM_PLATFORM)

cm_inline void 
cm_key_state_update(cmKeyState *s, b32 is_down) {
	b32 was_down = (*s & cmKeyState_Down) != 0;
	is_down = is_down != 0; // NOTE(bill): Make sure it's a boolean
	CM_MASK_SET(*s, is_down,               cmKeyState_Down);
	CM_MASK_SET(*s, !was_down &&  is_down, cmKeyState_Pressed);
	CM_MASK_SET(*s,  was_down && !is_down, cmKeyState_Released);
}

#if defined(CM_SYS_WINDOWS)

#ifndef ERROR_DEVICE_NOT_CONNECTED
#define ERROR_DEVICE_NOT_CONNECTED 1167
#endif

CM_XINPUT_GET_STATE(cmXInputGetState_Stub) {
	cm_unused(dwUserIndex); cm_unused(pState);
	return ERROR_DEVICE_NOT_CONNECTED;
}

CM_XINPUT_SET_STATE(cmXInputSetState_Stub) {
	cm_unused(dwUserIndex); cm_unused(pVibration);
	return ERROR_DEVICE_NOT_CONNECTED;
}

cm_internal cm_inline f32 
cm__process_xinput_stick_value(i16 value, i16 dead_zone_threshold) {
	f32 result = 0;

	if (value < -dead_zone_threshold) {
		result = cast(f32) (value + dead_zone_threshold) / (32768.0f - dead_zone_threshold);
	} else if (value > dead_zone_threshold) {
		result = cast(f32) (value - dead_zone_threshold) / (32767.0f - dead_zone_threshold);
	}

	return result;
}

cm_internal void 
cm__platform_resize_dib_section(cmPlatform *p, i32 width, i32 height) {
	if ((p->renderer_type == cmRenderer_Software) &&
	    !(p->window_width == width && p->window_height == height)) {
		BITMAPINFO bmi = {0};

		if (width == 0 || height == 0) {
			return;
		}

		p->window_width  = width;
		p->window_height = height;

		// TODO(bill): Is this slow to get the desktop mode everytime?
		p->sw_framebuffer.bits_per_pixel = cm_video_mode_get_desktop().bits_per_pixel;
		p->sw_framebuffer.pitch = (p->sw_framebuffer.bits_per_pixel * width / 8);

		bmi.bmiHeader.biSize = cm_size_of(bmi.bmiHeader);
		bmi.bmiHeader.biWidth       = width;
		bmi.bmiHeader.biHeight      = height; // NOTE(bill): -ve is top-down, +ve is bottom-up
		bmi.bmiHeader.biPlanes      = 1;
		bmi.bmiHeader.biBitCount    = cast(u16)p->sw_framebuffer.bits_per_pixel;
		bmi.bmiHeader.biCompression = 0 /*BI_RGB*/;

		p->sw_framebuffer.win32_bmi = bmi;


		if (p->sw_framebuffer.memory) {
			cm_vm_free(cm_virtual_memory(p->sw_framebuffer.memory, p->sw_framebuffer.memory_size));
		}

		{
			isize memory_size = p->sw_framebuffer.pitch * height;
			cmVirtualMemory vm = cm_vm_alloc(0, memory_size);
			p->sw_framebuffer.memory      = vm.data;
			p->sw_framebuffer.memory_size = vm.size;
		}
	}
}


cm_internal cmKeyType 
cm__win32_from_vk(unsigned int key) {
	// NOTE(bill): Letters and numbers are defined the same for VK_* and CM_*
	if (key >= 'A' && key < 'Z') return cast(cmKeyType)key;
	if (key >= '0' && key < '9') return cast(cmKeyType)key;
	switch (key) {
	case VK_ESCAPE: return cmKey_Escape;

	case VK_LCONTROL: return cmKey_Lcontrol;
	case VK_LSHIFT:   return cmKey_Lshift;
	case VK_LMENU:    return cmKey_Lalt;
	case VK_LWIN:     return cmKey_Lsystem;
	case VK_RCONTROL: return cmKey_Rcontrol;
	case VK_RSHIFT:   return cmKey_Rshift;
	case VK_RMENU:    return cmKey_Ralt;
	case VK_RWIN:     return cmKey_Rsystem;
	case VK_MENU:     return cmKey_Menu;

	case VK_OEM_4:      return cmKey_Lbracket;
	case VK_OEM_6:      return cmKey_Rbracket;
	case VK_OEM_1:      return cmKey_Semicolon;
	case VK_OEM_COMMA:  return cmKey_Comma;
	case VK_OEM_PERIOD: return cmKey_Period;
	case VK_OEM_7:      return cmKey_Quote;
	case VK_OEM_2:      return cmKey_Slash;
	case VK_OEM_5:      return cmKey_Backslash;
	case VK_OEM_3:      return cmKey_Grave;
	case VK_OEM_PLUS:   return cmKey_Equals;
	case VK_OEM_MINUS:  return cmKey_Minus;

	case VK_SPACE:  return cmKey_Space;
	case VK_RETURN: return cmKey_Return;
	case VK_BACK:   return cmKey_Backspace;
	case VK_TAB:    return cmKey_Tab;

	case VK_PRIOR:  return cmKey_Pageup;
	case VK_NEXT:   return cmKey_Pagedown;
	case VK_END:    return cmKey_End;
	case VK_HOME:   return cmKey_Home;
	case VK_INSERT: return cmKey_Insert;
	case VK_DELETE: return cmKey_Delete;

	case VK_ADD:      return cmKey_Plus;
	case VK_SUBTRACT: return cmKey_Subtract;
	case VK_MULTIPLY: return cmKey_Multiply;
	case VK_DIVIDE:   return cmKey_Divide;

	case VK_LEFT:  return cmKey_Left;
	case VK_RIGHT: return cmKey_Right;
	case VK_UP:    return cmKey_Up;
	case VK_DOWN:  return cmKey_Down;

	case VK_NUMPAD0:   return cmKey_Numpad0;
	case VK_NUMPAD1:   return cmKey_Numpad1;
	case VK_NUMPAD2:   return cmKey_Numpad2;
	case VK_NUMPAD3:   return cmKey_Numpad3;
	case VK_NUMPAD4:   return cmKey_Numpad4;
	case VK_NUMPAD5:   return cmKey_Numpad5;
	case VK_NUMPAD6:   return cmKey_Numpad6;
	case VK_NUMPAD7:   return cmKey_Numpad7;
	case VK_NUMPAD8:   return cmKey_Numpad8;
	case VK_NUMPAD9:   return cmKey_Numpad9;
	case VK_SEPARATOR: return cmKey_NumpadEnter;
	case VK_DECIMAL:   return cmKey_NumpadDot;

	case VK_F1:  return cmKey_F1;
	case VK_F2:  return cmKey_F2;
	case VK_F3:  return cmKey_F3;
	case VK_F4:  return cmKey_F4;
	case VK_F5:  return cmKey_F5;
	case VK_F6:  return cmKey_F6;
	case VK_F7:  return cmKey_F7;
	case VK_F8:  return cmKey_F8;
	case VK_F9:  return cmKey_F9;
	case VK_F10: return cmKey_F10;
	case VK_F11: return cmKey_F11;
	case VK_F12: return cmKey_F12;
	case VK_F13: return cmKey_F13;
	case VK_F14: return cmKey_F14;
	case VK_F15: return cmKey_F15;

	case VK_PAUSE: return cmKey_Pause;
	}
	return cmKey_Unknown;
}
LRESULT CALLBACK 
cm__win32_window_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// NOTE(bill): Silly callbacks
	cmPlatform *platform = cast(cmPlatform *)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
	b32 window_has_focus = (platform != NULL) && platform->window_has_focus;

	if (msg == WM_CREATE) { // NOTE(bill): Doesn't need the platform
		// NOTE(bill): https://msdn.microsoft.com/en-us/library/windows/desktop/ms645536(v=vs.85).aspx
		RAWINPUTDEVICE rid[2] = {0};

		// NOTE(bill): Keyboard
		rid[0].usUsagePage = 0x01;
		rid[0].usUsage     = 0x06;
		rid[0].dwFlags     = 0x00000030/*RIDEV_NOLEGACY*/; // NOTE(bill): Do not generate legacy messages such as WM_KEYDOWN
		rid[0].hwndTarget  = hWnd;

		// NOTE(bill): Mouse
		rid[1].usUsagePage = 0x01;
		rid[1].usUsage     = 0x02;
		rid[1].dwFlags     = 0; // NOTE(bill): adds HID mouse and also allows legacy mouse messages to allow for window movement etc.
		rid[1].hwndTarget  = hWnd;

		if (RegisterRawInputDevices(rid, cm_count_of(rid), cm_size_of(rid[0])) == false) {
			DWORD err = GetLastError();
			CM_PANIC("Failed to initialize raw input device for win32."
			         "Err: %u", err);
		}
	}

	if (!platform) {
		return DefWindowProcW(hWnd, msg, wParam, lParam);
	}

	switch (msg) {
	case WM_CLOSE:
	case WM_DESTROY:
		platform->window_is_closed = true;
		return 0;

	case WM_QUIT: {
		platform->quit_requested = true;
	} break;

	case WM_UNICHAR: {
		if (window_has_focus) {
			if (wParam == '\r') {
				wParam = '\n';
			}
			// TODO(bill): Does this need to be thread-safe?
			platform->char_buffer[platform->char_buffer_count++] = cast(Rune)wParam;
		}
	} break;


	case WM_INPUT: {
		RAWINPUT raw = {0};
		unsigned int size = cm_size_of(RAWINPUT);

		if (!GetRawInputData(cast(HRAWINPUT)lParam, RID_INPUT, &raw, &size, cm_size_of(RAWINPUTHEADER))) {
			return 0;
		}
		switch (raw.header.dwType) {
		case RIM_TYPEKEYBOARD: {
			// NOTE(bill): Many thanks to https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/
			// for the
			RAWKEYBOARD *raw_kb = &raw.data.keyboard;
			unsigned int vk = raw_kb->VKey;
			unsigned int scan_code = raw_kb->MakeCode;
			unsigned int flags = raw_kb->Flags;
			// NOTE(bill): e0 and e1 are escape sequences used for certain special keys, such as PRINT and PAUSE/BREAK.
			// NOTE(bill): http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
			b32 is_e0   = (flags & RI_KEY_E0) != 0;
			b32 is_e1   = (flags & RI_KEY_E1) != 0;
			b32 is_up   = (flags & RI_KEY_BREAK) != 0;
			b32 is_down = !is_up;

			// TODO(bill): Should I handle scan codes?

			if (vk == 255) {
				// NOTE(bill): Discard "fake keys"
				return 0;
			} else if (vk == VK_SHIFT) {
				// NOTE(bill): Correct left/right shift
				vk = MapVirtualKeyW(scan_code, MAPVK_VSC_TO_VK_EX);
			} else if (vk == VK_NUMLOCK) {
				// NOTE(bill): Correct PAUSE/BREAK and NUM LOCK and set the extended bit
				scan_code = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC) | 0x100;
			}

			if (is_e1) {
				// NOTE(bill): Escaped sequences, turn vk into the correct scan code
				// except for VK_PAUSE (it's a bug)
				if (vk == VK_PAUSE) {
					scan_code = 0x45;
				} else {
					scan_code = MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
				}
			}

			switch (vk) {
			case VK_CONTROL: vk = (is_e0) ? VK_RCONTROL : VK_LCONTROL; break;
			case VK_MENU:    vk = (is_e0) ? VK_RMENU    : VK_LMENU;   break;

			case VK_RETURN: if (is_e0)  vk = VK_SEPARATOR; break; // NOTE(bill): Numpad return
			case VK_DELETE: if (!is_e0) vk = VK_DECIMAL;   break; // NOTE(bill): Numpad dot
			case VK_INSERT: if (!is_e0) vk = VK_NUMPAD0;   break;
			case VK_HOME:   if (!is_e0) vk = VK_NUMPAD7;   break;
			case VK_END:    if (!is_e0) vk = VK_NUMPAD1;   break;
			case VK_PRIOR:  if (!is_e0) vk = VK_NUMPAD9;   break;
			case VK_NEXT:   if (!is_e0) vk = VK_NUMPAD3;   break;

			// NOTE(bill): The standard arrow keys will always have their e0 bit set, but the
			// corresponding keys on the NUMPAD will not.
			case VK_LEFT:  if (!is_e0) vk = VK_NUMPAD4; break;
			case VK_RIGHT: if (!is_e0) vk = VK_NUMPAD6; break;
			case VK_UP:    if (!is_e0) vk = VK_NUMPAD8; break;
			case VK_DOWN:  if (!is_e0) vk = VK_NUMPAD2; break;

			// NUMPAD 5 doesn't have its e0 bit set
			case VK_CLEAR: if (!is_e0) vk = VK_NUMPAD5; break;
			}

			// NOTE(bill): Set appropriate key state flags
			cm_key_state_update(&platform->keys[cm__win32_from_vk(vk)], is_down);

		} break;
		case RIM_TYPEMOUSE: {
			RAWMOUSE *raw_mouse = &raw.data.mouse;
			u16 flags = raw_mouse->usButtonFlags;
			long dx = +raw_mouse->lLastX;
			long dy = -raw_mouse->lLastY;

			if (flags & RI_MOUSE_WHEEL) {
				platform->mouse_wheel_delta = cast(i16)raw_mouse->usButtonData;
			}

			platform->mouse_raw_dx = dx;
			platform->mouse_raw_dy = dy;
		} break;
		}
	} break;

	default: break;
	}

	return DefWindowProcW(hWnd, msg, wParam, lParam);
}


typedef void *wglCreateContextAttribsARB_Proc(void *hDC, void *hshareContext, int const *attribList);


b32 
cm__platform_init(cmPlatform *p, char const *window_title, cmVideoMode mode, cmRendererType type, u32 window_flags) {
	WNDCLASSEXW wc = {cm_size_of(WNDCLASSEXW)};
	DWORD ex_style = 0, style = 0;
	RECT wr;
	u16 title_buffer[256] = {0}; // TODO(bill): cm_local_persist this?

	wc.style = CS_HREDRAW | CS_VREDRAW; // | CS_OWNDC
	wc.lpfnWndProc   = cm__win32_window_callback;
	wc.hbrBackground = cast(HBRUSH)GetStockObject(0/*WHITE_BRUSH*/);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = L"cm-win32-wndclass"; // TODO(bill): Is this enough?
	wc.hInstance     = GetModuleHandleW(NULL);

	if (RegisterClassExW(&wc) == 0) {
		MessageBoxW(NULL, L"Failed to register the window class", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if ((window_flags & cmWindow_Fullscreen) && !(window_flags & cmWindow_Borderless)) {
		DEVMODEW screen_settings = {cm_size_of(DEVMODEW)};
		screen_settings.dmPelsWidth	 = mode.width;
		screen_settings.dmPelsHeight = mode.height;
		screen_settings.dmBitsPerPel = mode.bits_per_pixel;
		screen_settings.dmFields     = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if (ChangeDisplaySettingsW(&screen_settings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			if (MessageBoxW(NULL, L"The requested fullscreen mode is not supported by\n"
			                L"your video card. Use windowed mode instead?",
			                L"",
			                MB_YESNO|MB_ICONEXCLAMATION) == IDYES) {
				window_flags &= ~cmWindow_Fullscreen;
			} else {
				mode = cm_video_mode_get_desktop();
				screen_settings.dmPelsWidth	 = mode.width;
				screen_settings.dmPelsHeight = mode.height;
				screen_settings.dmBitsPerPel = mode.bits_per_pixel;
				ChangeDisplaySettingsW(&screen_settings, CDS_FULLSCREEN);
			}
		}
	}


	// ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	// style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_THICKFRAME | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;

	style |= WS_VISIBLE;

	if (window_flags & cmWindow_Hidden)       style &= ~WS_VISIBLE;
	if (window_flags & cmWindow_Resizable)    style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
	if (window_flags & cmWindow_Maximized)    style |=  WS_MAXIMIZE;
	if (window_flags & cmWindow_Minimized)    style |=  WS_MINIMIZE;

	// NOTE(bill): Completely ignore the given mode and just change it
	if (window_flags & cmWindow_FullscreenDesktop) {
		mode = cm_video_mode_get_desktop();
	}

	if ((window_flags & cmWindow_Fullscreen) || (window_flags & cmWindow_Borderless)) {
		style |= WS_POPUP;
	} else {
		style |= WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	}


	wr.left   = 0;
	wr.top    = 0;
	wr.right  = mode.width;
	wr.bottom = mode.height;
	AdjustWindowRect(&wr, style, false);

	p->window_flags  = window_flags;
	p->window_handle = CreateWindowExW(ex_style,
	                                   wc.lpszClassName,
	                                   cast(wchar_t const *)cm_utf8_to_ucs2(title_buffer, cm_size_of(title_buffer), window_title),
	                                   style,
	                                   CW_USEDEFAULT, CW_USEDEFAULT,
	                                   wr.right - wr.left, wr.bottom - wr.top,
	                                   0, 0,
	                                   GetModuleHandleW(NULL),
	                                   NULL);

	if (!p->window_handle) {
		MessageBoxW(NULL, L"Window creation failed", L"Error", MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	p->win32_dc = GetDC(cast(HWND)p->window_handle);

	p->renderer_type = type;
	switch (p->renderer_type) {
	case cmRenderer_Opengl: {
		wglCreateContextAttribsARB_Proc *wglCreateContextAttribsARB;
		i32 attribs[8] = {0};
		isize c = 0;

		PIXELFORMATDESCRIPTOR pfd = {cm_size_of(PIXELFORMATDESCRIPTOR)};
		pfd.nVersion     = 1;
		pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType   = PFD_TYPE_RGBA;
		pfd.cColorBits   = 32;
		pfd.cAlphaBits   = 8;
		pfd.cDepthBits   = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType   = PFD_MAIN_PLANE;

		SetPixelFormat(cast(HDC)p->win32_dc, ChoosePixelFormat(cast(HDC)p->win32_dc, &pfd), NULL);
		p->opengl.context = cast(void *)wglCreateContext(cast(HDC)p->win32_dc);
		wglMakeCurrent(cast(HDC)p->win32_dc, cast(HGLRC)p->opengl.context);

		if (p->opengl.major > 0) {
			attribs[c++] = 0x2091; // WGL_CONTEXT_MAJOR_VERSION_ARB
			attribs[c++] = CM_MAX(p->opengl.major, 1);
		}
		if (p->opengl.major > 0 && p->opengl.minor >= 0) {
			attribs[c++] = 0x2092; // WGL_CONTEXT_MINOR_VERSION_ARB
			attribs[c++] = CM_MAX(p->opengl.minor, 0);
		}

		if (p->opengl.core) {
			attribs[c++] = 0x9126; // WGL_CONTEXT_PROFILE_MASK_ARB
			attribs[c++] = 0x0001; // WGL_CONTEXT_CORE_PROFILE_BIT_ARB
		} else if (p->opengl.compatible) {
			attribs[c++] = 0x9126; // WGL_CONTEXT_PROFILE_MASK_ARB
			attribs[c++] = 0x0002; // WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
		}
		attribs[c++] = 0; // NOTE(bill): tells the proc that this is the end of attribs

		wglCreateContextAttribsARB = cast(wglCreateContextAttribsARB_Proc *)wglGetProcAddress("wglCreateContextAttribsARB");
		if (wglCreateContextAttribsARB) {
			HGLRC rc = cast(HGLRC)wglCreateContextAttribsARB(p->win32_dc, 0, attribs);
			if (rc && wglMakeCurrent(cast(HDC)p->win32_dc, rc)) {
				p->opengl.context = rc;
			} else {
				// TODO(bill): Handle errors from GetLastError
				// ERROR_INVALID_VERSION_ARB 0x2095
				// ERROR_INVALID_PROFILE_ARB 0x2096
			}
		}

	} break;

	case cmRenderer_Software:
		cm__platform_resize_dib_section(p, mode.width, mode.height);
		break;

	default:
		CM_PANIC("Unknown window type");
		break;
	}

	SetForegroundWindow(cast(HWND)p->window_handle);
	SetFocus(cast(HWND)p->window_handle);
	SetWindowLongPtrW(cast(HWND)p->window_handle, GWLP_USERDATA, cast(LONG_PTR)p);

	p->window_width  = mode.width;
	p->window_height = mode.height;

	if (p->renderer_type == cmRenderer_Opengl) {
		p->opengl.dll_handle = cm_dll_load("opengl32.dll");
	}

	{ // Load XInput
		// TODO(bill): What other dlls should I look for?
		cmDllHandle xinput_library = cm_dll_load("xinput1_4.dll");
		p->xinput.get_state = cmXInputGetState_Stub;
		p->xinput.set_state = cmXInputSetState_Stub;

		if (!xinput_library) xinput_library = cm_dll_load("xinput9_1_0.dll");
		if (!xinput_library) xinput_library = cm_dll_load("xinput1_3.dll");
		if (!xinput_library) {
			// TODO(bill): Proper Diagnostic
			cm_printf_err("XInput could not be loaded. Controllers will not work!\n");
		} else {
			p->xinput.get_state = cast(cmXInputGetStateProc *)cm_dll_proc_address(xinput_library, "XInputGetState");
			p->xinput.set_state = cast(cmXInputSetStateProc *)cm_dll_proc_address(xinput_library, "XInputSetState");
		}
	}

	// Init keys
	cm_zero_array(p->keys, cm_count_of(p->keys));

	p->is_initialized = true;
	return true;
}

cm_inline b32 
cm_platform_init_with_software(cmPlatform *p, char const *window_title, i32 width, i32 height, u32 window_flags) {
	cmVideoMode mode;
	mode.width          = width;
	mode.height         = height;
	mode.bits_per_pixel = 32;
	return cm__platform_init(p, window_title, mode, cmRenderer_Software, window_flags);
}

cm_inline b32 
cm_platform_init_with_opengl(cmPlatform *p, char const *window_title,
    i32 width, i32 height, u32 window_flags, 
    i32 major, i32 minor, b32 core, b32 compatible) {
	cmVideoMode mode;
	mode.width          = width;
	mode.height         = height;
	mode.bits_per_pixel = 32;
	p->opengl.major      = major;
	p->opengl.minor      = minor;
	p->opengl.core       = cast(b16)core;
	p->opengl.compatible = cast(b16)compatible;
	return cm__platform_init(p, window_title, mode, cmRenderer_Opengl, window_flags);
}

#ifndef _XINPUT_H_
typedef struct _XINPUT_GAMEPAD {
	u16 wButtons;
	u8  bLeftTrigger;
	u8  bRightTrigger;
	u16 sThumbLX;
	u16 sThumbLY;
	u16 sThumbRX;
	u16 sThumbRY;
} XINPUT_GAMEPAD;

typedef struct _XINPUT_STATE {
	DWORD          dwPacketNumber;
	XINPUT_GAMEPAD Gamepad;
} XINPUT_STATE;

typedef struct _XINPUT_VIBRATION {
	u16 wLeftMotorSpeed;
	u16 wRightMotorSpeed;
} XINPUT_VIBRATION;

#define XINPUT_GAMEPAD_DPAD_UP              0x00000001
#define XINPUT_GAMEPAD_DPAD_DOWN            0x00000002
#define XINPUT_GAMEPAD_DPAD_LEFT            0x00000004
#define XINPUT_GAMEPAD_DPAD_RIGHT           0x00000008
#define XINPUT_GAMEPAD_START                0x00000010
#define XINPUT_GAMEPAD_BACK                 0x00000020
#define XINPUT_GAMEPAD_LEFT_THUMB           0x00000040
#define XINPUT_GAMEPAD_RIGHT_THUMB          0x00000080
#define XINPUT_GAMEPAD_LEFT_SHOULDER        0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER       0x0200
#define XINPUT_GAMEPAD_A                    0x1000
#define XINPUT_GAMEPAD_B                    0x2000
#define XINPUT_GAMEPAD_X                    0x4000
#define XINPUT_GAMEPAD_Y                    0x8000
#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30
#endif

#ifndef XUSER_MAX_COUNT
#define XUSER_MAX_COUNT 4
#endif

void 
cm_platform_update(cmPlatform *p) {
	isize i;

	{ // NOTE(bill): Set window state
		// TODO(bill): Should this be moved to cm__win32_window_callback ?
		RECT window_rect;
		i32 x, y, w, h;

		GetClientRect(cast(HWND)p->window_handle, &window_rect);
		x = window_rect.left;
		y = window_rect.top;
		w = window_rect.right - window_rect.left;
		h = window_rect.bottom - window_rect.top;

		if ((p->window_width != w) || (p->window_height != h)) {
			if (p->renderer_type == cmRenderer_Software) {
				cm__platform_resize_dib_section(p, w, h);
			}
		}


		p->window_x = x;
		p->window_y = y;
		p->window_width = w;
		p->window_height = h;
		CM_MASK_SET(p->window_flags, IsIconic(cast(HWND)p->window_handle) != 0, cmWindow_Minimized);

		p->window_has_focus = GetFocus() == cast(HWND)p->window_handle;
	}

	{ // NOTE(bill): Set mouse position
		POINT mouse_pos;
		DWORD win_button_id[cmMouseButton_Count] = {
			VK_LBUTTON,
			VK_MBUTTON,
			VK_RBUTTON,
			VK_XBUTTON1,
			VK_XBUTTON2,
		};

		// NOTE(bill): This needs to be GetAsyncKeyState as RAWMOUSE doesn't aways work for some odd reason
		// TODO(bill): Try and get RAWMOUSE to work for key presses
		for (i = 0; i < cmMouseButton_Count; i++) {
			cm_key_state_update(p->mouse_buttons+i, GetAsyncKeyState(win_button_id[i]) < 0);
		}

		GetCursorPos(&mouse_pos);
		ScreenToClient(cast(HWND)p->window_handle, &mouse_pos);
		{
			i32 x = mouse_pos.x;
			i32 y = p->window_height-1 - mouse_pos.y;
			p->mouse_dx = x - p->mouse_x;
			p->mouse_dy = y - p->mouse_y;
			p->mouse_x = x;
			p->mouse_y = y;
		}

		if (p->mouse_clip) {
			b32 update = false;
			i32 x = p->mouse_x;
			i32 y = p->mouse_y;
			if (p->mouse_x < 0) {
				x = 0;
				update = true;
			} else if (p->mouse_y > p->window_height-1) {
				y = p->window_height-1;
				update = true;
			}

			if (p->mouse_y < 0) {
				y = 0;
				update = true;
			} else if (p->mouse_x > p->window_width-1) {
				x = p->window_width-1;
				update = true;
			}

			if (update) {
				cm_platform_set_mouse_position(p, x, y);
			}
		}


	}


	// NOTE(bill): Set Key/Button states
	if (p->window_has_focus) {
		p->char_buffer_count = 0; // TODO(bill): Reset buffer count here or else where?

		// NOTE(bill): Need to update as the keys only get updates on events
		for (i = 0; i < cmKey_Count; i++) {
			b32 is_down = (p->keys[i] & cmKeyState_Down) != 0;
			cm_key_state_update(&p->keys[i], is_down);
		}

		p->key_modifiers.control = p->keys[cmKey_Lcontrol] | p->keys[cmKey_Rcontrol];
		p->key_modifiers.alt     = p->keys[cmKey_Lalt]     | p->keys[cmKey_Ralt];
		p->key_modifiers.shift   = p->keys[cmKey_Lshift]   | p->keys[cmKey_Rshift];

	}

	{ // NOTE(bill): Set Controller states
		isize max_controller_count = XUSER_MAX_COUNT;
		if (max_controller_count > cm_count_of(p->game_controllers)) {
			max_controller_count = cm_count_of(p->game_controllers);
		}

		for (i = 0; i < max_controller_count; i++) {
			cmGameController *controller = &p->game_controllers[i];
			XINPUT_STATE controller_state = {0};
			if (p->xinput.get_state(cast(DWORD)i, &controller_state) != 0) {
				// NOTE(bill): The controller is not available
				controller->is_connected = false;
			} else {
				// NOTE(bill): This controller is plugged in
				// TODO(bill): See if ControllerState.dwPacketNumber increments too rapidly
				XINPUT_GAMEPAD *pad = &controller_state.Gamepad;

				controller->is_connected = true;

				// TODO(bill): This is a square deadzone, check XInput to verify that the deadzone is "round" and do round deadzone processing.
				controller->axes[cmControllerAxis_LeftX]  = cm__process_xinput_stick_value(pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
				controller->axes[cmControllerAxis_LeftY]  = cm__process_xinput_stick_value(pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
				controller->axes[cmControllerAxis_RightX] = cm__process_xinput_stick_value(pad->sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
				controller->axes[cmControllerAxis_RightY] = cm__process_xinput_stick_value(pad->sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

				controller->axes[cmControllerAxis_LeftTrigger]  = cast(f32)pad->bLeftTrigger / 255.0f;
				controller->axes[cmControllerAxis_RightTrigger] = cast(f32)pad->bRightTrigger / 255.0f;


				if ((controller->axes[cmControllerAxis_LeftX] != 0.0f) ||
					(controller->axes[cmControllerAxis_LeftY] != 0.0f)) {
					controller->is_analog = true;
				}

			#define CM__PROCESS_DIGITAL_BUTTON(button_type, xinput_button) \
				cm_key_state_update(&controller->buttons[button_type], (pad->wButtons & xinput_button) == xinput_button)

				CM__PROCESS_DIGITAL_BUTTON(cmControllerButton_A,              XINPUT_GAMEPAD_A);
				CM__PROCESS_DIGITAL_BUTTON(cmControllerButton_B,              XINPUT_GAMEPAD_B);
				CM__PROCESS_DIGITAL_BUTTON(cmControllerButton_X,              XINPUT_GAMEPAD_X);
				CM__PROCESS_DIGITAL_BUTTON(cmControllerButton_Y,              XINPUT_GAMEPAD_Y);
				CM__PROCESS_DIGITAL_BUTTON(cmControllerButton_LeftShoulder,  XINPUT_GAMEPAD_LEFT_SHOULDER);
				CM__PROCESS_DIGITAL_BUTTON(cmControllerButton_RightShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER);
				CM__PROCESS_DIGITAL_BUTTON(cmControllerButton_Start,          XINPUT_GAMEPAD_START);
				CM__PROCESS_DIGITAL_BUTTON(cmControllerButton_Back,           XINPUT_GAMEPAD_BACK);
				CM__PROCESS_DIGITAL_BUTTON(cmControllerButton_Left,           XINPUT_GAMEPAD_DPAD_LEFT);
				CM__PROCESS_DIGITAL_BUTTON(cmControllerButton_Right,          XINPUT_GAMEPAD_DPAD_RIGHT);
				CM__PROCESS_DIGITAL_BUTTON(cmControllerButton_Down,           XINPUT_GAMEPAD_DPAD_DOWN);
				CM__PROCESS_DIGITAL_BUTTON(cmControllerButton_Up,             XINPUT_GAMEPAD_DPAD_UP);
				CM__PROCESS_DIGITAL_BUTTON(cmControllerButton_LeftThumb,     XINPUT_GAMEPAD_LEFT_THUMB);
				CM__PROCESS_DIGITAL_BUTTON(cmControllerButton_RightThumb,    XINPUT_GAMEPAD_RIGHT_THUMB);
			#undef CM__PROCESS_DIGITAL_BUTTON
			}
		}
	}

	{ // NOTE(bill): Process pending messages
		MSG message;
		for (;;) {
			BOOL is_okay = PeekMessageW(&message, 0, 0, 0, PM_REMOVE);
			if (!is_okay) break;

			switch (message.message) {
			case WM_QUIT:
				p->quit_requested = true;
				break;

			default:
				TranslateMessage(&message);
				DispatchMessageW(&message);
				break;
			}
		}
	}
}

void 
cm_platform_display(cmPlatform *p) {
	if (p->renderer_type == cmRenderer_Opengl) {
		SwapBuffers(cast(HDC)p->win32_dc);
	} else if (p->renderer_type == cmRenderer_Software) {
		StretchDIBits(cast(HDC)p->win32_dc,
		              0, 0, p->window_width, p->window_height,
		              0, 0, p->window_width, p->window_height,
		              p->sw_framebuffer.memory,
		              &p->sw_framebuffer.win32_bmi,
		              DIB_RGB_COLORS, SRCCOPY);
	} else {
		CM_PANIC("Invalid window rendering type");
	}

	{
		f64 prev_time = p->curr_time;
		f64 curr_time = cm_time_now();
		p->dt_for_frame = curr_time - prev_time;
		p->curr_time = curr_time;
	}
}


void 
cm_platform_destroy(cmPlatform *p) {
	if (p->renderer_type == cmRenderer_Opengl) {
		wglDeleteContext(cast(HGLRC)p->opengl.context);
	} else if (p->renderer_type == cmRenderer_Software) {
		cm_vm_free(cm_virtual_memory(p->sw_framebuffer.memory, p->sw_framebuffer.memory_size));
	}

	DestroyWindow(cast(HWND)p->window_handle);
}

void 
cm_platform_show_cursor(cmPlatform *p, b32 show) {
	cm_unused(p);
	ShowCursor(show);
}

void 
cm_platform_set_mouse_position(cmPlatform *p, i32 x, i32 y) {
	POINT point;
	point.x = cast(LONG)x;
	point.y = cast(LONG)(p->window_height-1 - y);
	ClientToScreen(cast(HWND)p->window_handle, &point);
	SetCursorPos(point.x, point.y);

	p->mouse_x = point.x;
	p->mouse_y = p->window_height-1 - point.y;
}

void 
cm_platform_set_controller_vibration(cmPlatform *p, isize index, f32 left_motor, f32 right_motor) {
	if (cm_is_between(index, 0, CM_MAX_GAME_CONTROLLER_COUNT-1)) {
		XINPUT_VIBRATION vibration = {0};
		left_motor  = cm_clamp01(left_motor);
		right_motor = cm_clamp01(right_motor);
		vibration.wLeftMotorSpeed  = cast(WORD)(65535 * left_motor);
		vibration.wRightMotorSpeed = cast(WORD)(65535 * right_motor);

		p->xinput.set_state(cast(DWORD)index, &vibration);
	}
}

void 
cm_platform_set_window_position(cmPlatform *p, i32 x, i32 y) {
	RECT rect;
	i32 width, height;

	GetClientRect(cast(HWND)p->window_handle, &rect);
	width  = rect.right - rect.left;
	height = rect.bottom - rect.top;
	MoveWindow(cast(HWND)p->window_handle, x, y, width, height, false);
}

void 
cm_platform_set_window_title(cmPlatform *p, char const *title, ...) {
	u16 buffer[256] = {0};
	char str[512] = {0};
	va_list va;
	va_start(va, title);
	cm_snprintf_va(str, cm_size_of(str), title, va);
	va_end(va);

	if (str[0] != '\0') {
		SetWindowTextW(cast(HWND)p->window_handle, cast(wchar_t const *)cm_utf8_to_ucs2(buffer, cm_size_of(buffer), str));
	}
}

void 
cm_platform_toggle_fullscreen(cmPlatform *p, b32 fullscreen_desktop) {
	// NOTE(bill): From the man himself, Raymond Chen! (Modified for my need.)
	HWND handle = cast(HWND)p->window_handle;
	DWORD style = cast(DWORD)GetWindowLongW(handle, GWL_STYLE);
	WINDOWPLACEMENT placement;

	if (style & WS_OVERLAPPEDWINDOW) {
		MONITORINFO monitor_info = {cm_size_of(monitor_info)};
		if (GetWindowPlacement(handle, &placement) &&
		    GetMonitorInfoW(MonitorFromWindow(handle, 1), &monitor_info)) {
			style &= ~WS_OVERLAPPEDWINDOW;
			if (fullscreen_desktop) {
				style &= ~WS_CAPTION;
				style |= WS_POPUP;
			}
			SetWindowLongW(handle, GWL_STYLE, style);
			SetWindowPos(handle, HWND_TOP,
			             monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
			             monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
			             monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
			             SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

			if (fullscreen_desktop) {
				p->window_flags |= cmWindow_FullscreenDesktop;
			} else {
				p->window_flags |= cmWindow_Fullscreen;
			}
		}
	} else {
		style &= ~WS_POPUP;
		style |= WS_OVERLAPPEDWINDOW | WS_CAPTION;
		SetWindowLongW(handle, GWL_STYLE, style);
		SetWindowPlacement(handle, &placement);
		SetWindowPos(handle, 0, 0, 0, 0, 0,
		             SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
		             SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		p->window_flags &= ~cmWindow_Fullscreen;
	}
}

void 
cm_platform_toggle_borderless(cmPlatform *p) {
	HWND handle = cast(HWND)p->window_handle;
	DWORD style = GetWindowLongW(handle, GWL_STYLE);
	b32 is_borderless = (style & WS_POPUP) != 0;

	CM_MASK_SET(style, is_borderless,  WS_OVERLAPPEDWINDOW | WS_CAPTION);
	CM_MASK_SET(style, !is_borderless, WS_POPUP);

	SetWindowLongW(handle, GWL_STYLE, style);

	CM_MASK_SET(p->window_flags, !is_borderless, cmWindow_Borderless);
}

cm_inline void 
cm_platform_make_opengl_context_current(cmPlatform *p) {
	if (p->renderer_type == cmRenderer_Opengl) {
		wglMakeCurrent(cast(HDC)p->win32_dc, cast(HGLRC)p->opengl.context);
	}
}

cm_inline void 
cm_platform_show_window(cmPlatform *p) {
	ShowWindow(cast(HWND)p->window_handle, SW_SHOW);
	p->window_flags &= ~cmWindow_Hidden;
}

cm_inline void 
cm_platform_hide_window(cmPlatform *p) {
	ShowWindow(cast(HWND)p->window_handle, SW_HIDE);
	p->window_flags |= cmWindow_Hidden;
}

cm_inline cmVideoMode 
cm_video_mode_get_desktop(void) {
	DEVMODEW win32_mode = {cm_size_of(win32_mode)};
	EnumDisplaySettingsW(NULL, ENUM_CURRENT_SETTINGS, &win32_mode);
	return cm_video_mode(win32_mode.dmPelsWidth, win32_mode.dmPelsHeight, win32_mode.dmBitsPerPel);
}

isize 
cm_video_mode_get_fullscreen_modes(cmVideoMode *modes, isize max_mode_count) {
	DEVMODEW win32_mode = {cm_size_of(win32_mode)};
	i32 count;
	for (count = 0;
	     count < max_mode_count && EnumDisplaySettingsW(NULL, count, &win32_mode);
	     count++) {
		modes[count] = cm_video_mode(win32_mode.dmPelsWidth, win32_mode.dmPelsHeight, win32_mode.dmBitsPerPel);
	}

	cm_sort_array(modes, count, cm_video_mode_dsc_cmp);
	return count;
}

b32 
cm_platform_has_clipboard_text(cmPlatform *p) {
	b32 result = false;

	if (IsClipboardFormatAvailable(1/*CF_TEXT*/) &&
	    OpenClipboard(cast(HWND)p->window_handle)) {
		HANDLE mem = GetClipboardData(1/*CF_TEXT*/);
		if (mem) {
			char *str = cast(char *)GlobalLock(mem);
			if (str && str[0] != '\0') {
				result = true;
			}
			GlobalUnlock(mem);
		} else {
			return false;
		}

		CloseClipboard();
	}

	return result;
}

// TODO(bill): Handle UTF-8
void 
cm_platform_set_clipboard_text(cmPlatform *p, char const *str) {
	if (OpenClipboard(cast(HWND)p->window_handle)) {
		isize i, len = cm_strlen(str)+1;

		HANDLE mem = cast(HANDLE)GlobalAlloc(0x0002/*GMEM_MOVEABLE*/, len);
		if (mem) {
			char *dst = cast(char *)GlobalLock(mem);
			if (dst) {
				for (i = 0; str[i]; i++) {
					// TODO(bill): Does this cause a buffer overflow?
					// NOTE(bill): Change \n to \r\n 'cause windows
					if (str[i] == '\n' && (i == 0 || str[i-1] != '\r')) {
						*dst++ = '\r';
					}
					*dst++ = str[i];
				}
				*dst = 0;
			}
			GlobalUnlock(mem);
		}

		EmptyClipboard();
		if (!SetClipboardData(1/*CF_TEXT*/, mem)) {
			return;
		}
		CloseClipboard();
	}
}

// TODO(bill): Handle UTF-8
char *
cm_platform_get_clipboard_text(cmPlatform *p, cmAllocator a) {
	char *text = NULL;

	if (IsClipboardFormatAvailable(1/*CF_TEXT*/) &&
	    OpenClipboard(cast(HWND)p->window_handle)) {
		HANDLE mem = GetClipboardData(1/*CF_TEXT*/);
		if (mem) {
			char *str = cast(char *)GlobalLock(mem);
			text = cm_alloc_str(a, str);
			GlobalUnlock(mem);
		} else {
			return NULL;
		}

		CloseClipboard();
	}

	return text;
}

#elif defined(CM_SYS_OSX)

#include <CoreGraphics/CoreGraphics.h>
#include <objc/objc.h>
#include <objc/message.h>
#include <objc/NSObjCRuntime.h>

#if __LP64__ || (TARGET_OS_EMBEDDED && !TARGET_OS_IPHONE) || TARGET_OS_WIN32 || NS_BUILD_32_LIKE_64
	#define NSIntegerEncoding  "q"
	#define NSUIntegerEncoding "L"
#else
	#define NSIntegerEncoding  "i"
	#define NSUIntegerEncoding "I"
#endif

#ifdef __OBJC__
	#import <Cocoa/Cocoa.h>
#else
	typedef CGPoint NSPoint;
	typedef CGSize  NSSize;
	typedef CGRect  NSRect;

	extern id NSApp;
	extern id const NSDefaultRunLoopMode;
#endif

#if defined(__OBJC__) && __has_feature(objc_arc)
#error TODO(bill): Cannot compile as objective-c code just yet!
#endif

// ABI is a bit different between platforms
#ifdef __arm64__
#define abi_objc_msgSend_stret objc_msgSend
#else
#define abi_objc_msgSend_stret objc_msgSend_stret
#endif
#ifdef __i386__
#define abi_objc_msgSend_fpret objc_msgSend_fpret
#else
#define abi_objc_msgSend_fpret objc_msgSend
#endif

#define objc_msgSend_id				((id (*)(id, SEL))objc_msgSend)
#define objc_msgSend_void			((void (*)(id, SEL))objc_msgSend)
#define objc_msgSend_void_id		((void (*)(id, SEL, id))objc_msgSend)
#define objc_msgSend_void_bool		((void (*)(id, SEL, BOOL))objc_msgSend)
#define objc_msgSend_id_char_const	((id (*)(id, SEL, char const *))objc_msgSend)

cm_internal NSUInteger 
cm__osx_application_should_terminate(id self, SEL _sel, id sender) {
	// NOTE(bill): Do nothing
	return 0;
}

cm_internal void 
cm__osx_window_will_close(id self, SEL _sel, id notification) {
	NSUInteger value = true;
	object_setInstanceVariable(self, "closed", cast(void *)value);
}

cm_internal void 
cm__osx_window_did_become_key(id self, SEL _sel, id notification) {
	gbPlatform *p = NULL;
	object_getInstanceVariable(self, "gbPlatform", cast(void **)&p);
	if (p) {
		// TODO(bill):
	}
}

b32 
cm__platform_init(cmPlatform *p, char const *window_title, cmVideoMode mode, cmRendererType type, u32 window_flags) {
	if (p->is_initialized) {
		return true;
	}
	// Init Platform
	{ // Initial OSX State
		Class appDelegateClass;
		b32 resultAddProtoc, resultAddMethod;
		id dgAlloc, dg, menubarAlloc, menubar;
		id appMenuItemAlloc, appMenuItem;
		id appMenuAlloc, appMenu;

		#if defined(ARC_AVAILABLE)
		#error TODO(bill): This code should be compiled as C for now
		#else
		id poolAlloc = objc_msgSend_id(cast(id)objc_getClass("NSAutoreleasePool"), sel_registerName("alloc"));
		p->osx_autorelease_pool = objc_msgSend_id(poolAlloc, sel_registerName("init"));
		#endif

		objc_msgSend_id(cast(id)objc_getClass("NSApplication"), sel_registerName("sharedApplication"));
		((void (*)(id, SEL, NSInteger))objc_msgSend)(NSApp, sel_registerName("setActivationPolicy:"), 0);

		appDelegateClass = objc_allocateClassPair((Class)objc_getClass("NSObject"), "AppDelegate", 0);
		resultAddProtoc = class_addProtocol(appDelegateClass, objc_getProtocol("NSApplicationDelegate"));
		assert(resultAddProtoc);
		resultAddMethod = class_addMethod(appDelegateClass, sel_registerName("applicationShouldTerminate:"), cast(IMP)cm__osx_application_should_terminate, NSUIntegerEncoding "@:@");
		assert(resultAddMethod);
		dgAlloc = objc_msgSend_id(cast(id)appDelegateClass, sel_registerName("alloc"));
		dg = objc_msgSend_id(dgAlloc, sel_registerName("init"));
		#ifndef ARC_AVAILABLE
		objc_msgSend_void(dg, sel_registerName("autorelease"));
		#endif

		objc_msgSend_void_id(NSApp, sel_registerName("setDelegate:"), dg);
		objc_msgSend_void(NSApp, sel_registerName("finishLaunching"));

		menubarAlloc = objc_msgSend_id(cast(id)objc_getClass("NSMenu"), sel_registerName("alloc"));
		menubar = objc_msgSend_id(menubarAlloc, sel_registerName("init"));
		#ifndef ARC_AVAILABLE
		objc_msgSend_void(menubar, sel_registerName("autorelease"));
		#endif

		appMenuItemAlloc = objc_msgSend_id(cast(id)objc_getClass("NSMenuItem"), sel_registerName("alloc"));
		appMenuItem = objc_msgSend_id(appMenuItemAlloc, sel_registerName("init"));
		#ifndef ARC_AVAILABLE
		objc_msgSend_void(appMenuItem, sel_registerName("autorelease"));
		#endif

		objc_msgSend_void_id(menubar, sel_registerName("addItem:"), appMenuItem);
		((id (*)(id, SEL, id))objc_msgSend)(NSApp, sel_registerName("setMainMenu:"), menubar);

		appMenuAlloc = objc_msgSend_id(cast(id)objc_getClass("NSMenu"), sel_registerName("alloc"));
		appMenu = objc_msgSend_id(appMenuAlloc, sel_registerName("init"));
		#ifndef ARC_AVAILABLE
		objc_msgSend_void(appMenu, sel_registerName("autorelease"));
		#endif

		{
			id processInfo = objc_msgSend_id(cast(id)objc_getClass("NSProcessInfo"), sel_registerName("processInfo"));
			id appName = objc_msgSend_id(processInfo, sel_registerName("processName"));

			id quitTitlePrefixString = objc_msgSend_id_char_const(cast(id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), "Quit ");
			id quitTitle = ((id (*)(id, SEL, id))objc_msgSend)(quitTitlePrefixString, sel_registerName("stringByAppendingString:"), appName);

			id quitMenuItemKey = objc_msgSend_id_char_const(cast(id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), "q");
			id quitMenuItemAlloc = objc_msgSend_id(cast(id)objc_getClass("NSMenuItem"), sel_registerName("alloc"));
			id quitMenuItem = ((id (*)(id, SEL, id, SEL, id))objc_msgSend)(quitMenuItemAlloc, sel_registerName("initWithTitle:action:keyEquivalent:"), quitTitle, sel_registerName("terminate:"), quitMenuItemKey);
			#ifndef ARC_AVAILABLE
			objc_msgSend_void(quitMenuItem, sel_registerName("autorelease"));
			#endif

			objc_msgSend_void_id(appMenu, sel_registerName("addItem:"), quitMenuItem);
			objc_msgSend_void_id(appMenuItem, sel_registerName("setSubmenu:"), appMenu);
		}
	}

	{ // Init Window
		NSRect rect = {{0, 0}, {cast(CGFloat)mode.width, cast(CGFloat)mode.height}};
		id windowAlloc, window, wdgAlloc, wdg, contentView, titleString;
		Class WindowDelegateClass;
		b32 resultAddProtoc, resultAddIvar, resultAddMethod;

		windowAlloc = objc_msgSend_id(cast(id)objc_getClass("NSWindow"), sel_registerName("alloc"));
		window = ((id (*)(id, SEL, NSRect, NSUInteger, NSUInteger, BOOL))objc_msgSend)(windowAlloc, sel_registerName("initWithContentRect:styleMask:backing:defer:"), rect, 15, 2, NO);
		#ifndef ARC_AVAILABLE
		objc_msgSend_void(window, sel_registerName("autorelease"));
		#endif

		// when we are not using ARC, than window will be added to autorelease pool
		// so if we close it by hand (pressing red button), we don't want it to be released for us
		// so it will be released by autorelease pool later
		objc_msgSend_void_bool(window, sel_registerName("setReleasedWhenClosed:"), NO);

		WindowDelegateClass = objc_allocateClassPair((Class)objc_getClass("NSObject"), "WindowDelegate", 0);
		resultAddProtoc = class_addProtocol(WindowDelegateClass, objc_getProtocol("NSWindowDelegate"));
		CM_ASSERT(resultAddProtoc);
		resultAddIvar = class_addIvar(WindowDelegateClass, "closed", cm_size_of(NSUInteger), rint(log2(cm_size_of(NSUInteger))), NSUIntegerEncoding);
		CM_ASSERT(resultAddIvar);
		resultAddIvar = class_addIvar(WindowDelegateClass, "gbPlatform", cm_size_of(void *), rint(log2(cm_size_of(void *))), "ˆv");
		CM_ASSERT(resultAddIvar);
		resultAddMethod = class_addMethod(WindowDelegateClass, sel_registerName("windowWillClose:"), cast(IMP)cm__osx_window_will_close,  "v@:@");
		CM_ASSERT(resultAddMethod);
		resultAddMethod = class_addMethod(WindowDelegateClass, sel_registerName("windowDidBecomeKey:"), cast(IMP)cm__osx_window_did_become_key,  "v@:@");
		CM_ASSERT(resultAddMethod);
		wdgAlloc = objc_msgSend_id(cast(id)WindowDelegateClass, sel_registerName("alloc"));
		wdg = objc_msgSend_id(wdgAlloc, sel_registerName("init"));
		#ifndef ARC_AVAILABLE
		objc_msgSend_void(wdg, sel_registerName("autorelease"));
		#endif

		objc_msgSend_void_id(window, sel_registerName("setDelegate:"), wdg);

		contentView = objc_msgSend_id(window, sel_registerName("contentView"));

		{
			NSPoint point = {20, 20};
			((void (*)(id, SEL, NSPoint))objc_msgSend)(window, sel_registerName("cascadeTopLeftFromPoint:"), point);
		}

		titleString = objc_msgSend_id_char_const(cast(id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), window_title);
		objc_msgSend_void_id(window, sel_registerName("setTitle:"), titleString);

		if (type == gbRenderer_Opengl) {
			// TODO(bill): Make sure this works correctly
			u32 opengl_hex_version = (p->opengl.major << 12) | (p->opengl.minor << 8);
			u32 gl_attribs[] = {
				8, 24,                  // NSOpenGLPFAColorSize, 24,
				11, 8,                  // NSOpenGLPFAAlphaSize, 8,
				5,                      // NSOpenGLPFADoubleBuffer,
				73,                     // NSOpenGLPFAAccelerated,
				//72,                   // NSOpenGLPFANoRecovery,
				//55, 1,                // NSOpenGLPFASampleBuffers, 1,
				//56, 4,                // NSOpenGLPFASamples, 4,
				99, opengl_hex_version, // NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
				0
			};

			id pixel_format_alloc, pixel_format;
			id opengl_context_alloc, opengl_context;

			pixel_format_alloc = objc_msgSend_id(cast(id)objc_getClass("NSOpenGLPixelFormat"), sel_registerName("alloc"));
			pixel_format = ((id (*)(id, SEL, const uint32_t*))objc_msgSend)(pixel_format_alloc, sel_registerName("initWithAttributes:"), gl_attribs);
			#ifndef ARC_AVAILABLE
			objc_msgSend_void(pixel_format, sel_registerName("autorelease"));
			#endif

			opengl_context_alloc = objc_msgSend_id(cast(id)objc_getClass("NSOpenGLContext"), sel_registerName("alloc"));
			opengl_context = ((id (*)(id, SEL, id, id))objc_msgSend)(opengl_context_alloc, sel_registerName("initWithFormat:shareContext:"), pixel_format, nil);
			#ifndef ARC_AVAILABLE
			objc_msgSend_void(opengl_context, sel_registerName("autorelease"));
			#endif

			objc_msgSend_void_id(opengl_context, sel_registerName("setView:"), contentView);
			objc_msgSend_void_id(window, sel_registerName("makeKeyAndOrderFront:"), window);
			objc_msgSend_void_bool(window, sel_registerName("setAcceptsMouseMovedEvents:"), YES);


			p->window_handle = cast(void *)window;
			p->opengl.context = cast(void *)opengl_context;
		} else {
			CM_PANIC("TODO(bill): Software rendering");
		}

		{
			id blackColor = objc_msgSend_id(cast(id)objc_getClass("NSColor"), sel_registerName("blackColor"));
			objc_msgSend_void_id(window, sel_registerName("setBackgroundColor:"), blackColor);
			objc_msgSend_void_bool(NSApp, sel_registerName("activateIgnoringOtherApps:"), YES);
		}
		object_setInstanceVariable(wdg, "gbPlatform", cast(void *)p);

		p->is_initialized = true;
	}

	return true;
}

// NOTE(bill): Software rendering
b32 
cm_platform_init_with_software(cmPlatform *p, char const *window_title, i32 width, i32 height, u32 window_flags) {
	CM_PANIC("TODO(bill): Software rendering in not yet implemented on OS X\n");
	return cm__platform_init(p, window_title, cm_video_mode(width, height, 32), gbRenderer_Software, window_flags);
}

// NOTE(bill): OpenGL Rendering
b32 
cm_platform_init_with_opengl(cmPlatform *p, char const *window_title, i32 width, i32 height, u32 window_flags,
                                 i32 major, i32 minor, b32 core, b32 compatible) {

	p->opengl.major = major;
	p->opengl.minor = minor;
	p->opengl.core  = core;
	p->opengl.compatible = compatible;
	return cm__platform_init(p, window_title, cm_video_mode(width, height, 32), gbRenderer_Opengl, window_flags);
}

// NOTE(bill): Reverse engineering can be fun!!!
cm_internal cmKeyType 
cm__osx_from_key_code(u16 key_code) {
	switch (key_code) {
	default: return cmKey_Unknown;
	// NOTE(bill): WHO THE FUCK DESIGNED THIS VIRTUAL KEY CODE SYSTEM?!
	// THEY ARE FUCKING IDIOTS!
	case 0x1d: return cmKey_0;
	case 0x12: return cmKey_1;
	case 0x13: return cmKey_2;
	case 0x14: return cmKey_3;
	case 0x15: return cmKey_4;
	case 0x17: return cmKey_5;
	case 0x16: return cmKey_6;
	case 0x1a: return cmKey_7;
	case 0x1c: return cmKey_8;
	case 0x19: return cmKey_9;

	case 0x00: return cmKey_A;
	case 0x0b: return cmKey_B;
	case 0x08: return cmKey_C;
	case 0x02: return cmKey_D;
	case 0x0e: return cmKey_E;
	case 0x03: return cmKey_F;
	case 0x05: return cmKey_G;
	case 0x04: return cmKey_H;
	case 0x22: return cmKey_I;
	case 0x26: return cmKey_J;
	case 0x28: return cmKey_K;
	case 0x25: return cmKey_L;
	case 0x2e: return cmKey_M;
	case 0x2d: return cmKey_N;
	case 0x1f: return cmKey_O;
	case 0x23: return cmKey_P;
	case 0x0c: return cmKey_Q;
	case 0x0f: return cmKey_R;
	case 0x01: return cmKey_S;
	case 0x11: return cmKey_T;
	case 0x20: return cmKey_U;
	case 0x09: return cmKey_V;
	case 0x0d: return cmKey_W;
	case 0x07: return cmKey_X;
	case 0x10: return cmKey_Y;
	case 0x06: return cmKey_Z;

	case 0x21: return cmKey_Lbracket;
	case 0x1e: return cmKey_Rbracket;
	case 0x29: return cmKey_Semicolon;
	case 0x2b: return cmKey_Comma;
	case 0x2f: return cmKey_Period;
	case 0x27: return cmKey_Quote;
	case 0x2c: return cmKey_Slash;
	case 0x2a: return cmKey_Backslash;
	case 0x32: return cmKey_Grave;
	case 0x18: return cmKey_Equals;
	case 0x1b: return cmKey_Minus;
	case 0x31: return cmKey_Space;

	case 0x35: return cmKey_Escape;       // Escape
	case 0x3b: return cmKey_Lcontrol;     // Left Control
	case 0x38: return cmKey_Lshift;       // Left Shift
	case 0x3a: return cmKey_Lalt;         // Left Alt
	case 0x37: return cmKey_Lsystem;      // Left OS specific: window (Windows and Linux), apple/cmd (MacOS X), ...
	case 0x3e: return cmKey_Rcontrol;     // Right Control
	case 0x3c: return cmKey_Rshift;       // Right Shift
	case 0x3d: return cmKey_Ralt;         // Right Alt
	// case 0x37: return gbKey_Rsystem;      // Right OS specific: window (Windows and Linux), apple/cmd (MacOS X), ...
	case 0x6e: return cmKey_Menu;         // Menu
	case 0x24: return cmKey_Return;       // Return
	case 0x33: return cmKey_Backspace;    // Backspace
	case 0x30: return cmKey_Tab;          // Tabulation
	case 0x74: return cmKey_Pageup;       // Page up
	case 0x79: return cmKey_Pagedown;     // Page down
	case 0x77: return cmKey_End;          // End
	case 0x73: return cmKey_Home;         // Home
	case 0x72: return cmKey_Insert;       // Insert
	case 0x75: return cmKey_Delete;       // Delete
	case 0x45: return cmKey_Plus;         // +
	case 0x4e: return cmKey_Subtract;     // -
	case 0x43: return cmKey_Multiply;     // *
	case 0x4b: return cmKey_Divide;       // /
	case 0x7b: return cmKey_Left;         // Left arrow
	case 0x7c: return cmKey_Right;        // Right arrow
	case 0x7e: return cmKey_Up;           // Up arrow
	case 0x7d: return cmKey_Down;         // Down arrow
	case 0x52: return cmKey_Numpad0;      // Numpad 0
	case 0x53: return cmKey_Numpad1;      // Numpad 1
	case 0x54: return cmKey_Numpad2;      // Numpad 2
	case 0x55: return cmKey_Numpad3;      // Numpad 3
	case 0x56: return cmKey_Numpad4;      // Numpad 4
	case 0x57: return cmKey_Numpad5;      // Numpad 5
	case 0x58: return cmKey_Numpad6;      // Numpad 6
	case 0x59: return cmKey_Numpad7;      // Numpad 7
	case 0x5b: return cmKey_Numpad8;      // Numpad 8
	case 0x5c: return cmKey_Numpad9;      // Numpad 9
	case 0x41: return cmKey_NumpadDot;    // Numpad .
	case 0x4c: return cmKey_NumpadEnter;  // Numpad Enter
	case 0x7a: return cmKey_F1;           // F1
	case 0x78: return cmKey_F2;           // F2
	case 0x63: return cmKey_F3;           // F3
	case 0x76: return cmKey_F4;           // F4
	case 0x60: return cmKey_F5;           // F5
	case 0x61: return cmKey_F6;           // F6
	case 0x62: return cmKey_F7;           // F7
	case 0x64: return cmKey_F8;           // F8
	case 0x65: return cmKey_F9;           // F8
	case 0x6d: return cmKey_F10;          // F10
	case 0x67: return cmKey_F11;          // F11
	case 0x6f: return cmKey_F12;          // F12
	case 0x69: return cmKey_F13;          // F13
	case 0x6b: return cmKey_F14;          // F14
	case 0x71: return cmKey_F15;          // F15
	// case : return gbKey_Pause;        // Pause // NOTE(bill): Not possible on OS X
	}
}

cm_internal void 
cm__osx_on_cocoa_event(cmPlatform *p, id event, id window) {
	if (!event) {
		return;
	} else if (objc_msgSend_id(window, sel_registerName("delegate"))) {
		NSUInteger event_type = ((NSUInteger (*)(id, SEL))objc_msgSend)(event, sel_registerName("type"));
		switch (event_type) {
		case 1: cm_key_state_update(&p->mouse_buttons[gbMouseButton_Left],  true);  break; // NSLeftMouseDown
		case 2: cm_key_state_update(&p->mouse_buttons[gbMouseButton_Left],  false); break; // NSLeftMouseUp
		case 3: cm_key_state_update(&p->mouse_buttons[gbMouseButton_Right], true);  break; // NSRightMouseDown
		case 4: cm_key_state_update(&p->mouse_buttons[gbMouseButton_Right], false); break; // NSRightMouseUp
		case 25: { // NSOtherMouseDown
			// TODO(bill): Test thoroughly
			NSInteger number = ((NSInteger (*)(id, SEL))objc_msgSend)(event, sel_registerName("buttonNumber"));
			if (number == 2) cm_key_state_update(&p->mouse_buttons[gbMouseButton_Middle], true);
			if (number == 3) cm_key_state_update(&p->mouse_buttons[gbMouseButton_X1],     true);
			if (number == 4) cm_key_state_update(&p->mouse_buttons[gbMouseButton_X2],     true);
		} break;
		case 26: { // NSOtherMouseUp
			NSInteger number = ((NSInteger (*)(id, SEL))objc_msgSend)(event, sel_registerName("buttonNumber"));
			if (number == 2) cm_key_state_update(&p->mouse_buttons[gbMouseButton_Middle], false);
			if (number == 3) cm_key_state_update(&p->mouse_buttons[gbMouseButton_X1],     false);
			if (number == 4) cm_key_state_update(&p->mouse_buttons[gbMouseButton_X2],     false);

		} break;

		// TODO(bill): Scroll wheel
		case 22: { // NSScrollWheel
			CGFloat dx = ((CGFloat (*)(id, SEL))abi_objc_msgSend_fpret)(event, sel_registerName("scrollingDeltaX"));
			CGFloat dy = ((CGFloat (*)(id, SEL))abi_objc_msgSend_fpret)(event, sel_registerName("scrollingDeltaY"));
			BOOL precision_scrolling = ((BOOL (*)(id, SEL))objc_msgSend)(event, sel_registerName("hasPreciseScrollingDeltas"));
			if (precision_scrolling) {
				dx *= 0.1f;
				dy *= 0.1f;
			}
			// TODO(bill): Handle sideways
			p->mouse_wheel_delta = dy;
			// p->mouse_wheel_dy = dy;
			// cm_printf("%f %f\n", dx, dy);
		} break;

		case 12: { // NSFlagsChanged
		#if 0
			// TODO(bill): Reverse engineer this properly
			NSUInteger modifiers = ((NSUInteger (*)(id, SEL))objc_msgSend)(event, sel_registerName("modifierFlags"));
			u32 upper_mask = (modifiers & 0xffff0000ul) >> 16;
			b32 shift   = (upper_mask & 0x02) != 0;
			b32 control = (upper_mask & 0x04) != 0;
			b32 alt     = (upper_mask & 0x08) != 0;
			b32 command = (upper_mask & 0x10) != 0;
		#endif

			// cm_printf("%u\n", keys.mask);
			// cm_printf("%x\n", cast(u32)modifiers);
		} break;

		case 10: { // NSKeyDown
			u16 key_code;

			id input_text = objc_msgSend_id(event, sel_registerName("characters"));
			char const *input_text_utf8 = ((char const *(*)(id, SEL))objc_msgSend)(input_text, sel_registerName("UTF8String"));
			p->char_buffer_count = cm_strnlen(input_text_utf8, cm_size_of(p->char_buffer));
			cm_memcopy(p->char_buffer, input_text_utf8, p->char_buffer_count);

			key_code = ((unsigned short (*)(id, SEL))objc_msgSend)(event, sel_registerName("keyCode"));
			cm_key_state_update(&p->keys[cm__osx_from_key_code(key_code)], true);
		} break;

		case 11: { // NSKeyUp
			u16 key_code = ((unsigned short (*)(id, SEL))objc_msgSend)(event, sel_registerName("keyCode"));
			cm_key_state_update(&p->keys[cm__osx_from_key_code(key_code)], false);
		} break;

		default: break;
		}

		objc_msgSend_void_id(NSApp, sel_registerName("sendEvent:"), event);
	}
}


void 
cm_platform_update(cmPlatform *p) {
	id window, key_window, content_view;
	NSRect original_frame;

	window = cast(id)p->window_handle;
	key_window = objc_msgSend_id(NSApp, sel_registerName("keyWindow"));
	p->window_has_focus = key_window == window; // TODO(bill): Is this right


	if (p->window_has_focus) {
		isize i;
		p->char_buffer_count = 0; // TODO(bill): Reset buffer count here or else where?

		// NOTE(bill): Need to update as the keys only get updates on events
		for (i = 0; i < gbKey_Count; i++) {
			b32 is_down = (p->keys[i] & gbKeyState_Down) != 0;
			cm_key_state_update(&p->keys[i], is_down);
		}

		for (i = 0; i < gbMouseButton_Count; i++) {
			b32 is_down = (p->mouse_buttons[i] & gbKeyState_Down) != 0;
			cm_key_state_update(&p->mouse_buttons[i], is_down);
		}

	}

	{ // Handle Events
		id distant_past = objc_msgSend_id(cast(id)objc_getClass("NSDate"), sel_registerName("distantPast"));
		id event = ((id (*)(id, SEL, NSUInteger, id, id, BOOL))objc_msgSend)(NSApp, sel_registerName("nextEventMatchingMask:untilDate:inMode:dequeue:"), NSUIntegerMax, distant_past, NSDefaultRunLoopMode, YES);
		cm__osx_on_cocoa_event(p, event, window);
	}

	if (p->window_has_focus) {
		p->key_modifiers.control = p->keys[gbKey_Lcontrol] | p->keys[gbKey_Rcontrol];
		p->key_modifiers.alt     = p->keys[gbKey_Lalt]     | p->keys[gbKey_Ralt];
		p->key_modifiers.shift   = p->keys[gbKey_Lshift]   | p->keys[gbKey_Rshift];
	}

	{ // Check if window is closed
		id wdg = objc_msgSend_id(window, sel_registerName("delegate"));
		if (!wdg) {
			p->window_is_closed = false;
		} else {
			NSUInteger value = 0;
			object_getInstanceVariable(wdg, "closed", cast(void **)&value);
			p->window_is_closed = (value != 0);
		}
	}



	content_view = objc_msgSend_id(window, sel_registerName("contentView"));
	original_frame = ((NSRect (*)(id, SEL))abi_objc_msgSend_stret)(content_view, sel_registerName("frame"));

	{ // Window
		NSRect frame = original_frame;
		frame = ((NSRect (*)(id, SEL, NSRect))abi_objc_msgSend_stret)(content_view, sel_registerName("convertRectToBacking:"), frame);
		p->window_width  = frame.size.width;
		p->window_height = frame.size.height;
		frame = ((NSRect (*)(id, SEL, NSRect))abi_objc_msgSend_stret)(window, sel_registerName("convertRectToScreen:"), frame);
		p->window_x = frame.origin.x;
		p->window_y = frame.origin.y;
	}

	{ // Mouse
		NSRect frame = original_frame;
		NSPoint mouse_pos = ((NSPoint (*)(id, SEL))objc_msgSend)(window, sel_registerName("mouseLocationOutsideOfEventStream"));
		mouse_pos.x = cm_clamp(mouse_pos.x, 0, frame.size.width-1);
		mouse_pos.y = cm_clamp(mouse_pos.y, 0, frame.size.height-1);

		{
			i32 x = mouse_pos.x;
			i32 y = mouse_pos.y;
			p->mouse_dx = x - p->mouse_x;
			p->mouse_dy = y - p->mouse_y;
			p->mouse_x = x;
			p->mouse_y = y;
		}

		if (p->mouse_clip) {
			b32 update = false;
			i32 x = p->mouse_x;
			i32 y = p->mouse_y;
			if (p->mouse_x < 0) {
				x = 0;
				update = true;
			} else if (p->mouse_y > p->window_height-1) {
				y = p->window_height-1;
				update = true;
			}

			if (p->mouse_y < 0) {
				y = 0;
				update = true;
			} else if (p->mouse_x > p->window_width-1) {
				x = p->window_width-1;
				update = true;
			}

			if (update) {
				cm_platform_set_mouse_position(p, x, y);
			}
		}
	}

	{ // TODO(bill): Controllers

	}

	// TODO(bill): Is this in the correct place?
	objc_msgSend_void(NSApp, sel_registerName("updateWindows"));
	if (p->renderer_type == gbRenderer_Opengl) {
		objc_msgSend_void(cast(id)p->opengl.context, sel_registerName("update"));
		cm_platform_make_opengl_context_current(p);
	}
}

void 
cm_platform_display(cmPlatform *p) {
	// TODO(bill): Do more
	if (p->renderer_type == cmRenderer_Opengl) {
		cm_platform_make_opengl_context_current(p);
		objc_msgSend_void(cast(id)p->opengl.context, sel_registerName("flushBuffer"));
	} else if (p->renderer_type == cmRenderer_Software) {
		// TODO(bill):
	} else {
		CM_PANIC("Invalid window rendering type");
	}

	{
		f64 prev_time = p->curr_time;
		f64 curr_time = cm_time_now();
		p->dt_for_frame = curr_time - prev_time;
		p->curr_time = curr_time;
	}
}

void 
cm_platform_destroy(cmPlatform *p) {
	cm_platform_make_opengl_context_current(p);

	objc_msgSend_void(cast(id)p->window_handle, sel_registerName("close"));

	#if defined(ARC_AVAILABLE)
	// TODO(bill): autorelease pool
	#else
	objc_msgSend_void(cast(id)p->osx_autorelease_pool, sel_registerName("drain"));
	#endif
}

void 
cm_platform_show_cursor(cmPlatform *p, b32 show) {
	if (show ) {
		// objc_msgSend_void(class_registerName("NSCursor"), sel_registerName("unhide"));
	} else {
		// objc_msgSend_void(class_registerName("NSCursor"), sel_registerName("hide"));
	}
}

void 
cm_platform_set_mouse_position(cmPlatform *p, i32 x, i32 y) {
	// TODO(bill):
	CGPoint pos = {cast(CGFloat)x, cast(CGFloat)y};
	pos.x += p->window_x;
	pos.y += p->window_y;
	CGWarpMouseCursorPosition(pos);
}

void 
cm_platform_set_controller_vibration(cmPlatform *p, isize index, f32 left_motor, f32 right_motor) {
	// TODO(bill):
}

b32 
cm_platform_has_clipboard_text(cmPlatform *p) {
	// TODO(bill):
	return false;
}

void 
cm_platform_set_clipboard_text(cmPlatform *p, char const *str) {
	// TODO(bill):
}

char *
cm_platform_get_clipboard_text(cmPlatform *p, gbAllocator a) {
	// TODO(bill):
	return NULL;
}

void 
cm_platform_set_window_position(cmPlatform *p, i32 x, i32 y) {
	// TODO(bill):
}

void 
cm_platform_set_window_title(cmPlatform *p, char const *title, ...) {
	id title_string;
	char buf[256] = {0};
	va_list va;
	va_start(va, title);
	cm_snprintf_va(buf, cm_count_of(buf), title, va);
	va_end(va);

	title_string = objc_msgSend_id_char_const(cast(id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), buf);
	objc_msgSend_void_id(cast(id)p->window_handle, sel_registerName("setTitle:"), title_string);
}

void 
cm_platform_toggle_fullscreen(cmPlatform *p, b32 fullscreen_desktop) {
	// TODO(bill):
}

void 
cm_platform_toggle_borderless(cmPlatform *p) {
	// TODO(bill):
}

void 
cm_platform_make_opengl_context_current(cmPlatform *p) {
	objc_msgSend_void(cast(id)p->opengl.context, sel_registerName("makeCurrentContext"));
}

void 
cm_platform_show_window(cmPlatform *p) {
	// TODO(bill):
}

void 
cm_platform_hide_window(cmPlatform *p) {
	// TODO(bill):
}

i32 
cm__osx_mode_bits_per_pixel(CGDisplayModeRef mode) {
	i32 bits_per_pixel = 0;
	CFStringRef pixel_encoding = CGDisplayModeCopyPixelEncoding(mode);
	if(CFStringCompare(pixel_encoding, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
		bits_per_pixel = 32;
	} else if(CFStringCompare(pixel_encoding, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
		bits_per_pixel = 16;
	} else if(CFStringCompare(pixel_encoding, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo) {
		bits_per_pixel = 8;
	}
    CFRelease(pixel_encoding);

	return bits_per_pixel;
}

i32 
cm__osx_display_bits_per_pixel(CGDirectDisplayID display) {
	CGDisplayModeRef mode = CGDisplayCopyDisplayMode(display);
	i32 bits_per_pixel = cm__osx_mode_bits_per_pixel(mode);
	CGDisplayModeRelease(mode);
	return bits_per_pixel;
}

cmVideoMode 
cm_video_mode_get_desktop(void) {
	CGDirectDisplayID display = CGMainDisplayID();
	return cm_video_mode(CGDisplayPixelsWide(display),
	                     CGDisplayPixelsHigh(display),
	                     cm__osx_display_bits_per_pixel(display));
}


isize 
cm_video_mode_get_fullscreen_modes(cmVideoMode *modes, isize max_mode_count) {
	CFArrayRef cg_modes = CGDisplayCopyAllDisplayModes(CGMainDisplayID(), NULL);
	CFIndex i, count;
	if (cg_modes == NULL) {
		return 0;
	}

	count = CM_MIN(CFArrayGetCount(cg_modes), max_mode_count);
	for (i = 0; i < count; i++) {
		CGDisplayModeRef cg_mode = cast(CGDisplayModeRef)CFArrayGetValueAtIndex(cg_modes, i);
		modes[i] = cm_video_mode(CGDisplayModeGetWidth(cg_mode),
		                         CGDisplayModeGetHeight(cg_mode),
		                         cm__osx_mode_bits_per_pixel(cg_mode));
	}

	CFRelease(cg_modes);

	cm_sort_array(modes, count, cm_video_mode_dsc_cmp);
	return cast(isize)count;
}

#endif


// TODO(bill): OSX Platform Layer
// NOTE(bill): Use this as a guide so there is no need for Obj-C https://github.com/jimon/osx_app_in_plain_c

cm_inline cmVideoMode 
cm_video_mode(i32 width, i32 height, i32 bits_per_pixel) {
	cmVideoMode m;
	m.width = width;
	m.height = height;
	m.bits_per_pixel = bits_per_pixel;
	return m;
}

cm_inline b32 
cm_video_mode_is_valid(cmVideoMode mode) {
	cm_local_persist cmVideoMode modes[256] = {0};
	cm_local_persist isize mode_count = 0;
	cm_local_persist b32 is_set = false;
	isize i;

	if (!is_set) {
		mode_count = cm_video_mode_get_fullscreen_modes(modes, cm_count_of(modes));
		is_set = true;
	}

	for (i = 0; i < mode_count; i++) {
		cm_printf("%d %d\n", modes[i].width, modes[i].height);
	}

	return cm_binary_search_array(modes, mode_count, &mode, cm_video_mode_cmp) >= 0;
}

CM_COMPARE_PROC(cm_video_mode_cmp) {
	cmVideoMode const *x = cast(cmVideoMode const *)a;
	cmVideoMode const *y = cast(cmVideoMode const *)b;

	if (x->bits_per_pixel == y->bits_per_pixel) {
		if (x->width == y->width) {
			return x->height < y->height ? -1 : x->height > y->height;
		}
		return x->width < y->width ? -1 : x->width > y->width;
	}
	return x->bits_per_pixel < y->bits_per_pixel ? -1 : +1;
}

CM_COMPARE_PROC(cm_video_mode_dsc_cmp) {
	return cm_video_mode_cmp(b, a);
}

#endif // CM_PLATFORM

CM_END_EXTERN