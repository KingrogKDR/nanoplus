#include<unistd.h>
#include<termios.h>
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

#define CTRL_KEY(k) (k & 0x1f)

struct termios original_termios;

/* terminal work */

void die(const char *s){
    perror(s);
    exit(1);
}

void disable_raw_mode(){
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1) die("tcsetattr");
}

void enable_raw_mode(){
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1) die("tcgetattr");
    atexit(disable_raw_mode);

    struct termios raw = original_termios;
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

char editorReadKey(){
    char c;
    int readVar;
    while((readVar = read(STDIN_FILENO, &c, 1)) != 1){
        if(readVar == -1) die("read");
    }
    return c;
}

/* input */

void editorProcessKey(){
    char c = editorReadKey();
    switch(c){
        case CTRL_KEY('q'):
            exit(0);
            break;
    }
}

/* output */

void refreshScreen(){
    write(STDOUT_FILENO, "\x1b[2J", 4);
}


/* init */

int main(){
    enable_raw_mode();

    while(1){
        editorProcessKey();
    }

    return 0;
}