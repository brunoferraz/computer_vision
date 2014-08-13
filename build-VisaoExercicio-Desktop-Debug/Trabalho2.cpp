#include "include/linearSolver.h"
#include "include/directLinearSolver.hpp"
#include <Dense>
#include <iostream>

#include <malloc.h>
#include <GL/glui.h>

using namespace std;
using namespace Eigen;

#define PROJECTIVE 1
#define AFFINE 2


struct point2D{
    double x;
    double y;
};

struct point3D{
    double x;
    double y;
    double w;
};

void mouse(int button, int state, int x, int y);

gcgIMAGE img; ///input image
gcgIMAGE newImg, newImg2; ///rectified image
float *pixelsInput, *pixelsAffine, *pixelsSimilarity; ///auxiliary pixel matrix (for glut)
GLUI *glui; ///glui window
point3D firstPoint, secondPoint; ///point to determine a line
int space = PROJECTIVE; ///the current space
point3D *lines; ///lines marked on the image
int countPoints = 0; ///count the number of points already saved
int countLines = 0; ///count the number of lines already saved
int method; ///the method chosen

///calculate the vectorial product between two vectors
void vectorialProduct(point3D *vector1, point3D *vector2, point3D *product){
    product->w = vector1->x*vector2->y - vector2->x*vector1->y;
    product->x = (vector1->y*vector2->w - vector2->y*vector1->w)/product->w;
    product->y = (vector2->x*vector1->w - vector1->x*vector2->w)/product->w;
    product->w = 1.;
}

///apply the matrix H to the point2D inputPoint and save the result in outputPoint
void matrixApplication(double **H, point2D *inputPoint, point2D *outputPoint){
    double z = H[2][0]*inputPoint->x + H[2][1]*inputPoint->y + H[2][2];
    outputPoint->x = (H[0][0]*inputPoint->x + H[0][1]*inputPoint->y + H[0][2])/z;
    outputPoint->y = (H[1][0]*inputPoint->x + H[1][1]*inputPoint->y + H[1][2])/z;
}

///glut display function
void displayInput(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawPixels(img.width, img.height, GL_RGBA, GL_FLOAT, pixelsInput);
    glutSwapBuffers();
}

void displayRectAffine(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawPixels(newImg.width, newImg.height, GL_RGBA, GL_FLOAT, pixelsAffine);
    glutSwapBuffers();
}

void displayRectSimilarity(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawPixels(newImg2.width, newImg2.height, GL_RGBA, GL_FLOAT, pixelsSimilarity);
    glutSwapBuffers();
}

void imgResize(gcgIMAGE *src, gcgIMAGE *dest, float scale){
    gcgIMAGE aux;
    aux.createImage((int) src->width*scale, (int) src->height*scale, 32);

    for(int i = 0; i < aux.width; i++){
        for(int j = 0; j < aux.height; j++){
            double a0 = floor(i/scale) + 1 - i/scale;
            double a1 = i/scale - floor(i/scale);
            double a00 = (floor(j/scale) + 1 - j/scale)*a0;
            double a01 = (j/scale - floor(j/scale))*a0;
            double a10 = (floor(j/scale) + 1 - j/scale)*a1;
            double a11 = (j/scale - floor(j/scale))*a1;
            VECTOR4 imgPixel00, imgPixel01, imgPixel10, imgPixel11;
            src->getPixelColor(floor(i/scale), floor(j/scale), imgPixel00);
            src->getPixelColor(floor(i/scale) + 1, floor(j/scale), imgPixel10);
            src->getPixelColor(floor(i/scale), floor(j/scale) + 1, imgPixel01);
            src->getPixelColor(floor(i/scale) + 1, floor(j/scale) + 1, imgPixel11);

            VECTOR4 newImgPixel = {
                a00*imgPixel00[0] + a10*imgPixel10[0] + a01*imgPixel01[0] + a11*imgPixel11[0],
                a00*imgPixel00[1] + a10*imgPixel10[1] + a01*imgPixel01[1] + a11*imgPixel11[1],
                a00*imgPixel00[2] + a10*imgPixel10[2] + a01*imgPixel01[2] + a11*imgPixel11[2],
                a00*imgPixel00[3] + a10*imgPixel10[3] + a01*imgPixel01[3] + a11*imgPixel11[3]
            };
            aux.setPixelColor(i, j, newImgPixel);
        }
    }

    dest->duplicateImage(&aux);
}

void removeProjective(point3D* lines){
    double xMin, xMax, yMin, yMax;

    double **H = (double**) malloc (3*sizeof(double*));
    double **invH = (double**) malloc (3*sizeof(double*));
    for(int i = 0; i < 3; i++){
        H[i] = (double*) malloc(3*sizeof(double));
        invH[i] = (double*) malloc(3*sizeof(double));
    }
    vectorialProduct(&lines[0], &lines[1], &firstPoint);
    vectorialProduct(&lines[2], &lines[3], &secondPoint);
    vectorialProduct(&firstPoint, &secondPoint, &lines[0]);

    H[0][0] = 1.;
    H[0][1] = 0.;
    H[0][2] = 0.;

    H[1][0] = 0.;
    H[1][1] = 1.;
    H[1][2] = 0.;

    H[2][0] = lines[0].x/lines[0].w;
    H[2][1] = lines[0].y/lines[0].w;
    H[2][2] = lines[0].w/lines[0].w;

    calculateInverseLU(H, invH, 3, 3);

    point2D result;
    point2D vertex = {0., 0.};
    matrixApplication(H, &vertex, &result);
    xMin = result.x;
    xMax = result.x;
    yMin = result.y;
    yMax = result.y;

    vertex = {img.width, 0.};
    matrixApplication(H, &vertex, &result);
    xMin = result.x < xMin ? result.x : xMin;
    xMax = result.x > xMax ? result.x : xMax;
    yMin = result.y < yMin ? result.y : yMin;
    yMax = result.y > yMax ? result.y : yMax;

    vertex = {0., img.height};
    matrixApplication(H, &vertex, &result);
    xMin = result.x < xMin ? result.x : xMin;
    xMax = result.x > xMax ? result.x : xMax;
    yMin = result.y < yMin ? result.y : yMin;
    yMax = result.y > yMax ? result.y : yMax;

    vertex = {img.width, img.height};
    matrixApplication(H, &vertex, &result);
    xMin = result.x < xMin ? result.x : xMin;
    xMax = result.x > xMax ? result.x : xMax;
    yMin = result.y < yMin ? result.y : yMin;
    yMax = result.y > yMax ? result.y : yMax;

    newImg.createImage((int)xMax - (int)xMin + 2, (int) yMax - (int) yMin + 2, 32);

    for(int i = (int) xMin; i < xMax; i++){
        for(int j = (int) yMin; j < yMax; j++){
            point2D imagePoint = {.x = i, .y = j};
            matrixApplication(invH, &imagePoint, &result);

            if(result.x >= 0 && result.x < img.width && result.y >= 0 && result.y < img.height){
                double a0 = floor(result.x) + 1 - result.x;
                double a1 = result.x - floor(result.x);
                double a00 = (floor(result.y) + 1 - result.y)*a0;
                double a01 = (result.y - floor(result.y))*a0;
                double a10 = (floor(result.y) + 1 - result.y)*a1;
                double a11 = (result.y - floor(result.y))*a1;
                VECTOR4 imgPixel00, imgPixel01, imgPixel10, imgPixel11;
                img.getPixelColor(floor(result.x), floor(result.y), imgPixel00);
                img.getPixelColor(floor(result.x) + 1, floor(result.y), imgPixel10);
                img.getPixelColor(floor(result.x), floor(result.y) + 1, imgPixel01);
                img.getPixelColor(floor(result.x) + 1, floor(result.y) + 1, imgPixel11);

                VECTOR4 newImgPixel = {
                    a00*imgPixel00[0] + a10*imgPixel10[0] + a01*imgPixel01[0] + a11*imgPixel11[0],
                    a00*imgPixel00[1] + a10*imgPixel10[1] + a01*imgPixel01[1] + a11*imgPixel11[1],
                    a00*imgPixel00[2] + a10*imgPixel10[2] + a01*imgPixel01[2] + a11*imgPixel11[2],
                    a00*imgPixel00[3] + a10*imgPixel10[3] + a01*imgPixel01[3] + a11*imgPixel11[3]
                };
                newImg.setPixelColor(i - (int) xMin, j - (int) yMin, newImgPixel);
            }
        }
    }

    imgResize(&newImg, &newImg, (float) img.height/newImg.height);

    newImg.saveBMP("img/rectAffineResized.bmp");

    glutInitWindowSize(newImg.width, newImg.height);
    gcgDISCRETE2D<double> channelR, channelG, channelB, alpha;
    glutInitWindowPosition(80, 80);
    newImg.exportChannels(&channelR, &channelG, &channelB, &alpha);
    glutCreateWindow("Afinne Rectified Image");
    pixelsAffine = (float*) malloc(newImg.width*newImg.height*4*sizeof(float));
    for(int i = 0; i < newImg.width; i++){
        for(int j = 0; j < newImg.height; j++){
            pixelsAffine[(i + j*newImg.width)*4] = channelR.getDataSample(i, j);
            pixelsAffine[(i + j*newImg.width)*4 + 1] = channelG.getDataSample(i, j);
            pixelsAffine[(i + j*newImg.width)*4 + 2] = channelB.getDataSample(i, j);
            pixelsAffine[(i + j*newImg.width)*4 + 3] = alpha.getDataSample(i, j);
        }
    }
    glutDisplayFunc(displayRectAffine);
    glutMouseFunc(mouse);

    ///free memory
    for(int i = 0; i < 3; i++){
        free(H[i]);
    }
    free(H);
}

void removeAffine(point3D *lines){
    gcgDISCRETE2D<double> *A = new gcgDISCRETE2D<double>(2, 2, 0, 0);
    gcgDISCRETE1D<double> *b = new gcgDISCRETE1D<double>(2, 0);
    gcgDISCRETE1D<double> *x = new gcgDISCRETE1D<double>(2, 0);

    for(int i = 0; i < 4; i++){
        printf("%f\t%f\t%f\n", lines[i].x, lines[i].y, lines[i].w);
    }

    A->setDataSample(0, 0, lines[0].x*lines[1].x);
    A->setDataSample(1, 0, lines[0].x*lines[1].y + lines[0].y*lines[1].x);

    A->setDataSample(0, 1, lines[2].x*lines[3].x);
    A->setDataSample(1, 1, lines[2].x*lines[3].y + lines[2].y*lines[3].x);

    b->setDataSample(0, -lines[0].y*lines[1].y);
    b->setDataSample(1, -lines[2].y*lines[3].y);

    gcgLinearSolver solver(A, b, x, 2, 2);
    solver.solve();

    MatrixXd KKt(2, 2);

    KKt(0, 0) = x->getDataSample(0);
    KKt(0, 1) = x->getDataSample(1);

    KKt(1, 0) = x->getDataSample(1);
    KKt(1, 1) = 1.;

    cout << "KKt = " << KKt << endl << endl;

    LLT<MatrixXd> llt(KKt);

    MatrixXd K = llt.matrixU();

    cout << "K = " << K << endl;

    double xMin, xMax, yMin, yMax;

    double **H = (double**) malloc (3*sizeof(double*));
    double **invH = (double**) malloc (3*sizeof(double*));
    for(int i = 0; i < 3; i++){
        H[i] = (double*) malloc(3*sizeof(double));
        invH[i] = (double*) malloc(3*sizeof(double));
    }

    H[0][0] = K(0, 0);
    H[0][1] = K(0, 1);
    H[0][2] = 0.;

    H[1][0] = K(1, 0);
    H[1][1] = K(1, 1);
    H[1][2] = 0.;

    H[2][0] = 0.;
    H[2][1] = 0.;
    H[2][2] = 1.;

    calculateInverseLU(H, invH, 3, 3);

    point2D result;
    point2D vertex = {0., 0.};
    matrixApplication(invH, &vertex, &result);
    xMin = result.x;
    xMax = result.x;
    yMin = result.y;
    yMax = result.y;

    vertex = {newImg.width, 0.};
    matrixApplication(invH, &vertex, &result);
    xMin = result.x < xMin ? result.x : xMin;
    xMax = result.x > xMax ? result.x : xMax;
    yMin = result.y < yMin ? result.y : yMin;
    yMax = result.y > yMax ? result.y : yMax;

    vertex = {0., newImg.height};
    matrixApplication(invH, &vertex, &result);
    xMin = result.x < xMin ? result.x : xMin;
    xMax = result.x > xMax ? result.x : xMax;
    yMin = result.y < yMin ? result.y : yMin;
    yMax = result.y > yMax ? result.y : yMax;

    vertex = {newImg.width, newImg.height};
    matrixApplication(invH, &vertex, &result);
    xMin = result.x < xMin ? result.x : xMin;
    xMax = result.x > xMax ? result.x : xMax;
    yMin = result.y < yMin ? result.y : yMin;
    yMax = result.y > yMax ? result.y : yMax;

    newImg2.createImage((int)xMax - (int)xMin + 2, (int) yMax - (int) yMin + 2, 32);

    for(int i = (int) xMin; i < xMax; i++){
        for(int j = (int) yMin; j < yMax; j++){
            point2D imagePoint = {.x = i, .y = j};
            matrixApplication(H, &imagePoint, &result);

            if(result.x >= 0 && result.x < newImg.width && result.y >= 0 && result.y < newImg.height){
                double a0 = floor(result.x) + 1 - result.x;
                double a1 = result.x - floor(result.x);
                double a00 = (floor(result.y) + 1 - result.y)*a0;
                double a01 = (result.y - floor(result.y))*a0;
                double a10 = (floor(result.y) + 1 - result.y)*a1;
                double a11 = (result.y - floor(result.y))*a1;
                VECTOR4 imgPixel00, imgPixel01, imgPixel10, imgPixel11;
                newImg.getPixelColor(floor(result.x), floor(result.y), imgPixel00);
                newImg.getPixelColor(floor(result.x) + 1, floor(result.y), imgPixel10);
                newImg.getPixelColor(floor(result.x), floor(result.y) + 1, imgPixel01);
                newImg.getPixelColor(floor(result.x) + 1, floor(result.y) + 1, imgPixel11);

                VECTOR4 newImg2Pixel = {
                    a00*imgPixel00[0] + a10*imgPixel10[0] + a01*imgPixel01[0] + a11*imgPixel11[0],
                    a00*imgPixel00[1] + a10*imgPixel10[1] + a01*imgPixel01[1] + a11*imgPixel11[1],
                    a00*imgPixel00[2] + a10*imgPixel10[2] + a01*imgPixel01[2] + a11*imgPixel11[2],
                    a00*imgPixel00[3] + a10*imgPixel10[3] + a01*imgPixel01[3] + a11*imgPixel11[3]
                };
                newImg2.setPixelColor(i - (int) xMin, j - (int) yMin, newImg2Pixel);
            }
        }
    }

    glutInitWindowSize(newImg2.width, newImg2.height);
    gcgDISCRETE2D<double> channelR, channelG, channelB, alpha;
    glutInitWindowPosition(80, 80);
    newImg2.exportChannels(&channelR, &channelG, &channelB, &alpha);
    glutCreateWindow("Similarity Rectified Image");
    pixelsSimilarity = (float*) malloc(newImg2.width*newImg2.height*4*sizeof(float));
    for(int i = 0; i < newImg2.width; i++){
        for(int j = 0; j < newImg2.height; j++){
            pixelsSimilarity[(i + j*newImg2.width)*4] = channelR.getDataSample(i, j);
            pixelsSimilarity[(i + j*newImg2.width)*4 + 1] = channelG.getDataSample(i, j);
            pixelsSimilarity[(i + j*newImg2.width)*4 + 2] = channelB.getDataSample(i, j);
            pixelsSimilarity[(i + j*newImg2.width)*4 + 3] = alpha.getDataSample(i, j);
        }
    }
    glutDisplayFunc(displayRectSimilarity);

    ///free memory
    for(int i = 0; i < 3; i++){
        free(H[i]);
    }
    free(H);
}

void metricRectification(point3D *lines){
    MatrixXd A(5, 5);
    MatrixXd b(5, 1);
    MatrixXd x(5, 1);

    for(int i = 0; i < 5; i++){
        b(i, 0) = -lines[2*i].x*lines[2*i + 1].x;
        A(i, 0) = (lines[2*i].x*lines[2*i + 1].y + lines[2*i].y*lines[2*i + 1].x)/2.;
        A(i, 1) = lines[2*i].y*lines[2*i + 1].y;
        A(i, 2) = (lines[2*i].x*lines[2*i + 1].w + lines[2*i].w*lines[2*i + 1].x)/2.;
        A(i, 3) = (lines[2*i].y*lines[2*i + 1].w + lines[2*i].w*lines[2*i + 1].y)/2.;
        A(i, 4) = lines[2*i].w*lines[2*i + 1].w;
    }

    cout << A << endl;
    cout << b << endl;

    x = A.colPivHouseholderQr().solve(b);

    cout << endl << x << endl;

    MatrixXd KKt(2, 2);

    KKt(0, 0) = 1.;
    KKt(0, 1) = x(0, 0)/2.;
    KKt(1, 0) = x(0, 0)/2.;
    KKt(1, 1) = x(1, 0);

    LLT< MatrixXd> llt(KKt);

    MatrixXd K = llt.matrixU();

    cout << endl << K << endl;

    MatrixXd v(1, 2);

    v(0, 0) = x(2, 0)/2.;
    v(0, 1) = x(3, 0)/2.;

    v = v*KKt.inverse();

    cout << endl << v << endl;

    double **H = (double**) malloc (3*sizeof(double*));
    double **invH = (double**) malloc (3*sizeof(double*));
    for(int i = 0; i < 3; i++){
        H[i] = (double*) malloc(3*sizeof(double));
        invH[i] = (double*) malloc(3*sizeof(double));
    }

    H[0][0] = K(0, 0);
    H[0][1] = K(0, 1);
    H[0][2] = 0.;

    H[1][0] = K(1, 0);
    H[1][1] = K(1, 1);
    H[1][2] = 0.;

    H[2][0] = v(0, 0);
    H[2][1] = v(0, 1);
    H[2][2] = 1.;

    double xMin, xMax, yMin, yMax;

    calculateInverseLU(H, invH, 3, 3);

    printMatrix(H, 3, 3);

    point2D result;
    point2D vertex = {0., 0.};
    matrixApplication(invH, &vertex, &result);
    xMin = result.x;
    xMax = result.x;
    yMin = result.y;
    yMax = result.y;

    vertex = {img.width, 0.};
    matrixApplication(invH, &vertex, &result);
    xMin = result.x < xMin ? result.x : xMin;
    xMax = result.x > xMax ? result.x : xMax;
    yMin = result.y < yMin ? result.y : yMin;
    yMax = result.y > yMax ? result.y : yMax;

    vertex = {0., img.height};
    matrixApplication(invH, &vertex, &result);
    xMin = result.x < xMin ? result.x : xMin;
    xMax = result.x > xMax ? result.x : xMax;
    yMin = result.y < yMin ? result.y : yMin;
    yMax = result.y > yMax ? result.y : yMax;

    vertex = {img.width, img.height};
    matrixApplication(invH, &vertex, &result);
    xMin = result.x < xMin ? result.x : xMin;
    xMax = result.x > xMax ? result.x : xMax;
    yMin = result.y < yMin ? result.y : yMin;
    yMax = result.y > yMax ? result.y : yMax;

    newImg.createImage((int)xMax - (int)xMin + 2, (int) yMax - (int) yMin + 2, 32);

    for(int i = (int) xMin; i < xMax; i++){
        for(int j = (int) yMin; j < yMax; j++){
            point2D imagePoint = {.x = i, .y = j};
            matrixApplication(H, &imagePoint, &result);

            if(result.x >= 0 && result.x < img.width && result.y >= 0 && result.y < img.height){
                double a0 = floor(result.x) + 1 - result.x;
                double a1 = result.x - floor(result.x);
                double a00 = (floor(result.y) + 1 - result.y)*a0;
                double a01 = (result.y - floor(result.y))*a0;
                double a10 = (floor(result.y) + 1 - result.y)*a1;
                double a11 = (result.y - floor(result.y))*a1;
                VECTOR4 imgPixel00, imgPixel01, imgPixel10, imgPixel11;
                img.getPixelColor(floor(result.x), floor(result.y), imgPixel00);
                img.getPixelColor(floor(result.x) + 1, floor(result.y), imgPixel10);
                img.getPixelColor(floor(result.x), floor(result.y) + 1, imgPixel01);
                img.getPixelColor(floor(result.x) + 1, floor(result.y) + 1, imgPixel11);

                VECTOR4 newImgPixel = {
                    a00*imgPixel00[0] + a10*imgPixel10[0] + a01*imgPixel01[0] + a11*imgPixel11[0],
                    a00*imgPixel00[1] + a10*imgPixel10[1] + a01*imgPixel01[1] + a11*imgPixel11[1],
                    a00*imgPixel00[2] + a10*imgPixel10[2] + a01*imgPixel01[2] + a11*imgPixel11[2],
                    a00*imgPixel00[3] + a10*imgPixel10[3] + a01*imgPixel01[3] + a11*imgPixel11[3]
                };
                newImg.setPixelColor(i - (int) xMin, j - (int) yMin, newImgPixel);
            }
        }
    }

    imgResize(&newImg, &newImg2, (float) img.height/newImg.height);

    newImg2.saveBMP("img/metric.bmp");

    glutInitWindowSize(newImg2.width, newImg2.height);
    gcgDISCRETE2D<double> channelR, channelG, channelB, alpha;
    glutInitWindowPosition(80, 80);
    newImg2.exportChannels(&channelR, &channelG, &channelB, &alpha);
    glutCreateWindow("Similarity Rectified Image");
    pixelsSimilarity = (float*) malloc(newImg2.width*newImg2.height*4*sizeof(float));
    for(int i = 0; i < newImg2.width; i++){
        for(int j = 0; j < newImg2.height; j++){
            pixelsSimilarity[(i + j*newImg2.width)*4] = channelR.getDataSample(i, j);
            pixelsSimilarity[(i + j*newImg2.width)*4 + 1] = channelG.getDataSample(i, j);
            pixelsSimilarity[(i + j*newImg2.width)*4 + 2] = channelB.getDataSample(i, j);
            pixelsSimilarity[(i + j*newImg2.width)*4 + 3] = alpha.getDataSample(i, j);
        }
    }
    glutDisplayFunc(displayRectSimilarity);

    ///free memory
    for(int i = 0; i < 3; i++){
        free(H[i]);
    }
    free(H);
}

void drawSquare(int width, int height, int x, int y, float *pixels){
    for(int i = x - 4; i < x + 4; i++){
        for(int j = y - 4; j < y + 4; j++){
            pixels[(i + (height - j)*width)*4] = 0.0;
            pixels[(i + (height - j)*width)*4 + 1] = 1.0;
            pixels[(i + (height - j)*width)*4 + 2] = 0.0;
            pixels[(i + (height - j)*width)*4 + 3] = 1.0;
        }
    }
}

void drawLine(float *pixels, int width, int height, point3D line, double xMin, double xMax){
    for(double x = xMin; x < xMax; x ++){
        double y = (-line.w - line.x*x)/line.y;
        pixels[((int) x + ((int) y)*width)*4] = 0.0;
        pixels[((int) x + ((int) y)*width)*4 + 1] = 1.0;
        pixels[((int) x + ((int) y)*width)*4 + 2] = 0.0;
        pixels[((int) x + ((int) y)*width)*4 + 3] = 1.0;
    }
}

void callback(int id){
    method = id;
    glui->close();
}

void mouse(int button, int state, int x, int y){
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        if(space == PROJECTIVE){
            drawSquare(img.width, img.height, x, y, pixelsInput);
        }
        else{
            drawSquare(newImg.width, newImg.height, x, y, pixelsAffine);
        }
        glutPostRedisplay();
        if(countPoints == 0){
            firstPoint.x = x;
            firstPoint.y = space == PROJECTIVE ? img.height - y: newImg.height - y;
            firstPoint.w = 1.;
            countPoints++;
        }
        else{
            secondPoint.x = x;
            secondPoint.y = space == PROJECTIVE ? img.height - y: newImg.height - y;
            secondPoint.w = 1.;
            vectorialProduct(&firstPoint, &secondPoint, &lines[countLines++]);
            double xMin, xMax;
            if(firstPoint.x < secondPoint.x){
                xMin = firstPoint.x;
                xMax = secondPoint.x;
            }
            else{
                xMax = firstPoint.x;
                xMin = secondPoint.x;
            }
            if(space == PROJECTIVE){
                drawLine(pixelsInput, img.width, img.height, lines[countLines - 1], xMin, xMax);
            }
            else{
                drawLine(pixelsAffine, newImg.width, newImg.height, lines[countLines - 1], xMin, xMax);
            }

            countPoints = 0;
            if(method == 1){
                if(countLines == 4){
                    if(space == PROJECTIVE){
                        removeProjective(lines);
                        countLines = 0;
                        space = AFFINE;
                    }
                    else{
                        removeAffine(lines);
                        countLines = 0;
                    }
                }
            }
            else{
                if(countLines == 10){
                    metricRectification(lines);
                    countLines = 0;
                }
            }
        }
    }
}

int main(int argc, char* argv[])
{
    gcgDISCRETE2D<double> channelR, channelG, channelB, alpha;
    img.loadBMP("img/example1.bmp");
    img.exportChannels(&channelR, &channelG, &channelB, &alpha);
    glutInit(&argc, argv);
    glutInitWindowSize(img.width, img.height);
    glutInitWindowPosition(70, 50);
    glutCreateWindow("Input Image");
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);
    pixelsInput = (float*) malloc (img.width*img.height*4*sizeof(float));
    for(int i = 0; i < img.width; i++){
        for(int j = 0; j < img.height; j++){
            pixelsInput[(i + j*img.width)*4] = channelR.getDataSample(i, j);
            pixelsInput[(i + j*img.width)*4 + 1] = channelG.getDataSample(i, j);
            pixelsInput[(i + j*img.width)*4 + 2] = channelB.getDataSample(i, j);
            pixelsInput[(i + j*img.width)*4 + 3] = alpha.getDataSample(i, j);
        }
    }
    lines = (point3D*) malloc(10*sizeof(point3D));
    glui = GLUI_Master.create_glui("Choose the method:", 0, 200, 200);
    glui->add_button("Method 1", 1, callback);
    glui->add_button("Method 2", 2, callback);
    glutDisplayFunc(displayInput);
    glutMouseFunc(mouse);
    glutMainLoop();
}
