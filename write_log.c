#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
Diese Funktion schreibt eine Nachricht in eine Log-Datei im APPEND-Modus. Existiert die Datei nicht, wird sie automatisch erstellt mit den lesenden und schreibenden Rechten für den User.
Das was geschrieben wird, wird vorher in der main-Funktion festgelegt. Dient als Übung dafür die man mit Dateien auf Systemebene umgeht und mit ihnen interagiert.
*/
void write_log(const char *log_msg){
    // hier aufpassen: Gänsefüßchen nehmen statt '
    const char *log_file = "error.log";

    //hier wichtig: die Flags mit einem ODER trennen und nicht mit einem Komma.
    //0660 sind hier die Dateirechte -rw-r----- ausgewählt. Steht sonst alles in der manpage. Alternativ geht auch: S_IRWXU
    //fd bedeutet Filedeskriptor, weil hier bitweise gelesen wird.
    int fd = open(log_file, O_WRONLY | O_APPEND | O_CREAT, 0660);

    //Weil Bitweise ausgelesen wird, gibt open() -1 zurück, wenn ein Fehler passiert ist.
    if(fd == -1){
        perror("Fehler bei open ");
        return;
    }

    /*
    Geschrieben wird hier mit ssize_t write(int fd, const void *buf, size_t count);
    Es wird ein buffer der Länge log_msg in die Datei aus fd geschrieben.
    write gibt die Anzahl der Bytes zurück die geschrieben werden, wenn es Fehlschlägt, dann -1.
    Der Systemaufruf write() wird in der if-Schleife ausgeführt und dann auf Richtigkeit überprüft.
    Weil write() keine Zeilenumbrüche verarbeiten kann muss dieser separat aufgeführt werden.
    */

    //Schreiben, wenn es nicht klappt dann Fehlermeldung ausgeben und die Datei schließen.
    if(write(fd, log_msg, strlen(log_msg)) == -1){
        perror("Fehler bei write");
        close(fd);
        return;
    }

    //Schreiben, wenn es nicht klappt dann Fehlermeldung ausgeben und die Datei schließen.
    if(write(fd, "\n", 1) == -1){
        perror("Fehler bei write");
        close(fd);
        return;
    }

    //Datei ordnungsgemäß schließen, wenn es geklappt hat.
    close(fd);

}

int main(void){
    write_log("Test-Log Nachricht");
    return 0;
}