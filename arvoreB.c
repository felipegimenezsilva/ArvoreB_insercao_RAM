#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define NOS 20
#define MIN 10
unsigned int quantidadeexistente;
// quantidade de registros : 	 4,294,967,296		4	G
// valor máximo unsigned long: 4,294,967,295		
// registros alocados aprox  : 						64 MB

typedef struct registro
{
	unsigned long chave;			//Campo chave Valor nao se repete.
	unsigned long naochave;		//Campo qualquer. Valor pode repetir.
	unsigned char outros[1008];//outros campos: 1008 bytes (dados)
}registro;

typedef struct data
{
	unsigned long v;// valor
	unsigned int b; // bloco
	struct data *p; // prox
	struct data *c; // conflito
	struct page *d; // direita
}data;

typedef struct page
{
	unsigned int q;	// qt ocupada
	data *d; 		   // lista dados
	struct page *e;   // esquerda
}page;

void exibeDataConflitos(data *d);

data* alocaData(unsigned int v, unsigned int b)
{
	// alocando data
	data *nova=(data*)malloc(sizeof(data));
	
	// testando alocação 
	assert(nova);
	
	// definindo valores iniciais
	nova->v = v;
	nova->b = b;
	nova->p = NULL;
	nova->c = NULL;
	nova->d = NULL;
	return nova;	
}

data* insereDataOrdenado(data *lista, data *valor)
{
	// retornando primeiro como valor
	// caso não exista lista
	if(!lista)
		return valor;
		
	// insere inicio ou meio
	data *first=lista;
	data *anter=NULL;
	while(lista)
		// verificando posição
		if(valor->v < lista->v)
		{
			// obtendo se é o primeiro
			if(first == lista)
			{
				// inserção no inicio
				valor->p = lista;
				return valor;
			}
			else
			{
				// inserção no meio
				valor->p = lista;
				anter->p = valor;
				return first;
			}
		}
		// verificando se é conflito
		else if(valor->v == lista->v)
		{
			// inserção na lista de conflitos
			valor->c = lista->c;
			lista->c = valor;
			return first;
		}
		// passando para proximo elemento
		else
		{
			// guardando proximo e atual
			anter = lista;
			lista = lista->p;
		}
	// inserindo no final da lista
	anter->p = valor;
	return first;	
}

data *quebraMeio(data *lista)
{
	int i=0;
	data *ant=NULL;
	// procurando posição minima
	while(lista)
		if(i==MIN)
		{
			// quebrando lista
			ant->p=NULL;
			return lista;
		}
		else
		{
			// passando para proxima
			// iteração
			i++;
			ant = lista;
			lista = lista->p;
		}
	// retorna null caso não
	// consiga chegar ao meio
	// estabelecido no define
	return NULL;
}

page *alocaPage(unsigned int q, data *d, page *esq)
{
	// alocando pagina
	page *nova=(page*)malloc(sizeof(page));
	
	// testando alocações
	assert(nova);
	
	// definindo valores iniciais
	nova->q = q;
	nova->d = d;
	nova->e = esq;
	return nova;
}

// retorna endereço da sub_página a partir de um valor
page *buscaDirecao(page *pagina, unsigned long valor)
{
	
	data *dado = pagina->d;
	pagina = pagina->e;

	while(dado && pagina)
		if(valor > dado->v)
		{
			pagina = dado->d;
			dado = dado->p;
		}
		else
			return pagina;
	return pagina;
}

data *buscaChave(page *pagina,unsigned long valor)
{
	data *dado = pagina->d;
	while(dado)
		if(dado->v==valor)
			return dado;
		else
			dado=dado->p;
	return NULL;
}

void exibeData(page *p)
{
	if(!p) return;
	
	data *d = p->d;
	
	printf("(");
	while(d)
	{
		quantidadeexistente++;
		printf(" %lu ",d->v);
		if(d->c) exibeDataConflitos(d->c);
		d = d->p;
	}
	puts(")");
	
	d=p->d;
	if(p->e)
		exibeData(p->e);
	while(d)
	{
		exibeData(d->d);
		d = d->p;
	}
	
}

void exibeDataConflitos(data *d)
{
	printf("{");
	while(d)
	{
		printf(" %lu ",d->v);
		d = d->c;
	}
	printf("} ");
}

void insereAB(page *pagina, data **dado, int *controle)
{

	// verificando se houve conflito de valores nessa pagina
	data *buscaC = buscaChave(pagina,(*dado)->v);
	
	// caso ocorreu o conflito de valores
	if(buscaC)
	{
   	puts("bateu");//------------------------------------
   	quantidadeexistente++;
		pagina->d = insereDataOrdenado(pagina->d,*dado);
		*controle = -1;
		return ;
	}
	else
	{
		// procurando por sub pagina
		page *buscaP = buscaDirecao(pagina,(*dado)->v);
		
		// não estamos no folha
		if(buscaP)
		{
			// descendo até os folhas
			insereAB(buscaP,dado,controle);
		}
		if(*controle > 0)
		{
			// nessa a partir dessa linha, subiremos na recursão
			if(pagina->q < NOS)
			{ 
				// inserindo tranquilamente em uma pagina
				pagina->d = insereDataOrdenado(pagina->d,*dado);
				pagina->q = pagina->q + 1;
				*controle = -1;
				return ;
			}
			else
			{
				// caso de quebra
				pagina->d = insereDataOrdenado(pagina->d,*dado);
				// quebrando a partir do meio
				data *quebra = quebraMeio(pagina->d);
				// definindo tamanho da lista
				pagina->q=MIN;
				//definindo valor do meio
				*dado = quebra;
				// guardando endereço
				page *aux=quebra->d;
				// removendo meio da lista
				quebra = quebra->p;
				(*dado)->p=NULL;
				// criando novo bloco
				page *nova=alocaPage(MIN,quebra,NULL);
				// passando valor da esquerda
				nova->e = aux;
				// dado apontando para direita
				(*dado)->d=nova;
				return;
			}
		}
	}
}


page *insereArvoreB(page *pagina, unsigned long valor, unsigned int bloco)
{
	if(!pagina) pagina=alocaPage(0,NULL,NULL);
	data *inserir = alocaData(valor,bloco);
	int controle = 1;
	insereAB(pagina,&inserir,&controle);
	
	// houve aumento no tamanho da arvore
	if(controle == 1)
	{
		page *raiz=alocaPage(1,inserir,pagina);
		return raiz;	
	}
	return pagina;	
}


page *read(FILE *arq)
{
	unsigned int i=0;
	page *p=NULL;
	registro reg[4];
	
	while(!feof(arq))
	{
		fread(reg,sizeof(registro),4,arq);
		p=insereArvoreB(p,reg[0].naochave,i);
		p=insereArvoreB(p,reg[1].naochave,i);
		p=insereArvoreB(p,reg[2].naochave,i);
		p=insereArvoreB(p,reg[3].naochave,i);
		i++;
	}
	return p;
}

data *pesquisa(page *pagina, unsigned long chave)
{
	data *buscaC = buscaChave(pagina,chave);
	if(!buscaC)
	{
		page *buscaP = buscaDirecao(pagina,chave);
		if(buscaP)
			return pesquisa(buscaP,chave);
		else
			return NULL;
	}
	return buscaC;
}

void acesso(FILE *file,data *dado)
{
	registro r[4];
	while(dado)
	{
		fseek(file,4096*dado->b,SEEK_SET);
		fread(r,sizeof(registro),4,file);
		for(int i=0;i<4;i++)
		{
			if(r[i].naochave == dado->v)
				printf("\n\nbloco: %d\nchave: %lu\nnao chave: %lu\n outros:\n%s\n",dado->b,r[i].chave,r[i].naochave,r[i].outros);
		}
		dado = dado->c;
	}
	
}


int main()
{
	quantidadeexistente=0;
	FILE *arq = fopen("base.bin","rb");
	if(!arq)puts("Não foi possivel ler o arquivo");
	page *pagina=read(arq);	
	//page *pagina=temporario();
	data *b;
	
	printf("Quantidade de nos existentes=%lu\n",quantidadeexistente);
	unsigned long p;
	while(1)
	{
		puts("Digite o nao chave:");
		scanf("%lu",&p);
		b=pesquisa(pagina,p);
		acesso(arq,b);
		if(!b)
			puts("não deu boa");
	}
	
	//printf("%lu %i\n",b->v,b->b);
	
	fclose(arq);
	//
	//exibeData(pagina);

}
