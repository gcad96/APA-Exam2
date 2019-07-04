#include <stdio.h>
#include <stdlib.h>

#define NAME "path/name.txt"
#define MAX 900000.0


int soluz1(int** dist, int N, int distMax);
void comb_sempl(int pos, int* sol, int k, int n, int start, int** dist, int* stop, int distMax);
int ob1(int** dist, int N, int* sol, int k, int distMax);

void soluz2(int **dist, int N, int *pop, int numStaz, int *stazCom);
void comb_sempl_bis(int pos, int* sol, int k, int** dist, int* pop, float* bestQ, int numStaz, int N, int start, int** bestSol, int* stazCom);
void pm(int pos, int* val, int k, int** dist, int* pop, float* bestQ, int** bestSol, int* sol, int numStaz, int N, int* stazCom);
float ob2(int* staz, int k, int N, int** dist, int* pop, int* sol);
int acc1(int* sol, int k, int numStaz, int* val);
int acc2(int* sol, int k, int numStaz, int* val);

void copiaSolInBestSol(int** bestSol, int* sol, int* val, int k);
void stampaSol(int* v, int dim);
void stampaBestSol(int** v);


int main(void)
{
    int N;
    int distMax, numStaz;

    int i, j;

    FILE* f;
    if((f= fopen(NAME, "r"))==NULL)
    {
        fprintf(stderr, "Errore durante l'apertura del file.\n");
        return 1;
    }

    fscanf(f, "%d", &N);

    fscanf(f, "%d", &distMax);
    fscanf(f, "%d", &numStaz);

    int** dist = malloc(N*sizeof(int*));
    for(i=0; i<N; i++)
    {
        dist[i] = malloc(N*sizeof(int));
        for(j=0; j<N; j++)
        {
            fscanf(f, "%d", &(dist[i][j]));
        }
    }

    int* pop = malloc(N*sizeof(int));
    for(i=0; i<N; i++)
    {
        fscanf(f, "%d", &pop[i]);
    }

    int* stazCom = malloc(N*sizeof(int));
    for(i=0; i<N; i++)
    {
        fscanf(f, "%d", &stazCom[i]);
    }

    //Lettura della proposta di allocazione delle risorse
    int k;
    fscanf(f, "%d", &k);
    int* sol = malloc(k*sizeof(int));
    for(i=0; i<k; i++)
    {
        fscanf(f, "%d", &sol[i]);
    }

    fclose(f);


    //Verifica delle condizioni imposte dalla funzione obiettivo1
    if(ob1(dist, N, sol, k, distMax))
    {
        printf("La soluzione proposta e' accettabile\n");
    }

    //Individuazione della soluzione ottima che rispetti i criteri della funzione obiettivo1
    if(!soluz1(dist, N, distMax))
    {
        printf("Nessuna soluzione e' accettabile\n");
    }

    //Individuazione della soluzione ottima che rispetti i criteri della funzione obiettivo2
    soluz2(dist, N, pop, numStaz, stazCom);

    for(i=0; i<N; i++)
        free(dist[i]);
    free(dist);
    free(pop);
    free(stazCom);
    free(sol);

    return 0;
}

int soluz1(int** dist, int N, int distMax)
{
    int stop;
    int* sol = malloc(N*sizeof(int));

    int i;
    for(i=1; i<N; i++)
    {
        stop = 0;
        comb_sempl(0, sol, i, N, 0, dist, &stop, distMax);
        if(stop)
        {
            printf("Numero minimo di stazioni e': %d\n", i);
            printf("Localizzate in\n");
            stampaSol(sol, i);
            return 1;
        }
    }

    free(sol);

    return 0;
}

void comb_sempl(int pos, int* sol, int k, int n, int start, int** dist, int* stop, int distMax)
{
    int i;

    if(pos>=k)
    {
        if(ob1(dist, n, sol, k, distMax))	*stop=1;
        return ;
    }

    for(i=start; i<n && (*stop==0); i++)
    {
        sol[pos] = i;
        comb_sempl(pos+1, sol, k, n, i+1, dist, stop, distMax);
    }
}

int ob1(int** dist, int N, int* sol, int k, int distMax)
{
    int ok;
    int i, j;

    for(i=0; i<N; i++)
    {
        ok=0;
        for(j=0; j<k; j++)
        {
            if(dist[i][sol[j]] < distMax)	ok=1;
        }
        if(ok==0)	return 0;
    }

    return 1;
}


void soluz2(int **dist, int N, int *pop, int numStaz, int *stazCom)
{
    int** bestSol = malloc(2*sizeof(int*));
    bestSol[0] = malloc((N+1)*sizeof(int));
    bestSol[1] = malloc((N+1)*sizeof(int)); /* in bestSol[i][0] si registra la dim della soluzione */
    float bestQ = MAX;

    int* sol = malloc(N*sizeof(int));
    int i;
    for(i=0; i<N; i++)
    {
        comb_sempl_bis(0, sol, i, dist, pop, &bestQ, numStaz, N, 0, bestSol, stazCom);
    }

    stampaBestSol(bestSol);

    free(bestSol[0]);
    free(bestSol[1]);
    free(bestSol);

    free(sol);
}

void comb_sempl_bis(int pos, int* sol, int k, int** dist, int* pop, float* bestQ, int numStaz, int N, int start, int** bestSol, int* stazCom)
{
    int i;
    if(pos>=k)
    {
        int* sol2 = malloc(k*sizeof(int));
        pm(0, sol, k, dist, pop, bestQ, bestSol, sol2, numStaz, N, stazCom);
        free(sol2);
        return;
    }

    for(i=start; i<N; i++)
    {
        sol[pos] = i;
        comb_sempl_bis(pos+1, sol, k, dist, pop, bestQ, numStaz, N, i+1, bestSol, stazCom);
    }
}

void pm(int pos, int* val, int k, int** dist, int* pop, float* bestQ, int** bestSol, int* sol, int numStaz, int N, int* stazCom)
{
    int i;
    float q;

    if(pos>=k)
    {
        if( acc2(sol, k, numStaz, val) )
        {
            q = ob2(sol, k, N, dist, pop, val);
            if (*bestQ > q)
            {
                *bestQ = q;
                copiaSolInBestSol(bestSol, sol, val, k);
            }
        }
        return;
    }

    for(i=1; i<stazCom[val[pos]]; i++)
    {
        if(acc1(sol, pos, numStaz, val)) //pruning
        {
            sol[pos] = i;
            pm(pos+1, val, k, dist, pop, bestQ, bestSol, sol, numStaz, N, stazCom);
        }
    }
}

float ob2(int* staz, int k, int N, int** dist, int* pop, int* sol)
{
    float q = 0;

    int i, j;
    for(i=0; i<N; i++)
    {
        int min = MAX;
        int num = 0;
        for(j=0; j<k; j++)
        {
            if(dist[i][sol[j]] < min)
            {
                min = dist[i][sol[j]];
                num = staz[sol[j]];
            }
        }
        q += ( (float) pop[i] ) * min/num;
    }

    return q;
}

int acc1(int* sol, int k, int numStaz, int* val)
{
    int c=0;

    int i;
    for(i=0; i<k; i++)
    {
        c+=sol[val[i]];
    }

    if(c<=numStaz)	return 1;
    else 			return 0;
}

int acc2(int* sol, int k, int numStaz, int* val)
{
    int c=0;

    int i;
    for(i=0; i<k; i++)
    {
        c+=sol[val[i]];
    }

    if(c!=numStaz)	return 0;
    else 			return 1;
}


void copiaSolInBestSol(int** bestSol, int* sol, int* val, int k)
{
    int i, j;

    i=0;
    bestSol[i][0] = k;

    for(j=0; j<k; j++)
    {
        bestSol[i][j+1] = sol[j];
    }

    i=1;
    bestSol[i][0] = k;

    for(j=0; j<k; j++)
    {
        bestSol[i][j+1] = val[j];
    }
}

void stampaSol(int* v, int dim)
{
    int i;
    for(i=0; i<dim; i++)
    {
        printf("%d", v[i]);
    }
    printf("\n");
}

void stampaBestSol(int** v)
{
    int i;
    printf("Soluzione:\n");
    int dim = v[0][0];

    for(i=1; i<=dim; i++)
    {
        printf("%d stazioni localizzate nel comune numero %d\n", v[0][i], v[1][i]);
    }
}