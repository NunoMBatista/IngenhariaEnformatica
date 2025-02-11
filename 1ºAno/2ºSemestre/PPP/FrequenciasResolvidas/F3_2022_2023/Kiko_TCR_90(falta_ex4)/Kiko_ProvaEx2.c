#include <stdio.h>
#include <string.h>

typedef struct aluno {
	char nome [50];
	int numero;
	char disciplinas[5][10];
} aluno;

void alteraDisciplina (FILE * f, char *d, int i, int n){
	aluno a;
	fseek(f, sizeof(aluno)*i, SEEK_SET);
	fread (&a, sizeof (struct aluno), 1, f);
	fseek(f, sizeof(aluno)*i, SEEK_SET);
	strcpy(a.disciplinas[n], d);
	fwrite(&a, sizeof(struct aluno), 1, f);
}

int main(){
	FILE * f = fopen("dados.bin", "r+");
	if (f!=NULL){
		alteraDisciplina (f, "novoNome", 3, 2); // alterou a disciplina 3 do aluno 2
		fflush(f);
	}
	return 0;
}