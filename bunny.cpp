/****************************************************************************************/
/*                     bunny.cpp                    */
/****************************************************************************************/
/*         Affiche un lapin en 3D         */
/****************************************************************************************/

/* inclusion des fichiers d'en-tete freeglut */
#include <cstdio>
#include <cstdlib>
#include <jpeglib.h>
#include <cmath>
#include <iostream>

#ifdef __APPLE__
#include <GLUT/glut.h> /* Pour Mac OS X */
#else

#include <GL/freeglut.h> /* Pour les autres systemes */

#endif

/*class Point*/
class Point {
public :
    //coordonnées x, y et z du point
    double x;
    double y;
    double z;
};

struct Vector3 {
    double x, y, z;
};

char presse;
int anglex, angley, xold, yold;

/* Variables d'animations */
float anglePupilles = 0.0, angleBras = 0.0, angleAvantBras = 0.0;
float cameraAngle = 0.0f;

/* Données pour l'éllipsoïde */
#define NM 26  // Nombre de méridiens
#define NP 24  // Nombre de parallèles
Point sommets[NM * (NP + 1)]; // Tableau de tous les points de l'éllipsoïde
int faces[NM * NP][4]; // Tableau des 4 indices des sommets par face
Vector3 faceNormales[NM * (NP - 1)]; //Tableau des normales aux faces

/* Configuration de la position de la caméra avec gluLookAt */
// Position de la caméra
double eyeX = 0.0;
double eyeY = 0.0;
double eyeZ = 1.0;
// Point que la caméra regarde (centre de la scène)
double centerX = 0.0;
double centerY = 0.0;
double centerZ = 0.0;
// Direction "vers le haut" de la caméra
double upX = 0.0;
double upY = 1.0;
double upZ = 0.0;
//Zoom 2D
double zoom = 3;

/* Texture */
const int largimg = 256;
const int hautimg = 256;
unsigned char image[largimg*hautimg*3];
unsigned char texture1[largimg][hautimg][3];
unsigned char texture2[largimg][hautimg][3];
GLuint textureID1;
bool methodeTexture = true; // true = texture envelopé et false = texture face par face

/* Lumières */
GLfloat position_source0[] = {5.0, 8.5, 2.0, 1.0}; //coord homogènes : position
GLfloat dif_0[] = {0.5, 0.5, 1.0, 1.0};  //composante diffuse
GLfloat amb_0[] = {0.5, 0.5, 0.5, 1.0};   //composante ambiante
GLfloat spec_0[] = {1.0, 1.0, 1.0, 1.0}; //composante spéculaire

// Lumière spot
GLfloat spot_position[] = {1.0, 1.5, 0.0}; // position de la lumière
GLfloat spot_direction[] = {0.0, -1.0, 0.0}; // direction de la lumière
GLfloat dif_1[] = {1.0, 0.0, 0.0, 1.0};  //composante diffuse
GLfloat amb_1[] = {0.5, 0.5, 0.5, 1.0};   //composante ambiante
GLfloat spec_1[] = {0.5, 0.0, 0.5, 1.0}; //composante spéculaire
GLfloat spot_cutoff = 60.0;  // angle d'ouverture du cône
GLfloat spot_exponent = 1.5; // exposant de la lumière spot

/* Prototype des fonctions */
void affichage();

void reshape(int x, int y);

void mouse(int button, int state, int x, int y);

void mousemotion(int x, int y);

void loadJpegImage(const char *fichier, unsigned char [largimg][hautimg][3]);

void redim(int, int);

void clavierLettres(unsigned char touche, int x, int y);

void clavierDirection(int touche, int x, int y);

Vector3 ProduitVectoriel(Vector3,Vector3 );

void normaliser(Vector3& );

void gazon(float, float, float);

void feuilleCarotte(double radius);

void carotte(float r, float h);

void monEllipsoide(float rX, float rY, float rZ, bool texture);

void maSphere();

void drawEarsLeft();

void drawEarsRight();

void drawTeeth();

void drawPupil();

void drawEyes();

void drawNose();

void drawHead();

void drawWhiskerLeft();

void drawWhiskerRight();

void drawBody();

void drawArm();

void drawForeArm();

void drawLeg();

void drawFoot();

void drawToes();

void drawQueue();

void drawBunny();

void Lumiere();

void Lumiere_Spot();

void Pupilles_Anim();

void MouvementBras(int sens);

/* Fonctions d'animation */

void Pupilles_Anim() { // Animation automatique
    anglePupilles += 1;
    if (anglePupilles >= 360) {
        anglePupilles -= 360; // Inverser la direction lorsqu'on atteint une limite
    }
    glutPostRedisplay();
}

void MouvementBras(int sens) { //
        if (sens == 1 && angleAvantBras < 98.25 && angleBras < 90) {
            angleAvantBras += 0.75;
            angleBras += 0.75;
        }
        // Sens inverse
        if (sens == -1 && angleAvantBras> 0 && angleBras> 0){
            angleAvantBras -= 0.75;
             angleBras -= 0.75;
        }

    glutPostRedisplay();
}

/* Fonctions formes géometriques */

void maSphere() {
    int i, j;
    double r = 0.4;
    // Générer les sommets de la sphère
    for (i = 0; i < NM; i++) {
        double theta = i * 2 * M_PI / NM;
        for (j = 0; j < NP; j++) {
            double phi = -(M_PI / 2) + j * M_PI / (NP - 1);

            double x = r * cos(theta) * cos(phi);
            double y = r * sin(theta) * cos(phi);
            double z = r * sin(phi);
            Point S = {x, y, z};
            sommets[i + j * NM] = S;
        }
    }

    // Générer les faces de la sphère
    for (i = 0; i < NM; i++) {
        for (j = 0; j < NP - 1; j++) {
            int i1 = i + j * NM;
            int i2 = (i + 1) % NM + j * NM;
            int j1 = (i + 1) % NM + (j + 1) * NM;
            int j2 = i + (j + 1) * NM;

            faces[i + j * NM][0] = i1;
            faces[i + j * NM][1] = i2;
            faces[i + j * NM][2] = j1;
            faces[i + j * NM][3] = j2;
        }
    }


        // Générer les normales aux Faces
    for (i = 0; i < NM; i++)
    {
        for (j = 0; j < NP - 1; j++)
        {
            // Calculer les vecteurs entre les sommets de la face
            Vector3 v1 = {
                sommets[faces[i + j * NM][1]].x - sommets[faces[i + j * NM][0]].x,
                sommets[faces[i + j * NM][1]].y - sommets[faces[i + j * NM][0]].y,
                sommets[faces[i + j * NM][1]].z - sommets[faces[i + j * NM][0]].z
            };

            Vector3 v2 = {
                sommets[faces[i + j * NM][2]].x - sommets[faces[i + j * NM][0]].x,
                sommets[faces[i + j * NM][2]].y - sommets[faces[i + j * NM][0]].y,
                sommets[faces[i + j * NM][2]].z - sommets[faces[i + j * NM][0]].z
            };

            // Calculer la normale à l'aide du produit vectoriel
            faceNormales[i + j * NM] = ProduitVectoriel(v1, v2);
        }
    }


    // Afficher la sphère
    glBegin(GL_QUADS);
    for (i = 0; i < NM; i++) {
        for (j = 0; j < NP - 1; j++) {
            for (int k = 0; k < 4; k++) {
                // Spécifier la normale aux faces
                glNormal3f(faceNormales[i + j * NM].x, faceNormales[i + j * NM].y, faceNormales[i + j * NM].z);
                glVertex3f(sommets[faces[i + j * NM][k]].x, sommets[faces[i + j * NM][k]].y,
                           sommets[faces[i + j * NM][k]].z);
            }
        }
    }
    glEnd();

}

void monEllipsoide(float rX, float rY, float rZ, bool activeTexture) {
    int i, j, k;
    // Générer les sommets de l'ellipsoide
    for (i = 0; i < NM; i++) {
        double theta = i * 2 * M_PI / NM;
        for (j = 0; j < NP; j++) {
            double phi = -(M_PI / 2) + j * M_PI / (NP - 1);

            double x = rX * cos(theta) * cos(phi);
            double y = rY * sin(theta) * cos(phi);
            double z = rZ * sin(phi);
            Point S = {x, y, z};
            sommets[i + j * NM] = S;
        }
    }
    // Générer les faces de l'ellipsoide
    for (i = 0; i < NM; i++) {
        for (j = 0; j < NP - 1; j++) {
            int i1 = i + j * NM;
            int i2 = (i + 1) % NM + j * NM;
            int j1 = (i + 1) % NM + (j + 1) * NM;
            int j2 = i + (j + 1) * NM;

            faces[i + j * NM][0] = i1;
            faces[i + j * NM][1] = i2;
            faces[i + j * NM][2] = j1;
            faces[i + j * NM][3] = j2;
        }
    }

        // Générer les normales aux Faces
    for (i = 0; i < NM; i++)
    {
        for (j = 0; j < NP - 1; j++)
        {
            // Calculer les vecteurs entre les sommets de la face
            Vector3 v1 = {
                sommets[faces[i + j * NM][1]].x - sommets[faces[i + j * NM][0]].x,
                sommets[faces[i + j * NM][1]].y - sommets[faces[i + j * NM][0]].y,
                sommets[faces[i + j * NM][1]].z - sommets[faces[i + j * NM][0]].z
            };

            Vector3 v2 = {
                sommets[faces[i + j * NM][2]].x - sommets[faces[i + j * NM][0]].x,
                sommets[faces[i + j * NM][2]].y - sommets[faces[i + j * NM][0]].y,
                sommets[faces[i + j * NM][2]].z - sommets[faces[i + j * NM][0]].z
            };

            // Calculer la normale à l'aide du produit vectoriel
            faceNormales[i + j * NM] = ProduitVectoriel(v1, v2);
        }
    }


    if (activeTexture) { // activation de la texture ou non
        glEnable(GL_TEXTURE_2D);
    }
    // Afficher l'ellipsoide
    glBegin(GL_QUADS);
    for (i = 0; i < NM; i++)
    {
        for (j = 0; j < NP - 1; j++)
        {
            float u0 = (float)i / NM;
            float v0 = (float)j / (NP - 1);
            float u1 = (float)(i + 1) / NM;
            float v1 = (float)(j + 1) / (NP - 1);

            // Spécifier la normale aux faces
            glNormal3f(faceNormales[i + j * NM].x, faceNormales[i + j * NM].y, faceNormales[i + j * NM].z);

            // Premier sommet d'une face
            if (methodeTexture)
                glTexCoord2f(u0, v0);
            else
                glTexCoord2f(0, 0);
            glVertex3f(sommets[faces[i + j * NM][0]].x, sommets[faces[i + j * NM][0]].y, sommets[faces[i + j * NM][0]].z);

            // Deuxième sommet d'une face
            if (methodeTexture)
                glTexCoord2f(u1, v0);
            else
                glTexCoord2f(0, 1);
            glVertex3f(sommets[faces[i + j * NM][1]].x, sommets[faces[i + j * NM][1]].y, sommets[faces[i + j * NM][1]].z);

            // Troisième sommet d'une face
            if (methodeTexture)
                glTexCoord2f(u1, v1);
            else
                glTexCoord2f(1, 1);
            glVertex3f(sommets[faces[i + j * NM][2]].x, sommets[faces[i + j * NM][2]].y, sommets[faces[i + j * NM][2]].z);

            // Quatrième et dernier sommet d'une face
            if (methodeTexture)
                glTexCoord2f(u0, v1);
            else
                glTexCoord2f(1, 0);
            glVertex3f(sommets[faces[i + j * NM][3]].x, sommets[faces[i + j * NM][3]].y, sommets[faces[i + j * NM][3]].z);
        }
    }
    glEnd();

    if (activeTexture) { // activation de la texture ou non
        glDisable(GL_TEXTURE_2D);
    }
}

/* Fonctions élement de décoration */

void gazon(float l, float L, float h) {
    // Couleur du gazon
    glColor3f(0.23f, 0.61f, 0.14f);

    // Dessine la parcelle
    glBegin(GL_QUADS);
    glVertex3f(-L, h, -l);
    glVertex3f(L, h, -l);
    glVertex3f(L, h, l);
    glVertex3f(-L, h, l);
    glEnd();
}

void feuilleCarotte(double radius) {
    // Rayon du cercle
    double angleDebut = 0;
    double angleFinal = 3.14159265359;
    //Bout droit
    glPushMatrix();
    glTranslatef(1, 0, 0);
    glRotatef(-90, 1, 0, 0);
    glRotatef(180, 0, 1, 0);
    glBegin(GL_LINE_STRIP);

    for (double theta = angleDebut; theta <= angleFinal / 2; theta += 0.2) {

        double x = radius * cos(theta);
        double y = radius * sin(theta);

        glVertex3f(x, y, 0);

    }
    glEnd();
    glPopMatrix();
    //Bout gauche
    glPushMatrix();
    glTranslatef(-1, 0, 0);
    glRotatef(-90, 1, 0, 0);
    glBegin(GL_LINE_STRIP);
    for (double theta = angleDebut; theta <= angleFinal / 2; theta += 0.2) {
        double x = radius * cos(theta);
        double y = radius * sin(theta);
        glVertex3f(x, y, 0);
    }
    glEnd();
    glPopMatrix();
}

void carotte(float r, float h) {

    glPushMatrix();
    glScalef(0.15, 0.15, 0.15); // reduction taille carotte
    glRotatef(90, 0.0, 1.0, 0.0);
    glPushMatrix();
    glColor3f(0, 0.6, 0);
    glPushMatrix();
    glTranslatef(0, 0, 0.25);
    glRotatef(135, 0, 0, 1);
    feuilleCarotte(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, 0.25);
    glRotatef(90, 0, 0, 1);
    feuilleCarotte(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, 0.25);
    glRotatef(45, 0, 0, 1);
    feuilleCarotte(1);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(0, 0, 0.25);
    feuilleCarotte(1);
    glPopMatrix();

    glPopMatrix();
    glPushMatrix();
    glColor3f(0.95, 0.4, 0.106); // Couleur de la carotte
    glScalef(1, 2, 1.0);
    glutSolidCone(r, h, 20, 10);  // Utilise un cône pour représenter la carotte
    glPopMatrix();

    glPopMatrix();
}

void drawHead() {
    //Forme de tête

    glPushMatrix();
    glTranslatef(0.18, 2.4, 0.55);
    glRotatef(180, 1, 0, 0);
    glColor3f(0.7, 0.7, 0.7);
    monEllipsoide(0.2, 0.3, 0.2, false);
    glPopMatrix();

    //Bouche et dents

    glPushMatrix();
    glTranslatef(0.38, 2.3, 0.55);
    glRotatef(90, 0, 1, 0);
    glRotatef(90, 0, 0, 1);
    glScalef(0.08, 0.03, 0.01);
    glColor3f(1, 1, 1);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.38, 2.3, 0.59);
    glRotatef(90, 0, 1, 0);
    glRotatef(90, 0, 0, 1);
    glScalef(0.08, 0.03, 0.01);
    glColor3f(1, 1, 1);
    glutSolidCube(1);
    glPopMatrix();


}

void drawEyes() {
    //Yeux
    //droit
    glPushMatrix();
    glTranslatef(0.35, 2.5, 0.48);
    glScalef(0.09, 0.08, 0.08);
    monEllipsoide(0.5, 1, 0.5, false);
    glPopMatrix();
    //gauche
    glPushMatrix();
    glTranslatef(0.35, 2.5, 0.63);
    glScalef(0.08, 0.08, 0.08);
    monEllipsoide(0.5, 1, 0.5, false);
    glPopMatrix();

}

void drawPupil() {
    //droite
    glPushMatrix();
    glColor3f(0, 0, 0);
    glScalef(0.035, 0.035, 0.035);
    maSphere();
    glPopMatrix();
}

void drawNose() {
    //Nez
    glPushMatrix();
    glTranslatef(0.27, 1.68, 0.39);
    glRotatef(90, 0, 0, 1);
    glScalef(0.03, 0.03, 0.03);
    glColor3f(1, 0.71, 0.75);
    glutSolidTetrahedron();
    glPopMatrix();
}

void drawWhiskerLeft() {
    glPushMatrix();
    glScalef(0.5, 0.5, 0.5);
    glLineWidth(1.0);  // Épaisseur de la ligne

    // Moustache gauche
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);   // Point de départ au niveau du nez
    glVertex3f(-0.2, 0.1, 0.3);  // Point d'extrémité
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(-0.2, 0.0, 0.3);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(-0.2, -0.1, 0.3);
    glEnd();

    glLineWidth(1.0);  // Rétablir l'épaisseur par défaut
    glPopMatrix();
}

void drawWhiskerRight() {
    glPushMatrix();
    glScalef(0.5, 0.5, 0.5);
    glLineWidth(1.0);  // Épaisseur de la ligne

    // Moustache droite
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);   // Point de départ au niveau du nez
    glVertex3f(0.2, 0.1, 0.3);   // Point d'extrémité
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.2, 0.0, 0.3);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.2, -0.1, 0.3);
    glEnd();

    glLineWidth(1.0);  // Rétablir l'épaisseur par défaut
    glPopMatrix();
}

void drawEarsLeft() {
//Bout Gauche
    glPushMatrix();
    glTranslatef(0.1,2.28,0.4);
    glRotatef(90,0,0,1);
    glColor3f(0.7,0.7,0.7);
    monEllipsoide(0.4,0.05,0.07,false);
    glPopMatrix();

    //bout rose g
    glPushMatrix();
    glTranslatef(0.1,2.28,0.4);
    glRotatef(90,0,0,1);
    glScalef(0.7,0.7,0.7);
    glColor3f(1,0.71,0.75);
    monEllipsoide(0.4,0.05,0.07,false);
    glPopMatrix();
}

void drawEarsRight() {
  //Bout droit
    glPushMatrix();
    glTranslatef(0.135,2.225,0.2);
    glRotatef(-45,1,1,0);
    glRotatef(90,0,0,1);
    glColor3f(0.7,0.7,0.7);
    monEllipsoide(0.4,0.05,0.07,false);
    glPopMatrix();
    //bour rose d
    glPushMatrix();
    glTranslatef(0.135,2.225,0.2);
    glRotatef(-45,1,1,0);
    glRotatef(90,0,0,1);
    glScalef(0.7,0.7,0.7);
    glColor3f(1,0.71,0.75);
    monEllipsoide(0.4,0.05,0.07,false);
    glPopMatrix();
}

void drawBody() {
    glPushMatrix();
    glTranslatef(0.1, 0.5, 0.4);
    glRotatef(90, 1.0, 0.0, 0.0);
    monEllipsoide(0.2, 0.2, 0.5, true);
    glPopMatrix();
}

void drawArm() {
    // Dessin du Rotule
    glPushMatrix();
    glTranslatef(0.0, 0, 0.4);
    glScalef(0.8, 0.8, 0.8);
    glColor3f(0.75, 0.75, 0.75);
    glutSolidSphere(0.08, 15, 15);
    glPopMatrix();

    // Dessin du cylindre
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.07);
    glScalef(0.8, 0.8, 0.8);
    glutSolidCylinder(0.05, 0.33, 25, 25);
    glPopMatrix();

    // Dessin du Rotule
    glPushMatrix();
    glScalef(0.8, 0.8, 0.8);
    glColor3f(0.75, 0.75, 0.75);
    glutSolidSphere(0.08, 15, 15);
    glPopMatrix();
}

void drawForeArm() {
    // Dessin du cylindre
    glPushMatrix();

    glScalef(0.8, 0.8, 0.8);
    glColor3f(0.75, 0.75, 0.75);
    glutSolidCylinder(0.05, 0.45, 25, 25);
    glPopMatrix();

}

void drawFoot() {
    // Dessinez la base de la patte de lapin (un ellipsoïde)
    glPushMatrix();
    glTranslatef(0.17, 0.05, 0.3);
    glScalef(1.0, 1.0, 1.0); // Facteurs d'échelle pour l'ellipsoïde
    glColor3f(0.75, 0.75, 0.75); // Couleur de la pate
    monEllipsoide(0.175, 0.05, 0.08, false); // Crée une sphère de rayon 1.0
    glPopMatrix();
}

void drawToes() {
    // Dessin des orteils (ellipsoïdes)
    glPushMatrix();
    glTranslatef(0.2, 0.0, 0.05); // Position de l orteil
    glScalef(0.10, 0.05, 0.05); // Facteurs d'échelle pour l'ellipsoïde
    monEllipsoide(0.6, 0.8, 0.5, false); // Crée une ellipsoïde
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.2, 0.0, 0.1); // Position de l orteil
    glScalef(0.10, 0.05, 0.05); // Facteurs d'échelle pour l'ellipsoïde
    monEllipsoide(0.6, 0.8, 0.5, false);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.2, 0.0, 0.15); // Position de l orteil
    glScalef(0.10, 0.05, 0.05); // Facteurs d'échelle pour l'ellipsoïde
    monEllipsoide(0.6, 0.8, 0.5, false);
    glPopMatrix();
}

void drawLeg() {
    glPushMatrix();
    glTranslatef(0.055, 0.06, 0.31);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glutSolidCylinder(0.05, 0.60, 25, 25);
    glPopMatrix();

}

void drawQueue() {
    glPushMatrix();
    glTranslatef(-0.1, 0.65, 0.4);
    glScalef(0.2, 0.2, 0.2);
    glColor3f(0.75, 0.75, 0.75);
    maSphere();
    glPopMatrix();
}

void drawBunny() {

    //corps
    glPushMatrix();
    glColor3f(0.75, 0.75, 0.75);
    glTranslatef(0.0, 0.5, 0.0);
    drawBody();
    glPopMatrix();
    // Tête
    glPushMatrix();
    glScalef(0.7, 0.7, 0.7);
    drawHead();
    glPopMatrix();

    // Yeux
    glPushMatrix();
    glScalef(0.7, 0.7, 0.7);
    drawEyes();
    glPopMatrix();

    //Pupille gauche
    glPushMatrix();
    glTranslatef(0.25, 1.75, 0.44);  // Translation à la position de la pupille
    glRotatef(anglePupilles, 1, 0, 0);  // Rotation autour de l'axe x (ou de l'axe que vous souhaitez)
    glTranslatef(0, 0, 0.015);
    drawPupil();
    glPopMatrix();
    //Pupille droite
    glPushMatrix();
    glTranslatef(0.25, 1.75, 0.335);
    glRotatef(anglePupilles, 1, 0, 0);
    glTranslatef(0, 0, 0.015);
    drawPupil();
    glPopMatrix();

    // Nez
    glPushMatrix();
    drawNose();
    glPopMatrix();

    // Moustache GAUCHE
    glPushMatrix();
    glTranslatef(0.27, 1.67, 0.45);
    glRotatef(40, 0.0, 1.0, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    drawWhiskerLeft();
    glPopMatrix();
    // Moustache DROITE
    glPushMatrix();
    glTranslatef(0.27, 1.67, 0.35);
    glRotatef(150, 0.0, 1.0, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    drawWhiskerRight();
    glPopMatrix();

    // Oreilles
    glPushMatrix();
    drawEarsLeft();
    glPopMatrix();

    glPushMatrix();
    drawEarsRight();
    glPopMatrix();



    //Bras droit
    glPushMatrix();
    glTranslatef(0.05, 1.3, 0.18);
    glRotatef(110, 1.0, 0.0, 0.0);
    glPushMatrix();
    drawArm();
    glPopMatrix();
    //Avant-bras droit
    glPushMatrix();
    glTranslatef(0.0, 0, 0.465);
    drawForeArm();
    glPopMatrix();
    glPopMatrix(); // il merde

    //Bras Gauche
    glPushMatrix();
    glTranslatef(0.05, 1.3, 0.62);
    glRotatef(angleBras, 0.0, 0.0, 1.0); // rotate bras
    glPushMatrix();
    glRotatef(70, 1.0, 0.0, 0.0);
    glTranslatef(0.0, 0, 0);
    drawArm();
    glPopMatrix();


    // Avant-bras gauche

    glPushMatrix();
    glRotatef(70, 1.0, 0.0, 0.0);
    glTranslatef(0.0, 0, 0.465);
    glRotatef(angleAvantBras, 1.0, 1.0, 0.0); // rotate avant-bras et carotte

    drawForeArm();
    glTranslatef(-0.04, 0.05, 0.40);
    glRotatef(-40,0,0,1);
    carotte(0.25, 3);
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();

    // Jambes
    glPushMatrix();
    glColor3f(0.75, 0.75, 0.75);
    //droite
    drawLeg();
    glPopMatrix();
    //gauche
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.18);
    glColor3f(0.75, 0.75, 0.75);
    drawLeg();
    glPopMatrix();

    //pates
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    //droite
    drawFoot();
    //gauche
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.185);
    drawFoot();
    glPopMatrix();
    glPopMatrix();

    // Orteils gauche
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glTranslatef(0.1, 0.05, 0.38);
    drawToes();
    glPopMatrix();
    // Orteils droite
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glTranslatef(0.1, 0.05, 0.2);
    drawToes();
    glPopMatrix();

    //Queue
    glPushMatrix();
    drawQueue();
    glPopMatrix();
}

/* Éclairage simple */
void Lumiere()
{
        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);
        //spécification des propriétés
        glLightfv(GL_LIGHT0, GL_POSITION,position_source0);
        glLightfv(GL_LIGHT0, GL_AMBIENT, amb_0);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, dif_0);
        glLightfv(GL_LIGHT0, GL_SPECULAR, spec_0);
}

/* Lumière issu du spot */
void Lumiere_Spot()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    //spécification des propriétés
    glPushMatrix();
    glLightfv(GL_LIGHT1, GL_POSITION, spot_position);
    glLightfv(GL_LIGHT1, GL_AMBIENT, amb_1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, dif_1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spec_1);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spot_cutoff);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, spot_exponent);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 2.0);
}


int main(int argc, char **argv) {
    /* initialisation de glut et creation
       de la fenetre */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Cute Bunny");
    glClearColor(0.0, 0.0, 0.0, 1.0);

    /////////////  LUMIERE  ///////////////
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Lumiere();
    Lumiere_Spot();

    ////////////////////////////////////////////////////////////////
    ///////////////////////TEXTURES//////////////////////////////

    // Charger la texture1 à partir de l'image (assurez-vous d'avoir chargé l'image correcte)
    loadJpegImage("./texture_fourrure.jpg", texture1);
    // Générer une texture
    glGenTextures(1, &textureID1);
    // Lier la texture
    glBindTexture(GL_TEXTURE_2D, textureID1);
    // Configurer les paramètres de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Charger les données de l'image dans la texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, hautimg, largimg, 0, GL_RGB, GL_UNSIGNED_BYTE, texture1);

    /* enregistrement des fonctions de rappel */
    glutDisplayFunc(affichage);
    glutIdleFunc(Pupilles_Anim);
    glutKeyboardFunc(clavierLettres);
    glutSpecialFunc(clavierDirection);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(mousemotion);
    /* Entree dans la boucle principale glut */
    glutMainLoop();
    return 0;
}

void reshape(int x, int y) {
    if (x < y)
        glViewport(0, (y - x) / 2, x, x);
    else
        glViewport((x - y) / 2, 0, y, y);
}

void affichage() {
    /* effacement de l'image avec la couleur de fond */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-zoom, zoom, -zoom + 2, zoom + 2, -zoom, zoom);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ); // Position de la caméra

    glRotatef(angley, 1.0, 0.0, 0.0);
    glRotatef(anglex, 0.0, 1.0, 0.0);

    ////////// Dessin du Lapin ////////////
    gazon(5, 5, 0);

    glPushMatrix();
    drawBunny();
    glPopMatrix();


    //Repère
    //axe x en rouge
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0, 0.0);
    glVertex3f(1.0, 0, 0.0);
    glEnd();
    //axe des y en vert
    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1, 0.0);
    glEnd();
    //axe des z en bleu
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0, 0, 0.0);
    glVertex3f(0, 0, 1.0);

    glEnd();
    glFlush();
    //On echange les buffers
    glutSwapBuffers();
}

void clavierLettres(unsigned char touche, int x, int y) {
    switch (touche) {
        case 'p': /* affichage du carre plein */
            MouvementBras(1);
            glutPostRedisplay();
            break;
        case 'm': /* affichage du carre plein */
            MouvementBras(-1);
            glutPostRedisplay();
            break;
        case 'f': /* affichage en mode fil de fer */
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glutPostRedisplay();
            break;
        case 'E': // active la texture envelopé
            methodeTexture = true;
            break;
        case 'F': // active la texture face par face
            methodeTexture = false;
            break;
        case 'Z' :
            zoom -= 0.5;
            glutPostRedisplay();
            break;
        case 'z' :
            zoom += 0.5;
            glutPostRedisplay();
            break;
        case 'b' :
            angleBras++;
            glutPostRedisplay();
            break;
        case '1':
            glEnable(GL_LIGHT0);
            break;
        case '2':
            glEnable(GL_LIGHT1);
            break;
        case '.':
            glDisable(GL_LIGHT1);
            break;
        case '0':
            glDisable(GL_LIGHT0);
            break;
        case 'q' : /*la touche 'q' permet de quitter le programme */
            exit(0);
    }

}

void clavierDirection(int touche, int x, int y) {
    switch (touche) {
        case GLUT_KEY_UP:
            // Rotation vers le haut
            eyeY += 0.1f;
            if (eyeY >= 3)
                eyeY *= (-1);
            break;
        case GLUT_KEY_DOWN:
            // Rotation vers le bas
            eyeY -= 0.1f;
            if (eyeY <= -4)
                eyeY *= -0.1f;
            break;
        case GLUT_KEY_LEFT:
            // Rotation vers la gauche
            cameraAngle -= 0.1f;
            eyeX = centerX + cos(cameraAngle);
            eyeZ = centerZ + sin(cameraAngle);
            break;
        case GLUT_KEY_RIGHT:
            // Rotation vers la droite
            cameraAngle += 0.1f;
            eyeX = centerX + cos(cameraAngle);
            eyeZ = centerZ + sin(cameraAngle);
            break;
    }
}

void mouse(int button, int state, int x, int y) {
    /* si on appuie sur le bouton gauche */
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        presse = 1; /* le booleen presse passe a 1 (vrai) */
        xold = x; /* on sauvegarde la position de la souris */
        yold = y;
    }
    /* si on relache le bouton gauche */
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        presse = 0; /* le booleen presse passe a 0 (faux) */
}

void mousemotion(int x, int y) {
    if (presse) /* si le bouton gauche est presse */
    {
        /* on modifie les angles de rotation de l'objet
        en fonction de la position actuelle de la souris et de la derniere
         position sauvegardee */
        anglex = anglex + (x - xold);
        angley = angley + (y - yold);
        glutPostRedisplay(); /* on demande un rafraichissement de l'affichage */
    }

    xold = x; /* sauvegarde des valeurs courante de le position de la souris */
    yold = y;
}

// Fonction pour normaliser un vecteur
void normaliser(Vector3& v) {
    float distance = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (distance != 0.0) {
        v.x /= distance;
        v.y /= distance;
        v.z /= distance;
    }
}

Vector3 ProduitVectoriel(Vector3 v1, Vector3 v2) {
    Vector3 result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    normaliser(result);
    return result;
}

void redim(int l, int h) {
    if (l < h)
        glViewport(0, (h - l) / 2, l, l);
    else
        glViewport((l - h) / 2, 0, h, h);
}

void loadJpegImage( const char *fichier, unsigned char t[largimg][hautimg][3] ) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *file;
    unsigned char *ligne;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
#ifdef __WIN32
    if (fopen_s(&file,fichier,"rb") != 0)
      {
        fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
        exit(1);
      }
#elif __GNUC__
    if ((file = fopen(fichier, "rb")) == 0) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier texture.jpg\n");
        exit(1);
    }
#endif
    jpeg_stdio_src(&cinfo, file);
    jpeg_read_header(&cinfo, TRUE);

    if ((cinfo.image_width!=256)||(cinfo.image_height!=256)) {
      fprintf(stdout,"Erreur : l'image doit etre de taille 256x256\n");
      exit(1);
    }

    if (cinfo.jpeg_color_space == JCS_GRAYSCALE) {
        fprintf(stdout, "Erreur : l'image doit etre de methodeTexture RGB\n");
        exit(1);
    }

    jpeg_start_decompress(&cinfo);
    ligne = image;
    while (cinfo.output_scanline < cinfo.output_height) {
        ligne = image + 3 * largimg * cinfo.output_scanline;
        jpeg_read_scanlines(&cinfo, &ligne, 1);
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    // création de la texture
    for (int i=0;i<256;i++)
    {
        for (int j=0;j<256;j++)
        {
            t[i][j][0]=image[i*256*3+j*3];
            t[i][j][1]=image[i*256*3+j*3+1];
            t[i][j][2]=image[i*256*3+j*3+2];
        }
    }
}
