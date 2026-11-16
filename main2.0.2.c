#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* status: ' ' = disponivel, 'R' = reservado, '*' = excluido */
struct Livro
{
    char titulo[50];
    char autor[50];
    int ano;
    char status;
};

int  tamanho(FILE *arq);
void cadastrar(FILE *arq);
void consultar(FILE *arq);
void geraarqtxt(FILE *arq);
void excluir(FILE *arq);
void contabilizar(FILE *arq);
void listar(FILE *arq);
void reservar(FILE *arq);
void devolver(FILE *arq);
void mostrarLivro(struct Livro livro, int codigo);
void lerTexto(const char *pergunta, char *destino, int tam);
int  lerInteiro(const char *pergunta);
void pausar(void);


int main()
{
    int op;
    FILE *arq;  
    
    if ((arq = fopen("livros.dat", "rb+")) == NULL)
    {
        if ((arq = fopen("livros.dat", "wb+")) == NULL)
        {
            printf("Falha ao abrir o arquivo!\n");
            return 1;   
        }
    }

    do
    {
		system("CLS");
        printf("\n======= BIBLIOTECA ======= \n");
        printf("1.Cadastrar\n");
        printf("2.Consultar por codigo\n");
        printf("3.Gerar arquivo\n");
        printf("4.Excluir registro\n");
        printf("5.Contabilizar registros\n");
        printf("6.Listar registros\n");
        printf("7.Reservar livro\n");
        printf("8.Devolver livro\n");
        printf("0.Sair\n");
        printf("===========Livros:%d=\n", tamanho(arq));
        op = lerInteiro("Opcao:");

        switch (op)
        {
        case 1: cadastrar(arq);    break;
        case 2: consultar(arq);    break;
        case 3: geraarqtxt(arq);   break;
        case 4: excluir(arq);      break;
        case 5: contabilizar(arq); break;
        case 6: listar(arq);       break;
        case 7: reservar(arq);     break;
        case 8: devolver(arq);     break;
        case 0: break;
        default: printf("Opcao invalida!\n");
        }

        if (op != 0)
            pausar();

    } while (op != 0);

    fclose(arq);
    printf("Ate logo!\n");
    return 0;
}

void lerTexto(const char *pergunta, char *destino, int tam)
{
    int c;
    printf("%s", pergunta);
    fgets(destino, tam, stdin);              /* le no maximo tam-1 caracteres */

    if (strchr(destino, '\n') != NULL)      
        destino[strcspn(destino, "\n")] = '\0'; 
    else
        while ((c = getchar()) != '\n' && c != EOF);   /* senao, joga fora o resto */
}

/* lerInteiro: le um numero inteiro digitado pelo usuario.
   Se ele digitar algo que nao e numero (ex: "abc"), avisa e pergunta
   de novo, ate receber um numero valido. */
int lerInteiro(const char *pergunta)
{
    char linha[50];
    int numero;

    while (1)
    {
        lerTexto(pergunta, linha, sizeof(linha)); 
        if (sscanf(linha, "%d", &numero) == 1)  
            return numero; 
        printf("Digite um numero valido.\n");
    }
}

/* pausar: espera o usuario apertar Enter.
   Substitui o system("pause"), que so funciona no Windows. */
void pausar(void)
{
    char lixo[10];
    lerTexto("\nPressione Enter para continuar...", lixo, sizeof(lixo));
}

/* tamanho: devolve quantos livros existem no arquivo.*/
int tamanho(FILE *arq)
{
    fseek(arq, 0, SEEK_END);                    /* vai para o fim do arquivo */
    return ftell(arq) / sizeof(struct Livro);   /* posicao do fim / tamanho de 1 livro */
}

void mostrarLivro(struct Livro livro, int codigo)
{
    printf("\nCodigo....:%d", codigo);
    printf("\nTitulo....:%s", livro.titulo);
    printf("\nAutor.....:%s", livro.autor);
    printf("\nAno.......:%d", livro.ano);
    printf("\nStatus....:%s\n", livro.status == ' ' ? "Disponivel" :
                               livro.status == 'R' ? "Reservado" : "Excluido");
}

void cadastrar(FILE *arq)
{
    struct Livro livro;
    char confirma[10];

    livro.status = ' ';   /* todo livro novo comeca como ativo */

    printf("Cadastrando novo registro:\n");
    printf("\nNumero do registro:%d\n", tamanho(arq) + 1);   /* proximo codigo livre */
    lerTexto("Titulo........:", livro.titulo, sizeof(livro.titulo));
    lerTexto("Autor.........:", livro.autor, sizeof(livro.autor));
    livro.ano = lerInteiro("Ano...........:");
    lerTexto("\nConfirma <s/n>:", confirma, sizeof(confirma));

    if (toupper(confirma[0]) == 'S')   /* aceita 's' ou 'S' */
    {
        printf("\ngravando...\n");
        fseek(arq, 0, SEEK_END);                       /* posiciona no fim do arquivo */
        fwrite(&livro, sizeof(struct Livro), 1, arq);  /* grava 1 registro */
        fflush(arq);                                   /* forca a gravacao no disco */
    }
}


void consultar(FILE *arq)
{
    struct Livro livro;
    int nr;

    printf("\nConsulta pelo codigo\n");
    nr = lerInteiro("\nInforme o Codigo...:");

    if (nr > 0 && nr <= tamanho(arq))
    {
        fseek(arq, (nr - 1) * sizeof(struct Livro), SEEK_SET);   /* vai ate o livro */
        fread(&livro, sizeof(struct Livro), 1, arq);             /* le 1 registro */
        mostrarLivro(livro, nr);
    }
    else
        printf("\nNumero de registro invalido!\n");
}


void geraarqtxt(FILE *arq)
{
    char nomearq[50];
    struct Livro livro;
    int nr;
    FILE *arqtxt;

    lerTexto("Nome do arquivo texto (sem .txt):", nomearq, 40);
    strcat(nomearq, ".txt");

    arqtxt = fopen(nomearq, "w");
    if (arqtxt == NULL)
    {
        printf("Nao foi possivel criar esse arquivo!\n");
        return;
    }

    fprintf(arqtxt, "Cod  Titulo                   Autor                    Ano   Status\n");
    fprintf(arqtxt, "====================================================================\n");

    for (nr = 0; nr < tamanho(arq); nr++)
    {
        fseek(arq, nr * sizeof(struct Livro), SEEK_SET);
        fread(&livro, sizeof(struct Livro), 1, arq);
        fprintf(arqtxt, "%-4d %-25s %-24s %-5d %s\n", nr + 1, livro.titulo,
                livro.autor, livro.ano, livro.status == ' ' ? "Disponivel" :
                livro.status == 'R' ? "Reservado" : "Excluido");
    }

    fclose(arqtxt);
    printf("Arquivo '%s' gerado!\n", nomearq);
}


void excluir(FILE *arq)
{
    struct Livro livro;
    char confirma[10];
    int nr;

    nr = lerInteiro("\nInforme o codigo do registro para excluir:");

    if (nr > 0 && nr <= tamanho(arq))
    {
        fseek(arq, (nr - 1) * sizeof(struct Livro), SEEK_SET);
        fread(&livro, sizeof(struct Livro), 1, arq);

        if (livro.status == ' ')   /* so exclui se ainda estiver ativo */
        {
            mostrarLivro(livro, nr);
            lerTexto("\nConfirma a exclusao <s/n>:", confirma, sizeof(confirma));

            if (toupper(confirma[0]) == 'S')
            {
                printf("\nexcluindo...\n");
                livro.status = '*';
                fseek(arq, (nr - 1) * sizeof(struct Livro), SEEK_SET);
                fwrite(&livro, sizeof(struct Livro), 1, arq);
                fflush(arq);
            }
        }
        else if (livro.status == 'R')
            printf("Livro reservado! Devolva antes de excluir.\n");
        else
            printf("Registro ja excluido!\n");
    }
    else
        printf("\nNumero de registro invalido!\n");
}

void contabilizar(FILE *arq)
{
    struct Livro livro;
    int nr, disponiveis = 0, reservados = 0;

    for (nr = 0; nr < tamanho(arq); nr++)
    {
        fseek(arq, nr * sizeof(struct Livro), SEEK_SET);
        fread(&livro, sizeof(struct Livro), 1, arq);
        if (livro.status == ' ')
            disponiveis++;
        else if (livro.status == 'R')
            reservados++;
    }

    printf("\nRegistros armazenados: %d\n", tamanho(arq));
    printf("Registros disponiveis: %d\n", disponiveis);
    printf("Registros reservados.: %d\n", reservados);
    printf("Registros excluidos..: %d\n", tamanho(arq) - disponiveis - reservados);
}

void listar(FILE *arq)
{
    struct Livro livro;
    int nr;

    if (tamanho(arq) == 0)
    {
        printf("\nAinda nao ha livros cadastrados.\n");
        return;
    }

    for (nr = 0; nr < tamanho(arq); nr++)
    {
        fseek(arq, nr * sizeof(struct Livro), SEEK_SET);
        fread(&livro, sizeof(struct Livro), 1, arq);
        mostrarLivro(livro, nr + 1);
    }
}


void reservar(FILE *arq)
{
    struct Livro livro;
    char confirma[10];
    int nr;

    nr = lerInteiro("\nInforme o codigo do livro para reservar:");

    if (nr > 0 && nr <= tamanho(arq))
    {
        fseek(arq, (nr - 1) * sizeof(struct Livro), SEEK_SET);
        fread(&livro, sizeof(struct Livro), 1, arq);

        if (livro.status == ' ')   /* so reserva se estiver disponivel */
        {
            mostrarLivro(livro, nr);
            lerTexto("\nConfirma a reserva <s/n>:", confirma, sizeof(confirma));

            if (toupper(confirma[0]) == 'S')
            {
                printf("\nreservando...\n");
                livro.status = 'R';
                fseek(arq, (nr - 1) * sizeof(struct Livro), SEEK_SET);
                fwrite(&livro, sizeof(struct Livro), 1, arq);
                fflush(arq);
            }
        }
        else if (livro.status == 'R')
            printf("Livro ja esta reservado!\n");
        else
            printf("Livro excluido, nao pode ser reservado!\n");
    }
    else
        printf("\nNumero de registro invalido!\n");
}

void devolver(FILE *arq)
{
    struct Livro livro;
    char confirma[10];
    int nr;

    nr = lerInteiro("\nInforme o codigo do livro para devolver:");

    if (nr > 0 && nr <= tamanho(arq))
    {
        fseek(arq, (nr - 1) * sizeof(struct Livro), SEEK_SET);
        fread(&livro, sizeof(struct Livro), 1, arq);

        if (livro.status == 'R')   /* so devolve se estiver reservado */
        {
            mostrarLivro(livro, nr);
            lerTexto("\nConfirma a devolucao <s/n>:", confirma, sizeof(confirma));

            if (toupper(confirma[0]) == 'S')
            {
                printf("\ndevolvendo...\n");
                livro.status = ' ';
                fseek(arq, (nr - 1) * sizeof(struct Livro), SEEK_SET);
                fwrite(&livro, sizeof(struct Livro), 1, arq);
                fflush(arq);
            }
        }
        else if (livro.status == ' ')
            printf("Livro nao esta reservado!\n");
        else
            printf("Livro excluido, nao pode ser devolvido!\n");
    }
    else
        printf("\nNumero de registro invalido!\n");
}
