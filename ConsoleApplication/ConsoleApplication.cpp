#include<windows.h>
#include<string.h>
#include <string> 

#define VK_D 68
#define VK_A 65
#define VK_W 87
#define VK_S 83

int WINDOW_WIDTH = 500;
int WINDOW_HIGH = 400;

HANDLE hBitmap;
BITMAP Bitmap;

wchar_t* inttostr(int num) {
	wchar_t* result = new wchar_t[10];
	swprintf(result, 10, L"%d", num);
	wprintf(L"[%ls]\n", result);
	return result;
}

class Sprite {
public:
	Sprite(int centerX, int centerY) {
		left = centerX - 25;
		top = centerY - 25;
	}
	void upSpeedX() {
		speedX += 1;
	}
	void downSpeedX() {
		speedX -= 1;
	}
	void upSpeedY() {
		speedY += 1;
	}
	void downSpeedY() {
		speedY -= 1;
	}
	void updateLocation(bool reduceSpeed) {
		if (left <= 0 && speedX <= 0 || left + width >= WINDOW_WIDTH && speedX >= 0) {
			speedX = speedX ? -speedX : (left <= 0 ? 5 : -5);
		}
		if (top <= 0 && speedY <= 0 || top + high >= WINDOW_HIGH && speedY >= 0) {
			speedY = speedY ? -speedY : (top <= 0 ? 5: -5);
		}
		if (reduceSpeed) {
			if (speedX != 0) {
				speedX += speedX > 0 ? -1 : 1;
			}
			if (speedY != 0) {
				speedY += speedY > 0 ? -1 : 1;
			}
		}
		left += speedX;
		top += speedY;
	}
	int getLeft() {
		return left;
	}
	int getTop() {
		return top;
	}
	int getRight() {
		return left + width;
	}
	int getBottom() {
		return top + high;
	}
	int getWidth() {
		return width;
	}
	int getHigh() {
		return high;
	}
private:
	int left, top, width = 40, high = 45;
	int speedX = 0, speedY = 0;
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

Sprite sprite = Sprite(WINDOW_WIDTH / 2, WINDOW_HIGH / 2);

static int tic = 0;

void UpdateSize(HWND hwnd) {
	RECT rect;
	GetClientRect(hwnd, &rect);
	WINDOW_WIDTH = rect.right;
	WINDOW_HIGH = rect.bottom;
}

VOID CALLBACK timer_proc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	tic++;
	if (tic == 20) {
		sprite.updateLocation(true);
		tic = 0;
	}
	else {
		sprite.updateLocation(false);
	}
	UpdateSize(hwnd);
	InvalidateRect(hwnd, NULL, true);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	HWND hWnd;
	MSG lpMsg;
	WNDCLASS w;

	w.lpszClassName = L"Имя программы"; //имя программы - объявлено выше
	w.hInstance = hInstance; //идентификатор текущего приложения
	w.lpfnWndProc = WndProc; //указатель на функцию окна
	w.hCursor = LoadCursor(NULL, IDC_ARROW); //загружаем курсор
	w.hIcon = 0;
	w.lpszMenuName = 0;
	w.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //цвет фона окна
	w.style = CS_HREDRAW | CS_VREDRAW;
	w.cbClsExtra = 0;
	w.cbWndExtra = 0;

	hBitmap = LoadImage(NULL, L"spr1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (!hBitmap) {
		MessageBox(NULL, L"File not found!", L"Error", MB_OK);
		return 0;
	}
	GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
	//Если не удалось зарегистрировать класс окна - выходим
	if (!RegisterClass(&w))
		return 0;

	//Создадим окно в памяти, заполнив аргументы CreateWindow
	hWnd = CreateWindow(L"Имя программы", //Имя программы
		L"Графические возможности Win32 API", //Заголовок окна
		WS_OVERLAPPEDWINDOW, //Стиль окна - перекрывающееся
		100, //положение окна на экране по х
		100, //положение по у
		WINDOW_WIDTH, //ширина
		WINDOW_HIGH, //высота
		(HWND)NULL, //идентификатор родительского окна
		(HMENU)NULL, //идентификатор меню
		(HINSTANCE)hInstance, //идентификатор экземпляра программы
		(HINSTANCE)NULL); //отсутствие дополнительных параметров

		//Выводим окно из памяти на экран
	ShowWindow(hWnd, nCmdShow);
	//Обновим содержимое окна
	UpdateWindow(hWnd);
	SetTimer(hWnd, 1, 20, &timer_proc);
	//Цикл обработки сообщений
	while (GetMessage(&lpMsg, NULL, 0, 0)) {
		TranslateMessage(&lpMsg);
		DispatchMessage(&lpMsg);
	}
	return(lpMsg.wParam);
}

//Функция окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, hCompatibleDC; //создаём контекст устройства
	PAINTSTRUCT ps; //создаём экземпляр структуры графического вывода
	HANDLE hOldBitmap;
	//Цикл обработки сообщений
	switch (messg)
	{
		//сообщение рисования
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		hCompatibleDC = CreateCompatibleDC(hdc);
		hOldBitmap = SelectObject(hCompatibleDC, hBitmap);
		StretchBlt(hdc, sprite.getLeft(), sprite.getTop(), sprite.getWidth(), sprite.getHigh(), hCompatibleDC, 13, 0, Bitmap.bmWidth - 25, Bitmap.bmHeight - 5, SRCCOPY);
		SelectObject(hCompatibleDC, hOldBitmap);

		ValidateRect(hWnd, NULL);
		EndPaint(hWnd, &ps);
		break;

		//сообщение выхода - разрушение окна
	case WM_DESTROY:
		PostQuitMessage(0); //Посылаем сообщение выхода с кодом 0 - нормальное завершение
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_D: // right
			sprite.upSpeedX();
			break;
		case VK_A: // left
			sprite.downSpeedX();
			break;
		case VK_W: // up
			sprite.downSpeedY();
			break;
		case VK_S: // down
			sprite.upSpeedY();
			break;
		default:
			break;
		}
		break;
	case WM_MOUSEWHEEL:
		if ((short)HIWORD(wParam) > 0) {
			wParam & MK_SHIFT ? sprite.upSpeedX() : sprite.downSpeedY();
		} else {
			wParam & MK_SHIFT ? sprite.downSpeedX() : sprite.upSpeedY();
		}
		break;
	default:
		return(DefWindowProc(hWnd, messg, wParam, lParam)); //освобождаем очередь приложения от нераспознаных
	}
	return 0;
}

