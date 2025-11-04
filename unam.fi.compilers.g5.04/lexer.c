#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef enum {
    PALABRA_CLAVE,
    IDENTIFICADOR, 
    OPERADOR,
    NUMERO,
    TEXTO,
    PUNTUACION
} TipoToken;

char *palabras_claves[] = {"print","int","float","char","void","if","else","for","while","return",
    "double","long","short","unsigned","signed","do","switch","case","default","break",
    "continue","sizeof","struct","union","typedef","const","volatile","static","extern","enum","include"};
int num_palabras = 31;

int letra(char c) { 
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; 
}

int digito(char c) { 
    return c >= '0' && c <= '9'; 
}

int operador(char c) {
    return strchr("+-*/=<>!&|", c) != NULL;
}

int puntuacion(char c) {
    return strchr("(){}[];,.:", c) != NULL;
}

void mostrar_token(TipoToken tipo, int *primero) {
    char *nombres[] = {"Keyword", "Identifier", "Operator", "Constant", "Literal", "Punctuation"};
    
    if (!*primero) printf(" ");
    printf("%s", nombres[tipo]);
    *primero = 0;
}

int palabra_clave(char *inicio, int largo) {
    for (int i = 0; i < num_palabras; i++) {

        if ((int)strlen(palabras_claves[i]) == largo && 
            strncmp(inicio, palabras_claves[i], largo) == 0)
            return 1;
    }
    return 0;
}

char *leer_archivo(FILE *archivo) {
    fseek(archivo, 0, SEEK_END);
    long tam = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);
    
    char *contenido = malloc(tam + 1);
    fread(contenido, 1, tam, archivo);
    contenido[tam] = '\0';
    return contenido;
}

int main(int argc, char **argv) {
    FILE *entrada = stdin;
    
    if (argc > 1) {
        entrada = fopen(argv[1], "r");
        if (!entrada) {
            printf("No se pudo abrir %s\n", argv[1]);
            return 1;
        }
    }
    
    char *codigo = leer_archivo(entrada);
    if (entrada != stdin) fclose(entrada);
    
    char *p = codigo;
    int total = 0, primero = 1;
    
    while (*p) {
        if (isspace(*p)) { 
            p++; 
            continue; 
        }
        
        // Saltar comentarios
        if (p[0] == '/' && p[1] == '/') {
            while (*p && *p != '\n') p++;
            continue;
        }
        
        // Texto entre comillas
        if (*p == '"') {
            p++;
            while (*p && *p != '"') {
                if (*p == '\\' && p[1]) p += 2;
                else p++;
            }
            if (*p == '"') p++;
            mostrar_token(TEXTO, &primero);
            total++;
            continue;
        }
        
        // Números
        if (digito(*p)) {
            while (digito(*p)) p++;
            mostrar_token(NUMERO, &primero);
            total++;
            continue;
        }
        
        // Identificadores y palabras clave
        if (letra
        (*p)) {
            char *inicio = p;
            while (letra
            (*p) || digito(*p)) p++;
            int largo = p - inicio;
            
            if (palabra_clave(inicio, largo))
                mostrar_token(PALABRA_CLAVE, &primero);
            else
                mostrar_token(IDENTIFICADOR, &primero);
            
            total++;
            continue;
        }
        
        // Operadores (+=, ==, etc)
        if (operador(*p)) {
            if ((p[0]=='=' && p[1]=='=') || (p[0]=='!' && p[1]=='=') ||
                (p[0]=='<' && p[1]=='=') || (p[0]=='>' && p[1]=='=') ||
                (p[0]=='&' && p[1]=='&') || (p[0]=='|' && p[1]=='|')) {
                p += 2;
            } else {
                p++;
            }
            mostrar_token(OPERADOR, &primero);
            total++;
            continue;
        }
        
        // Signos de puntuación
        if (puntuacion(*p)) {
            p++;
            mostrar_token(PUNTUACION, &primero);
            total++;
            continue;
        }
        
        p++; // Caracter no reconocido
    }
    
    printf("\nTotal of tokens: %d\n", total);
    free(codigo);
    return 0;
}