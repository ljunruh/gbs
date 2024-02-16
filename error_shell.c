#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



int main(){

    //Größe des Eingabestrings
    char cmd[1024];

    // So sieht dann die Eingabe der eigenen Shell aus
    printf("$$: ");
    
    /*
    fflush(stdout) sorgt dafür, dass der Puffer aller Daten gezwungen wird auf die
    Standartausgabe geschrieben wird. Das ist wichtig, weil in C die Ausgabedaten oft
    gesammelt werden und zu anderen Bedingungen geschrieben wird als wir es hier in diesem
    Programm haben wollen.
    */
    fflush(stdout);


    /*
    Hier liest fgets() eine Zeile Text von der Standardeingabe (stdin) und speichert 
    sie im Array cmd, wobei maximal 1023 Zeichen gelesen werden, um Platz für das 
    abschließende Nullzeichen zu lassen, das das Ende der Zeichenkette markiert.
    Hier mit Fehlerbehandlung, falls fgets() NULL zurückgibt, wird die Shell beendet.
    */
    if(fgets(cmd, sizeof(cmd), stdin) == NULL){
        perror("Fehler beim Lesen des Befehls");
        return 1;
    }

    /*
    Damit sichergestellt werden kann, dass die Zeichenkette "cmd" korrekt für weitere Schritte
    formatiert ist müssen wir auf ein "\n" prüfen und es ggf. entfernen.
    Viele Funktionen in C erwarten das Nullzeichen am Ende eines Strings und ein verbleibendes
    Zeilenendenzeichen könnte unerwünschte Effekte haben.
    */
    //Hier also: Wenn das letzte Element in dem cmd String ein "\n" ist wird es durch "\0" ersetzt.
    if (cmd[strlen(cmd) - 1] == '\n') {
        cmd[strlen(cmd) - 1] = '\0'; // Entferne den Zeilenumbruch am Ende
    }


    /*
    Mit dem Systemaufruf fork() wird ein neuer Kindprozess erstellt, welcher eine Kopie des Elternprozesses ist.
    Bei fork(), werden beide nun vorhandenen Prozesse ihre Ausführung fortsetzen.
    fork() gibt im Elternprozess die PID des Kindprozesses zurück und im Kindprozess 0.
    Wenn kein Kindprozess erstellt werden kann gibt fork() im Elternprozess -1 zurück.
    Somit unterscheidet man zwischen Eltern und Kindprozess durch den Rückgabewert von fork().
    */
    int pid = fork();
    if(pid == -1) { // Rückgabe von -1 bedeutet Fehler.
        perror("fork failed");
        return 1;
    } else if (pid > 0){
        /*
        Weil fork() die PID des Kindes zurückgibt passiert alles was in pid größer 0 ist im 
        Elternprozess.
        */
        printf("Started [%s] pid=%d\n", cmd, pid);

        /*
        Wenn der Elternprozess waitpid() aufruft, wartet er auf die Beendigung des Kindprozesses und 
        speichert dessen Beendigungsstatus in der Variable status.
        */
        int status;
        waitpid(pid, &status, 0);

        /*
        Anschließend wird überprüft, ob der Kindprozess normal beendet wurde (WIFEXITED).
        Ist dies der Fall, gibt der Elternprozess eine Meldung aus, die die PID des Kindprozesses
        und dessen Exit-Status (exitstatus) enthält.
        */
        if (WIFEXITED(status)) {
            printf("Exited pid=%d exitstatus=%d\n", pid, WEXITSTATUS(status));
        
    } else {
        // Hier ist die PID == 0, also sind wir im Kindprozess.
        // Teile den Befehlsstring in Argumente auf
        char *argv[64]; // Angenommen, es gibt nicht mehr als 64 Argumente
        int argc = 0;
        argv[argc] = strtok(cmd, " ");
        while (argv[argc] != NULL && argc < 63) {
            argv[++argc] = strtok(NULL, " ");
        }
        
        execvp(argv[0], argv);
        // Wenn execvp zurückkehrt, trat ein Fehler auf
        perror(argv[0]);
        exit(EXIT_FAILURE);
    }

    return 0;

    }


    
    
}