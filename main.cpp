#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include "menu.h"
#include <windowsx.h>
#include<stdio.h>
#include <sstream>
#include<ctime> //for seeding srand() for random values

typedef char * (_cdecl * externalSecretWord)(char*, int);

bool drawHangman(HWND hwnd, char* fileName);
char * generateSecretWord(char * WordArray, int length);

void LoadWBank();
char * GetWord();
void SelectWord();
int LetterFill (char, char []);

char FileContent[2000]; //The text file will be read into this
int NewLineCount = 0; // to count the number of words in text file
const int MaxL = 30; //The max length of a word
const int MaxW = 200; //Max words in the word bank
bool win = false;
externalSecretWord secretWord;

int CurrentW; //to store which word is in process
int UsedW[MaxW]; //to store all the SrNo of used words
int NoUsedW = 0; //to store no of used words
int NoTries = 7; //No of tries depends on the level selected
CHAR * UnWord;

char Letter; //user's guessed letter
int NoWrongGuess=0; //no of wrong guesses

class WBank //class to store all the words
{
public:
	char W[MaxL]; //the word
	WBank() //constructor
	{
		strcpy_s(W,"null");
	}
};
WBank Word[MaxW];

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (hThisInstance, MAKEINTRESOURCE(ID_IKONA));
    wincl.hIconSm = LoadIcon (hThisInstance, MAKEINTRESOURCE(ID_IKONA));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) CreateSolidBrush (RGB(33,49,74));

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Countries hangman"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           50,       /* Windows decides the position */
           50,       /* where the window ends up on the screen */
           900,                 /* The programs width */
           700,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           LoadMenu(hThisInstance, MAKEINTRESOURCE(IDR_MYMENU)),
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

void init(HWND hwnd){
    CreateWindow(
                 "BUTTON",
                 "Start Game",
                 WS_TABSTOP | WS_VISIBLE| WS_CHILD | BS_DEFPUSHBUTTON,
                 10,
                 10,
                 100,
                 80,
                 hwnd,
                 (HMENU) BUTTON_1,
                 (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE),
                 NULL
                 );
    CreateWindow(
                 "static",
                 "",
                 WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                 10,
                 150,
                 200,
                 50,
                 hwnd,
                 (HMENU) LABEL_1,
                 NULL,
                 NULL
                 );
    CreateWindow(
                 "static",
                 "Your guess:",
                 WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                 10,
                 210,
                 100,
                 50,
                 hwnd,
                 (HMENU) LABEL_2,
                 (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE),
                 NULL
                 );
    CreateWindow(
                 "EDIT",
                 "",
                 WS_VISIBLE| WS_CHILD | ES_LEFT | ES_AUTOVSCROLL | ES_MULTILINE | WS_BORDER,
                 10,
                 280,
                 30,
                 30,
                 hwnd,
                 (HMENU) EDIT_2,
                 NULL,
                 NULL
                 );
    CreateWindow(
                 "BUTTON",
                 "Enter",
                 WS_TABSTOP | WS_VISIBLE| WS_CHILD | BS_DEFPUSHBUTTON | WS_DISABLED ,
                 60,
                 280,
                 50,
                 30,
                 hwnd,
                 (HMENU) BUTTON_2,
                 (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE),
                 NULL
                 );
    CreateWindow(
                 "static",
                 "Tries left:",
                 WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                 400,
                 30,
                 80,
                 20,
                 hwnd,
                 (HMENU) LABEL_3,
                 (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE),
                 NULL
                 );
    CreateWindow(
                 "static",
                 "",
                 WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                 490,
                 30,
                 20,
                 20,
                 hwnd,
                 (HMENU) LABEL_4,
                 (HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE),
                 NULL
                 );


}
void LoadWBank() //to load the read file into our class WBank
{
	int j=0, k=0;

	for(int i=0; FileContent[i]!='\0'; i++)
	{
		if(FileContent[i] == '\n')
		{
			NewLineCount++;
			Word[j].W[k] = '\0';
			j++;
			k=0;
			continue;
		}

		Word[j].W[k] = FileContent[i];
		k++;
	}
	for (int l=0; l<j;l++){
        Word[j].W[sizeof(Word[j].W)] = '\0';
	}
	return;
}

void SelectWord() //Word Selection Engine
{
	/*generate a random int and compare it to all the previously
	selected words, so that the same word is not put up again*/
	srand(time(NULL));
	int i=0;

	int n = rand()% NewLineCount;
	for(i=0;i<NoUsedW;i++)
	{
		if(UsedW[i] == n)
		{
			i=0;
			GetWord();
		}
	}
	UsedW[NoUsedW] = n;
	NoUsedW++;
	CurrentW = n;
}

int LetterFill (char guess, char guessword[], HWND hwnd) //the function which checks each guess
{
	int i;
	int matches=0;
	printf("%c", guessword[1]);
	for (i = 0; Word[CurrentW].W[i]!='\0'; i++)
	{

		// Did we already match this letter in a previous guess?
		if (guess == guessword[i])
			return 0;

		// Is the guess in the secret word?
		if (guess == Word[CurrentW].W[i])
		{
			guessword[i] = guess;
			SetWindowText(GetDlgItem(hwnd, LABEL_1), (LPCSTR)guessword);
			matches++;
		}
	}
	return matches;
}

char* getSecretWord(char * WordArray, int length)
{
    return secretWord(WordArray, length);
}

char * GetWord()
{
	SelectWord();
	int length = strlen(Word[CurrentW].W);

	return getSecretWord(Word[CurrentW].W, length);
}

/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{


    HINSTANCE hinstLib;
    int index = 0;
    CHAR* buffer;
    HANDLE file;
    DWORD b_read;
    BOOL rez;

    switch (message)                  /* handle the messages */
    {

        case WM_CREATE:
            init(hwnd);
            break;
        case WM_COMMAND:
            switch(LOWORD(wParam)){
                case BUTTON_1:
                    NoTries = 7;
                    NoWrongGuess = 0;
                    EnableWindow(GetDlgItem(hwnd, BUTTON_2), TRUE);
                    index = Edit_GetTextLength(GetDlgItem(hwnd, EDIT_1)) + 1;
                    buffer = new CHAR[index];
                    Edit_GetText(GetDlgItem(hwnd, EDIT_1), buffer, index);
                    SetWindowText(hwnd, buffer);

                    //DLL
                    hinstLib = LoadLibrary("gameDll\\bin\\debug\\gameDll.dll");
                    if(hinstLib != NULL){
                        secretWord = (externalSecretWord)GetProcAddress(hinstLib, "generateSecretWord");

                        if (secretWord == NULL){
                            MessageBox(hwnd, "Something is wrong with the dll function", "DLL", MB_OK);
                        }

                    }

                    //READ FILE
                    file = CreateFile("Words.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING,
                                      FILE_ATTRIBUTE_NORMAL, NULL);

                    if(file == INVALID_HANDLE_VALUE){
                        MessageBox(hwnd, "Something is wrong with the file", "File", MB_OK);
                    }

                    b_read = 0;
                    rez = ReadFile(file, FileContent, 2048, &b_read, NULL);

                    if(rez){
                        LoadWBank();

                        UnWord = GetWord();


                        SetWindowText(GetDlgItem(hwnd, LABEL_1), (LPCSTR)UnWord);
                        SetWindowText(GetDlgItem(hwnd, LABEL_4), "7");
                        drawHangman(hwnd, "start.bmp");
                        LetterFill('\n\r', UnWord, hwnd);
                    }
                    else{
                        MessageBox(hwnd, "Text was not read", "READ", MB_OK);
                    }
                    CloseHandle(file);
                    break;

               case BUTTON_2:

                    index = Edit_GetTextLength(GetDlgItem(hwnd, EDIT_2)) + 1;
                    buffer = new CHAR[index];
                    Edit_GetText(GetDlgItem(hwnd, EDIT_2), buffer, index);


                    if (NoWrongGuess < NoTries)
                    {
                        Letter = buffer[0];

                        if (LetterFill(Letter, UnWord, hwnd)==0) //if user provides wrong guess
                        {
                            NoWrongGuess++;
                            switch(NoWrongGuess){
                            case 1:
                                drawHangman(hwnd, "1.bmp");
                                break;
                            case 2:
                                drawHangman(hwnd, "2.bmp");
                                break;
                            case 3:
                                drawHangman(hwnd, "3.bmp");
                                break;
                            case 4:
                                drawHangman(hwnd, "4.bmp");
                                break;
                            case 5:
                                drawHangman(hwnd, "5.bmp");
                                break;
                            case 6:
                                drawHangman(hwnd, "6.bmp");
                                break;
                            case 7:
                                drawHangman(hwnd, "7.bmp");
                                break;
                            }
                            int tries = NoTries-NoWrongGuess;
                            std::basic_stringstream<TCHAR> ss;
                            ss << tries;
                            SetWindowText(GetDlgItem(hwnd, LABEL_4), (LPCSTR)ss.str().c_str());
                        }

                        if (strcmp(Word[CurrentW].W, UnWord) == 0) //checking if he guessed the word
                        {
                            win = true;
                            const char * theWord = "Congrats! The word was: " ;
                            MessageBox(hwnd,Word[CurrentW].W,theWord, MB_OK);
                            break;
                        }
                    }

                    if(NoWrongGuess == NoTries) //condition for user to lose
                    {
                        const char * theWord = "Sorry, you lose...you've been hanged.. The word was: " ;
                        MessageBox(hwnd,Word[CurrentW].W,theWord, MB_OK);
                    }

                    break;

                case ID_FILE_EXIT:
                    SendMessage(hwnd, WM_CLOSE, 0, 0);
                    break;
            }
            break;
        case WM_DESTROY:
            FreeLibrary(hinstLib);
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

bool drawHangman(HWND hwnd, char* fileName){
    HDC hDC;
    hDC = GetDC(hwnd);
    if(hDC == NULL){
        MessageBox(hwnd, "Nera DC", "Klaida", MB_OK);
        return false;
    }
    HANDLE hBmp = LoadImage(NULL, fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if(hBmp == NULL){
        MessageBox(hwnd, "Nera paveikslelio", "Klaida", MB_OK);
        return false;
    }

    HDC dcmem = CreateCompatibleDC(NULL);
    if (SelectObject(dcmem, hBmp) == NULL){
        MessageBox(hwnd, "Nepavyko ikrauti", "Klaida", MB_OK);
        DeleteDC(dcmem);
        return false;
    }
    BITMAP bm;
    GetObject(hBmp, sizeof(bm), &bm);
    if(BitBlt(hDC, 400, 100, bm.bmWidth, bm.bmHeight, dcmem,0, 0, SRCCOPY) ==0){
        MessageBox(hwnd, "Nepavyko ikrauti bmp", "Klaida", MB_OK);
        DeleteDC(dcmem);
        return false;
    }
    DeleteDC(hDC);
}



