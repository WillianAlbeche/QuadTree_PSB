#include "quadtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>     /* OpenGL functions */
#endif

unsigned int first = 1;
char desenhaBorda = 1;

float levelDetailOfRegion(Img *pic, QuadNode* regiao);
QuadNode* geraQuadtreeChild(QuadNode* root, float minDetail, float detalheAtual,Img* pic);
QuadNode* newNode(int x, int y, int width, int height)
{
    QuadNode* n = malloc(sizeof(QuadNode));
    n->x = x;
    n->y = y;
    n->width = width;
    n->height = height;
    n->NW = n->NE = n->SW = n->SE = NULL;
    n->color[0] = n->color[1] = n->color[2] = 0;
    n->id = first++;
    return n;
}
QuadNode* geraQuadtreeChild(QuadNode* root, float minDetail, float detalheAtual,Img* pic){
    detalheAtual++;
    RGB (*pixels)[pic->width] = (RGB(*)[pic->width]) pic->img;
    float level = levelDetailOfRegion(pic,root);
    if(level<=16){
        root->status = CHEIO;
    }else if(level >=235){
        root->status = NULL;
    }
    else{
        root->status = PARCIAL;
    }

    if(minDetail>detalheAtual){
        root->NW = geraQuadtreeChild(newNode(root->x,root->y,root->width/2,root->height/2),minDetail,0,pic);
        root->NE = geraQuadtreeChild(newNode(root->x,root->y,root->width/2,root->height/2),minDetail,0,pic);
        root->SW = geraQuadtreeChild(newNode(root->x,root->y,root->width/2,root->height/2),minDetail,0,pic);
        root->SE = geraQuadtreeChild(newNode(root->x,root->y,root->width/2,root->height/2),minDetail,0,pic); 
    }

    return root;

}

QuadNode* geraQuadtree(Img* pic, float minDetail)
{
    // Converte o vetor RGB para uma MATRIZ que pode acessada por pixels[linha][coluna]
    RGB (*pixels)[pic->width] = (RGB(*)[pic->width]) pic->img;

    // Veja como acessar os primeiros 10 pixels da imagem, por exemplo:
    int i;
    for(i=0; i<10; i++)
        printf("%02X %02X %02X\n",pixels[0][i].r,pixels[1][i].g,pixels[2][i].b);

    int width = pic->width;
    int height = pic->height;

    QuadNode* root = newNode(0,0 , width, height);

    float level = levelDetailOfRegion(pic,root);
    if(level<=16){
        root->status = CHEIO;
    }else if(level >=235){
        root->status = NULL;
    }
    else{
        root->status = PARCIAL;
    }
    
    if(minDetail>0){
        root->NW = geraQuadtreeChild(newNode(root->x,root->y,root->width/2,root->height/2),minDetail,0,pic);
        root->NE = geraQuadtreeChild(newNode(root->x,root->y,root->width/2,root->height/2),minDetail,0,pic);
        root->SW = geraQuadtreeChild(newNode(root->x,root->y,root->width/2,root->height/2),minDetail,0,pic);
        root->SE = geraQuadtreeChild(newNode(root->x,root->y,root->width/2,root->height/2),minDetail,0,pic); 
    }

    return root;
    //////////////////////////////////////////////////////////////////////////
    // Implemente aqui o algoritmo que gera a quadtree, retornando o nodo raiz
    //////////////////////////////////////////////////////////////////////////

// COMENTE a linha abaixo quando seu algoritmo ja estiver funcionando
// Caso contrario, ele ira gerar uma arvore de teste com 3 nodos

//#define DEMO
#ifdef DEMO
    
    /************************************************************/
    /* Teste: criando uma raiz e dois nodos a mais              */
    /************************************************************/

    QuadNode* raiz = newNode(0,0,width,height);
    raiz->status = PARCIAL;
    raiz->color[0] = 0;
    raiz->color[1] = 0;
    raiz->color[2] = 255;

    QuadNode* nw = newNode(width/2,0,width/2,height/2);
    nw->status = PARCIAL;
    nw->color[0] = 0;
    nw->color[1] = 0;
    nw->color[2] = 255;

    // Aponta da raiz para o nodo nw
    raiz->NW = nw;

    QuadNode* nw2 = newNode(width/2+width/4,0,width/4,height/4);
    nw2->status = CHEIO;
    nw2->color[0] = 255;
    nw2->color[1] = 0;
    nw2->color[2] = 0;

    // Aponta do nodo nw para o nodo nw2
    nw->NW = nw2;
    
#endif
    // Finalmente, retorna a raiz da árvore
    //return raiz;
}

float levelDetailOfRegion(Img *pic, QuadNode* regiao){
    regiao->color[0] = regiao->color[1] = regiao->color[2] = 0;

     RGB (*pixels)[pic->width] = (RGB(*)[pic->width]) pic->img;

    float diferencaR = 0;
    for (int i = regiao->y; i < regiao->height; i++){
        for (int j = regiao->x; j < regiao->width; j++)
        {
            regiao->color[0] += pixels[i][j].r;
            regiao->color[1] += pixels[i][j].g;
            regiao->color[2] += pixels[i][j].b;
        }
    }
    
    float areaR = regiao->width * regiao->height;

    regiao->color[0] = regiao->color[0]/(areaR);
    regiao->color[1] = regiao->color[1]/(areaR);
    regiao->color[2] = regiao->color[2]/(areaR);
    
    for (int i = regiao->y; i < regiao->height; i++){
        for (int j = regiao->x; j < regiao->width; j++){
            diferencaR = sqrt(pow(pixels[i][j].r - regiao->color[0],2) + (pow(pixels[i][j].g - regiao->color[1],2))+ (pow(pixels[i][j].b - regiao->color[2],2)));
        }
    }

    diferencaR = diferencaR/(areaR);

    return diferencaR;
}

// Limpa a memória ocupada pela árvore
void clearTree(QuadNode* n)
{
    if(n == NULL) return;
    if(n->status == PARCIAL)
    {
        clearTree(n->NE);
        clearTree(n->NW);
        clearTree(n->SE);
        clearTree(n->SW);
    }
    //printf("Liberando... %d - %.2f %.2f %.2f %.2f\n", n->status, n->x, n->y, n->width, n->height);
    free(n);
}

// Ativa/desativa o desenho das bordas de cada região
void toggleBorder() {
    desenhaBorda = !desenhaBorda;
    printf("Desenhando borda: %s\n", desenhaBorda ? "SIM" : "NÃO");
}

// Desenha toda a quadtree
void drawTree(QuadNode* raiz) {
    if(raiz != NULL)
        drawNode(raiz);
}

// Grava a árvore no formato do Graphviz
void writeTree(QuadNode* raiz) {
    FILE* fp = fopen("quad.dot", "w");
    fprintf(fp, "digraph quadtree {\n");
    if (raiz != NULL)
        writeNode(fp, raiz);
    fprintf(fp, "}\n");
    fclose(fp);
    printf("\nFim!\n");
}

void writeNode(FILE* fp, QuadNode* n)
{
    if(n == NULL) return;

    if(n->NE != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->NE->id);
    if(n->NW != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->NW->id);
    if(n->SE != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->SE->id);
    if(n->SW != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->SW->id);
    writeNode(fp, n->NE);
    writeNode(fp, n->NW);
    writeNode(fp, n->SE);
    writeNode(fp, n->SW);
}

// Desenha todos os nodos da quadtree, recursivamente
void drawNode(QuadNode* n)
{
    if(n == NULL) return;

    glLineWidth(0.1);

    if(n->status == CHEIO) {
        glBegin(GL_QUADS);
        glColor3ubv(n->color);
        glVertex2f(n->x, n->y);
        glVertex2f(n->x+n->width-1, n->y);
        glVertex2f(n->x+n->width-1, n->y+n->height-1);
        glVertex2f(n->x, n->y+n->height-1);
        glEnd();
    }

    else if(n->status == PARCIAL)
    {
        if(desenhaBorda) {
            glBegin(GL_LINE_LOOP);
            glColor3ubv(n->color);
            glVertex2f(n->x, n->y);
            glVertex2f(n->x+n->width-1, n->y);
            glVertex2f(n->x+n->width-1, n->y+n->height-1);
            glVertex2f(n->x, n->y+n->height-1);
            glEnd();
        }
        drawNode(n->NE);
        drawNode(n->NW);
        drawNode(n->SE);
        drawNode(n->SW);
    }
    // Nodos vazios não precisam ser desenhados... nem armazenados!
}

