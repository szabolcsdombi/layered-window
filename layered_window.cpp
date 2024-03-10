#include <Python.h>
#include <Windows.h>
#include <Dwmapi.h>

POINT zero = {0, 0};
BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

const char * title = "layered_window";
int disable_move;
int always_on_top;
int tool_window;
int window_flags;
SIZE window_size;
POINT window_pos;
HDC screen_hdc;
HWND hwnd;
HDC hdc;
HBITMAP hbmp;
HDC mem_hdc;
void * bits;
HANDLE ready;

LRESULT CALLBACK CustomWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_MOVE: {
            if (!disable_move) {
                window_pos.x = LOWORD(lParam);
                window_pos.y = HIWORD(lParam);
            }
            break;
        }
        case WM_NCHITTEST: {
            LRESULT hit = DefWindowProc(hWnd, uMsg, wParam, lParam);
            if (!disable_move && hit == HTCLIENT) {
                return HTCAPTION;
            }
            return hit;
        }
        case WM_CLOSE: {
            exit(0);
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void window_thread() {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    HINSTANCE hinst = GetModuleHandle(NULL);
    WNDCLASS wc = {0, CustomWindowProc, 0, 0, hinst, NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, "layered_window"};
    RegisterClass(&wc);

    hwnd = CreateWindowEx(window_flags, "layered_window", title, WS_POPUP | WS_VISIBLE, window_pos.x, window_pos.y, 0, 0, NULL, NULL, hinst, NULL);

    screen_hdc = GetDC(NULL);
    BITMAPINFO bmi = {{sizeof(BITMAPINFOHEADER), window_size.cx, window_size.cy, 1, 32, BI_RGB, 0, 0, 0, 0, 0}};
    hbmp = CreateDIBSection(screen_hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    mem_hdc = CreateCompatibleDC(screen_hdc);
    SelectObject(mem_hdc, hbmp);

    SetEvent(ready);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

static PyObject * meth_init(PyObject * self, PyObject * args, PyObject * kwargs) {
    const char * keywords[] = {"size", "position", "disable_move", "always_on_top", "tool_window", "title", NULL};

    int args_ok = PyArg_ParseTupleAndKeywords(
        args, kwargs, "(ii)|(ii)ppps", (char **)keywords,
        &window_size.cx, &window_size.cy, &window_pos.x, &window_pos.y,
        &disable_move, &always_on_top, &tool_window,
        &title
    );

    if (!args_ok) {
        return NULL;
    }

    window_flags = WS_EX_LAYERED;
    if (always_on_top) {
        window_flags |= WS_EX_TOPMOST;
    }
    if (tool_window) {
        window_flags |= WS_EX_TOOLWINDOW;
    }

    ready = CreateEvent(NULL, FALSE, FALSE, NULL);
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)window_thread, NULL, 0, NULL);
    WaitForSingleObject(ready, INFINITE);

    return PyMemoryView_FromMemory((char *)bits, window_size.cx * window_size.cy * 4, PyBUF_WRITE);
}

static PyObject * meth_update(PyObject * self, PyObject * args, PyObject * kwargs) {
    const char * keywords[] = {"multiply_alpha", "flush", NULL};

    int multiply_alpha = true;
    int flush = true;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|pp", (char **)keywords, &multiply_alpha, &flush)) {
        return NULL;
    }

    if (multiply_alpha) {
        int count = window_size.cx * window_size.cy;
        unsigned char * ptr = (unsigned char *)bits;
        while (count--) {
            ptr[0] = ptr[0] * ptr[3] / 255;
            ptr[1] = ptr[1] * ptr[3] / 255;
            ptr[2] = ptr[2] * ptr[3] / 255;
            ptr += 4;
        }
    }

    UpdateLayeredWindow(hwnd, screen_hdc, &window_pos, &window_size, mem_hdc, &zero, 0, &blend, ULW_ALPHA);

    if (flush) {
        DwmFlush();
    }
    Py_RETURN_NONE;
}

static PyMethodDef module_methods[] = {
    {"init", (PyCFunction)meth_init, METH_VARARGS | METH_KEYWORDS},
    {"update", (PyCFunction)meth_update, METH_VARARGS | METH_KEYWORDS},
    {},
};

static PyModuleDef module_def = {PyModuleDef_HEAD_INIT, "layered_window", NULL, -1, module_methods};

extern "C" PyObject * PyInit_layered_window() {
    PyObject * module = PyModule_Create(&module_def);
    return module;
}
