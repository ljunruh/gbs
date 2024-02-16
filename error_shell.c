#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
Die Error-Shell dient als Fehlerausgabe-Shell. Sie ersetzt die Shell in der sie gestartet wird indem mithilfe von fork() und exec() mit 
Eltern-Kindprozessen gearbeitet wird. Die Shell nimmt einen Befehl über die Standarteingabe an, führt ihn in einem Kindprozess aus und
schreibt Informationen über den Vorgang ins Terminal.
*/

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

        /*
        Es wird angenommen, dass ein Argument nicht mehr als 64 Befehle enthält.
        Diese Zahl ist willkürlich gewählt und kann durch alles ausgetauscht werden.
        Mithilfe von strtok() wird der Inhalt von cmd immer bei Auftreten eines Leerzeichens
        geteilt und in eine Sequenz von Token überführt (parsen). Trennzeichen (hier " ") werden
        durch Nullzeichen ('\0') ersetzt.
        Beim ersten ausführen von strtok() muss der Command mit einem Symbol angegeben werden, danach 
        muss die Funktion mit NULL übergeben werden (wie in der While-Schleife zu sehen).
        */
        char *argv[64];
        int argc = 0;
        argv[argc] = strtok(cmd, " ");

        //Wenn keine weiteren Token gefunden werden gibt strtok NULL zurück. Hier auch die Bedingung der maximalen Argumente beachten.
        while (argv[argc] != NULL && argc < 63) {
            //strtok mit NULL
            argv[++argc] = strtok(NULL, " ");
        }
        
        /*
        execvp() wird aufgerufen um den Befehl auszuführen, der im argv-Array gespeichert ist. argv[0] enthält den eigentlichen Befehl wie
        zB: "echo" oder "cat" und argv selbst ist das Array von den Argumenten wie zB: "1" oder "error.log" aus den Befehlen "echo 1" oder
        "cat error.log".
        Das Array muss mit NULL enden wofür die while-Schleife oben sorgt, damit sichergestellt werden kann, dass es keine weiteren Argumente gibt.
        execvp() ersetzt das aktuelle Programm im Speicher des Prozesses durch das neue Programm, das durch argv[0] spezifiziert wird. 
        Die Argumente für das neue Programm werden durch das argv-Array bereitgestellt. 
        Im Gegensatz zu einigen anderen exec-Varianten sucht execvp automatisch nach dem ausführbaren Programm im Systempfad (PATH), wenn der angegebene Befehl keinen Verzeichnispfad enthält.
        */
        execvp(argv[0], argv);
        /*
        Wenn execvp erfolgreich ist, wird das neue Programm ausgeführt, und execvp kehrt nicht zum aufrufenden Programm zurück.
        Das bedeutet, dass alle Codezeilen nach execvp nur ausgeführt werden, wenn ein Fehler auftritt (z.B. wenn das Programm nicht gefunden wird).
        */
        perror(argv[0]);
        exit(EXIT_FAILURE);
    }

    return 0;

    }    
}