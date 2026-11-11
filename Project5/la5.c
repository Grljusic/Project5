#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct cvor {
    double broj;
    struct cvor* next;
} Cvor;

/* push - novi element ide odmah iza head-a (to je vrh stoga) */
int dodajNaStog(Cvor* head, double broj)
{
    Cvor* novi = (Cvor*)malloc(sizeof(Cvor));
    if (novi == NULL) {
        printf("Greska: alokacija memorije nije uspjela.\n");
        return -1;
    }

    novi->broj = broj;
    novi->next = head->next;
    head->next = novi;
    return 0;
}

/* pop - skida element s vrha stoga i vraca njegovu vrijednost preko pokazivaca */
int skiniSaStoga(Cvor* head, double* broj)
{
    Cvor* vrh = head->next;
    if (vrh == NULL) {
        printf("Greska: stog je prazan (premalo brojeva u izrazu).\n");
        return -1;
    }

    *broj = vrh->broj;
    head->next = vrh->next;
    free(vrh);
    return 0;
}

/* oslobada sve sto je ostalo na stogu */
void obrisiStog(Cvor* head)
{
    double x;
    while (head->next != NULL)
        skiniSaStoga(head, &x);
}

/* je li rijec jedan od operatora + - * / */
int jeOperator(const char* rijec)
{
    return strlen(rijec) == 1 && strchr("+-*/", rijec[0]) != NULL;
}

int jeBroj(const char* rijec, double* broj)
{
    char* kraj = NULL;
    *broj = strtod(rijec, &kraj);
    return kraj != rijec && *kraj == '\0';
}

/* racuna a op b */
int izracunaj(double a, double b, char op, double* rezultat)
{
    switch (op) {
    case '+': *rezultat = a + b; return 0;
    case '-': *rezultat = a - b; return 0;
    case '*': *rezultat = a * b; return 0;
    case '/':
        if (b == 0) {
            printf("Greska: dijeljenje s nulom.\n");
            return -1;
        }
        *rezultat = a / b;
        return 0;
    }
    return -1;
}

int izracunajPostfiks(const char* imeDatoteke, double* rezultat)
{
    Cvor head = { 0, NULL };
    char rijec[100];
    double broj, a, b, r;
    int greska = 0;

    FILE* datoteka = fopen(imeDatoteke, "r");
    if (datoteka == NULL) {
        printf("Greska: ne mogu otvoriti datoteku %s.\n", imeDatoteke);
        return -1;
    }

    /* rrazmaci i novi redovi se preskacu */
    while (fscanf(datoteka, "%99s", rijec) == 1) {
        if (jeOperator(rijec)) {
            /* prvo se skida DRUGI operand (b), pa onda prvi (a) */
            if (skiniSaStoga(&head, &b) != 0 ||
                skiniSaStoga(&head, &a) != 0 ||
                izracunaj(a, b, rijec[0], &r) != 0 ||
                dodajNaStog(&head, r) != 0) {
                greska = 1;
                break;
            }
        }
        else if (jeBroj(rijec, &broj)) {
            if (dodajNaStog(&head, broj) != 0) {
                greska = 1;
                break;
            }
        }
        else {
            printf("Greska: nepoznat znak \"%s\" u izrazu.\n", rijec);
            greska = 1;
            break;
        }
    }

    fclose(datoteka);

    /* na kraju na stogu mora ostati tocno jedan broj - rezultat */
    if (!greska) {
        if (skiniSaStoga(&head, rezultat) != 0)
            greska = 1;
        else if (head.next != NULL) {
            printf("Greska: previse brojeva u izrazu.\n");
            greska = 1;
        }
    }

    obrisiStog(&head);
    return greska ? -1 : 0;
}

int main()
{
    double rezultat;

    if (izracunajPostfiks("postfix.txt", &rezultat) != 0)
        return 1;

    printf("Rezultat: %g\n", rezultat);
    return 0;
}