.386
.MODEL FLAT,STDCALL

EXTRN  GetStdHandle@4:NEAR
EXTRN  WriteConsoleA@20:NEAR
EXTRN  CharToOemA@8:NEAR
EXTRN  ReadConsoleA@20:NEAR
EXTRN  ExitProcess@4:NEAR
EXTRN  lstrlenA@4:NEAR

RMVSYMB MACRO SOURCE, SYM, COUNT
	MOV COUNT, 0
	MOV ESI, SOURCE
	MOV BL, [ESI]
	.WHILE BL != 0
		.IF BL == SYM
			PUSH ESI
			.WHILE BL != 0
				MOV EDI,ESI
				INC ESI
				MOV AL,[ESI]
				MOV [EDI],AL
				MOV BL, [ESI]
			.ENDW
			POP ESI
			DEC ESI
			INC COUNT
		.ENDIF
		INC ESI
		MOV BL, [ESI]
	.ENDW
ENDM

.DATA
DOUT DD ? ; дескриптор вывода
DIN DD ? ; дескриптор ввода
LENS DD ? ; длина строки
BUF  DB 1000 DUP (?) ; текст
BUF2 DB ? ; символ
IBUF DD ? ; адрес на текущего места в тексте
LENALL DD ? ; длина текста
STR1 DB "Введите символ: ",13,10,0
STR2 DB "Введите текст: ",13,10,0
STR3 DB "Результат: ",13,10,0

.CODE
START:
; Перекодируем строки
PUSH OFFSET STR1
PUSH OFFSET STR1
CALL CharToOemA@8
PUSH OFFSET STR2
PUSH OFFSET STR2
CALL CharToOemA@8
PUSH OFFSET STR3
PUSH OFFSET STR3
CALL CharToOemA@8
; Получим дескриптор ввода
PUSH -10
CALL GetStdHandle@4
MOV DIN, EAX
; Получим дескриптор вывода
PUSH -11
CALL GetStdHandle@4
MOV DOUT, EAX
; Вывод строки
PUSH OFFSET STR1
CALL lstrlenA@4
PUSH 0
PUSH OFFSET LENS
PUSH EAX
PUSH OFFSET STR1
PUSH DOUT
CALL  WriteConsoleA@20
; Ввод символа
PUSH 0
PUSH OFFSET LENS
PUSH 200
PUSH OFFSET BUF2
PUSH DIN
CALL ReadConsoleA@20
; Инициализация переменных
MOV LENALL,0
MOV IBUF, OFFSET BUF
; Вывод строки
PUSH OFFSET STR2
CALL lstrlenA@4
PUSH 0
PUSH OFFSET LENS
PUSH EAX
PUSH OFFSET STR2
PUSH DOUT
CALL  WriteConsoleA@20
; Цикл ввода текста
READ_NEXT:
; Ввод строки
PUSH 0
PUSH OFFSET LENS
PUSH 200
PUSH IBUF
PUSH DIN
CALL ReadConsoleA@20 
; Проверка окончания ввода
MOV ESI,IBUF
MOV BL, [ESI]
MOV AL, 13 ;101
.IF BL == AL
	JMP EOF_FIND
.ENDIF
; Удаление символов
RMVSYMB IBUF, BUF2, ECX
MOV EAX,IBUF
MOV EBX,LENS
SUB EBX,ECX
.IF EBX > 2
	ADD EAX,EBX
	MOV IBUF,EAX
	MOV EAX,LENALL
	ADD EAX,EBX
	MOV LENALL,EAX
.ENDIF
; Повтор цикла
JMP READ_NEXT
; Выход из цикла
EOF_FIND:
; Вывод строки
PUSH OFFSET STR3
CALL lstrlenA@4
PUSH 0
PUSH OFFSET LENS
PUSH EAX
PUSH OFFSET STR3
PUSH DOUT
CALL  WriteConsoleA@20
; Выводим результат
PUSH 0
PUSH OFFSET LENALL
PUSH LENALL
PUSH OFFSET BUF
PUSH DOUT
CALL  WriteConsoleA@20
; Небольшая задержка
MOV ECX, 0AFFFFFFFH
L1: LOOP L1
; Выход из программы 
PUSH 0
CALL ExitProcess@4
END START
