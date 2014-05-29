#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <gl/glut.h>

// to denote direction
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

int lvl = 1; // level of the game currently
int points = 0;
int size = 0;
byte gameOver = true;
byte EnableLight = true;
byte wall=false; // for walled / unwalled game modes
byte sphere=false; // for spherical / cubical body part.

int bodyPos[2][100] = {{}}; // stores the postion of the head successively
int _x = 5; // head pos
int _z = 10;
int _oldX[2] = {};
int _oldZ[2] = {};
byte direction = 0;

int _bx = 0; // food pos
int _bz = 0;

int _w = 1366;
int _h = 768;
int _Giw = 0;
int _Gih = 0;
int _Gfw = 150;
int _Gfh = 150;

float view_rotx ;
float view_roty ;
float view_rotz ;

static float headRotation=90.0F ;
static float zoom=-300.0F ;

void initLight()
{

    float ambientColor[] = {0.5f, 1.0f, 0.5f, 0.5f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    float lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f};
    float lightPos0[] = {4.0f, 0.0f, 8.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);


    float lightColor1[] = {0.5f, 0.2f, 0.2f, 1.0f};

    float lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
}

void Initialize(void)
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    if(EnableLight)
    {
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glEnable(GL_NORMALIZE);
    }
}

void resize (int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1, 800.0);
}

void Write(char *string)
{
    while(*string)
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *string++);
}

void ManipulateViewAngle()
{
    glRotatef(view_rotx,1.0,0.0,0.0);
    glRotatef(view_roty,0.0,1.0,0.0);
    glRotatef(view_rotz,0.0,0.0,1.0);
}

void Reset()
{
    _x = 5;
    _z = 10;
    direction = 0;
    lvl = 1;
    points = 0;
    size = 0;
    gameOver = false;
    view_rotx=45.0F ;
    view_roty=0.0F ;
    view_rotz=0.0F ;
    headRotation=90.0F ;
    zoom=-300.0F ;
}

void WelcomeScreen()
{
    glColor3f(1,0, 0);
    glRasterPos2f(0, 30);
    Write("Welcome To Snake 3D Game.");

    glColor3f(0, 0, 1);
    glRasterPos2f(0, 10);
    Write("developed by kushagra singh");

    glColor3f(0, 0, 1);
    glRasterPos2f(0, 0);
    Write("To Start Playing please press Enter. Enjoy");
}

void DrawSnake()
{
    int i;

    glPushMatrix();
    ManipulateViewAngle();

// create the grid
    glPushMatrix();
    glColor3f(0.6f, 0.7f, 0.8f);
    glTranslatef(75.0, -16.0, 75.0);
    glScalef(155,5.0,155);
    glutSolidCube(1);
    glPopMatrix();

// actual snake.
    glColor3f(1.0,1.0,0.0);
    glTranslatef(_x,-10.0,_z);
    glScalef(0.5,0.5,0.5);
    glutSolidSphere(10,20,20);
    glRotatef(headRotation, 0.0, 1.0, 0.0);
    glColor3f(1.0,1.0,0.0);
    glTranslatef(0,0.0,6.0);
    glScalef(0.8,1.0,1.0);
    glutSolidCone(10,10,20,20);
    glColor3f(0,0,0);
    glTranslatef(-4.0,10.0,0.0);
    glScalef(0.3,0.3,0.3);
    glutSolidSphere(10,20,20);
    glTranslatef(26.0,0.0,0.0);
    glutSolidSphere(10,20,20);
    glPopMatrix();

    for(i=0; i<size; i++)
    {
        glPushMatrix();
        ManipulateViewAngle();
        glTranslatef(bodyPos[0][i],-10.0,bodyPos[1][i]);
        glColor3f(1.0,1.0,0.6);
        glScalef(0.5,0.5,0.5);
        if(sphere)
            glutSolidSphere(7,20,20);
        else
            glutSolidCube(10);
        glPopMatrix();
    }
}

void DrawFood()
{
    glPushMatrix();
    ManipulateViewAngle();
    glTranslatef(_bx,-10.0,_bz);
    glColor3f(0.8, 0.4, 0.4);
    glScalef(0.5,0.5,0.5);
    glutSolidSphere(7,20,20);
    glPopMatrix();
}

void GameStatus()
{
    char tmp_str[40];
    char mode_str[10];
    glColor3f(0, 0, 0);
    glRasterPos2f(5, 50);

    if(wall)
        sprintf(mode_str,"Walled");
    else
        sprintf(mode_str,"Unwalled");
    sprintf(tmp_str, "Level: %d Points: %d Game Mode : %s", lvl, points,mode_str);

    Write(tmp_str);
}

int RandomNumber(int high, int low)
{
    return (rand() % (high-low))+low;
}

void newFood()
{
    time_t seconds;
    time(&seconds);
// seed for the rand function
    srand((unsigned int) seconds);
    _bx = RandomNumber((_Gfw-5)/6, _Giw/6)*6+5;
    _bz = RandomNumber((_Gfh-10)/6, _Gih/6)*6+10;
}

bool collision()
{
    int i;

    for(i=0; i<size; i++)
    {
        if((bodyPos[0][i] == _x && bodyPos[1][i] == _z))
            return true;
    }
    return false;
}

void Run(int value)
{
    int i;
// oldX used to store head pos in body pos.
    _oldX[1] = _x;
    _oldZ[1] = _z;
    switch(direction)
    {
    case RIGHT:
        headRotation =90;
        _x += 6;
        if(_x > _Gfw-2)
        {
            if(wall)
                gameOver=true;
            else
                _x = _Giw-1;
        }
        break;
    case LEFT:
        headRotation =-90;
        _x -= 6;
        if(_x < 0)
        {
            if(wall)
                gameOver=true;
            else
                _x = _Gfw-2;
        }
        break;
    case UP:
        headRotation =0;
        _z += 6;
        if(_z > _Gfh-2)
        {
            if(wall)
                gameOver=true;
            else
                _z = _Gih-1;
        }
        break;
    case DOWN:
        headRotation =180;
        _z -= 6;
        if(_z < 2)
        {
            if(wall)
                gameOver=true;
            else
                _z = _Gfh-2;
        }
        break;
    }

    if(collision()) gameOver = true;

    if((_x == _bx && _z == _bz) ||
            ((_x >= _bx) && (_x <= _bx + 4) && (_z >= _bz) && (_z <= _bz + 4)) ||
            ((_x <= _bx) && (_x >= _bx - 4) && (_z <= _bz) && (_z >= _bz - 4)) ||
            ((_x <= _bx) && (_x >= _bx - 4) && (_z >= _bz) && (_z <= _bz + 4)) ||
            ((_x >= _bx) && (_x <= _bx + 4) && (_z <= _bz) && (_z >= _bz - 4)))
    {
        points++;
        if(points < 100) size++;
        if(points%5 == 0 && lvl < 15) lvl++;
        newFood();
    }

    for(i = 0; i<size; i++)
    {
        _oldX[0] = _oldX[1];
        _oldZ[0] = _oldZ[1];
        _oldX[1] = bodyPos[0][i];
        _oldZ[1] = bodyPos[1][i];
        bodyPos[0][i] = _oldX[0];
        bodyPos[1][i] = _oldZ[0];
    }

    glutTimerFunc(130-lvl*4, Run,0);
}

void Display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if(EnableLight)
        initLight();

    glTranslatef (-60.0, 40.0, zoom);

    if(!gameOver)
    {
        GameStatus();
        DrawFood();
        DrawSnake();
    }
    else
        WelcomeScreen();

    glutPostRedisplay();
    glutSwapBuffers();
}

void Special(int key, int x, int y)
{
    switch(key)
    {
    case GLUT_KEY_RIGHT :
        if(direction != LEFT)
            direction = RIGHT;
        break;
    case GLUT_KEY_LEFT :
        if(direction != RIGHT)
            direction = LEFT;
        break;
    case GLUT_KEY_UP :
        if(direction != UP)
            direction = DOWN;
        break;
    case GLUT_KEY_DOWN :
        if(direction != DOWN)
            direction = UP;
        break;
    }
}

void keyboard (unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'x' :
        view_rotx +=2 ;
        glutPostRedisplay();
        break;
    case 'X' :
        view_rotx -=2 ;
        glutPostRedisplay();
        break;
    case 'y' :
        view_roty +=2 ;
        glutPostRedisplay();
        break;
    case 'Y' :
        view_roty -=2 ;
        glutPostRedisplay();
        break;
    case 'z' :
        view_rotz +=2 ;
        glutPostRedisplay();
        break;
    case 'Z' :
        view_rotz -=2 ;
        glutPostRedisplay();
        break;
    case '+' :
        zoom++ ;
        glutPostRedisplay();
        break;
    case '-' :
        zoom-- ;
        glutPostRedisplay();
        break;
    case '\r' :
        Reset() ;
        glutPostRedisplay();
        break;
    case '1':
        wall=false;
        break;
    case '2':
        wall=true;
        break;
    case '3':
        sphere=false;
        break;
    case '4':
        sphere=true;
        break;
    case 27:
        exit(0);
        break;
    default:
        break;
    }
}

int main(void)
{
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(_w,_h);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Snake 3D");
    glutFullScreen();
    glutSpecialFunc(Special);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(Display);
    glutReshapeFunc(resize);
    newFood();
    Run(0);
    Initialize();
    glutMainLoop();
}

