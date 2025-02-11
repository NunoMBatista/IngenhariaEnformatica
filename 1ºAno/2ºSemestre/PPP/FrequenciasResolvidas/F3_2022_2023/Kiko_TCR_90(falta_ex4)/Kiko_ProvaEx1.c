#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct data {
    int dia, mes, ano;
} data;
struct pessoa {
    char * nome;
    struct data * data_nasc;
} pessoa;


void add_pessoa (int pos, char * nome, struct data d, struct pessoa pessoas []) {
    pessoas[pos].nome = (char*)malloc(sizeof(char));
    strcpy(pessoas[pos].nome, nome);
    pessoas[pos].data_nasc = (struct data *)malloc (sizeof (struct data));
    memcpy(pessoas[pos].data_nasc, &d, sizeof(struct data));
}

int main (){
struct data dt; char nome [50]; int qt;
printf ("Quantas pessoas pretende registar: "); scanf (" %d", &qt);
struct pessoa *pessoas = (struct pessoa *)malloc((qt)*sizeof(struct pessoa));
for (int i=0; i<qt; i++){
printf ("Nome data de nascimento (nome dia/mes/ano): ");
scanf (" %s %d/ %d/ %d", nome, &dt.dia, &dt.mes, &dt.ano);
add_pessoa (i, nome, dt, pessoas);
}
/* TODO fazer o resto do programa */
for (int i=0; i<qt; i++){
    printf ("Nome: %s\n", pessoas[i].nome);
    printf ("Data de nascimento: %d/%d/%d\n", pessoas[i].data_nasc->dia, pessoas[i].data_nasc->mes, pessoas[i].data_nasc->ano);
}

return 0;
}