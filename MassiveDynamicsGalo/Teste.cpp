#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/glut.h>

//#ifndef M_PI
//#define M_PI 3.1415926535897932384626433832795
//#endif

//#define rtd(x)   (180*(x)/M_PI)
//#define dtr(x)   (M_PI*(x)/180)

#define DEBUG 1

/* VARI�VEIS GLOBAIS */

typedef struct {
	GLboolean   doubleBuffer;
	/*GLint       delay;*/
}Estado;

typedef struct {
	int player;
	int computador;
	int start_game;
	int win;
}Jogo;

typedef struct {
	// variaveis de rato: Win = windows size, mouse = posicao rato
	int mouse_x, mouse_y, Win_x, Win_y, object_select;
}Rato;

// alinhamento das caixas, quando se ganha
// 8 possibilidades, 3 na vertical, 3 na horizontal and 2 na diagonal
//
// 0 | 1 | 2
// 3 | 4 | 5
// 6 | 7 | 8
//

static int caixa[8][3] = { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }, { 0, 3, 6 },
{ 1, 4, 7 }, { 2, 5, 8 }, { 0, 4, 8 }, { 2, 4, 6 } };

static int mapa_caixa[9];
// centro x,y da localiza��o para cada caixa
static int mapa_objeto[9][2] = { { -6, 6 }, { 0, 6 }, { 6, 6 }, { -6, 0 }, { 0, 0 }, { 6, 0 }, { -6, -6 }, { 0, -6 }, { 6, -6 } };

Jogo jogo;
Rato rato;
Estado estado;

GLUquadricObj *cylinder;
//Modelo modelo;


/* Inicializa��o do ambiente OPENGL */
void Init(void)
{

	//struct tm *current_time;
	//time_t timer = time(0);

	//delay para o timer
	/*estado.delay = 1000;*/

	/*modelo.tamLado = 1;
	modelo.numLados = 5;
	modelo.raio = 0.75;*/


	// L� hora do Sistema
	//current_time = localtime(&timer);
	/*modelo.hora.hor = current_time->tm_hour;
	modelo.hora.min = current_time->tm_min;
	modelo.hora.seg = current_time->tm_sec;*/

	glClearColor(1, 1, 1, 0.0);

	/*glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);*/

	jogo.start_game = 0;
	jogo.win = 0;

	cylinder = gluNewQuadric();
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	gluQuadricNormals(cylinder, GLU_SMOOTH);
	gluQuadricOrientation(cylinder, GLU_OUTSIDE);

}

void init_game(){
	for (int i = 0; i < 9; i++)
	{
		mapa_caixa[i] = 0;
	}

	jogo.win = 0;
	jogo.start_game = 1;
}

/**************************************
***  callbacks de janela/desenho    ***
**************************************/

// CALLBACK PARA REDIMENSIONAR JANELA

void Reshape(int width, int height)
{
	rato.Win_x = width;
	rato.Win_y = height;
	GLint size;
	GLfloat ratio = (GLfloat)width / height;
	GLfloat ratio1 = (GLfloat)height / width;

	if (width < height)
		size = width;
	else
		size = height;

	// glViewport(botom, left, width, height)
	// define parte da janela a ser utilizada pelo OpenGL

	//glViewport(0, 0, (GLint) size, (GLint) size);
	glViewport(0, 0, width, height);


	// Matriz Projec��o
	// Matriz onde se define como o mundo e apresentado na janela
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// gluOrtho2D(left,right,bottom,top); 
	// projeccao ortogonal 2D, com profundidade (Z) entre -1 e 1
	if (width < height)
		gluOrtho2D(-9, 9, -9 * ratio1, 9 * ratio1);
	else
		gluOrtho2D(-9 * ratio, 9 * ratio, -9, 9);

	// Matriz Modelview
	// Matriz onde s�o realizadas as tranforma��es dos modelos desenhados
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// � usada para escrever no ecra
void imprimir(int x, int y, char *st)
{
	int tam, i;

	tam = strlen(st); // tamanho da string
	glRasterPos2i(x, y); // coordenadas para come�ar a escrever
	for (i = 0; i < tam; i++)  // loop para imprimir char
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, st[i]); // imprimir para o ecra
	}

}

void mensagem_inicio(){
	glPushMatrix();
	glColor3f(1, 0.0, 0.0);
	imprimir(-2, 0, "Jogo do Galo");
	imprimir(-3, -1, "Para come�ar o jogo carrega: ");
	imprimir(-3, -2, "botao direito do rato para X's");
	imprimir(-3, -3, "botao esquerdo do rato para O's");
	glPopMatrix();
}

void mensagem_final(int win){
	glPushMatrix();
	glColor3f(1.0, 0.0, 0.0);
	if (win == 1) imprimir(-2, 1, "Ganhou!!!");
	if (win == -1) imprimir(-2, 1, "Perdeu...");
	if (win == 2) imprimir(-2, 1, "Empate...");
	glPopMatrix();
}


// ... defini��o das rotinas auxiliares de desenho ...

void desenhagrelha(void){
	for (int ix = 0; ix < 4; ix++)
	{
		glPushMatrix();
		glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_LINES);
		glVertex2i(-9, -9 + ix * 6);
		glVertex2i(9, -9 + ix * 6);
		glEnd();
		glPopMatrix();
	}

	for (int iy = 0; iy < 4; iy++)
	{
		glPushMatrix();
		glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_LINES);
		glVertex2i(-9 + iy * 6, 9);
		glVertex2i(-9 + iy * 6, -9);
		glEnd();
		glPopMatrix();
	}
}

void desenhaO(int x, int y, int z){
	glPushMatrix();
	glColor3f(0.0, 0.0, 1.0);
	glTranslatef(x, y, z);
	glutSolidTorus(0.5, 2.0, 8, 16);
	glPopMatrix();
}

void desenhaX(int x, int y, int z){
	glPushMatrix();
	glTranslatef(x, y, z);

	glPushMatrix();
	glColor3f(0.0, 0.0, 1.0);
	glRotatef(90, 0, 1, 0);
	glRotatef(45, 1, 0, 0);
	glTranslatef(0, 0, -3);
	gluCylinder(cylinder, 0.5, 0.5, 6, 16, 16);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.0, 0.0, 1.0);
	glRotatef(90, 0, 1, 0);
	glRotatef(315, 1, 0, 0);
	glTranslatef(0, 0, -3);
	gluCylinder(cylinder, 0.5, 0.5, 6, 16, 16);
	glPopMatrix();

	glPopMatrix();
}

// Callback de desenho

void Draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	// ... chamada das rotinas auxiliares de desenho ...

	desenhagrelha();

	if (jogo.start_game == 0)
	{
		mensagem_inicio();
	}

	if (jogo.win != 0)
	{
		mensagem_final(jogo.win);
	}

	desenhaX(-6, 6, 0);
	desenhaO(-6, -6, 0);

	glFlush();
	if (estado.doubleBuffer)
		glutSwapBuffers();
}

/*******************************
***   callbacks timer/idle   ***
*******************************/

// Callback Idle

void Idle(void)
{

	// accoes a tomar quando o glut está idle 

	// redesenhar o ecra 
	// glutPostRedisplay();
}

// Callback de temporizador (n�o colocar instru��es de desenho aqui...)

void Timer(int value)
{

	// ... ac��es do temporizador ... 


	// redesenhar o ecr� (invoca o callback de desenho)
	glutPostRedisplay();

	glutTimerFunc(10, Timer, 1);
}


void imprime_ajuda(void)
{
	printf("\n\nDesenho de um quadrado\n");
	printf("h,H - Ajuda \n");
	printf("+   - Aumentar tamanho do Lado\n");
	printf("-   - Diminuir tamanho do Lado\n");
	printf("ESC - Sair\n");
}

/*******************************
***  callbacks de teclado    ***
*******************************/

// Callback para interac��o via teclado (carregar na tecla)

void Key(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:  // Tecla Escape
		exit(1);
		// ... ac��es sobre outras teclas ... 

	case 'h':
	case 'H':
		imprime_ajuda();
		break;
		//case '+':
		//	if (modelo.tamLado<1.8)
		//	{
		//		modelo.tamLado += 0.05;
		//		glutPostRedisplay(); // redesenhar o ecr�
		//	}
		//	break;
		//case '-':
		//	if (modelo.tamLado>0.2)
		//	{
		//		modelo.tamLado -= 0.05;
		//		glutPostRedisplay(); // redesenhar o ecr�
		//	}
		//	break;

	}

	if (DEBUG)
		printf("Carregou na tecla %c\n", key);

}

// Callback para interac��o via teclado (largar a tecla)

void KeyUp(unsigned char key, int x, int y)
{

	if (DEBUG)
		printf("Largou a tecla %c\n", key);
}

// Callback para interac��o via teclas especiais  (carregar na tecla)

void SpecialKey(int key, int x, int y)
{
	// ... accoes sobre outras teclas especiais ... 
	//    GLUT_KEY_F1 ... GLUT_KEY_F12
	//    GLUT_KEY_UP
	//    GLUT_KEY_DOWN
	//    GLUT_KEY_LEFT
	//    GLUT_KEY_RIGHT
	//    GLUT_KEY_PAGE_UP
	//    GLUT_KEY_PAGE_DOWN
	//    GLUT_KEY_HOME
	//    GLUT_KEY_END
	//    GLUT_KEY_INSERT 

	switch (key) {

		// redesenhar o ecra 
		//glutPostRedisplay();
	}


	if (DEBUG)
		printf("Carregou na tecla especial %d\n", key);
}

// Callback para interac��o via teclas especiais (largar a tecla)

void SpecialKeyUp(int key, int x, int y)
{

	if (DEBUG)
		printf("Largou a tecla especial %d\n", key);

}

void main(int argc, char **argv)
{
	estado.doubleBuffer = GL_TRUE;  // colocar a GL_TRUE para ligar o Double Buffer

	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(300, 300);
	glutInitDisplayMode(((estado.doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE) | GLUT_RGB);
	if (glutCreateWindow("Exemplo") == GL_FALSE)
		exit(1);

	glutSetWindowTitle("Jogo do Galo");

	Init();

	imprime_ajuda();

	// Registar callbacks do GLUT

	// callbacks de janelas/desenho
	glutDisplayFunc(Draw);
	glutReshapeFunc(Reshape);

	// Callbacks de teclado
	glutKeyboardFunc(Key);
	//glutKeyboardUpFunc(KeyUp);
	//glutSpecialFunc(SpecialKey);
	//glutSpecialUpFunc(SpecialKeyUp);

	// callbacks timer/idle
	glutTimerFunc(10, Timer, 1);
	//glutIdleFunc(Idle);


	// COME�AR...
	glutMainLoop();
}