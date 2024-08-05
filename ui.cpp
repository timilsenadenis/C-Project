#include <windows.h>
#include <string>

// Global variables
HINSTANCE hInst;
HWND hwndTextArea, hwndRunButton, hwndOutput;
HBRUSH hbrBackground, hbrOutputBackground;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "InterpreterClass";

    RegisterClass(&wc);

    hInst = hInstance;

    HWND hwnd = CreateWindowEx(
        0,
        "InterpreterClass",
        "DENIS's INTERPRETER",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 650, // Initial window size
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        hwndTextArea = CreateWindowEx(
            WS_EX_CLIENTEDGE, "EDIT", "",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
            20, 20, 740, 400, // Initial coding area size with margins
            hwnd, NULL, hInst, NULL);

        hwndRunButton = CreateWindow(
            "BUTTON", "RUN",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            680, 430, 90, 30,
            hwnd, (HMENU)1, hInst, NULL);

        hwndOutput = CreateWindowEx(
            WS_EX_CLIENTEDGE, "STATIC", "",
            WS_CHILD | WS_VISIBLE,
            20, 470, 740, 100, // Initial output area size with margins
            hwnd, NULL, hInst, NULL);

        // Create a light background brush for the coding area
        hbrBackground = CreateSolidBrush(RGB(245, 235, 225)); // Light grey color
        hbrOutputBackground = CreateSolidBrush(RGB(230, 230, 250)); // Light lavender color
        break;
    }
    case WM_SIZE: {
        if (wParam != SIZE_MINIMIZED) {
            // Get new dimensions of the window
            RECT rect;
            GetClientRect(hwnd, &rect);

            // Adjust dimensions of the child windows with margins
            MoveWindow(hwndTextArea, 20, 20, rect.right - 40, rect.bottom - 190, TRUE);
            MoveWindow(hwndRunButton, rect.right - 110, rect.bottom - 150, 90, 30, TRUE);
            MoveWindow(hwndOutput, 20, rect.bottom - 120, rect.right - 40, 100, TRUE);
        }
        break;
    }
    case WM_CTLCOLOREDIT: {
        HDC hdcEdit = (HDC)wParam;
        SetTextColor(hdcEdit, RGB(0, 0, 0)); // White text
        SetBkMode(hdcEdit, TRANSPARENT);
        return (INT_PTR)hbrBackground; // Return light grey background for the coding area
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, RGB(0, 0, 0)); // White text
        SetBkMode(hdcStatic, TRANSPARENT);
        if ((HWND)lParam == hwndOutput) {
            return (INT_PTR)hbrOutputBackground; // Light lavender background for output area
        }
        return (INT_PTR)GetStockObject(WHITE_BRUSH);
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == 1) {
            char code[1024];
            GetWindowText(hwndTextArea, code, 1024);

           // Here you would call your interpreter and execute the code
            // For simplicity, we'll just display the input code in the output area

            SetWindowText(hwndOutput, code);
        }
        break;
    }
    case WM_DESTROY: {
        DeleteObject(hbrBackground);
        DeleteObject(hbrOutputBackground);
        PostQuitMessage(0);
        break;
    }
    default: {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    }
    return 0;
}
//cd "c:\Users\user\Documents\c and c++\" ; if ($?) { g++ ui.cpp -o ui -lgdi32 -luser32 } ; if ($?) { .\ui }
