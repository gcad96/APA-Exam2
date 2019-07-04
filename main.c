#include <stdio.h>
#include <stdlib.h>

#define MAX 900000.0
#define M 5
#define K 2


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
    int N = M;
    int distMax = 9;
    int numStaz = 2;

    int i, j;

    int** dist = malloc(N*sizeof(int*));
    for(i=0; i<N; i++)
    {
        dist[i] = malloc(N*sizeof(int));
    }

    dist[0][0] = 0;
    dist[0][1] = 8;
    dist[0][2] = 10;
    dist[0][3] = 7;
    dist[0][4] = 12;
    dist[1][0] = 8;
    dist[1][1] = 0;
    dist[1][2] = 7;
    dist[1][3] = 9;
    dist[1][4] = 11;
    dist[2][0] = 10;
    dist[2][1] = 7;
    dist[2][2] = 0;
    dist[2][3] = 10;
    dist[2][4] = 9;
    dist[3][0] = 7;
    dist[3][1] = 9;
    dist[3][2] = 10;
    dist[3][3] = 0;
    dist[3][4] = 8;
    dist[4][0] = 12;
    dist[4][1] = 11;
    dist[4][2] = 9;
    dist[4][3] = 8;
    dist[4][4] = 0;

    int pop[M] = {15, 5, 50, 30, 25};

    int stazCom[M] = {1,1,4,3,2};

    //Proposta di allocazione delle risorse
    int k = K;
    int sol[K] = {1,3};


    printf("\n\nES1.\n");
    //Verifica delle condizioni imposte dalla funzione obiettivo1
    if(ob1(dist, N, sol, k, distMax))
    {
        printf("La soluzione proposta e' accettabile\n");
    }
    else
    {
        printf("La soluzione proposta non e' accettabile\n");
    }

    printf("\n\nES2.\n");
    //Individuazione della soluzione ottima che rispetti i criteri della funzione obiettivo1
    if(!soluz1(dist, N, distMax))
    {
        printf("Nessuna soluzione e' accettabile\n");
    }

    printf("\n\nES3.\n");
    //Individuazione della soluzione ottima che rispetti i criteri della funzione obiettivo2
    soluz2(dist, N, pop, numStaz, stazCom);

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
            printf("Localizzate in   ");
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

    for(i=1; i<=stazCom[val[pos]]; i++)
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
                num = staz[j];
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
        c+=sol[i];
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
        c+=sol[i];
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
        printf("%d ", v[i]);
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
