#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define true 1
#define false !true

const int pWidth = 800, pHeight = 800, // Physical window size
    lWidth = 1000, lHeight = 1000,     // Logical window size
    btnWidth = 200, btnHeight = 100,
          gridWidth = 800, gridSpacing = 3,
          nCells = 50,
          sideSqWid = 50, rate = 2, delay = 10,
          nButtons = 2;
const int cX = lWidth / 2, cY = lHeight / 2,
          gX = (lWidth + gridWidth) / 2, gY = (lHeight + gridWidth) / 2,
          cellWidth = (gridWidth / nCells) - gridSpacing * 2;
int alphaSq = 0,
    mX = 0, mY = 0,
    screen = 0, // 0 -> Main Menu, 1 -> Game
    currGen = 0, maxGen = 0;
int cellToggleFlag = true;
int buttonBounds[nButtons][4] = {}, // minX, maxX, minY, maxY
    cellAlive[nCells][nCells] = {};

class node
{
public:
    int (*generation)[nCells];
    node *prev;
    node *next;
};

node *head, *currentNode;

node *addNode(node *currentNode)
{
    node *newNode = new node;
    currentNode->next = newNode;
    newNode->prev = currentNode;
    newNode->next = NULL;
    return newNode;
}

void delList()
{
    currentNode = head;
    while (currentNode->next != NULL)
    {
        currentNode = currentNode->next;
        delete currentNode->prev;
    }
    delete currentNode;
}

void initList()
{
    head = new node;
    currentNode = head;
    currentNode->prev = NULL;
    currentNode->next = NULL;
    currentNode->generation = NULL;
}

void init2D(float r, float g, float b)
{
    glClearColor(r, g, b, 0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, lWidth, 0.0, lHeight);
}

void drawString(const char *str, int x, int y, void *font)
{
    int maxlen = strlen(str);
    glRasterPos2d(x, y);
    for (int i = 0; i < maxlen; ++i)
    {
        if (str[i] == '\n')
        {
            y -= 0.05 * lHeight;
            glRasterPos2d(x, y);
        }
        else
            glutBitmapCharacter(font, str[i]);
    }
}

void drawButton(int cX, int cY, const char *btnText)
{
    glColor3f(1, 1, 0);

    glBegin(GL_QUADS);
    glColor3f(0.36, 0.16, 0.46);
    glVertex2i(cX - btnWidth / 2, cY - btnHeight / 2);
    glColor3f(0.36, 0, 0.76);
    glVertex2i(cX + btnWidth / 2, cY - btnHeight / 2);
    glVertex2i(cX + btnWidth / 2, cY + btnHeight / 2);
    glColor3f(0.36, 0.16, 0.46);
    glVertex2i(cX - btnWidth / 2, cY + btnHeight / 2);
    glEnd();

    glLineWidth(10);
    glColor3f(1, 0.647, 0);
    glBegin(GL_LINE_LOOP);
    glVertex2i(cX - btnWidth / 2, cY - btnHeight / 2);
    glVertex2i(cX + btnWidth / 2, cY - btnHeight / 2);
    glVertex2i(cX + btnWidth / 2, cY + btnHeight / 2);
    glVertex2i(cX - btnWidth / 2, cY + btnHeight / 2);
    glEnd();

    glColor3f(1, 0.647, 0);
    drawString(btnText, cX - 0.15 * btnWidth, cY - 0.1 * btnHeight, GLUT_BITMAP_TIMES_ROMAN_24);
}

void drawSquare(int cX, int cY, int sqWid)
{
    glBegin(GL_QUADS);
    glVertex2i(cX - sqWid / 2, cY - sqWid / 2);
    glVertex2i(cX + sqWid / 2, cY - sqWid / 2);
    glVertex2i(cX + sqWid / 2, cY + sqWid / 2);
    glVertex2i(cX - sqWid / 2, cY + sqWid / 2);
    glEnd();
}

void drawCells()
{
    glColor3f(1, 1, 1);
    for (int i = 0; i < nCells; ++i)
    {
        for (int j = 0; j < nCells; ++j)
        {
            if (cellAlive[i][j])
                glColor3f(0, 0, 1);
            else
                glColor3f(1, 1, 1);
            drawSquare(gX - gridSpacing - (i + 0.5) * (cellWidth + gridSpacing * 2), gY - gridSpacing - (j + 0.5) * (cellWidth + gridSpacing * 2), cellWidth);
        }
    }
}

void setButtonBounds(int cX, int cY, int button)
{
    buttonBounds[button][0] = cX - btnWidth / 2;
    buttonBounds[button][1] = cX + btnWidth / 2;
    buttonBounds[button][2] = cY - btnHeight / 2;
    buttonBounds[button][3] = cY + btnHeight / 2;
}

void timer(int value)
{
    glutTimerFunc(delay, timer, 10);
    glutPostRedisplay();
}

void displayMainMenu()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 1, 1);

    drawString("Conway's Game Of Life", cX - 0.15 * lWidth, cY + 0.3 * lHeight, GLUT_BITMAP_TIMES_ROMAN_24);

    drawButton(cX, cY + btnHeight, "Play");
    drawButton(cX, cY - btnHeight, "Exit");

    glColor3f(1, 1, 1);
    for (int i = 0; i < 15; ++i)
    {
        drawSquare(0.06 * lWidth, i / 10.0 * lHeight - alphaSq, sideSqWid);
        drawSquare(0.94 * lWidth, i / 10.0 * lHeight + alphaSq, sideSqWid);
    }

    if (alphaSq >= 2 * sideSqWid)
        alphaSq = 0;
    else
        alphaSq += rate;

    glutSwapBuffers();
    glFlush();
}

void displayGame()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1, 1, 1);

    drawString("F1 - Restart\nF2 - Main Menu", 0.02 * lWidth, 0.98 * lHeight, GLUT_BITMAP_HELVETICA_10);
    drawString("-> - Next Generation\n<- Previous Generation", 0.84 * lWidth, 0.98 * lHeight, GLUT_BITMAP_HELVETICA_10);
    drawString("Conway's Game Of Life", cX - 0.15 * lWidth, cY + 0.45 * lHeight, GLUT_BITMAP_TIMES_ROMAN_24);
    if (!cellToggleFlag) // Cells not toggleable, therefore game has started
    {
        drawString("Game in progress.", 0.4 * lWidth, 0.05 * lHeight, GLUT_BITMAP_TIMES_ROMAN_24);
        char msg[60];
        sprintf(msg, "Current\nGeneration:\n%d\nMaximum\nGeneration:\n%d", currGen, maxGen);
        drawString(msg, 0 * lWidth, 0.7 * lHeight, GLUT_BITMAP_HELVETICA_10);
    }
    else // Game hasn't started yet
        drawString("Set Initial Cells.", 0.4 * lWidth, 0.05 * lHeight, GLUT_BITMAP_TIMES_ROMAN_24);

    glColor3f(0, 0, 0);
    drawSquare(gX - (gridWidth / 2) - gridSpacing, gY - (gridWidth / 2) - gridSpacing, gridWidth + (4 * gridSpacing));
    drawCells();

    glutSwapBuffers();
    glFlush();
}

void resetCells() // Set all cells to dead
{
    for (int i = 0; i < nCells; ++i)
        for (int j = 0; j < nCells; ++j)
            cellAlive[i][j] = false;
}

void loadCells()
{
    for (int i = 0; i < nCells; ++i)
        for (int j = 0; j < nCells; ++j)
            cellAlive[i][j] = currentNode->generation[i][j];
}

int getCellIndex(int x, int y)
{
    int neighbours = 0;
    for (int i = x - 1; i <= x + 1; ++i)
        for (int j = y - 1; j <= y + 1; ++j)
        {
            if (i == x && j == y) // Skip current cell
                continue;
            if (i > 0 && i < nCells && j > 0 && j < nCells && cellAlive[i][j]) // If neighbour indices are of a valid alive cell
                neighbours++;
        }
    return neighbours;
}

void saveGen0() // Save the initial generation in the memory
{
    int(*currAlive)[nCells] = new int[nCells][nCells];
    for (int i = 0; i < nCells; ++i)
        for (int j = 0; j < nCells; ++j)
            currAlive[i][j] = cellAlive[i][j];
    currentNode->generation = currAlive;
}

void nextGeneration()
{
    if (currentNode->next != NULL)  // If next generation has already been calculated, go to it.
        currentNode = currentNode->next;
    else    // Otherwise calculate new generation.
    {
        int(*currAlive)[nCells] = new int[nCells][nCells];
        for (int i = 0; i < nCells; ++i)
            for (int j = 0; j < nCells; ++j)
                currAlive[i][j] = cellAlive[i][j];
        for (int i = 0; i < nCells; ++i)
            for (int j = 0; j < nCells; ++j)
            {
                int index = getCellIndex(i, j);
                if (index >= 0 && (index < 2 || index > 3))
                    currAlive[i][j] = false;
                else if (index == 3)
                    currAlive[i][j] = true;
                else if (index == 2)
                    ;
                else
                    printf("????????? butHow??\n");
            }
        currentNode = addNode(currentNode);
        currentNode->generation = currAlive;
        maxGen++;
    }
    loadCells();
    currGen++;
}

void prevGeneration()
{
    if (currentNode->prev == NULL)
        return;
    currentNode = currentNode->prev;
    loadCells();
    currGen--;
}

void resetGame()
{
    screen = 1;
    cellToggleFlag = true;
    currGen = 0;
    maxGen = 0;
    resetCells();
    delList();
    initList();
}

void keyboard(unsigned char key, int x, int y)
{
    glutPostRedisplay();
}

void speshul(int key, int x, int y)
{
    if (screen == 1)
    {
        if (key == GLUT_KEY_F1) // Restart
            resetGame();
        else if (key == GLUT_KEY_F2) // Main Menu
        {
            screen = 0;
            glutDisplayFunc(displayMainMenu);
        }
        else if (key == GLUT_KEY_RIGHT)
        {
            cellToggleFlag = false;
            if(currentNode == head && currentNode->generation == NULL)
                saveGen0();
            nextGeneration();
        }
        else if (key == GLUT_KEY_LEFT)
            prevGeneration();
    }
    glutPostRedisplay();
}

void click(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        mX = x;
        mX = 0.5 + 1.0 * mX * lWidth / pWidth;
        mY = pHeight - y;
        mY = 0.5 + 1.0 * mY * lHeight / pHeight;
        if (screen == 1 && mX >= gX - gridWidth && mX <= gX && mY >= gY - gridWidth && mY <= gY && cellToggleFlag) // Mouse clicked in bounds of grid, cells toggleable
        {
            int x = ((gX - mX - gridSpacing) / (cellWidth + 2 * gridSpacing)) % 50;
            int y = ((gY - mY - gridSpacing) / (cellWidth + 2 * gridSpacing)) % 50;
            cellAlive[x][y] = !cellAlive[x][y]; // Toggle cell when clicked
        }
        for (int i = 0; i < nButtons && screen == 0; ++i) // Don't loop if not in main menu
        {
            if (mX >= buttonBounds[i][0] && mX <= buttonBounds[i][1] && mY >= buttonBounds[i][2] && mY <= buttonBounds[i][3]) // Mouse clicked a button
            {
                if (i == 0) // Play button
                {
                    screen = 1;
                    glutDisplayFunc(displayGame);
                }
                else if (i == 1) // Exit button
                {
                    delList();
                    exit(0);
                }
            }
        }
    }
    glutPostRedisplay();
}

int main(int argc, char *argv[])
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(pWidth, pHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Conway's Game Of Life");
    init2D(0.3583, 0.1741, 0.4676);
    glutDisplayFunc(displayMainMenu);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(speshul);
    glutMouseFunc(click);
    glutTimerFunc(delay, timer, 10);

    setButtonBounds(cX, cY + btnHeight, 0);
    setButtonBounds(cX, cY - btnHeight, 1);

    initList(); // Initialize linked list

    glutMainLoop();
}