#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
// 4096 2048
#define NOS 32
#define MEI 16

// quantidade de registros : 	 4,294,967,296		4	G
// valor máximo unsigned long: 4,294,967,295		
// registros alocados aprox  : 						64 MB
// 

typedef struct registro
{
	unsigned long chave;			//Campo chave Valor nao se repete.
	unsigned long naochave;		//Campo qualquer. Valor pode repetir.
	unsigned char outros[1008];//outros campos: 1008 bytes (dados)
}registro;

typedef struct dat
{
	unsigned long v; //valor
	unsigned int b; //bloco
	struct dat *p;
}dat;

typedef struct pag
{
	int q;				//qt ocupada
	dat **d; 		   //dados
	struct pag **p;   //pags
}pag;

pag *constroi(pag *p,unsigned long v, unsigned int b);
/*
char *busca(dat *d, unsigned long c)
{
	pag *p
	int x= getposD(p,*d);
		if((*d)->v == p->d[x]->v)
			printf("FIND!!\n");
		else if((*d)->v > p->d[x]->v)
			x++;
	}

	// descendo até o folha
	if(p->p[x])
	{
		*ctrl = *ctrl + 1;
		biP(p->p[x],d,de,ctrl);
	}
}
*/
// printa valores dos blocos em pré ordem
void printP(pag *p)
{
	int i = 0;
	printf("(");
	for( i ; i < p->q ; i++)
		printf("%lu ",p->d[i]->v);
	puts(")");
	for(i=0; i<= p->q ; i++)
		if(p->p[i])printP(p->p[i]);
}

pag* criaP()
{
	// criando pagina
	pag* p=(pag*)malloc(sizeof(pag));
	assert(p);
	
	// criando ponteiro dados
	p->d=(dat**)malloc(sizeof(dat*)*NOS);
	assert(p->d);
	
	// criando poteiro paginas
	p->p=(pag**)malloc(sizeof(pag*)*(NOS+1));
	assert(p->d);
	
	// quantidade = 0
	p->q=0;
	
	// zerando valores
	for(int i=0;i<NOS;i++)
		p->d[i]=NULL;
	
	// zerando ponteiros
	for(int i=0;i<NOS+1;i++)
		p->p[i]=NULL;
	
	// retornando endereço
	return p;	
}

dat* criaD(unsigned long v, unsigned int b)
{
	// criando dado
	dat *d=(dat*)malloc(sizeof(dat));
	assert(d);
	
	// passando valores
	d->b = b;
	d->v = v;
	
	// retornando endereço
	return d;

}

void deslocaD(pag *p, unsigned short int s)
{
	// obtendo ultima posicao
	int i = p->q - 1;
	
	// deslocando dados
	for(i;i>=s;i--)
		p->d[i+1]=p->d[i];
}

void deslocaP(pag *p, unsigned short int s)
{
	// obtendo ultima posicao
	int i = p->q;
	
	// deslocando ponteiros
	for(i;i>=s;i--)
		p->p[i+1]=p->p[i];
}

void insereD(pag *p, dat *d, unsigned short int x)
{
	// insere dado na posicao X
	p->d[x] = d ;
}

void insereP(pag *p, pag **de, unsigned short int x)
{
	// insere ponteiro na esquerda
	p->p[x]   = de[0];
	// insere ponteiro na direita
	p->p[x+1] = de[1];
}

void repassaD(pag *a, pag *b)
{
	// repassando dados a partir do
	// valor mediano de (a)
	for(int i=0;i<MEI;i++)
	{
		b->d[i] = a->d[MEI + i + 1];
		a->d[MEI + i + 1] = NULL;
		a->q= a->q -1;
	}
}

void repassaP(pag *a, pag *b)
{
	// repassando paginas a partir do
	// valor ponteiro mediano de (a)
	for(int i=0;i<=MEI;i++)
	{
		b->p[i] = a->p[MEI + i + 1];
		a->p[MEI + i + 1] = NULL;
	}
}

int getposD(pag *p, dat *d)
{
	// criando variaveis
	int i = 0;
	int f = p->q - 1;
	int m = m=(i+f)/2;
	
	// utilizando busca binaria
	for(;i<f;)
	{
		if(d->v == p->d[m]->v)
			return m;
		else if(d->v > p->d[m]->v)
			i = m + 1;
		else
			f = m - 1;
		m=(i+f)/2;
	}
	
	// não encontrou a posicao
	// retorna um indice proximo
	return i;
}

// insere com deslocamento em P 
void inP(pag *p, dat *d, pag **de,unsigned short int x)
{
	deslocaD(p,x);
	deslocaP(p,x);
	insereD(p,d,x);
	insereP(p,de,x);
	p->q = p->q + 1;
}

void realocando(pag *p, int q)
{
	
	int q1 = (q + 0) * sizeof(dat*) ;//168;//
	int q2 = (q + 1) * sizeof(pag*) ;//printf("Chegoo\n%i, %i\n",q1,q2);//176;//
	p->p=(pag**)realloc(p->p,q2);//puts("ok X");
	p->d=(dat**)realloc(p->d,q1);//puts("ok O");
	
	
}

//busca para inserção
pag* biP(pag *p, dat **d, pag **de, int *ctrl)
{	
	// buscando indice chave e indice do ponteiro
	int x=0;
	if(p->q > 0){
		x = getposD(p,*d);
		if((*d)->v == p->d[x]->v)
			printf("FIND!!\n");
		else if((*d)->v > p->d[x]->v)
			x++;
	}

	// descendo até o folha
	if(p->p[x])
	{
		*ctrl = *ctrl + 1;
		biP(p->p[x],d,de,ctrl);
	}
	*ctrl = *ctrl - 1;
	// estamos em um nó folha
	if(*ctrl >= 0)
	{
		if(p->q < NOS)
		{
			*ctrl = -1; 
			// insere com deslocamento
			inP(p,*d,de,x);
			return p;
		}
		else
		{
			// ocorreu uma quebra
			// aumentando temporariamente tamanho do no
			//printf("OOO - p->q+1 (%i) p->q+2 (%i) \n",p->q+1,p->q+2);
			//printf("%x %x\n",p->d,p->p);
			
			realocando(p,NOS+1);
			
			inP(p,*d,de,x);
			
			// repassando valores
			pag *n = criaP();
			repassaP(p,n);
			repassaD(p,n);
			
			// escolhendo meio
			dat *nd = p->d[MEI];
			
			// (re)definindo tamanho dos vetores
			n->q = MEI;
			p->q = MEI;
			
			// realocando matriz
			//printf("XXX - MEI (%i) MEI+1 (%i) \n",MEI,MEI+1);
			realocando(p,MEI);			
	
			// dado que sobe de nivel
			*d=nd;
			// ponteiros direita e esquerda
			de[0]=p;
			de[1]=n;

			// nova raiz
			// caso não haja recursão antrerior
			if(*ctrl == 0)
			{
				// criando nova raiz
				int c=1;
				pag *raiz=criaP();
				raiz = biP(raiz,d,de,&c);
				*ctrl = -1;
				return raiz;
			}
			return p;
		}
	}
	return p;
}

pag *constroi(pag *p,unsigned long v, unsigned int b)
{
	if(!p) p = criaP();
	pag *d=NULL, *e=NULL;
	pag **de=(pag**)malloc(sizeof(pag*)*2);
	de[0]=d;
	de[1]=e;
	int ctrl = 1;
	dat *dado=criaD(v,b);
	p = biP(p,&dado,de,&ctrl);
	free(de);	
	return p;
}

pag *read(FILE *arq)
{
	unsigned int i=0;
	pag *p=NULL;
	registro reg[4];
	
	while(!feof(arq))
	{
		fread(reg,sizeof(registro),4,arq);
		p=constroi(p,reg[0].chave,i);
		p=constroi(p,reg[1].chave,i);
		p=constroi(p,reg[2].chave,i);
		p=constroi(p,reg[3].chave,i);
		i++;
	}
	
	return p;
}


int main()
{
	FILE *arq = fopen("base.bin","rb");
	if(!arq)puts("Não foi possivel ler o arquivo");
	pag *p=read(arq);
	
	
	fclose(arq);	
	//printP(p);

}
