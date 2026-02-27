#include <windows.h>
#include <cstring>   // memset
#include <cstdio>    // sprintf
#include <cstdlib>   // strtod

// ---- IDs ----
#define ID_EDIT1 101
#define ID_EDIT2 102
#define ID_ADD   201
#define ID_SUB   202
#define ID_MUL   203
#define ID_DIV   204

static HWND hEdit1 = NULL;
static HWND hEdit2 = NULL;
static HBRUSH gBgBrush = CreateSolidBrush(RGB(200, 0, 120)); // สีพื้นหลังสด ๆ

/* This is where all the input to the window goes to */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message) {

        case WM_CREATE: {
            // ทำให้ window เปลี่ยนขนาดไม่ได้ + ไม่มีปุ่ม maximize
            LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
            style &= ~WS_THICKFRAME;
            style &= ~WS_MAXIMIZEBOX;
            SetWindowLongPtr(hwnd, GWL_STYLE, style);
            SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

            // Label
            CreateWindowEx(0, "STATIC", "Please input two numbers",
                WS_CHILD | WS_VISIBLE,
                20, 20, 210, 25,
                hwnd, NULL, GetModuleHandle(NULL), NULL);

            // Input 2 ช่อง
            hEdit1 = CreateWindowEx(0, "EDIT", "",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                50, 55, 150, 25,
                hwnd, (HMENU)ID_EDIT1, GetModuleHandle(NULL), NULL);

            hEdit2 = CreateWindowEx(0, "EDIT", "",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                50, 85, 150, 25,
                hwnd, (HMENU)ID_EDIT2, GetModuleHandle(NULL), NULL);

            // Buttons + - * /
            CreateWindow("BUTTON", "+", WS_CHILD | WS_VISIBLE,
                70, 130, 30, 25, hwnd, (HMENU)ID_ADD, GetModuleHandle(NULL), NULL);

            CreateWindow("BUTTON", "-", WS_CHILD | WS_VISIBLE,
                105, 130, 30, 25, hwnd, (HMENU)ID_SUB, GetModuleHandle(NULL), NULL);

            CreateWindow("BUTTON", "*", WS_CHILD | WS_VISIBLE,
                140, 130, 30, 25, hwnd, (HMENU)ID_MUL, GetModuleHandle(NULL), NULL);

            CreateWindow("BUTTON", "/", WS_CHILD | WS_VISIBLE,
                175, 130, 30, 25, hwnd, (HMENU)ID_DIV, GetModuleHandle(NULL), NULL);

            break;
        }

        // ล็อกขนาด window เป็น 250x200
        case WM_GETMINMAXINFO: {
            MINMAXINFO* mmi = (MINMAXINFO*)lParam;
            mmi->ptMinTrackSize.x = 250;
            mmi->ptMinTrackSize.y = 200;
            mmi->ptMaxTrackSize.x = 250;
            mmi->ptMaxTrackSize.y = 200;
            break;
        }

        // พื้นหลังสีสด ๆ
        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, gBgBrush);
            return 1;
        }

        // ทำให้ label/edit พื้นหลังขาว ตัวหนังสือดำ
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, RGB(0,0,0));
            SetBkColor(hdc, RGB(255,255,255));
            return (LRESULT)GetStockObject(WHITE_BRUSH);
        }

		case WM_COMMAND: {
            int id = LOWORD(wParam);
            int code = HIWORD(wParam);

            // สำคัญ: ทำงานเฉพาะตอน "คลิกปุ่ม" ไม่ใช่ตอนพิมพ์ใน edit
            if (code != BN_CLICKED) break;

            // อ่านค่าจาก Edit
            char s1[128], s2[128];
            GetWindowText(hEdit1, s1, sizeof(s1));
            GetWindowText(hEdit2, s2, sizeof(s2));

            char* e1 = NULL;
            char* e2 = NULL;
            double a = strtod(s1, &e1);
            double b = strtod(s2, &e2);

            // ถ้าไม่ใช่ตัวเลข
            if (e1 == s1 || e2 == s2) {
                MessageBox(hwnd, "Please input valid numbers.", "Result", MB_OK | MB_ICONWARNING);
                break;
            }

            double ans = 0.0;

            if (id == ID_ADD) ans = a + b;
            else if (id == ID_SUB) ans = a - b;
            else if (id == ID_MUL) ans = a * b;
            else if (id == ID_DIV) {
                if (b == 0.0) {
                    MessageBox(hwnd, "Cannot divide by zero.", "Result", MB_OK | MB_ICONERROR);
                    break;
                }
                ans = a / b;
            } else {
                break;
            }

            // แสดงผลลัพธ์ใน MessageBox ชื่อ Result (ตาม Hint)
            char t[100];
            sprintf(t, "%f", ans);
            MessageBox(hwnd, t, "Result", MB_OK);

			break;
		}

		/* Upon destruction, tell the main thread to stop */
		case WM_DESTROY: {
            if (gBgBrush) { DeleteObject(gBgBrush); gBgBrush = NULL; }
			PostQuitMessage(0);
			break;
		}

		/* All other messages (a lot of them) are processed using default procedures */
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG msg; /* A temporary location for all messages */

	/* zero out the struct and set the stuff we want to modify */
	memset(&wc,0,sizeof(wc));
	wc.cbSize	 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* This is where we will send messages to */
	wc.hInstance	 = hInstance;
	wc.hCursor	 = LoadCursor(NULL, IDC_ARROW);

	/* (พื้นหลังจริงเราเติมเองใน WM_ERASEBKGND แล้ว) */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "WindowClass";
	wc.hIcon	 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm	 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","My Calculator",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		250,
		200,
		NULL,NULL,hInstance,NULL);

	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	while(GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}