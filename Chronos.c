#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <string.h>
#include <mmsystem.h>
#include <time.h>
#include <uchar.h>
#include "resource.h"

int i = 0;
char tempPath[MAX_PATH];

typedef struct {
	char* lineas[10];
	int anchoConejo;
} Dibujo;

Dibujo* crearDibujo(int ancho) {
	Dibujo* dibujo = (Dibujo*)malloc(sizeof(Dibujo));
	dibujo->anchoConejo = ancho;
	return dibujo;
}

struct Dialogo {
    char16_t linea[10][132];
};

struct Despedida {
    char16_t lineaD[8][128];
};

struct Indicador {
	char pLinea[4][6];
};

void OcultarCursor() {
	HANDLE consola = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO infoCursor;
	GetConsoleCursorInfo(consola, &infoCursor);
	infoCursor.bVisible = FALSE;
	SetConsoleCursorInfo(consola, &infoCursor);
}

void OcultarPunteroRaton() {
    RECT rect;
    GetWindowRect(GetDesktopWindow(), &rect);
    rect.left = rect.right;
    ClipCursor(&rect);
}

void MostrarPunteroRaton() {
    ClipCursor(NULL);
}

void ModoPantallaCompleta() {
	HWND hwnd = GetConsoleWindow();
	DWORD estilo = GetWindowLong(hwnd, GWL_STYLE);
	SetWindowLong(hwnd, GWL_STYLE, estilo & ~WS_OVERLAPPEDWINDOW);
	ShowWindow(hwnd, SW_MAXIMIZE);
	
	CONSOLE_SCREEN_BUFFER_INFO info;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &info);
	
	COORD newSize;
	newSize.X = info.srWindow.Right - info.srWindow.Left + 1;
	newSize.Y = info.srWindow.Bottom - info.srWindow.Top + 1;
	
	SetConsoleScreenBufferSize(hConsole, newSize);
}

void obtenerSizeConsola(int* anchoConsola, int* altoConsola) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	*anchoConsola = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	*altoConsola = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void obtenerPosicionCursor(int *row, int *col) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    
    GetConsoleScreenBufferInfo(hStdout, &csbi);
    
    *col = csbi.dwCursorPosition.X + 1;
    *row = csbi.dwCursorPosition.Y + 1;
}

void ReproducirAudio(LPCSTR idAudio, int audioLoop) {
    HMODULE hModule = GetModuleHandle(NULL);
    HRSRC hResource = FindResource(hModule, idAudio, RT_RCDATA);
    HGLOBAL hLoadedResource = LoadResource(hModule, hResource);
    LPVOID pLockedResource = LockResource(hLoadedResource);
    DWORD resourceSize = SizeofResource(hModule, hResource);

    GetTempPath(MAX_PATH, tempPath);
    strcat(tempPath, "temp.wav");

    FILE *tempFile = fopen(tempPath, "wb");
    if (tempFile != NULL) {
        fwrite(pLockedResource, resourceSize, 1, tempFile);
        fclose(tempFile);

        if (audioLoop) {
            PlaySound(tempPath, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        } else {
            PlaySound(tempPath, NULL, SND_FILENAME | SND_ASYNC);
        }
    }
}

void DetenerAudio() {
    PlaySound(NULL, 0, 0);
    DeleteFile(tempPath);
}

void mostrarDibujo(Dibujo* dibujo, int lineaInicio) {
	int j;
	
	for (j = 0; j < lineaInicio; j++){
		printf("\x1b[B");
	}
	
    for (i = lineaInicio; i < 10; i++) {
        printf("%s", dibujo->lineas[i]);
        if (i < 9) {
            printf("\x1b[B");
            for (j = 0; j < dibujo->anchoConejo; j++) {
                printf("\x1b[D");
            }
        }
        fflush(stdout);
    }
    
    if (lineaInicio > 0){
    	for (j = 0; j < lineaInicio; j++){
    		printf ("\x1b[B");
		}
	}
    Sleep(150);

    for (i = lineaInicio; i < 10; i++) {
        printf("%*s", dibujo->anchoConejo, " ");
        if (i < 9) {
            printf("\x1b[B");
        } else {
        	if (lineaInicio == 0){
        		printf("\x1b[9A");	
			}
            else if (lineaInicio > 0){
            	for (j = 0; j < 9 - lineaInicio; j++){
            		printf("\x1b[A");
				}
			}
        }
        for (j = 0; j < dibujo->anchoConejo; j++) {
            printf("\x1b[D");
        }
        fflush(stdout);
    }
    
    for (j = 0; j < lineaInicio; j++){
		printf("\x1b[A");
	}
}

void animacionEntrada (Dibujo* conejo[7]){
	int rb, j, ocultaLinea = 12;
	ReproducirAudio("IDR_WAVE4", 0);
	for (rb = 1; rb < 5; rb++){
		for (i = 0; i < 9; i++){
			for (j = ocultaLinea; j <= conejo[rb]->anchoConejo; j++){
				printf("%c", conejo[rb]->lineas[i][j]);
			}
            printf("\x1b[E");
		}
		if (rb < 4){
			printf("\x1b[9F");
		}
		else if (rb == 4){
			printf("           `--`\x1b[9F");
		}
		fflush(stdout);
		
		Sleep (100);
		
		for (i = 0; i < 10; i++) {
        	printf("%*s", conejo[rb]->anchoConejo - ocultaLinea, " ");
        	if (i < 9) {
            	printf("\x1b[E");
        	} else {
            	printf("\x1b[9F");
        	}
        	fflush(stdout);
    	}
    	
    	if (rb == 1){
    		ocultaLinea -= 2;
		}
		else if (rb > 1 && rb < 4){
			ocultaLinea -= 4;
		}
	}
}

void animacionSalto(int *columnaFrame, Dibujo* conejo[7]) {
	int iniCol, saltoFrame = 0, iFrame;
	
	for (iFrame = 0; iFrame < 5; iFrame++){
		if (iFrame == 0){
			ReproducirAudio("IDR_WAVE4", 0);
		}
		for (iniCol = 0; iniCol < saltoFrame; iniCol++) {
			printf ("\x1b[C");
		}
		mostrarDibujo(conejo[iFrame], 0);
		fflush(stdout);
		if (iFrame < 2) {
			*columnaFrame += 2;
			saltoFrame = 2;
		}
		else if (iFrame > 1 && iFrame < 4) {
			*columnaFrame += 4;
			saltoFrame = 4;
		}
		else if (iFrame == 4){
			*columnaFrame += 3;
			saltoFrame = 3;
		}	
	}
	for (iniCol = 0; iniCol < saltoFrame; iniCol++) {
		printf ("\x1b[C");
	}
	mostrarDibujo(conejo[0], 0);
	fflush(stdout);
}

void dialogoConejo (){
	int row = 0, col = 0, sL, sC, anchoConsola, altoConsola;
	obtenerSizeConsola(&anchoConsola, &altoConsola);
	obtenerPosicionCursor(&row, &col);
	sL = (altoConsola - 32) / 2;
	sC = (anchoConsola - 131) / 2;
	
	struct Dialogo burbuja = {
        {
			{32, 220, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 220, 32, L'\0'},
	        {219, 32, 32, 32, 32, 220, 219, 32, 32, 32, 32, 219, 220, 32, 32, 32, 32, 32, 32, 32, 220, 219, 219, 219, 219, 219, 219, 219, 219, 32, 32, 32, 32, 220, 219, 219, 219, 219, 219, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 32, 220, 219, 219, 219, 219, 219, 219, 219, 219, 32, 32, 32, 32, 32, 32, 220, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 220, 32, 32, 32, 32, 32, 220, 219, 219, 219, 219, 219, 219, 219, 219, 32, 32, 32, 32, 220, 219, 219, 219, 219, 219, 219, 220, 32, 32, 32, 220, 219, 219, 219, 219, 219, 219, 220, 32, 32, 219, L'\0'},
    	    {219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 223, 219, 219, 219, 219, 219, 219, 219, 219, 219, 220, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 219, L'\0'},
        	{219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 223, 32, 32, 32, 32, 32, 223, 219, 219, 219, 223, 223, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 223, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 223, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 219, L'\0'},
			{219, 32, 32, 220, 219, 219, 219, 220, 220, 220, 220, 219, 219, 219, 220, 220, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 223, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 220, 219, 219, 219, 220, 220, 220, 32, 32, 32, 32, 32, 32, 220, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 219, L'\0'},
        	{219, 32, 223, 223, 219, 219, 219, 223, 223, 223, 223, 219, 219, 219, 223, 32, 32, 223, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 32, 223, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 223, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 223, 223, 219, 219, 219, 223, 223, 223, 32, 32, 32, 32, 32, 223, 223, 219, 219, 219, 32, 219, 219, 219, 219, 220, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 219, L'\0'},
	        {219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 220, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 219, L'\0'},
	        {219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 220, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 220, 32, 32, 32, 32, 220, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 219, L'\0'},
	        {219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 223, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 223, 32, 32, 32, 220, 219, 219, 219, 219, 219, 219, 219, 219, 223, 32, 32, 32, 32, 32, 220, 219, 219, 219, 219, 223, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 223, 32, 32, 32, 32, 32, 32, 219, 219, 219, 219, 219, 220, 220, 219, 219, 32, 219, 219, 219, 219, 219, 219, 219, 219, 223, 32, 32, 32, 32, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 32, 32, 32, 219, 219, 219, 219, 219, 219, 219, 219, 223, 32, 32, 32, 223, 219, 219, 219, 219, 219, 219, 223, 32, 32, 219, L'\0'},
    	    {32, 223, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 223, 32, L'\0'}
		}
    };
	
	struct Indicador puntero = {
        {
			{32, 32, 32, 32, 220, '\0'},
	        {223, 220, 32, 32, 219, '\0'},
    	    {32, 32, 223, 220, 219, '\0'},
        	{32, 32, 32, 32, 223, '\0'}
		} 
    };    
    
    printf ("\x1b[38;5;14m\x1b[H");
    for (i = 0; i < sL; i++) {
		printf ("\x1b[B");
	}
	for (i = 0; i < sC; i++) {
		printf ("\x1b[C");
	}
	
	printf ("\x1b[s");
	for (i = 0; i < 10; i++) {
		wprintf(L"%ls", burbuja.linea[i]);
		if (i < 9){
			printf ("\x1b[u\x1b[%dB", i + 1);
		}
		else if (i == 9){
			printf ("\x1b[%d;%dH", row, col);
		}
		fflush(stdout);		
	}
	
	printf ("\x1b[2A\x1b[8C");    
    for (i = 0; i < 4; i++){
    	printf("%s\x1b[B\x1b[5D", puntero.pLinea[i]);
		fflush(stdout);		
	}
	printf ("\x1b[8D\x1b[2A\x1b[0m");
}

void animacionWink (Dibujo* conejo[7]){
	int rbw, j, row = 0, col = 0;
	
	for (rbw = 5; rbw < 7; rbw++){
		mostrarDibujo(conejo[rbw], 0);
	}
	
	for (i = 0; i < 10; i++) {
        printf("%s", conejo[6]->lineas[i]);
        if (i < 9) {
            printf("\x1b[B");
            for (j = 0; j < conejo[6]->anchoConejo; j++) {
                printf("\x1b[D");
            }
        }
        fflush(stdout);
    }
    
    Sleep(20);
    dialogoConejo();
    ReproducirAudio("IDR_WAVE2", 0);
    printf ("\x1b[38;5;13m");
      
    Sleep(100);
    printf("\x1b[4B\x1b[11C_\x1b[D");
    Sleep(150);
    printf(",\x1b[4A\x1b[12D");
    Sleep(500);
	
	for (rbw = 6; rbw > -1; rbw--){
		mostrarDibujo(conejo[rbw], 2);
		if (rbw == 5){
			rbw -= 4;
		}
	}
	
	printf ("\x1b[2B");
	
	for (i = 2; i < 10; i++) {
        printf("%s", conejo[0]->lineas[i]);
        if (i < 9) {
            printf("\x1b[B");
            for (j = 0; j < conejo[0]->anchoConejo; j++) {
                printf("\x1b[D");
            }
        }
        fflush(stdout);
    }
	
    obtenerPosicionCursor(&row, &col);
    
    printf ("\x1b[u");
    for (i = 0; i < 10; i++) {
    	if (i == 0 || i == 9){
    		for (j = 0; j < 131; j++){
    			printf(" ");
			}
		}
		else if (i > 0 && i < 9){
			printf(" ");
			for (j = 0; j < 129; j++){
    			printf("\x1b[C");
			}
			printf(" ");
		}
		
		if (i < 9){
			printf ("\x1b[u\x1b[%dB", i + 1);
		}
		else if (i == 9){
			printf ("\x1b[%d;%dH", row, col);
		}		
	}
	
	printf ("\x1b[2A\x1b[8C");    
    for (i = 0; i < 4; i++){
    	printf("     \x1b[B\x1b[5D");		
	}
	printf ("\x1b[%d;%dH", row, col);	
}

void animacionSalida(Dibujo* conejo[7], int anchoConsola, int *columnaFrame){
	int rbOUT, j, ocultaLinea, row, col, saltoRB = 0;
	
	for (rbOUT = 0; *columnaFrame < anchoConsola; rbOUT++){
		obtenerPosicionCursor(&row, &col);
		
		if (rbOUT == 0){
			ReproducirAudio("IDR_WAVE4", 0);
		}
		
		if (anchoConsola - *columnaFrame < conejo[rbOUT]->anchoConejo){
			ocultaLinea = anchoConsola - *columnaFrame;
			
			for (i = 0; i < 10; i++){				
				for (j = 0; j < ocultaLinea; j++){
					printf("%c", conejo[rbOUT]->lineas[i][j]);
				}				
				if (i < 9){
					printf("\x1b[%d;%dH\x1b[%dB", row, col, i + 1);
				}
				else if (i == 9){
					printf ("\x1b[%d;%dH", row, col);
				}
				fflush(stdout);
			}
			
			Sleep (150);
			
			for (i = 0; i < 10; i++){				
				for (j = 0; j < ocultaLinea; j++){
					printf(" ");
				}				
				if (i < 9){
					printf("\x1b[%d;%dH\x1b[%dB", row, col, i + 1);
				}
				else if (i == 9){
					printf ("\x1b[%d;%dH", row, col);
				}
				fflush(stdout);
			}			
		}
		else {
			for (i = 0; i < 10; i++){
				printf("%s", conejo[rbOUT]->lineas[i]);
				if (i < 9){
					printf("\x1b[%d;%dH\x1b[%dB", row, col, i + 1);
				}
				else if (i == 9){
					printf ("\x1b[%d;%dH", row, col);
				}
				fflush(stdout);
			}
			
			Sleep (150);
			
			for (i = 0; i < 10; i++){				
				for (j = 0; j < conejo[rbOUT]->anchoConejo; j++){
					printf(" ");
				}				
				if (i < 9){
					printf("\x1b[%d;%dH\x1b[%dB", row, col, i + 1);
				}
				else if (i == 9){
					printf ("\x1b[%d;%dH", row, col);
				}
				fflush(stdout);
			}
		}
		
		if (rbOUT < 2) {
			*columnaFrame += 2;
			saltoRB = 2;
		}
		else if (rbOUT > 1 && rbOUT < 4) {
			*columnaFrame += 4;
			saltoRB = 4;
		}
		else if (rbOUT == 4){
			*columnaFrame += 3;
			saltoRB = 3;
			rbOUT = -1;
		}
		for (i = 0; i < saltoRB; i++) {
			printf ("\x1b[C");
		}
	}
}

void dimmHastaLuego(int anchoConsola, int altoConsola, int color){
	int saltoLinea = (altoConsola - 8) / 2;
	int espacioLateral = (anchoConsola - 127) / 2;
	
	struct Despedida hastaluego = {
        {
	        {32, 32, 32, 220, 219, 32, 32, 32, 32, 219, 220, 32, 32, 32, 32, 32, 32, 32, 220, 219, 219, 219, 219, 219, 219, 219, 219, 32, 32, 32, 32, 220, 219, 219, 219, 219, 219, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 32, 220, 219, 219, 219, 219, 219, 219, 219, 219, 32, 32, 32, 32, 32, 32, 220, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 220, 32, 32, 32, 32, 32, 220, 219, 219, 219, 219, 219, 219, 219, 219, 32, 32, 32, 32, 220, 219, 219, 219, 219, 219, 219, 220, 32, 32, 32, 220, 219, 219, 219, 219, 219, 219, 220, 32, L'\0'},
    	    {32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 223, 219, 219, 219, 219, 219, 219, 219, 219, 219, 220, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, L'\0'},
        	{32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 223, 32, 32, 32, 32, 32, 223, 219, 219, 219, 223, 223, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 223, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 223, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, L'\0'},
			{32, 220, 219, 219, 219, 220, 220, 220, 220, 219, 219, 219, 220, 220, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 223, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 220, 219, 219, 219, 220, 220, 220, 32, 32, 32, 32, 32, 32, 220, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, L'\0'},
        	{223, 223, 219, 219, 219, 223, 223, 223, 223, 219, 219, 219, 223, 32, 32, 223, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 32, 223, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 223, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 223, 223, 219, 219, 219, 223, 223, 223, 32, 32, 32, 32, 32, 223, 223, 219, 219, 219, 32, 219, 219, 219, 219, 220, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, L'\0'},
	        {32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 220, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, L'\0'},
	        {32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 220, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 32, 219, 219, 219, 220, 32, 32, 32, 32, 220, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, 32, 219, 219, 219, 32, 32, 32, 32, 219, 219, 219, L'\0'},
	        {32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 223, 32, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 223, 32, 32, 32, 220, 219, 219, 219, 219, 219, 219, 219, 219, 223, 32, 32, 32, 32, 32, 220, 219, 219, 219, 219, 223, 32, 32, 32, 32, 32, 219, 219, 219, 32, 32, 32, 32, 219, 223, 32, 32, 32, 32, 32, 32, 219, 219, 219, 219, 219, 220, 220, 219, 219, 32, 219, 219, 219, 219, 219, 219, 219, 219, 223, 32, 32, 32, 32, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 32, 32, 32, 219, 219, 219, 219, 219, 219, 219, 219, 223, 32, 32, 32, 223, 219, 219, 219, 219, 219, 219, 223, 32, L'\0'}
		}
    };
    
    
    for (i = 0; i < saltoLinea; i++){
    	printf("\x1b[B");
	}
	for (i = 0; i < espacioLateral; i++){
    	printf("\x1b[C");
	}
	printf ("\x1b[s\x1b[38;5;%dm", color);
	
	for (i = 0; i < 8; i++) {
		wprintf(L"%ls", hastaluego.lineaD[i]);
		if (i < 9){
			printf ("\x1b[u\x1b[%dB", i + 1);
		}
		else if (i == 9){
			printf ("\x1b[u\x1b[0m");
		}
		fflush(stdout);		
	}
	
	Sleep (50);
	system ("cls");
}

void animacionDespedida(int anchoConsola, int altoConsola) {
	int saltoLinea = (altoConsola - 10) / 2;
	int columnaCentro = anchoConsola / 2;
	int columnaFrame = 0;
	int colorD;
	
	Dibujo* conejo[7];
	conejo[0] = crearDibujo(16);
    conejo[1] = crearDibujo(14);
    conejo[2] = crearDibujo(14);
    conejo[3] = crearDibujo(19);
    conejo[4] = crearDibujo(19);
    conejo[5] = crearDibujo(16);
    conejo[6] = crearDibujo(16);
	
	conejo[0]->lineas[0] = strdup("                ");
	conejo[0]->lineas[1] = strdup("                ");
	conejo[0]->lineas[2] = strdup("         ,\\     ");
	conejo[0]->lineas[3] = strdup("         \\\\\\,_  ");
	conejo[0]->lineas[4] = strdup("          \\` ,\\ ");
	conejo[0]->lineas[5] = strdup("     __,.-\" =__)");
	conejo[0]->lineas[6] = strdup("   .\"        )  ");
	conejo[0]->lineas[7] = strdup(",_/   ,    \\/\\_ ");
	conejo[0]->lineas[8] = strdup("\\_(    )_-\\ \\_-`");
	conejo[0]->lineas[9] = strdup("   `-----` `--`\x1b[9A\x1b[15D");
	
    conejo[1]->lineas[0] = strdup("              ");
    conejo[1]->lineas[1] = strdup("    ,,,,,,,_  ");
    conejo[1]->lineas[2] = strdup("     `'\"\\` ,\\ ");
    conejo[1]->lineas[3] = strdup("       ,\" =__)");
    conejo[1]->lineas[4] = strdup("     _/   (   ");
    conejo[1]->lineas[5] = strdup("   .\"      )  ");
    conejo[1]->lineas[6] = strdup(",_/   ,  \\/\\  ");
    conejo[1]->lineas[7] = strdup("\\_(_, )-\\ \\_) ");
    conejo[1]->lineas[8] = strdup("  << <_  \\_)  ");
    conejo[1]->lineas[9] = strdup("   \\\\__)\x1b[9A\x1b[8D");
    
    conejo[2]->lineas[0] = strdup("       ,,,,_  " );
    conejo[2]->lineas[1] = strdup("      //\\` ,\\ ");
    conejo[2]->lineas[2] = strdup("     //,\" =__)");
    conejo[2]->lineas[3] = strdup("     '/    (  ");
    conejo[2]->lineas[4] = strdup("    .\"      ) ");
    conejo[2]->lineas[5] = strdup("  _/   ,  \\/\\ ");
    conejo[2]->lineas[6] = strdup(" /_(_, )-\\ \\\xEF ");
    conejo[2]->lineas[7] = strdup("\xEF  << <  \xEF\xF9\xEF  ");
    conejo[2]->lineas[8] = strdup("    \\\\ \\      ");
    conejo[2]->lineas[9] = strdup("    \xEF\xEF\xF9\xEF\x1b[9A\x1b[8D");
    
    conejo[3]->lineas[0] = strdup("         ,,,,,,,_  ");
    conejo[3]->lineas[1] = strdup("          `'\"\\` ,\\ ");
    conejo[3]->lineas[2] = strdup("        _,---\" =__)");
    conejo[3]->lineas[3] = strdup("  ,_,-\xFA\",       )  ");
    conejo[3]->lineas[4] = strdup(" _(_(   )-\xFA,_ \\/\\_ ");
    conejo[3]->lineas[5] = strdup("(______/     \\ \\_-`");
    conejo[3]->lineas[6] = strdup("              `--` ");
    conejo[3]->lineas[7] = strdup("                   ");
    conejo[3]->lineas[8] = strdup("                   ");
    conejo[3]->lineas[9] = strdup("\x1b[9A");
    
    conejo[4]->lineas[0] = strdup("                   ");
    conejo[4]->lineas[1] = strdup("                   ");
    conejo[4]->lineas[2] = strdup("            ,\\     ");
    conejo[4]->lineas[3] = strdup("  ,__,-\xFA\xFA-, \\\\\\,_  ");
    conejo[4]->lineas[4] = strdup(" _(_/      \\ \\` ,\\ ");
    conejo[4]->lineas[5] = strdup("(_     ),   `\" =__)");
    conejo[4]->lineas[6] = strdup("  `\xFA-\xFA\xEF \\       )  ");
    conejo[4]->lineas[7] = strdup("         \"-.\\ \\/\\_ ");
    conejo[4]->lineas[8] = strdup("             \\ \\_-`");
    conejo[4]->lineas[9] = strdup("              `--`\x1b[9A\x1b[18D");
    
    conejo[5]->lineas[0] = strdup("                ");
    conejo[5]->lineas[1] = strdup("                ");
    conejo[5]->lineas[2] = strdup("         \\  \\   ");
    conejo[5]->lineas[3] = strdup("         \\\\_\\\\  ");
    conejo[5]->lineas[4] = strdup("          \\`,,\\ ");
    conejo[5]->lineas[5] = strdup("     __,.-\" = T)");
    conejo[5]->lineas[6] = strdup("   .\"        )\xF9 ");
    conejo[5]->lineas[7] = strdup(",_/   ,    \\/\\_ ");
    conejo[5]->lineas[8] = strdup("\\_(    )_-\\ \\_-`");
    conejo[5]->lineas[9] = strdup("   `-----` `--`\x1b[9A\x1b[15D");
    
    conejo[6]->lineas[0] = strdup("                ");
    conejo[6]->lineas[1] = strdup("                ");
    conejo[6]->lineas[2] = strdup("          \\   / ");
    conejo[6]->lineas[3] = strdup("          \\\\_// ");
    conejo[6]->lineas[4] = strdup("          |, ,| ");
    conejo[6]->lineas[5] = strdup("     __,.-(=T=) ");
    conejo[6]->lineas[6] = strdup("   .\"       \"/  ");
    conejo[6]->lineas[7] = strdup(",_/   ,    \\/\\_ ");
    conejo[6]->lineas[8] = strdup("\\_(    )_-\\ \\_-`");
    conejo[6]->lineas[9] = strdup("   `-----` `--`\x1b[9A\x1b[15D");

	printf ("\x1b[38;5;13m");
	
	for (i = 0; i < saltoLinea; i++) {
		printf ("\x1b[E");
	}
	
	animacionEntrada(conejo);
	mostrarDibujo(conejo[0], 0);

	while (columnaFrame < columnaCentro){
		animacionSalto(&columnaFrame, conejo);
	}
	
	animacionWink(conejo);
	
	while (columnaFrame < anchoConsola - 31){
		animacionSalto(&columnaFrame, conejo);
	}
	
	free(conejo[0]->lineas[9]);
	conejo[0]->lineas[9] = strdup("   `-----` `--`");	
	free(conejo[1]->lineas[9]);
	conejo[1]->lineas[9] = strdup("   \\\\__)");	
	free(conejo[2]->lineas[9]);
	conejo[2]->lineas[9] = strdup("    \xEF\xEF\xF9\xEF");	
	free(conejo[3]->lineas[9]);
	conejo[3]->lineas[9] = strdup("            ");	
	free(conejo[4]->lineas[9]);
	conejo[4]->lineas[9] = strdup("              `--`");
	
	animacionSalida(conejo, anchoConsola, &columnaFrame);
	
	Sleep (1500);
	
	for (i = 0; i < 11; i++){
		printf("\x1b[T");
		Sleep (150);
	}
	
	system ("cls");
	
	
	dimmHastaLuego(anchoConsola, altoConsola, 195);
	ReproducirAudio("IDR_WAVE8", 0);	
	for (colorD = 255; colorD > 231; colorD--){
		dimmHastaLuego(anchoConsola, altoConsola, colorD);
	}	
	dimmHastaLuego(anchoConsola, altoConsola, 0);
}

void cambiarColorTexto(char* nombreColor) {
	int rojo = FOREGROUND_RED | FOREGROUND_INTENSITY;
	int verde = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	int azul = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	int amarillo = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	int cian = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	int magenta = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	int blanco = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	int gris = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	int color = 0;
	if (nombreColor == "rojo"){
		color = rojo;
	}
	else if (nombreColor == "verde"){
		color = verde;
	}
	else if (nombreColor == "azul"){
		color = azul;
	}
	else if (nombreColor == "amarillo"){
		color = amarillo;
	}
	else if (nombreColor == "cian"){
		color = cian;
	}
	else if (nombreColor == "magenta"){
		color = magenta;
	}
	else if (nombreColor == "blanco"){
		color = blanco;
	}
	else if (nombreColor == "gris"){
		color = gris;
	}
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

void imprimirCentrado(char* texto, int anchoConsola) {
	int longitudTexto = strlen(texto);
	int espacioBlanco = (anchoConsola - longitudTexto) / 2;
	for (i = 0; i < espacioBlanco; i++) {
		printf (" ");
	}
	printf ("%s\n", texto);
	Sleep (40);
}

void imprimirSimbolosCentrado(int* valoresAscii, int cantidad, int anchoConsola){
	int anchoSimbolos = cantidad;
	int espacioBlanco = (anchoConsola - anchoSimbolos) / 2;
	for (i = 0; i < espacioBlanco; i++) {
		printf (" ");
	}
	for (i = 0; i < cantidad; i++) {
		if (valoresAscii[i] == 186 || valoresAscii[i] == 187 || valoresAscii[i] == 188 || valoresAscii[i] == 200 || valoresAscii[i] == 201 || valoresAscii[i] == 203 || valoresAscii[i] == 205){
			cambiarColorTexto("azul");
		}
		else if (valoresAscii[i] == 46 || valoresAscii[i] == 58 || valoresAscii[i] == 250){
			cambiarColorTexto("amarillo");
		}
		else if (valoresAscii[i] == 207){
			cambiarColorTexto("magenta");
		}
		else if (valoresAscii[i] == 95){
			cambiarColorTexto("verde");
		}
		else {
			cambiarColorTexto("cian");
		}
		printf ("%c", (char)valoresAscii[i]);
		Sleep (10);
	}
	printf ("\n");
}

void imprimirTiempoCentrado(int* tiempo, int anchoConsola) {
	char rOn[14] = "\x1b[38;5;14m";
	char rOff[13] = "\x1b[38;5;8m";
	char marcoC[13] = "\x1b[38;5;4m";
	char undMetriC[13] = "\x1b[38;5;9m";
	char separadoresR[14] = "\x1b[38;5;12m";
	char formatB[8] = "\x1B[1m";
	char resetF[8] = "\x1b[0m";
	char relojDigitosL1[500] = "";
	char relojDigitosL2[500] = "";
	char relojDigitosL3[500] = "";
	char temp[100] = "";
	int tercioD[4] = {0, 0, 0};
	int dou = 0;
	int digit = 0;
	for (i = 0; i < 4; i++){ 
		for (dou = 0; dou < 2; dou++){
			if (i == 3){
				digit = tiempo[i];
				dou = 2;
			}
			else if (dou == 0) {
				digit = tiempo[i] / 10;
			}
			else if (dou == 1) {
				digit = tiempo[i] - (tiempo[i] / 10) * 10;
			}		
			switch (digit) {
				case 0:
					tercioD[0] = 1;
					tercioD[1] = 5;
					tercioD[2] = 2;
				break;
				case 1:
					tercioD[0] = 2;
					tercioD[1] = 2;
					tercioD[2] = 1;
				break;
				case 2:
					tercioD[0] = 1;
					tercioD[1] = 3;
					tercioD[2] = 4;
				break;
				case 3:
					tercioD[0] = 1;
					tercioD[1] = 3;
					tercioD[2] = 3;
				break;
				case 4:
					tercioD[0] = 2;
					tercioD[1] = 1;
					tercioD[2] = 1;
				break;
				case 5:
					tercioD[0] = 1;
					tercioD[1] = 4;
					tercioD[2] = 3;
				break;
				case 6:
					tercioD[0] = 1;
					tercioD[1] = 4;
					tercioD[2] = 2;
				break;
				case 7:
					tercioD[0] = 1;
					tercioD[1] = 2;
					tercioD[2] = 1;
				break;
				case 8:
					tercioD[0] = 1;
					tercioD[1] = 1;
					tercioD[2] = 2;
				break;
				case 9:
					tercioD[0] = 1;
					tercioD[1] = 1;
					tercioD[2] = 1;
				break;
			}		
			switch (tercioD[0]) {
				case 1:
					sprintf (temp, "%s %s_ %s", formatB, rOn, resetF);						/*0,2,3,5,6,7,8,9*/
				break;
				case 2:
					sprintf (temp, "%s %s_ %s", formatB, rOff, resetF);					/*1,4*/
				break;
			}		
			strcat (relojDigitosL1, temp);		
			switch (tercioD[1]) {
				case 1:
					sprintf (temp, "%s%s|_|%s", formatB, rOn, resetF);					/*4,8,9*/
				break;
				case 2:
					sprintf (temp, "%s%s|_%s|%s", formatB, rOff, rOn, resetF);			/*1,7*/
				break;
				case 3:
					sprintf (temp, "%s%s|%s_|%s", formatB, rOff, rOn, resetF);			/*2,3*/
				break;
				case 4:
					sprintf (temp, "%s%s|_%s|%s", formatB, rOn, rOff, resetF);			/*5,6*/
				break;
				case 5:
					sprintf (temp, "%s%s|%s_%s|%s", formatB, rOn, rOff, rOn, resetF);	/*0*/
				break;
			}		
			strcat (relojDigitosL2, temp);		
			switch (tercioD[2]) {
				case 1:
					sprintf (temp, "%s%s|_%s|%s", formatB, rOff, rOn, resetF);			/*1,4,7,9*/
				break;
				case 2:
					sprintf (temp, "%s%s|_|%s", formatB, rOn, resetF);					/*0,6,8*/
				break;
				case 3:
					sprintf (temp, "%s%s|%s_|%s", formatB, rOff, rOn, resetF);					/*3,5*/
				break;
				case 4:
					sprintf (temp, "%s%s|_%s|%s", formatB, rOn, rOff, resetF);			/*2*/
				break;
			}		
			strcat (relojDigitosL3, temp);			
			switch (dou) {
				case 0:
					strcat(relojDigitosL1, undMetriC);
					strcat(relojDigitosL1, formatB);
					strcat (relojDigitosL2, " ");
					strcat (relojDigitosL3, " ");
					switch (i) {
						case 0:
							strcat (relojDigitosL1, "H");
						break;
						case 1:
							strcat (relojDigitosL1, "M");
						break;
						case 2:
							strcat (relojDigitosL1, "S");
						break;
					}
				break;
				case 1:
					strcat (relojDigitosL1, "   ");
					strcat (relojDigitosL3, separadoresR);
					strcat (relojDigitosL3, " \xFE ");
					if (i < 2){
						strcat (relojDigitosL2, separadoresR);
						strcat (relojDigitosL2, " \xFE ");
					}
					else if (i == 2){
						strcat (relojDigitosL2, "   ");
					}					
				break;					
			}
		}
	}
	int espacioBlanco = (anchoConsola - 35) / 2;
	for (i = 0; i < espacioBlanco; i++) {
		printf(" ");
	}
	printf ("%s%s\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBB\n", formatB, marcoC);
	for (i = 0; i < espacioBlanco; i++) {
		printf(" ");
	}
	printf ("%s%s\xBA", formatB, marcoC);
	printf("%s", relojDigitosL1);
	printf ("%s%s\xBA\n", formatB, marcoC);
	for (i = 0; i < espacioBlanco; i++) {
		printf(" ");
	}
	printf ("%s%s\xBA", formatB, marcoC);
	printf("%s", relojDigitosL2);
	printf ("%s%s\xBA\n", formatB, marcoC);
	for (i = 0; i < espacioBlanco; i++) {
		printf(" ");
	}
	printf ("%s%s\xBA", formatB, marcoC);
	printf("%s", relojDigitosL3);
	printf ("%s%s\xBA\n", formatB, marcoC);
	for (i = 0; i < espacioBlanco; i++) {
		printf(" ");
	}
	printf ("\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xBC\n%s", resetF);
	printf ("\x1b[5F");
}

void solicitarTiempo(int* tiempo, int anchoConsola){
	int dou = 0, digit = 0, bC = 0, j, isT, centro = (anchoConsola / 2) - 16, control = 0;
	char bajar[7] = "\x1b[B";
	char avanza[7] = "\x1b[C";
	char atras[7] = "\x1b[D";
	char dOn[17] = " \x1b[38;5;14m_ ";
	char dOff[23] = "\x1B[1m\x1b[38;5;8m _ ";
	char dBlink[23] = "";
	char dCon[47] = "\x1b[B\x1b[3D|_|\x1b[B\x1b[3D|_|\x1b[2A\x1b[3D";
	char dConBlink[14] = "";
	char dConBOn[14] = "\x1b[38;5;14m";
	char AwOff[13] = "\x1b[38;5;8m";
	char undBlink[13] = "";
	char undOn[13] = "\x1b[38;5;9m";
	int limits[4][2] = { {2, 3}, {5, 9}, {5, 9}, {9, 9} };
		
	printf ("%s", bajar);
	for (j = 0; j < centro; j++) {
		printf ("%s", avanza);
	}
	for (isT = 0; isT < 4; isT++) {
		for (dou = 0; dou < 2; dou++){
			do {
				if ((isT > 0 || dou > 0) && control == 0){
					control++;		
					switch (isT) {
						case 1:
							printf ("\x1b[10C");
						break;
						case 2:
							printf ("\x1b[20C");
						break;
						case 3:
							printf ("\x1b[30C");
						break;
					}
					if (dou == 1){
						printf ("\x1b[4C");
					}
				}
				switch (bC) {
					case 0:
						bC++;
						strcpy (dBlink, dOff);
						strcpy (undBlink, AwOff);
						strcpy (dConBlink, AwOff);
					break;
					case 1:
						bC--;
						strcpy (dBlink, dOn);
						strcpy (undBlink, undOn);
						strcpy (dConBlink, dConBOn);
					break;
				}								
				switch (dou){
					case 0:
						printf ("%s", dBlink);
						switch (isT){
							case 0:
								printf ("%sH%s", undBlink, atras);
							break;
							case 1:
								printf ("%sM%s", undBlink, atras);
							break;
							case 2:
								printf ("%sS%s", undBlink, atras);
							break;
						}					
						printf ("%s%s", dConBlink, dCon);
					break;
					case 1:						
						switch (isT){
							case 0:
								printf ("%s%sH", atras, undBlink);
							break;
							case 1:
								printf ("%s%sM", atras, undBlink);
							break;
							case 2:
								printf ("%s%sS", atras, undBlink);
							break;
						}					
						printf ("%s%s", dBlink, dCon);
					break;
				}								
				Sleep (200);
																
				if (_kbhit()){
					digit = getch();
				}
				if (digit - '0' > -1 && digit - '0' < 10){
					ReproducirAudio("IDR_WAVE3", 0);
				}
				else {
					digit = 0;
				}
								
				if (digit - '0' > limits[isT][dou]) {
					if (isT == 0 && dou == 1 && tiempo[0] < 20){
					}
					else {
						digit = limits[isT][dou] + '0';
					}					
				}									
			}while (digit < '0' || digit > '9');
			digit -= '0';
			control = 0;			
			if (isT < 3) {
				switch (dou){
					case 0:
						tiempo[isT] = digit * 10;
					break;
					case 1:
						tiempo[isT] += digit;
					break;
				}				
			}
			else if (isT == 3){
				tiempo[isT] = digit;
				dou = 2;
			}		
			printf ("\x1b[F");
			imprimirTiempoCentrado(tiempo, anchoConsola);
			printf ("%s", bajar);
			for (j = 0; j < centro; j++) {
				printf ("%s", avanza);
			}	
		}
	}
	printf ("\x1b[F\x1b[0m");
}

void escogeSonidoAlarma(int *sonidoAlarma, int anchoConsola){
	char infoSonA[121] = {83, 101, 108, 101, 99, 99, 105, 111, 110, 97, 32, 117, 110, 97, 32, 109, 101, 108, 111, 100, 161, 97, 32, 99, 111, 110, 32, 108, 97, 115, 32, 102, 108, 101, 99, 104, 97, 115, 32, 105, 122, 113, 117, 105, 101, 114, 100, 97, 32, 121, 32, 100, 101, 114, 101, 99, 104, 97, 44, 32, 108, 117, 101, 103, 111, 32, 112, 117, 108, 115, 97, 32, 105, 110, 116, 114, 111, 32, 112, 97, 114, 97, 32, 101, 115, 116, 97, 98, 108, 101, 99, 101, 114, 32, 101, 115, 101, 32, 115, 111, 110, 105, 100, 111, 32, 112, 97, 114, 97, 32, 108, 97, 32, 97, 108, 97, 114, 109, 97, 46};
	char botonSup[39] = {201, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 187, 32, 201, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 187, 32, 201, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 187};
	char botonMed[39] = {186, 32, 65, 108, 97, 114, 109, 97, 32, 49, 32, 186, 32, 186, 32, 65, 108, 97, 114, 109, 97, 32, 50, 32, 186, 32, 186, 32, 65, 108, 97, 114, 109, 97, 32, 51, 32, 186};
	char botonInf[39] = {200, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 188, 32, 200, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 188, 32, 200, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 188};
	int espacioBlanco = (anchoConsola - 120) / 2;
	int limitMin, limitMax, j;
	*sonidoAlarma = 1;
	char validar;
	
	printf ("\x1b[6E");
	
	for (i = 0; i < espacioBlanco; i++){
		printf(" ");
	}
	
	ReproducirAudio("IDR_WAVE5", 1);
				
	for (i = 0; i < 120; i++){
		if (i >= 0 && i < 10 || i > 30 && i < 48 || i > 50 && i < 58 || i > 65 && i < 77){
			printf ("\x1b[38;5;11m");
		}
		else if (i > 82 && i < 93 || i > 112 && i < 119){
			printf ("\x1b[38;5;14m");
		}
		else if (i > 14 && i < 22 || i > 97 && i < 104){
			printf ("\x1b[38;5;9m");
		}
		else {
			printf ("\x1b[38;5;10m");
		}
		printf("%c", infoSonA[i]);				
		Sleep (10);
	}
	DetenerAudio();
	printf ("\n\n");
	
	do {
		validar = '\0';
		switch (*sonidoAlarma){
			case 1:
				limitMin = 0;
				limitMax = 11;
				PlaySound(TEXT("C:\\Windows\\Media\\Alarm01.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			break;
			case 2:
				limitMin = 13;
				limitMax = 24;
				PlaySound(TEXT("C:\\Windows\\Media\\Alarm02.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			break;
			case 3:
				limitMin = 26;
				limitMax = 37;
				PlaySound(TEXT("C:\\Windows\\Media\\Alarm03.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			break;
		}	
		for (j = 0; j < 3; j++){
			espacioBlanco = (anchoConsola - 38) / 2;
			for (i = 0; i < espacioBlanco; i++){
				printf(" ");
			}
			for (i = 0; i < 38; i++){
				if (i >= limitMin && i <= limitMax){
					printf ("\x1b[38;5;10m");
				}
				else {
					printf ("\x1b[38;5;8m");
				}
				switch (j){
					case 0:
						printf("%c", botonSup[i]);
					break;
					case 1:
						printf("%c", botonMed[i]);
					break;
					case 2:
						printf("%c", botonInf[i]);
					break;
				}
				if (i == 37){
					if (j < 2){
						printf ("\n");
					}
					else if (j == 2){
						printf ("\x1b[2F");
					}
				}								
			}
		}		
		do {
			if (_kbhit()){
				if (GetAsyncKeyState(VK_LEFT) & 0x8000){
					_getch();
					if (*sonidoAlarma > 1){
						(*sonidoAlarma)--;
						validar = 7;
					}
				}
				if (GetAsyncKeyState(VK_RIGHT) & 0x8000){
					_getch();
					if (*sonidoAlarma < 3){
						(*sonidoAlarma)++;
						validar = 7;
					}
				}
				if (GetAsyncKeyState(VK_RETURN) & 0x8000){
					validar = 13;
				}
			}			
		} while (validar != 13 && validar != 7);
		_getch();
		PlaySound(NULL, 0, 0);
		ReproducirAudio("IDR_WAVE3", 0);			
	} while (validar != 13);
	printf ("\x1b[3F\x1b[6M\x1b[5F");
}

void imprimirPausaCentrado(int* pausa, int anchoConsola) {	
	char tiempoPausa[11] = "";
	for (i = 0; i < 4; i++){
		char tempTP[4];
		if (i < 2) {
			sprintf(tempTP, "%02d:", pausa[i]);
		}
		else if (i == 2) {
			sprintf(tempTP, "%02d.", pausa[i]);
		}
		else if (i == 3) {
			sprintf(tempTP, "%d", pausa[i]);
		}
		strcat(tiempoPausa, tempTP);
	}
	int espacioBlanco = (anchoConsola - 10) / 2;
	for (i = 0; i < espacioBlanco; i++) {
		printf(" ");
	}
	printf("\x1b[38;5;14m%s\x1b[0m", tiempoPausa);
	for (i = 2; i < espacioBlanco; i++) {
		printf(" ");
	}
	printf("\r");
}

void numeroEnTexto(int pausa[4], int anchoConsola) {
	int restCar = 0;
    char textoPausa[240] = {0};
	char eAcento[2] = {130, '\0'};
	char oAcento[2] = {162, '\0'};
	char uAcento[2] = {163, '\0'};
    
    for (i = 0; i < 4; i++){
    	char textoNum[32] = {0};
    	int nadaYcoma = 0;
    	int unidad = pausa[i] % 10;    	
    	strcpy(textoNum, "\x1b[38;5;14m");
    	restCar += 10;    	
    	if (pausa[i] > 49) {
	        strcat(textoNum, "cincuenta");			       
 	       	if (pausa[i] == 50){
	            goto final;
	        }
    	} else if (pausa[i] > 39) {
        	strcat(textoNum, "cuarenta");        	
    	    if (pausa[i] == 40){
        	    goto final;
	        }
    	} else if (pausa[i] > 29) {
        	strcat(textoNum, "treinta");        	
    	    if (pausa[i] == 30){
        	    goto final;
  	      	}
    	} else if (pausa[i] > 9) {
    		switch (pausa[i]){
    			case 10:
    				strcat(textoNum, "diez");
					goto final;    				
    			break;
    			case 11:
    				strcat(textoNum, "once");
					goto final;    				
    			break;
    			case 12:
    				strcat(textoNum, "doce");
					goto final;    				
    			break;
    			case 13:
    				strcat(textoNum, "trece");
					goto final;    				
    			break;
    			case 14:
    				strcat(textoNum, "catorce");
    				goto final;
    			break;
    			case 15:
    				strcat(textoNum, "quince");
    				goto final;
    			break;
    			case 16:
    				strcat(textoNum, "diecis");
    				strcat(textoNum, eAcento);
    				strcat(textoNum, "is");
    				goto final;
    			break;
    			case 17:
    				strcat(textoNum, "diecisiete");
    				goto final;
    			break;
    			case 18:
    				strcat(textoNum, "dieciocho");
    				goto final;
    			break;
    			case 19:
    				strcat(textoNum, "diecinueve");
    				goto final;
    			break;
    			case 20:
    				strcat(textoNum, "veinte");
    				goto final;
    			break;
    			case 21:
    				if (i > 0){
    					strcat(textoNum, "veinti");
    					strcat(textoNum, uAcento);
    					strcat(textoNum, "n");
					} else if (i == 0){
						strcat(textoNum, "veintiuna");
					}
    				
    				goto final;
    			break;
    			case 22:
    				strcat(textoNum, "veintid");
    				strcat(textoNum, oAcento);
    				strcat(textoNum, "s");
    				goto final;
    			break;
    			case 23:
    				strcat(textoNum, "veintitr");
    				strcat(textoNum, eAcento);
    				strcat(textoNum, "s");
    				goto final;
    			break;
    			case 24:
    				strcat(textoNum, "veinticuatro");
    				goto final;
    			break;
    			case 25:
    				strcat(textoNum, "veinticinco");
    				goto final;
    			break;
    			case 26:
    				strcat(textoNum, "veintis");
    				strcat(textoNum, eAcento);
    				strcat(textoNum, "is");
    				goto final;
    			break;
    			case 27:
    				strcat(textoNum, "veintisiete");
    				goto final;
    			break;
    			case 28:
    				strcat(textoNum, "veintiocho");
    				goto final;
    			break;
    			case 29:
    				strcat(textoNum, "veintinueve");
    				goto final;
    			break;    			
			}
    	}  
    	if (pausa[i] > 30) {
        	strcat(textoNum, " y ");
    	} 
    	if (unidad > 0) {
    	    switch (unidad) {
        	    case 1:
        	    	if (i == 0 || i == 3){
        	    		strcat(textoNum, "una");
					} else if (i == 1 || i == 2) {
						strcat(textoNum, "un");
					}            	    
                break;
            	case 2:
                	strcat(textoNum, "dos");
                break;
            	case 3:
                	strcat(textoNum, "tres");
                break;
            	case 4:
                	strcat(textoNum, "cuatro");
                break;
            	case 5:
                	strcat(textoNum, "cinco");
                break;
            	case 6:
                	strcat(textoNum, "seis");
                break;
            	case 7:
                	strcat(textoNum, "siete");
                break;
            	case 8:
                	strcat(textoNum, "ocho");
                break;
            	case 9:
                	strcat(textoNum, "nueve");
                break;
        	}
    	} 
    	final:
    	strcat(textoPausa, textoNum);
    	if (pausa[i] > 0){
    		switch (i){
	    		case 0:
    				strcat(textoPausa, "\x1b[38;5;9m hora");
    				restCar += 9;
    				if (pausa[1] > 0){
    					if (pausa[2] > 0 || pausa[3] > 0){
    						nadaYcoma++;
						}
						nadaYcoma++;    					
					} else if (pausa[2] > 0){
						if (pausa[3] > 0){
							nadaYcoma++;
						}
						nadaYcoma++;
					} else if (pausa[3] > 0){
						nadaYcoma++;
					}
    			break;
    			case 1:
    				strcat(textoPausa, "\x1b[38;5;9m minuto");
    				restCar += 9;
    				if (pausa[2] > 0){
						if (pausa[3] > 0){
							nadaYcoma++;
						}
						nadaYcoma++;
					} else if (pausa[3] > 0){
						nadaYcoma++;
					}
  		  		break;
    			case 2:
    				strcat(textoPausa, "\x1b[38;5;9m segundo");
    				restCar += 9;
    				if (pausa[3] > 0){
    					nadaYcoma++;
					}
	    		break;
    			case 3:
    				strcat(textoPausa, "\x1b[38;5;9m d");    				
    				restCar += 9;
    				strcat(textoPausa, eAcento);
    				strcat(textoPausa, "cima");
    			break;
			}
			if (pausa[i] > 1){
				strcat(textoPausa, "s");
			}
			if (pausa[3] > 0 && i == 3){
				strcat(textoPausa, " de segundo");
			}
			switch (nadaYcoma){
				case 1:
					strcat(textoPausa, "\x1b[38;5;11m y ");
					restCar += 10;
				break;
				case 2:
					strcat(textoPausa, "\x1b[38;5;11m, ");
					restCar += 10;
				break;
			}
		}     	
	}
    size_t totalCarImp = strlen(textoPausa) - restCar;
    int espacioBlanco = (anchoConsola - (52 + totalCarImp)) / 2;
	for (i = 0; i < espacioBlanco; i++) {
		printf (" ");
	}
    printf ("\x1b[38;5;11m\x1b[1mEl temporizador ha permanecido pausado un total de %s\x1b[38;5;11m.\x1b[0m\x1b[2E", textoPausa); 			
}

void avisoAlarma(char *aOff, int sound, int anchoConsola){
	*aOff = '\0';
	char bordeReloj[17] = "\x1b[38;5;4m\xBA";
	char wOff[13] = "\x1b[38;5;8m";		// Gris
	char nOn[14] = "\x1b[38;5;14m";		// Cian
	char UmOn[13] = "\x1b[38;5;9m";		// Rojo
	char wOn[14] = "\x1b[38;5;12m";		// Azul
	char bajarL[7] = "\x1b[E";
	char back2start[8] = "\x1b[2F";
	int espacioBlanco = (anchoConsola - 35) / 2;
	int sw = 0;
	char wState[14] = "";
	char UmState[13] = "";
	char nState[14] = "";
	
	switch (sound){
		case 0:
			PlaySound(TEXT("C:\\Windows\\Media\\Windows Default.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
		break;
		case 1:
			PlaySound(TEXT("C:\\Windows\\Media\\Alarm01.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
		break;
		case 2:
			PlaySound(TEXT("C:\\Windows\\Media\\Alarm02.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
		break;
		case 3:
			PlaySound(TEXT("C:\\Windows\\Media\\Alarm03.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
		break;
	}
	printf ("\x1b[E");	
	while (*aOff != ' ' && *aOff != 8 && *aOff != 13 && *aOff != 27){
		switch (sw){
			case 0:
				strcpy (wState, wOff);
				strcpy (UmState, wOff);
				strcpy (nState, wOff);
				sw++;
			break;
			case 1:
				strcpy (wState, wOn);
				strcpy (UmState, UmOn);
				strcpy (nState, nOn);
				sw--;
			break;
		}
		for (i = 0; i < espacioBlanco; i++) {
			printf(" ");
		}
		printf ("%s%s _ %sH%s _     _ %sM%s _     _ %sS%s _     _ %s%s", bordeReloj, nState, UmState, nState, UmState, nState, UmState, nState, bordeReloj, bajarL);
		for (i = 0; i < espacioBlanco; i++) {
			printf(" ");
		}
		printf ("%s%s|_| |_| %s\xFE%s |_| |_| %s\xFE%s |_| |_|   |_|%s%s", bordeReloj, nState, wState, nState, wState, nState, bordeReloj, bajarL);
		for (i = 0; i < espacioBlanco; i++) {
			printf(" ");
		}
		printf ("%s%s|_| |_| %s\xFE%s |_| |_| %s\xFE%s |_| |_| %s\xFE%s |_|%s%s", bordeReloj, nState, wState, nState, wState, nState, wState, nState, bordeReloj, back2start);
		Sleep (200);														
		if (_kbhit()){
			*aOff = getch();
		}
	}
	if (*aOff == ' ' || *aOff == 13){
		printf ("\x1b[F\x1b[0m");
	} else if (*aOff == 8){
		printf ("\x1b[2E\x1b[0m");
	}
	PlaySound(NULL, 0, 0);
	ReproducirAudio("IDR_WAVE3", 0);	
}

void animacionLogo(int anchoConsola) {
	int valoresAscii1[62] = {201, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 187};
	int cantidad = sizeof(valoresAscii1) / sizeof(valoresAscii1[0]);
	imprimirSimbolosCentrado(valoresAscii1, cantidad, anchoConsola);
	int valoresAscii2[96] = {124, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 124, 32, 186, 32, 32, 95, 95, 32, 32, 32, 32, 32, 32, 95, 95, 46, 95, 95, 46, 95, 95, 32, 32, 32, 32, 32, 32, 46, 95, 95, 95, 32, 95, 95, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 46, 95, 95, 32, 32, 32, 32, 32, 32, 32, 32, 32, 186, 32, 124, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 124};
	cantidad = sizeof(valoresAscii2) / sizeof(valoresAscii2[0]);
	imprimirSimbolosCentrado(valoresAscii2, cantidad, anchoConsola);
	int valoresAscii3[94] = {92, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 47, 32, 32, 186, 32, 47, 32, 32, 92, 32, 32, 32, 32, 47, 32, 32, 92, 95, 95, 124, 32, 32, 124, 32, 32, 32, 95, 95, 124, 32, 95, 47, 47, 32, 32, 124, 95, 32, 32, 95, 95, 95, 95, 32, 95, 95, 95, 95, 95, 95, 32, 124, 95, 95, 124, 95, 95, 95, 95, 32, 32, 32, 32, 186, 32, 32, 92, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 47};
	cantidad = sizeof(valoresAscii3) / sizeof(valoresAscii3[0]);
	imprimirSimbolosCentrado(valoresAscii3, cantidad, anchoConsola);
	int valoresAscii4[92] = {92, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 47, 32, 32, 32, 186, 32, 92, 32, 32, 32, 92, 47, 92, 47, 32, 32, 32, 47, 32, 32, 124, 32, 32, 124, 32, 32, 47, 32, 95, 95, 32, 124, 92, 32, 32, 32, 95, 95, 92, 47, 32, 32, 95, 32, 92, 92, 95, 95, 95, 95, 32, 92, 124, 32, 32, 92, 95, 95, 32, 32, 92, 32, 32, 32, 186, 32, 32, 32, 92, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 47};
	cantidad = sizeof(valoresAscii4) / sizeof(valoresAscii4[0]);
	imprimirSimbolosCentrado(valoresAscii4, cantidad, anchoConsola);
	int valoresAscii5[90] = {92, 58, 58, 250, 46, 46, 250, 58, 58, 47, 32, 32, 32, 32, 186, 32, 32, 92, 32, 32, 32, 32, 32, 32, 32, 32, 47, 124, 32, 32, 124, 32, 32, 124, 95, 47, 32, 47, 95, 47, 32, 124, 32, 124, 32, 32, 124, 32, 40, 32, 32, 60, 95, 62, 32, 41, 32, 32, 124, 95, 62, 32, 62, 32, 32, 124, 47, 32, 95, 95, 32, 92, 95, 32, 186, 32, 32, 32, 32, 92, 58, 58, 250, 46, 46, 250, 58, 58, 47};
	cantidad = sizeof(valoresAscii5) / sizeof(valoresAscii5[0]);
	imprimirSimbolosCentrado(valoresAscii5, cantidad, anchoConsola);
	int valoresAscii6[88] = {92, 58, 58, 58, 58, 58, 58, 47, 32, 32, 32, 32, 32, 186, 32, 32, 32, 92, 95, 95, 47, 92, 32, 32, 47, 32, 124, 95, 95, 124, 95, 95, 95, 95, 124, 95, 95, 95, 95, 32, 124, 32, 124, 95, 95, 124, 32, 32, 92, 95, 95, 95, 95, 47, 124, 32, 32, 32, 95, 95, 47, 124, 95, 95, 40, 95, 95, 95, 95, 32, 32, 47, 32, 186, 32, 32, 32, 32, 32, 92, 58, 58, 58, 58, 58, 58, 47};
	cantidad = sizeof(valoresAscii6) / sizeof(valoresAscii6[0]);
	imprimirSimbolosCentrado(valoresAscii6, cantidad, anchoConsola);
	int valoresAscii7[86] = {92, 58, 58, 58, 58, 47, 32, 32, 32, 32, 32, 32, 186, 32, 32, 32, 32, 32, 32, 32, 32, 92, 47, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 92, 47, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 124, 95, 95, 124, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 92, 47, 32, 32, 186, 32, 32, 32, 32, 32, 32, 92, 58, 58, 58, 58, 47};
	cantidad = sizeof(valoresAscii7) / sizeof(valoresAscii7[0]);
	imprimirSimbolosCentrado(valoresAscii7, cantidad, anchoConsola);
	int valoresAscii8[84] = {41, 58, 58, 40, 32, 32, 32, 32, 32, 32, 32, 200, 205, 205, 203, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 203, 205, 205, 188, 32, 32, 32, 32, 32, 32, 32, 41, 58, 58, 40};
	cantidad = sizeof(valoresAscii8) / sizeof(valoresAscii8[0]);
	imprimirSimbolosCentrado(valoresAscii8, cantidad, anchoConsola);
	int valoresAscii9[86] = {47, 32, 58, 58, 32, 92, 32, 32, 32, 32, 32, 32, 32, 32, 32, 186, 32, 95, 95, 95, 95, 95, 95, 95, 95, 95, 32, 46, 95, 95, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 186, 32, 32, 32, 32, 32, 32, 32, 32, 32, 47, 32, 58, 58, 32, 92};
	cantidad = sizeof(valoresAscii9) / sizeof(valoresAscii9[0]);
	imprimirSimbolosCentrado(valoresAscii9, cantidad, anchoConsola);
	int valoresAscii10[88] = {47, 32, 32, 58, 58, 32, 32, 92, 32, 32, 32, 32, 32, 32, 32, 32, 186, 32, 92, 95, 32, 32, 32, 95, 95, 95, 32, 92, 124, 32, 32, 124, 95, 95, 95, 95, 95, 95, 95, 95, 95, 32, 32, 95, 95, 95, 95, 32, 32, 32, 95, 95, 95, 95, 32, 32, 32, 95, 95, 95, 95, 32, 32, 95, 95, 95, 95, 95, 95, 32, 186, 32, 32, 32, 32, 32, 32, 32, 32, 47, 32, 32, 58, 58, 32, 32, 92};
	cantidad = sizeof(valoresAscii10) / sizeof(valoresAscii10[0]);
	imprimirSimbolosCentrado(valoresAscii10, cantidad, anchoConsola);
	int valoresAscii11[90] = {47, 32, 32, 32, 58, 58, 32, 32, 32, 92, 32, 32, 32, 32, 32, 32, 32, 186, 32, 47, 32, 32, 32, 32, 92, 32, 32, 92, 47, 124, 32, 32, 124, 32, 32, 92, 95, 32, 32, 95, 95, 32, 92, 47, 32, 32, 95, 32, 92, 32, 47, 32, 32, 32, 32, 92, 32, 47, 32, 32, 95, 32, 92, 47, 32, 32, 95, 95, 95, 47, 32, 186, 32, 32, 32, 32, 32, 32, 32, 47, 32, 32, 32, 58, 58, 32, 32, 32, 92};
	cantidad = sizeof(valoresAscii11) / sizeof(valoresAscii11[0]);
	imprimirSimbolosCentrado(valoresAscii11, cantidad, anchoConsola);
	int valoresAscii12[92] = {47, 32, 32, 32, 46, 58, 58, 46, 32, 32, 32, 92, 32, 32, 32, 32, 32, 32, 186, 32, 92, 32, 32, 32, 32, 32, 92, 95, 95, 95, 124, 32, 32, 32, 89, 32, 32, 92, 32, 32, 124, 32, 92, 40, 32, 32, 60, 95, 62, 32, 41, 32, 32, 32, 124, 32, 32, 40, 32, 32, 60, 95, 62, 32, 41, 95, 95, 95, 32, 92, 32, 32, 186, 32, 32, 32, 32, 32, 32, 47, 32, 32, 32, 46, 58, 58, 46, 32, 32, 32, 92};
	cantidad = sizeof(valoresAscii12) / sizeof(valoresAscii12[0]);
	imprimirSimbolosCentrado(valoresAscii12, cantidad, anchoConsola);
	int valoresAscii13[94] = {47, 32, 32, 46, 250, 58, 58, 58, 58, 250, 46, 32, 32, 92, 32, 32, 32, 32, 32, 186, 32, 32, 92, 95, 95, 95, 95, 95, 95, 32, 32, 47, 95, 95, 95, 124, 32, 32, 47, 95, 95, 124, 32, 32, 32, 92, 95, 95, 95, 95, 47, 124, 95, 95, 95, 124, 32, 32, 47, 92, 95, 95, 95, 95, 47, 95, 95, 95, 95, 32, 32, 62, 32, 186, 32, 32, 32, 32, 32, 47, 32, 32, 46, 250, 58, 58, 58, 58, 250, 46, 32, 32, 92};
	cantidad = sizeof(valoresAscii13) / sizeof(valoresAscii13[0]);
	imprimirSimbolosCentrado(valoresAscii13, cantidad, anchoConsola);
	int valoresAscii14[96] = {124, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 124, 32, 32, 32, 32, 186, 32, 32, 32, 32, 32, 32, 32, 32, 32, 92, 47, 32, 32, 32, 32, 32, 92, 47, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 92, 47, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 92, 47, 32, 32, 186, 32, 32, 32, 32, 124, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 207, 124};
	cantidad = sizeof(valoresAscii14) / sizeof(valoresAscii14[0]);
	imprimirSimbolosCentrado(valoresAscii14, cantidad, anchoConsola);
	int valoresAscii15[56] = {200, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 188};
	cantidad = sizeof(valoresAscii15) / sizeof(valoresAscii15[0]);
	imprimirSimbolosCentrado(valoresAscii15, cantidad, anchoConsola);
	printf ("\n");
}

void Menu(int anchoConsola){
	char bordeSuperiorM[39] = {218, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 191};
	char tituloMenuM[39] = {179, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 77, 101, 110, 163, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 179};
	char bordeMedioM[39] = {195, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 194, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 180};
	char alarmMenuM[39] = {179, 32, 65, 32, 32, 32, 32, 32, 32, 32, 32, 32, 179, 32, 65, 108, 97, 114, 109, 97, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 179};
	char cronoMenuM[39] = {179, 32, 67, 32, 32, 32, 32, 32, 32, 32, 32, 32, 179, 32, 67, 114, 111, 110, 162, 109, 101, 116, 114, 111, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 179};
	char tempoMenuM[39] = {179, 32, 84, 32, 32, 32, 32, 32, 32, 32, 32, 32, 179, 32, 84, 101, 109, 112, 111, 114, 105, 122, 97, 100, 111, 114, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 179};
	char pausaMenuM[39] = {179, 32, 69, 115, 112, 97, 99, 105, 111, 32, 32, 32, 179, 32, 73, 110, 105, 99, 105, 111, 47, 80, 97, 117, 115, 97, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 179};
	char introMenuM[39] = {179, 32, 73, 110, 116, 114, 111, 32, 32, 32, 32, 32, 179, 32, 40, 67, 41, 71, 117, 97, 114, 100, 97, 114, 47, 40, 84, 41, 67, 97, 110, 99, 101, 108, 97, 114, 32, 179};
	char retroMenuM[39] = {179, 32, 82, 101, 116, 114, 111, 99, 101, 115, 111, 32, 179, 32, 82, 101, 103, 114, 101, 115, 97, 114, 32, 97, 108, 32,77, 101, 110, 163, 32, 32, 32, 32, 32, 32, 32, 179};
	char escMenuM[39] = {179, 32, 69, 115, 99, 32, 32, 32, 32, 32, 32, 32, 179, 32, 83, 97, 108, 105, 114, 32, 97, 108, 32, 69, 115, 99, 114, 105, 116, 111, 114, 105, 111, 32, 32, 32, 32, 179};
	char bordeInferiorM[39] = {192, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 193, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 217};
	cambiarColorTexto("rojo");	
	imprimirCentrado(bordeSuperiorM, anchoConsola);	
	imprimirCentrado(tituloMenuM, anchoConsola);	
	imprimirCentrado(bordeMedioM, anchoConsola);
	imprimirCentrado(alarmMenuM, anchoConsola);	
	imprimirCentrado(cronoMenuM, anchoConsola);	
	imprimirCentrado(tempoMenuM, anchoConsola);
	imprimirCentrado(pausaMenuM, anchoConsola);
	imprimirCentrado(introMenuM, anchoConsola);
	imprimirCentrado(retroMenuM, anchoConsola);
	imprimirCentrado(escMenuM, anchoConsola);
	imprimirCentrado(bordeInferiorM, anchoConsola);
	cambiarColorTexto("blanco");
}

void borradoLineas(int nlineas){
	for (i = 0; i < nlineas; i++){
		printf ("\x1b[A\x1b[M");
		Sleep (40);
	}
}

void alarmaMenu(int anchoConsola){
	printf ("\n");
	cambiarColorTexto("verde");
	char bordeSuperiorAM[65] = {218, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 191};
	char tituloAM[65] = {179, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 65, 108, 97, 114, 109, 97, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 179};
	char bordeMedioAM[65] = {195, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 194, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 194, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 180};
	char menuAlarmaAM[65] = {179, 32, 69, 115, 112, 97, 99, 105, 111, 32, 61, 32, 82, 101, 105, 110, 105, 99, 105, 97, 114, 32, 97, 108, 97, 114, 109, 97, 32, 179, 32, 82, 101, 116, 114, 111, 99, 101, 115, 111, 32, 61, 32, 65, 116, 114, 160, 115, 32, 179, 32, 69, 115, 99, 32, 61, 32, 83, 97, 108, 105, 114, 32, 179};
	char bordeInferiorAM[65] = {192, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 193, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 193, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 217};
	imprimirCentrado(bordeSuperiorAM, anchoConsola);	
	imprimirCentrado(tituloAM, anchoConsola);	
	imprimirCentrado(bordeMedioAM, anchoConsola);	
	imprimirCentrado(menuAlarmaAM, anchoConsola);	
	imprimirCentrado(bordeInferiorAM, anchoConsola);
	cambiarColorTexto("blanco");
}

void cronometroMenu(int anchoConsola){
	printf ("\n");
	cambiarColorTexto("cian");
	char bordeSuperiorCM[79] = {218, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 191};
	imprimirCentrado(bordeSuperiorCM, anchoConsola);
	char tituloCM[79] = {179, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 67, 114, 111, 110, 162, 109, 101, 116, 114, 111, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 179};
	imprimirCentrado(tituloCM, anchoConsola);
	char bordeMedioCM[79] = {195, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 194, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 194, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 194, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 180};
	imprimirCentrado(bordeMedioCM, anchoConsola);
	char menuCronoCM[79] = {179, 32, 69, 115, 112, 97, 99, 105, 111, 32, 61, 32, 73, 110, 105, 99, 105, 111, 47, 80, 97, 117, 115, 97, 32, 179, 32, 73, 110, 116, 114, 111, 32, 61, 32, 71, 117, 97, 114, 100, 97, 114, 32, 179, 32, 82, 101, 116, 114, 111, 99, 101, 115, 111, 32, 61, 32, 65, 116, 114, 160, 115, 32, 179, 32, 69, 115, 99, 32, 61, 32, 83, 97, 108, 105, 114, 32, 179};
	imprimirCentrado(menuCronoCM, anchoConsola);
	char bordeInferiorCM[79] = {192, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 193, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 193, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 193, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 217};
	imprimirCentrado(bordeInferiorCM, anchoConsola);
	cambiarColorTexto("blanco");
}

void temporizadorMenu(int anchoConsola){
	printf ("\n");
	cambiarColorTexto("amarillo");
	char bordeSuperiorTM[80] = {218, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 191};
	imprimirCentrado(bordeSuperiorTM, anchoConsola);
	char tituloTM[80] = {179, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 84, 101, 109, 112, 111, 114, 105, 122, 97, 100, 111, 114, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 179};
	imprimirCentrado(tituloTM, anchoConsola);
	char bordeMedioTM[80] = {195, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 194, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 194, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 194, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 180};
	imprimirCentrado(bordeMedioTM, anchoConsola);
	char menuTempoTM[80] = {179, 32, 69, 115, 112, 97, 99, 105, 111, 32, 61, 32, 73, 110, 105, 99, 105, 111, 47, 80, 97, 117, 115, 97, 32, 179, 32, 73, 110, 116, 114, 111, 32, 61, 32, 67, 97, 110, 99, 101, 108, 97, 114, 32, 179, 32, 82, 101, 116, 114, 111, 99, 101, 115, 111, 32, 61, 32, 65, 116, 114, 160, 115, 32, 179, 32, 69, 115, 99, 32, 61, 32, 83, 97, 108, 105, 114, 32, 179};
	imprimirCentrado(menuTempoTM, anchoConsola);
	char bordeInferiorTM[80] = {192, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 193, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 193, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 193, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 196, 217};
	imprimirCentrado(bordeInferiorTM, anchoConsola);
	cambiarColorTexto("blanco");
}

int main(){
	int h, m, s, ds, j, anchoConsola, altoConsola, espacioBlanco, decAct, tCreg;
	int horaLocal[4] = {0};
	char c = '\0';
	
	ModoPantallaCompleta();
	obtenerSizeConsola(&anchoConsola, &altoConsola);
	OcultarCursor();
	OcultarPunteroRaton();
	printf ("\n\n\n");
	ReproducirAudio("IDR_WAVE1", 1);
	animacionLogo(anchoConsola);
	DetenerAudio();
	ReproducirAudio("IDR_WAVE7", 0);
	Sleep (8000);
	
	while (1){
		c = '\0';
		Menu(anchoConsola);
		system ("");
		
		ReproducirAudio("IDR_WAVE6", 1);
	
		do {
			time_t t = time(NULL);
        	struct tm tiempoLocal = *localtime(&t);        	
        	clock_t start = clock();
        	decAct = (start % CLOCKS_PER_SEC) / (CLOCKS_PER_SEC / 10);
        	
        	horaLocal[0] = tiempoLocal.tm_hour;
        	horaLocal[1] = tiempoLocal.tm_min;
        	horaLocal[2] = tiempoLocal.tm_sec;
        	horaLocal[3] = decAct;
        	
        	imprimirTiempoCentrado (horaLocal, anchoConsola);
        	Sleep (100);
        	
        	if (_kbhit()){
        		c = getch();
			}
		}while (c != 'C' && c != 'c' && c != 'A' && c != 'a' && c != 'T' && c != 't' && c != 27);
		
		DetenerAudio();
		ReproducirAudio("IDR_WAVE3", 0);
	
		if (c == 'C' || c == 'c'){														//Cronómetro
			char LA[14] = "\x1b[38;5;11m";
			char LN[13] = "\x1b[38;5;0m";
			char FA[14] = "\x1b[48;5;11m";
			char FN[13] = "\x1b[48;5;0m";
			char AP[26] = {0};
			int regTc = 1;
			c = '\0';
			
			borradoLineas(12);
			printf ("\x1b[5M");
			cronometroMenu(anchoConsola);
			printf ("\x1b[3E");
						
			for (h = 0; h < 24; h++){
				for (m = 0; m < 60; m++){
					for (s = 0; s < 60; s++){
						for (ds = 0; ds < 10; ds++){
							int tiempo[4] = {h, m, s, ds};
							imprimirTiempoCentrado(tiempo, anchoConsola);
							if (h == 0 && m == 0 && s == 0 && ds == 0){
								do {
									c = getch();
								}while (c != ' ' && c != 8 && c != 27);
								ReproducirAudio("IDR_WAVE3", 0);
								Sleep(100);
								ReproducirAudio("IDR_WAVE6", 1);
							}
							if (kbhit()){
								c = getch();
								if (c == 13 || c == ' ' || c == 8 || c == 27){
									DetenerAudio();
									ReproducirAudio("IDR_WAVE3", 0);
								}
								if (c == 13){
									c = '\0';
									if (regTc == 10){
										regTc++;
									}
									else if (regTc < 10){
										printf ("\x1b[6E");
										espacioBlanco = (anchoConsola - 216) / 2;
										for (tCreg = 0; tCreg < espacioBlanco; tCreg++){
											printf ("\x1b[C");
										}
										if (regTc > 1){
											j = 1;
											do {
												printf ("\x1b[22C");
												if (j > 1){
													printf ("\x1b[2C");
												}										
												j++;
											}while(j < regTc);
											printf ("\x1b[38;5;12m|\x1b[0m ");
										}
										printf ("\x1b[38;5;14m\x1b[4m%02d:%02d:%02d.%d\x1b[0m \x1b[38;5;11m- \x1b[38;5;9m%i pausa.\x1b[0m\x1b[6F", h, m, s, ds, regTc);
										regTc++;
									}								
									h = m = s = ds = tiempo[0] = tiempo[1] = tiempo[2] = tiempo[3] = 0;
									imprimirTiempoCentrado(tiempo, anchoConsola);
									if (regTc == 11) {
										regTc++;
										espacioBlanco = (anchoConsola - 203) / 2;
										printf ("\x1b[2F");
										j = 1;
										while (c != ' ' && c != 8 && c != 27) {
											if (j == 1){
												j++;
												strcpy (AP, LA);
												strcat (AP, FN);
											}
											else if (j == 2){
												j--;
												strcpy (AP, LN);
												strcat (AP, FA);
											}
											printf ("\x1b[0m");
											for (i = 0; i < espacioBlanco; i++) {
												printf (" ");
											}									
											printf ("%s\x1B[1mHa alcanzado el m\xA0ximo de almacenamiento de pausas disponibles. Puede seguir pausando y reanundando con el \"espacio\" o reiniciando con el \"intro\" el cron\xA2metro, pero no se registrar\xA0n los nuevos tiempos.\r", AP);
											Sleep (333);
											if (_kbhit()){
												c = getch();
											}									
										}
										if (c == ' ' || c == 8 || c == 27) {
											ReproducirAudio("IDR_WAVE3", 0);											
										}								
									}
									while (c != ' ' && c != 8 && c != 27) {
										c = getch();
										if (c == ' ' || c == 8 || c == 27) {
											ReproducirAudio("IDR_WAVE3", 0);
										}																			
									}
									
									if (c == ' '){
										c = '\0';
										Sleep(100);
										ReproducirAudio("IDR_WAVE6", 1);
									}
									if (regTc == 12){
										regTc++;
										printf ("\x1b[2E\x1b[0m");
									}							
								}	
								if (c == ' '){
									c = '\0';
									while (c != ' ' && c != 8 && c != 27) {
										c = getch();
									}
									if (c == ' ' || c == 8 || c == 27) {
										ReproducirAudio("IDR_WAVE3", 0);
										if (c == ' '){
											Sleep(100);
											ReproducirAudio("IDR_WAVE6", 1);
										}
									}
								}										
							}
							if (c == 8 || c == 27){
									h = 24, m = 60, s = 60, ds = 10;
									if (c == 8){
										printf ("\x1b[9F\x1b[16M\n");
									}									
								}
							Sleep (100);
						}	
					}
				}
			}
		}
		else if (c == 'A' || c == 'a'){													// Alarma
			char infoAlarm1[104] = {73, 110, 116, 114, 111, 100, 117, 99, 101, 32, 101, 108, 32, 116, 105, 101, 109, 112, 111, 32, 112, 97, 114, 97, 32, 105, 110, 105, 99, 105, 97, 114, 32, 108, 97, 32, 97, 108, 97, 114, 109, 97, 46, 32, 69, 108, 32, 108, 161, 109, 105, 116, 101, 32, 109, 160, 120, 105, 109, 111, 32, 100, 101, 32, 116, 105, 101, 109, 112, 111, 32, 112, 97, 114, 97, 32, 99, 97, 100, 97, 32, 112, 111, 115, 105, 99, 105, 162, 110, 32, 101, 115, 32, 50, 51, 58, 53, 57, 58, 53, 57, 46, 57};
			char infoAlarm2[109] = {83, 105, 32, 112, 117, 108, 115, 97, 115, 32, 117, 110, 32, 110, 163, 109, 101, 114, 111, 32, 109, 97, 121, 111, 114, 32, 112, 97, 114, 97, 32, 97, 108, 103, 117, 110, 97, 32, 100, 101, 32, 108, 97, 115, 32, 112, 111, 115, 105, 99, 105, 111, 110, 101, 115, 44, 32, 115, 101, 32, 101, 115, 116, 97, 98, 108, 101, 99, 101, 114, 160, 32, 101, 108, 32, 118, 97, 108, 111, 114, 32, 109, 160, 120, 105, 109, 111, 32, 112, 97, 114, 97, 32, 100, 105, 99, 104, 97, 32, 112, 111, 115, 105, 99, 105, 162, 110, 46};
			int ha, ma, sa, dsa, sonidoAlarma;
			borradoLineas(12);
			printf ("\x1b[5M");
			alarmaMenu(anchoConsola);
			
			do {
				c = '\0';
				ha = ma = sa = 88;
				dsa = 8;
				int tiempo[4] = {ha, ma, sa, dsa};
				
				printf ("\n");
				espacioBlanco = (anchoConsola - 103) / 2;			
				for (i = 0; i < espacioBlanco; i++){
					printf(" ");
				}
				
				ReproducirAudio("IDR_WAVE5", 1);
							
				for (i = 0; i < 103; i++){
					if (i > 12 && i < 19 || i > 35 && i < 42 || i > 63 && i < 70 || i > 80 && i < 89 || i > 92 && i < 103){
						printf ("\x1b[38;5;14m");
					}
					else if (i > 46 && i < 60){
						printf ("\x1b[38;5;9m");
					}
					else if (i >= 0 && i < 9 || i > 24 && i < 32){
						printf ("\x1b[38;5;11m");
					}
					else {
						printf ("\x1b[38;5;10m");
					}
					printf("%c", infoAlarm1[i]);				
					Sleep (10);
				}
				
				printf ("\n");
				espacioBlanco = (anchoConsola - 108) / 2;
				for (i = 0; i < espacioBlanco; i++){
					printf(" ");
				}
				
				for (i = 0; i < 108; i++){
					if (i > 44 && i < 55 || i > 98 && i < 107){
						printf ("\x1b[38;5;14m");
					}
					else if (i > 12 && i < 25 || i > 74 && i < 87){
						printf ("\x1b[38;5;9m");
					}
					else if (i >= 2 && i < 9){
						printf ("\x1b[38;5;11m");
					}
					else {
						printf ("\x1b[38;5;10m");
					}
					printf("%c", infoAlarm2[i]);				
					Sleep (10);
				}
				DetenerAudio();			
				printf ("\n\n");
				
				imprimirTiempoCentrado(tiempo, anchoConsola);
				solicitarTiempo(tiempo, anchoConsola);
				escogeSonidoAlarma(&sonidoAlarma, anchoConsola);
				printf ("\x1b[3F\x1b[8M");
				ha = tiempo[0];
				ma = tiempo[1];
				sa = tiempo[2];
				dsa = tiempo[3];
				
				espacioBlanco = (anchoConsola - 48) / 2;
				for (i = 0; i < espacioBlanco; i++){
					printf(" ");
				}
				printf ("\x1b[38;5;10mLa alarma se ha establecido para las: \x1b[38;5;14m%02d:%02d:%02d.%d\x1b[0m\n\n", ha, ma, sa, dsa);
				
				ReproducirAudio("IDR_WAVE6", 1);
				
				do {
					time_t t = time(NULL);
        			struct tm tiempoLocal = *localtime(&t);        	
        			clock_t start = clock();
        			decAct = (start % CLOCKS_PER_SEC) / (CLOCKS_PER_SEC / 10);
        	
        			horaLocal[0] = tiempoLocal.tm_hour;
        			horaLocal[1] = tiempoLocal.tm_min;
        			horaLocal[2] = tiempoLocal.tm_sec;
        			horaLocal[3] = decAct;
        	
        			imprimirTiempoCentrado (horaLocal, anchoConsola);
        			if (tiempoLocal.tm_hour == ha && tiempoLocal.tm_min == ma && tiempoLocal.tm_sec == sa){
        				avisoAlarma(&c, sonidoAlarma, anchoConsola);
        				if (c == 8){
        					printf ("\x1b[3F\x1b[3M");
						}
        				goto finAlarma;
					}
        			Sleep (100);
        	
        			if (_kbhit()){
        				c = getch();
					}
				} while (c != ' ' && c != 8 && c != 27);
				finAlarma:
				if (c == ' '){
					printf ("\x1b[3F\x1b[9M");
					ReproducirAudio("IDR_WAVE3", 0);
				}
				else if (c == 8){
					printf ("\x1b[8F\x1b[15M");
					ReproducirAudio("IDR_WAVE3", 0);
				}
			} while (c != 8 && c != 27);	
		}
		else if (c == 'T' || c == 't'){													// Temporizador
			char infoTemp1[126] = {73, 110, 116, 114, 111, 100, 117, 99, 101, 32, 101, 108, 32, 116, 105, 101, 109, 112, 111, 32, 121, 32, 112, 117, 108, 115, 97, 32, 101, 115, 112, 97, 99, 105, 111, 32, 112, 97, 114, 97, 32, 105, 110, 105, 99, 105, 97, 114, 32, 101, 108, 32, 116, 101, 109, 112, 111, 114, 105, 122, 97, 100, 111, 114, 46, 32, 69, 108, 32, 108, 161, 109, 105, 116, 101, 32, 109, 160, 120, 105, 109, 111, 32, 100, 101, 32, 116, 105, 101, 109, 112, 111, 32, 112, 97, 114, 97, 32, 99, 97, 100, 97, 32, 112, 111, 115, 105, 99, 105, 162, 110, 32, 101, 115, 32, 50, 51, 58, 53, 57, 58, 53, 57, 46, 57};
			char infoTemp2[109] = {83, 105, 32, 112, 117, 108, 115, 97, 115, 32, 117, 110, 32, 110, 163, 109, 101, 114, 111, 32, 109, 97, 121, 111, 114, 32, 112, 97, 114, 97, 32, 97, 108, 103, 117, 110, 97, 32, 100, 101, 32, 108, 97, 115, 32, 112, 111, 115, 105, 99, 105, 111, 110, 101, 115, 44, 32, 115, 101, 32, 101, 115, 116, 97, 98, 108, 101, 99, 101, 114, 160, 32, 101, 108, 32, 118, 97, 108, 111, 114, 32, 109, 160, 120, 105, 109, 111, 32, 112, 97, 114, 97, 32, 100, 105, 99, 104, 97, 32, 112, 111, 115, 105, 99, 105, 162, 110, 46};
			int ht, mt, st, dst;
			
			borradoLineas(12);
			printf ("\x1b[5M");
			temporizadorMenu(anchoConsola);
			
			do {
				reinicioTempo:
				j = 0;
				c = '\0';
				ht = mt = st = 88;
				dst = 8;
				int tiempo[4] = {ht, mt, st, dst};
				int hTp = 0, mTp = 0, sTp = 0, dsTp = 0, pausa[4] = {0};
				
				printf ("\n");
				espacioBlanco = (anchoConsola - 125) / 2;			
				for (i = 0; i < espacioBlanco; i++){
					printf(" ");
				}
				
				ReproducirAudio("IDR_WAVE5", 1);
							
				for (i = 0; i < 125; i++){
					if (i > 12 && i < 19 || i > 51 && i < 64 || i > 85 && i < 92 || i > 102 && i < 111 || i > 114 && i < 125){
						printf ("\x1b[38;5;14m");
					}
					else if (i > 68 && i < 82){
						printf ("\x1b[38;5;9m");
					}
					else if (i >= 0 && i < 9 || i > 21 && i < 35 || i > 40 && i < 48){
						printf ("\x1b[38;5;10m");
					}
					else {
						printf ("\x1b[38;5;11m");
					}
					printf("%c", infoTemp1[i]);				
					Sleep (10);
				}
				
				printf ("\n");
				espacioBlanco = (anchoConsola - 108) / 2;
				for (i = 0; i < espacioBlanco; i++){
					printf(" ");
				}
				
				for (i = 0; i < 108; i++){
					if (i > 44 && i < 55 || i > 98 && i < 107){
						printf ("\x1b[38;5;14m");
					}
					else if (i > 12 && i < 25 || i > 74 && i < 87){
						printf ("\x1b[38;5;9m");
					}
					else if (i >= 2 && i < 9){
						printf ("\x1b[38;5;10m");
					}
					else {
						printf ("\x1b[38;5;11m");
					}
					printf("%c", infoTemp2[i]);				
					Sleep (10);
				}
				DetenerAudio();			
				printf ("\n\n");			
			
				imprimirTiempoCentrado(tiempo, anchoConsola);
				solicitarTiempo(tiempo, anchoConsola);				
				printf ("\x1b[3A\x1b[8M\x1b[2E");
				ht = tiempo[0];
				mt = tiempo[1];
				st = tiempo[2];
				dst = tiempo[3];
				
				for (h = ht; h > -1; h--){
					for (m = mt; m > -1; m--){
						for (s = st; s > -1; s--){
							for (ds = dst; ds > -1; ds--){
								int tiempo[4] = {h, m, s, ds};
								imprimirTiempoCentrado(tiempo, anchoConsola);
								if (h == ht && m == mt && s == st && ds == dst){
									do {
										c = getch();
									}while (c != ' ' && c != 8 && c != 13 && c != 27);
									ReproducirAudio("IDR_WAVE3", 0);
									Sleep (100);
									ReproducirAudio("IDR_WAVE6", 1);
									ht = 23;
									mt = st = 59;
									dst = 9;
								}
								if (kbhit()){
									c = getch();
									if (c == 13 || c == ' ' || c == 8 || c == 27){
										DetenerAudio();
										ReproducirAudio("IDR_WAVE3", 0);
									}									
									if (c == ' '){
										c = '\0';
										printf ("\x1b[2F");
										for (hTp = pausa[0]; hTp < 24; hTp++){
											for (mTp = pausa[1]; mTp < 60; mTp++){
												for (sTp = pausa[2]; sTp < 60; sTp++){
													for (dsTp = pausa[3]; dsTp < 10; dsTp++){
														pausa[0] = hTp;
														pausa[1] = mTp;
														pausa[2] = sTp;
														pausa[3] = dsTp;
														imprimirPausaCentrado(pausa, anchoConsola);					
														if (kbhit()){
															c = getch();
															if (c == ' '){
																numeroEnTexto(pausa, anchoConsola);
																ReproducirAudio("IDR_WAVE3", 0);
																Sleep (100);
																ReproducirAudio("IDR_WAVE6", 1);																							
																goto salidaPausa;
															} else if (c == 13){
																c = '\0';
																ReproducirAudio("IDR_WAVE3", 0);
																printf ("\x1b[A\x1b[8M");
																goto reinicioTempo;
															} else if (c == 8 || c == 27){
																printf("\x1b[2B");
																goto endTempo;
															}					
														}
														Sleep (100);
														if (hTp == 23 && mTp == 59 && sTp == 59 && dsTp == 9){
															goto salidaPausa;
														}
														if (dsTp == 9){
															pausa[3] = 0;
														}
														if (sTp == 59){
															pausa[2] = 0;
														}
														if (mTp == 59){
															pausa[1] = 0;
														}														
													}	
												}
											}
										}
										salidaPausa:										
										c = '\0';									
									}					
								}
								if (c == 13){
									c = '\0';
									DetenerAudio();
									ReproducirAudio("IDR_WAVE3", 0);
									printf ("\x1b[3A\x1b[8M");
									goto reinicioTempo;
								}
								else if (c == 8 || c == 27){									
									goto endTempo;
								}
								Sleep (100);								
							}	
						}
					}
				}
				avisoAlarma(&c, 0, anchoConsola);
				printf ("\x1b[3A\x1b[8M");
				endTempo:
				if (c == 8){
					DetenerAudio();
					ReproducirAudio("IDR_WAVE3", 0);
					printf ("\x1b[9F\x1b[16M\n");
				}			
			} while (c != 8 && c != 27);
		}
		if (c == 27){
			DetenerAudio();
			ReproducirAudio("IDR_WAVE3", 0);
			system ("cls");
			Sleep (500);
			animacionDespedida(anchoConsola, altoConsola);
			Sleep (1500);
			DetenerAudio();
			MostrarPunteroRaton();
			return 0;
		}
	}
}
