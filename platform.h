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

#ifndef CM_PLATFORM_H
#define CM_PLATFORM_H

#include "memory.h"

CM_BEGIN_EXTERN

/////////////////////////////////////////////////////////////////////////////////
//
// Platform Stuff
//
/////////////////////////////////////////////////////////////////////////////////


#if defined(CM_PLATFORM)

// NOTE(bill):
// Coordiate system - +ve x - left to right
//                  - +ve y - bottom to top
//                  - Relative to window

// TODO(bill): Proper documentation for this with code examples

// Window Support - Complete
// OS X Support - Missing:
//     * Sofware framebuffer
//     * (show|hide) window
//     * show_cursor
//     * toggle (fullscreen|borderless)
//     * set window position
//     * Clipboard
//     * GameControllers
// Linux Support - None
// Other OS Support - None

#ifndef CM_MAX_GAME_CONTROLLER_COUNT
#define CM_MAX_GAME_CONTROLLER_COUNT 4
#endif

typedef enum cmKeyType {
	cmKey_Unknown = 0,  // Unhandled key

	// NOTE(bill): Allow the basic printable keys to be aliased with their chars
	cmKey_0 = '0',
	cmKey_1,
	cmKey_2,
	cmKey_3,
	cmKey_4,
	cmKey_5,
	cmKey_6,
	cmKey_7,
	cmKey_8,
	cmKey_9,

	cmKey_A = 'A',
	cmKey_B,
	cmKey_C,
	cmKey_D,
	cmKey_E,
	cmKey_F,
	cmKey_G,
	cmKey_H,
	cmKey_I,
	cmKey_J,
	cmKey_K,
	cmKey_L,
	cmKey_M,
	cmKey_N,
	cmKey_O,
	cmKey_P,
	cmKey_Q,
	cmKey_R,
	cmKey_S,
	cmKey_T,
	cmKey_U,
	cmKey_V,
	cmKey_W,
	cmKey_X,
	cmKey_Y,
	cmKey_Z,

	cmKey_Lbracket  = '[',
	cmKey_Rbracket  = ']',
	cmKey_Semicolon = ';',
	cmKey_Comma     = ',',
	cmKey_Period    = '.',
	cmKey_Quote     = '\'',
	cmKey_Slash     = '/',
	cmKey_Backslash = '\\',
	cmKey_Grave     = '`',
	cmKey_Equals    = '=',
	cmKey_Minus     = '-',
	cmKey_Space     = ' ',

	cmKey__Pad = 128,   // NOTE(bill): make sure ASCII is reserved

	cmKey_Escape,       // Escape
	cmKey_Lcontrol,     // Left Control
	cmKey_Lshift,       // Left Shift
	cmKey_Lalt,         // Left Alt
	cmKey_Lsystem,      // Left OS specific: window (Windows and Linux), apple/cmd (MacOS X), ...
	cmKey_Rcontrol,     // Right Control
	cmKey_Rshift,       // Right Shift
	cmKey_Ralt,         // Right Alt
	cmKey_Rsystem,      // Right OS specific: window (Windows and Linux), apple/cmd (MacOS X), ...
	cmKey_Menu,         // Menu
	cmKey_Return,       // Return
	cmKey_Backspace,    // Backspace
	cmKey_Tab,          // Tabulation
	cmKey_Pageup,       // Page up
	cmKey_Pagedown,     // Page down
	cmKey_End,          // End
	cmKey_Home,         // Home
	cmKey_Insert,       // Insert
	cmKey_Delete,       // Delete
	cmKey_Plus,         // +
	cmKey_Subtract,     // -
	cmKey_Multiply,     // *
	cmKey_Divide,       // /
	cmKey_Left,         // Left arrow
	cmKey_Right,        // Right arrow
	cmKey_Up,           // Up arrow
	cmKey_Down,         // Down arrow
	cmKey_Numpad0,      // Numpad 0
	cmKey_Numpad1,      // Numpad 1
	cmKey_Numpad2,      // Numpad 2
	cmKey_Numpad3,      // Numpad 3
	cmKey_Numpad4,      // Numpad 4
	cmKey_Numpad5,      // Numpad 5
	cmKey_Numpad6,      // Numpad 6
	cmKey_Numpad7,      // Numpad 7
	cmKey_Numpad8,      // Numpad 8
	cmKey_Numpad9,      // Numpad 9
	cmKey_NumpadDot,    // Numpad .
	cmKey_NumpadEnter,  // Numpad Enter
	cmKey_F1,           // F1
	cmKey_F2,           // F2
	cmKey_F3,           // F3
	cmKey_F4,           // F4
	cmKey_F5,           // F5
	cmKey_F6,           // F6
	cmKey_F7,           // F7
	cmKey_F8,           // F8
	cmKey_F9,           // F8
	cmKey_F10,          // F10
	cmKey_F11,          // F11
	cmKey_F12,          // F12
	cmKey_F13,          // F13
	cmKey_F14,          // F14
	cmKey_F15,          // F15
	cmKey_Pause,        // Pause

	cmKey_Count,
} cmKeyType;


/* TODO(bill): Change name? */
typedef u8 cmKeyState;
typedef enum cmKeyStateFlag {
	cmKeyState_Down     = CM_BIT(0),
	cmKeyState_Pressed  = CM_BIT(1),
	cmKeyState_Released = CM_BIT(2)
} cmKeyStateFlag;

CM_DEF void cm_key_state_update(cmKeyState *s, b32 is_down);

typedef enum cmMouseButtonType {
	cmMouseButton_Left,
	cmMouseButton_Middle,
	cmMouseButton_Right,
	cmMouseButton_X1,
	cmMouseButton_X2,

	cmMouseButton_Count
} cmMouseButtonType;

typedef enum cmControllerAxisType {
	cmControllerAxis_LeftX,
	cmControllerAxis_LeftY,
	cmControllerAxis_RightX,
	cmControllerAxis_RightY,
	cmControllerAxis_LeftTrigger,
	cmControllerAxis_RightTrigger,

	cmControllerAxis_Count
} cmControllerAxisType;



typedef enum cmControllerButtonType {
	cmControllerButton_Up,
	cmControllerButton_Down,
	cmControllerButton_Left,
	cmControllerButton_Right,
	cmControllerButton_A,
	cmControllerButton_B,
	cmControllerButton_X,
	cmControllerButton_Y,
	cmControllerButton_LeftShoulder,
	cmControllerButton_RightShoulder,
	cmControllerButton_Back,
	cmControllerButton_Start,
	cmControllerButton_LeftThumb,
	cmControllerButton_RightThumb,

	cmControllerButton_Count
} cmControllerButtonType;

typedef struct cmGameController {
	b16 is_connected, is_analog;

	f32        axes[cmControllerAxis_Count];
	cmKeyState buttons[cmControllerButton_Count];
} cmGameController;

#if defined(CM_SYS_WINDOWS)
	typedef struct _XINPUT_GAMEPAD XINPUT_GAMEPAD;
	typedef struct _XINPUT_STATE   XINPUT_STATE;
	typedef struct _XINPUT_VIBRATION XINPUT_VIBRATION;

	#define CM_XINPUT_GET_STATE(name) unsigned long __stdcall name(unsigned long dwUserIndex, XINPUT_STATE *pState)
	typedef CM_XINPUT_GET_STATE(cmXInputGetStateProc);

	#define CM_XINPUT_SET_STATE(name) unsigned long __stdcall name(unsigned long dwUserIndex, XINPUT_VIBRATION *pVibration)
	typedef CM_XINPUT_SET_STATE(cmXInputSetStateProc);
#endif


typedef enum cmWindowFlag {
	cmWindow_Fullscreen        = CM_BIT(0),
	cmWindow_Hidden            = CM_BIT(1),
	cmWindow_Borderless        = CM_BIT(2),
	cmWindow_Resizable         = CM_BIT(3),
	cmWindow_Minimized         = CM_BIT(4),
	cmWindow_Maximized         = CM_BIT(5),
	cmWindow_FullscreenDesktop = cmWindow_Fullscreen | cmWindow_Borderless,
} gbWindowFlag;

typedef enum cmRendererType {
	cmRenderer_Opengl,
	cmRenderer_Software,

	cmRenderer_Count,
} cmRendererType;

#if defined(CM_SYS_WINDOWS) && !defined(_WINDOWS_)
typedef struct tagBITMAPINFOHEADER {
	unsigned long biSize;
	long          biWidth;
	long          biHeight;
	u16           biPlanes;
	u16           biBitCount;
	unsigned long biCompression;
	unsigned long biSizeImage;
	long          biXPelsPerMeter;
	long          biYPelsPerMeter;
	unsigned long biClrUsed;
	unsigned long biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
typedef struct tagRGBQUAD {
	u8 rgbBlue;
	u8 rgbGreen;
	u8 rgbRed;
	u8 rgbReserved;
} RGBQUAD;
typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD          bmiColors[1];
} BITMAPINFO, *PBITMAPINFO;
#endif

typedef struct cmPlatform {
	b32 is_initialized;

	void *window_handle;
	i32   window_x, window_y;
	i32   window_width, window_height;
	u32   window_flags;
	b16   window_is_closed, window_has_focus;

#if defined(CM_SYS_WINDOWS)
	void *win32_dc;
#elif defined(CM_SYS_OSX)
	void *osx_autorelease_pool; // TODO(bill): Is this really needed?
#endif

	cmRendererType renderer_type;
	union {
		struct {
			void *      context;
			i32         major;
			i32         minor;
			b16         core, compatible;
			cmDllHandle dll_handle;
		} opengl;

		// NOTE(bill): Software rendering
		struct {
#if defined(CM_SYS_WINDOWS)
			BITMAPINFO win32_bmi;
#endif
			void *     memory;
			isize      memory_size;
			i32        pitch;
			i32        bits_per_pixel;
		} sw_framebuffer;
	};

	cmKeyState keys[cmKey_Count];
	struct {
		cmKeyState control;
		cmKeyState alt;
		cmKeyState shift;
	} key_modifiers;

	Rune  char_buffer[256];
	isize char_buffer_count;

	b32 mouse_clip;
	i32 mouse_x, mouse_y;
	i32 mouse_dx, mouse_dy; // NOTE(bill): Not raw mouse movement
	i32 mouse_raw_dx, mouse_raw_dy; // NOTE(bill): Raw mouse movement
	f32 mouse_wheel_delta;
	cmKeyState mouse_buttons[cmMouseButton_Count];

	cmGameController game_controllers[CM_MAX_GAME_CONTROLLER_COUNT];

	f64              curr_time;
	f64              dt_for_frame;
	b32              quit_requested;

#if defined(CM_SYS_WINDOWS)
	struct {
		cmXInputGetStateProc *get_state;
		cmXInputSetStateProc *set_state;
	} xinput;
#endif
} cmPlatform;

typedef struct cmVideoMode {
	i32 width, height;
	i32 bits_per_pixel;
} cmVideoMode;

#define CM_COMPARE_PROC(name) int name(void const *a, void const *b)
typedef CM_COMPARE_PROC(cmCompareProc);

CM_DEF cmVideoMode cm_video_mode                     (i32 width, i32 height, i32 bits_per_pixel);
CM_DEF b32         cm_video_mode_is_valid            (cmVideoMode mode);
CM_DEF cmVideoMode cm_video_mode_get_desktop         (void);
CM_DEF isize       cm_video_mode_get_fullscreen_modes(cmVideoMode *modes, isize max_mode_count); // NOTE(bill): returns mode count
CM_DEF CM_COMPARE_PROC(cm_video_mode_cmp);     // NOTE(bill): Sort smallest to largest (Ascending)
CM_DEF CM_COMPARE_PROC(cm_video_mode_dsc_cmp); // NOTE(bill): Sort largest to smallest (Descending)


// NOTE(bill): Software rendering
CM_DEF b32   cm_platform_init_with_software         (cmPlatform *p, char const *window_title, i32 width, i32 height, u32 window_flags);
// NOTE(bill): OpenGL Rendering
CM_DEF b32   cm_platform_init_with_opengl           (cmPlatform *p, char const *window_title, i32 width, i32 height, u32 window_flags, i32 major, i32 minor, b32 core, b32 compatible);
CM_DEF void  cm_platform_update                     (cmPlatform *p);
CM_DEF void  cm_platform_display                    (cmPlatform *p);
CM_DEF void  cm_platform_destroy                    (cmPlatform *p);
CM_DEF void  cm_platform_show_cursor                (cmPlatform *p, b32 show);
CM_DEF void  cm_platform_set_mouse_position         (cmPlatform *p, i32 x, i32 y);
CM_DEF void  cm_platform_set_controller_vibration   (cmPlatform *p, isize index, f32 left_motor, f32 right_motor);
CM_DEF b32   cm_platform_has_clipboard_text         (cmPlatform *p);
CM_DEF void  cm_platform_set_clipboard_text         (cmPlatform *p, char const *str);
CM_DEF char *cm_platform_get_clipboard_text         (cmPlatform *p, cmAllocator a);
CM_DEF void  cm_platform_set_window_position        (cmPlatform *p, i32 x, i32 y);
CM_DEF void  cm_platform_set_window_title           (cmPlatform *p, char const *title, ...) CM_PRINTF_ARGS(2);
CM_DEF void  cm_platform_toggle_fullscreen          (cmPlatform *p, b32 fullscreen_desktop);
CM_DEF void  cm_platform_toggle_borderless          (cmPlatform *p);
CM_DEF void  cm_platform_make_opengl_context_current(cmPlatform *p);
CM_DEF void  cm_platform_show_window                (cmPlatform *p);
CM_DEF void  cm_platform_hide_window                (cmPlatform *p);

#endif // GB_PLATFORM

CM_END_EXTERN

#endif //CM_PLATFORM_H