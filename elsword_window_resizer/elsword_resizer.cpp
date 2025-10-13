#include <windows.h>
#include <vector>
#include <cwchar>

// 컨트롤 ID 정의
enum {
    // 탭 및 공용 컨트롤
    ID_TAB_SIZE = 101,
    ID_TAB_POS = 102,
    ID_BUTTON_FIND = 201,
    ID_CHECK_TOPMOST = 202, // '항상 위로' 체크박스
    ID_STATUS_LABEL = 301,
    // 크기 조절 탭 컨트롤
    ID_BTN_RESIZE_1080 = 1001,
    ID_BTN_RESIZE_810 = 1002,
    // 위치 조절 탭 컨트롤
    ID_BTN_ALIGN_TOP = 2001,
    ID_BTN_ALIGN_MID = 2002,
    ID_BTN_ALIGN_BOT = 2003,
    ID_EDIT_NUDGE = 2004,
    ID_BTN_NUDGE_L = 2005,
    ID_BTN_NUDGE_U = 2006,
    ID_BTN_NUDGE_R = 2007,
    ID_BTN_NUDGE_D = 2008,
    ID_LABEL_NUDGE = 2009
};

// 전역 변수
HWND g_hGameWnd = NULL;
HFONT g_hFont = NULL;
HBRUSH g_hBrushBg, g_hBrushBtn, g_hBrushBtnActive, g_hBrushTabText;

HWND g_hStatusLabel;
std::vector<HWND> g_sizeControls;
std::vector<HWND> g_posControls;

// --- 함수 선언 ---
void UpdateStatus(const wchar_t* text);
void SwitchTab(HWND hWnd, int newTabId);

// --- 창 탐색 및 조작 로직 ---
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    wchar_t windowTitle[256];
    if (GetWindowTextW(hwnd, windowTitle, 256)) {
        if (wcsstr(windowTitle, L"[x64] Elsword -")) {
            g_hGameWnd = hwnd;
            return FALSE;
        }
    }
    return TRUE;
}

void FindGameWindow() {
    g_hGameWnd = NULL;
    EnumWindows(EnumWindowsProc, 0);
    if (g_hGameWnd) {
        wchar_t title[256];
        GetWindowTextW(g_hGameWnd, title, 256);
        wchar_t message[300];
        // BUG FIX: 유니코드 문자열 포맷 지정자를 %s 에서 %ls 로 변경하여 글자 깨짐 수정
        swprintf(message, 300, L"성공: '%ls' 창을 찾았습니다.", title);
        UpdateStatus(message);
    } else {
        UpdateStatus(L"오류: 엘소드 게임 창을 찾을 수 없습니다.");
    }
}

void ResizeGameWindow(int width, int height) {
    if (!g_hGameWnd) {
        UpdateStatus(L"오류: 먼저 게임 창을 감지해주세요.");
        return;
    }
    // UX FIX: SWP_NOACTIVATE 플래그를 추가하여 게임 창이 맨 위로 올라오지 않도록 함
    if (SetWindowPos(g_hGameWnd, NULL, 0, 0, width, height, SWP_NOZORDER | SWP_NOACTIVATE)) {
        wchar_t message[100];
        swprintf(message, 100, L"크기 변경: %d x %d 적용 완료.", width, height);
        UpdateStatus(message);
    } else {
        UpdateStatus(L"오류: 크기 변경에 실패했습니다.");
    }
}

void MoveGameWindow(int alignType) { // 0: Top, 1: Mid, 2: Bot
    if (!g_hGameWnd) {
        UpdateStatus(L"오류: 먼저 게임 창을 감지해주세요.");
        return;
    }
    RECT rect;
    GetWindowRect(g_hGameWnd, &rect);
    int windowHeight = rect.bottom - rect.top;
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int newY = 0;
    const wchar_t* alignStr = L"";
    if (alignType == 0) { newY = 0; alignStr = L"최상단"; }
    else if (alignType == 1) { newY = (screenHeight - windowHeight) / 2; alignStr = L"중앙"; }
    else { newY = screenHeight - windowHeight; alignStr = L"최하단"; }

    // UX FIX: SWP_NOACTIVATE 플래그 추가
    if (SetWindowPos(g_hGameWnd, NULL, rect.left, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE)) {
        wchar_t message[100];
        // BUG FIX: 유니코드 문자열 포맷 지정자를 %s 에서 %ls 로 변경
        swprintf(message, 100, L"위치 이동: %ls 정렬 완료.", alignStr);
        UpdateStatus(message);
    } else {
        UpdateStatus(L"오류: 위치 이동에 실패했습니다.");
    }
}

void NudgeGameWindow(HWND hWnd, int dx, int dy) {
    if (!g_hGameWnd) {
        UpdateStatus(L"오류: 먼저 게임 창을 감지해주세요.");
        return;
    }
    HWND hEdit = GetDlgItem(hWnd, ID_EDIT_NUDGE);
    wchar_t nudgeValStr[10];
    GetWindowTextW(hEdit, nudgeValStr, 10);
    int nudgeVal = _wtoi(nudgeValStr);
    if (nudgeVal <= 0) nudgeVal = 1;

    RECT rect;
    GetWindowRect(g_hGameWnd, &rect);
    int newX = rect.left + (dx * nudgeVal);
    int newY = rect.top + (dy * nudgeVal);

    // UX FIX: SWP_NOACTIVATE 플래그 추가
    if (SetWindowPos(g_hGameWnd, NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE)) {
        wchar_t message[100];
        swprintf(message, 100, L"미세 이동: (%+d, %+d)px 이동 완료.", dx * nudgeVal, dy * nudgeVal);
        UpdateStatus(message);
    } else {
        UpdateStatus(L"오류: 미세 이동에 실패했습니다.");
    }
}

// --- UI 관련 함수 ---
void UpdateStatus(const wchar_t* text) {
    SetWindowTextW(g_hStatusLabel, text);
}

void SwitchTab(HWND hWnd, int newTabId) {
    for (HWND hCtrl : g_sizeControls) ShowWindow(hCtrl, SW_HIDE);
    for (HWND hCtrl : g_posControls) ShowWindow(hCtrl, SW_HIDE);
    
    std::vector<HWND>& controlsToShow = (newTabId == ID_TAB_SIZE) ? g_sizeControls : g_posControls;
    for (HWND hCtrl : controlsToShow) ShowWindow(hCtrl, SW_SHOW);

    InvalidateRect(GetDlgItem(hWnd, ID_TAB_SIZE), NULL, TRUE);
    InvalidateRect(GetDlgItem(hWnd, ID_TAB_POS), NULL, TRUE);

    // BUG FIX: 탭 전환 시 컨트롤 잔상 제거를 위해 부모 윈도우 전체를 강제로 다시 그림
    InvalidateRect(hWnd, NULL, TRUE);
}

// --- 윈도우 프로시저 ---
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static int activeTab = ID_TAB_SIZE;
    switch (uMsg) {
        case WM_CREATE: {
            g_hBrushBg = CreateSolidBrush(RGB(240, 240, 240));
            g_hBrushBtn = CreateSolidBrush(RGB(220, 220, 220));
            g_hBrushBtnActive = CreateSolidBrush(RGB(0, 122, 255));
            g_hBrushTabText = CreateSolidBrush(RGB(200, 200, 200));
            g_hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

            CreateWindowW(L"BUTTON", L"크기 조절", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 10, 10, 100, 30, hWnd, (HMENU)ID_TAB_SIZE, NULL, NULL);
            CreateWindowW(L"BUTTON", L"위치 조절", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 115, 10, 100, 30, hWnd, (HMENU)ID_TAB_POS, NULL, NULL);
            CreateWindowW(L"BUTTON", L"게임 창 감지", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 10, 250, 325, 40, hWnd, (HMENU)ID_BUTTON_FIND, NULL, NULL);
            g_hStatusLabel = CreateWindowW(L"STATIC", L"상태: 프로그램을 시작했습니다.", WS_VISIBLE | WS_CHILD | SS_CENTER, 10, 335, 325, 20, hWnd, (HMENU)ID_STATUS_LABEL, NULL, NULL);
            CreateWindowW(L"BUTTON", L"항상 위로 고정", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 300, 150, 30, hWnd, (HMENU)ID_CHECK_TOPMOST, NULL, NULL);

            g_sizeControls.push_back(CreateWindowW(L"BUTTON", L"1920 x 1080 적용", WS_CHILD | BS_OWNERDRAW, 40, 80, 260, 40, hWnd, (HMENU)ID_BTN_RESIZE_1080, NULL, NULL));
            g_sizeControls.push_back(CreateWindowW(L"BUTTON", L"1920 x 810 적용", WS_CHILD | BS_OWNERDRAW, 40, 130, 260, 40, hWnd, (HMENU)ID_BTN_RESIZE_810, NULL, NULL));

            g_posControls.push_back(CreateWindowW(L"BUTTON", L"최상단", WS_CHILD | BS_OWNERDRAW, 10, 80, 100, 30, hWnd, (HMENU)ID_BTN_ALIGN_TOP, NULL, NULL));
            g_posControls.push_back(CreateWindowW(L"BUTTON", L"중앙", WS_CHILD | BS_OWNERDRAW, 120, 80, 100, 30, hWnd, (HMENU)ID_BTN_ALIGN_MID, NULL, NULL));
            g_posControls.push_back(CreateWindowW(L"BUTTON", L"최하단", WS_CHILD | BS_OWNERDRAW, 230, 80, 100, 30, hWnd, (HMENU)ID_BTN_ALIGN_BOT, NULL, NULL));
            g_posControls.push_back(CreateWindowW(L"STATIC", L"미세 이동(px):", WS_CHILD | SS_RIGHT, 10, 145, 100, 25, hWnd, (HMENU)ID_LABEL_NUDGE, NULL, NULL));
            g_posControls.push_back(CreateWindowW(L"EDIT", L"10", WS_CHILD | WS_BORDER | ES_NUMBER | ES_CENTER, 120, 140, 50, 25, hWnd, (HMENU)ID_EDIT_NUDGE, NULL, NULL));
            g_posControls.push_back(CreateWindowW(L"BUTTON", L"←", WS_CHILD | BS_OWNERDRAW, 190, 160, 40, 40, hWnd, (HMENU)ID_BTN_NUDGE_L, NULL, NULL));
            g_posControls.push_back(CreateWindowW(L"BUTTON", L"↑", WS_CHILD | BS_OWNERDRAW, 240, 120, 40, 40, hWnd, (HMENU)ID_BTN_NUDGE_U, NULL, NULL));
            g_posControls.push_back(CreateWindowW(L"BUTTON", L"→", WS_CHILD | BS_OWNERDRAW, 290, 160, 40, 40, hWnd, (HMENU)ID_BTN_NUDGE_R, NULL, NULL));
            g_posControls.push_back(CreateWindowW(L"BUTTON", L"↓", WS_CHILD | BS_OWNERDRAW, 240, 160, 40, 40, hWnd, (HMENU)ID_BTN_NUDGE_D, NULL, NULL));

            EnumChildWindows(hWnd, [](HWND hChild, LPARAM lParam) -> BOOL { SendMessage(hChild, WM_SETFONT, (WPARAM)lParam, TRUE); return TRUE; }, (LPARAM)g_hFont);
            SwitchTab(hWnd, activeTab);
            break;
        }
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            if (HIWORD(wParam) == BN_CLICKED) {
                switch (wmId) {
                    case ID_TAB_SIZE: case ID_TAB_POS: activeTab = wmId; SwitchTab(hWnd, activeTab); break;
                    case ID_BUTTON_FIND: FindGameWindow(); break;
                    case ID_BTN_RESIZE_1080: ResizeGameWindow(1920, 1080); break;
                    case ID_BTN_RESIZE_810: ResizeGameWindow(1920, 810); break;
                    case ID_BTN_ALIGN_TOP: MoveGameWindow(0); break;
                    case ID_BTN_ALIGN_MID: MoveGameWindow(1); break;
                    case ID_BTN_ALIGN_BOT: MoveGameWindow(2); break;
                    case ID_BTN_NUDGE_L: NudgeGameWindow(hWnd, -1, 0); break;
                    case ID_BTN_NUDGE_R: NudgeGameWindow(hWnd, 1, 0); break;
                    case ID_BTN_NUDGE_U: NudgeGameWindow(hWnd, 0, -1); break;
                    case ID_BTN_NUDGE_D: NudgeGameWindow(hWnd, 0, 1); break;
                    case ID_CHECK_TOPMOST: {
                        BOOL checked = IsDlgButtonChecked(hWnd, ID_CHECK_TOPMOST);
                        SetWindowPos(hWnd, checked ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
                        UpdateStatus(checked ? L"'항상 위로'가 활성화되었습니다." : L"'항상 위로'가 비활성화되었습니다.");
                        break;
                    }
                }
            }
            break;
        }
        case WM_DRAWITEM: {
            DRAWITEMSTRUCT* pdis = (DRAWITEMSTRUCT*)lParam;
            if (pdis->CtlType == ODT_BUTTON) {
                wchar_t text[100]; GetWindowTextW(pdis->hwndItem, text, 100);
                bool isTab = (pdis->CtlID == ID_TAB_SIZE || pdis->CtlID == ID_TAB_POS), isActive = (isTab && pdis->CtlID == activeTab), isFindBtn = (pdis->CtlID == ID_BUTTON_FIND);
                HBRUSH hBrush = isFindBtn ? g_hBrushBtnActive : (isTab ? (isActive ? g_hBrushTabText : g_hBrushBtn) : g_hBrushBtn);
                FillRect(pdis->hDC, &pdis->rcItem, hBrush);
                if (isFindBtn) SetTextColor(pdis->hDC, RGB(255, 255, 255)); else if (isActive) SetTextColor(pdis->hDC, RGB(50, 50, 50)); else SetTextColor(pdis->hDC, RGB(100, 100, 100));
                SetBkMode(pdis->hDC, TRANSPARENT);
                DrawTextW(pdis->hDC, text, -1, &pdis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
            return TRUE;
        }
        case WM_CTLCOLORSTATIC: case WM_CTLCOLORBTN: {
            HDC hdc = (HDC)wParam;
            SetBkColor(hdc, RGB(240, 240, 240));
            return (INT_PTR)g_hBrushBg;
        }
        case WM_DESTROY: {
            DeleteObject(g_hFont); DeleteObject(g_hBrushBg); DeleteObject(g_hBrushBtn); DeleteObject(g_hBrushBtnActive); DeleteObject(g_hBrushTabText);
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

// --- 프로그램 시작점 ---
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"Modern Elsword Resizer";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc; wc.hInstance = hInstance; wc.lpszClassName = CLASS_NAME; wc.hbrBackground = CreateSolidBrush(RGB(240, 240, 240)); wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassW(&wc);

    // FEATURE: 프로그램 실행 시 화면 중앙에 위치하도록 좌표 계산
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth = 360, windowHeight = 420; // 창 크기 업데이트
    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;

    HWND hWnd = CreateWindowExW(0, CLASS_NAME, L"엘소드 해상도/위치 조절기", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        x, y, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

    if (!hWnd) return 0;
    ShowWindow(hWnd, nCmdShow);
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    return 0;
}