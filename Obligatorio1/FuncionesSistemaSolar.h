#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void Sumar_Vec(double* v1, double* v2, int n){
    int i;
    for(i=0; i<n; i++){
        *(v1+i)=*(v1+i)+*(v2+i);//Esto hace que vaya aumentando las posiciones del puntero v1, mientras le voy sumando a cada posición el valor de v2.
    }    
}
//Funcion que me calcula las aceleraciones de cada planeta respecto a los otros.
void Aceleracion(double* a, double* r, double* m, int n, int d) {
    int i, j, k;
    double R[n][n][d];
    double M[n][n];
    for (i = 0; i < n; i++) { // Ciclo para cada planeta
        for (j = i; j < n; j++) {    // Ciclo para interactuar con cada otro planeta
            // Calcular R entre los planetas i y j
            for (k = 0; k < d; k++) {//Iteración entre dimensiones x,y.
                R[i][j][k] = *(r + d*i + k) - *(r + d*j + k);//i*d es para que me salte al planeta i, saltandose las dimensiones, que al final son posiciones.
                R[j][i][k]=-R[i][j][k];//Para la parte inferior de la matriz de distancias R. (ANTISIMÉTRICA)
            }
            M[i][j]=sqrt(pow(R[i][j][0],2)+ pow(R[i][j][1],2)); //Matriz de módulos
            M[j][i]=M[i][j];//Hago la matriz simétrica.

        }
    }
    for (i = 0; i < n; i++) {//Inicializo el puntero aceleracion para no arrastar basura.
            for (k = 0; k < d; k++) {
                *(a+i*d+k)=0;
            }
    }
    for (i = 0; i < n; i++) {//Para un planeta
        for (j = 0; j < n; j++) {// Ciclo para interactuar con cada otro planeta
            if(i!=j){//La aceleración de un planeta consigo mismo es 0, no me interesa.
            // Calcular a entre los planetas i y j
            for (k = 0; k < d; k++) {
                *(a+i*d+k)+=-*(m+j)*R[i][j][k]/pow(M[i][j],3);//sumatoria de aceleraciones.
                }
            }
        }
    }
}
//Funcion que aplica el algoritmo de Verlet, para calcular nuevas posiciones de los planetas.
void Verlet(double* a,double* m, double* r, double* v, int n, int d, double h){
    int i, j, k, z;
    double w[n][d];//Vector w temporal.
    for (i = 0; i < n; i++) {//Bucle para todos los planetas
            for (k = 0; k < d; k++) {//Bucle Dimensiones x,y.
                *(r+i*d+k)+=*(v+i*d+k)*h+h*h*(*(a+i*d+k))/2; //Actualizo posiciones con la anterior.
                w[i][k]=*(v+i*d+k)+h*(*(a+i*d+k))/2;//Calculo w
            }
    }//Ahora ya tengo posiciones nuevas y w para todos mis planetas.
    Aceleracion(a, r, m, n, d);//Actualizo aceleración
     for (i = 0; i < n; i++) {
            for (k = 0; k < d; k++) {
                *(v+i*d+k)=w[i][k]+h*(*(a+i*d+k))/2;//Calculo mi nueva velocidad, ahora ya puedo calcular mis nuevas r.
            }
    }
}
//Función que reescala mis constantes, para trabajar con datos menos pesados.
void ReescaladoSolar(double* r, double t, double* v, double* m, int n, int d){
    int i,k;
    double c=1.496e11;
    double G=6.67e-11;
    double M=1.99e30;
    for(i=0; i<n; i++){
        *(m+i)= *(m+i)/M;
        for(k=0;k<d; k++){
            *(r+i*d+k)=*(r+i*d+k)/c;
            *(v+i*d+k)= *(v+i*d+k)*sqrt(c/(G*M));
        }
    }
    t=sqrt((G*M)/(pow(c,3)))*t;
}

//FUNCION QUE CALCULA LAS ENERGIAS DE UN PLANETA A LO LARGO DE SU PERIODO ORBITAL.
void Energia(double* T, double* U, double* r, double* m, double* v, int n, int d)
{
int i, j, k;
double R[n][n][d];
double M[n][n];
    for (i = 0; i < n; i++) {
        *(T+i)=0;
    }
    for (i = 0; i < n; i++) {
            for (k = 0; k < d; k++) {
                *(T+i)+=10e7*(*(m+i))*(*(v+i*d+k)*(*(v+i*d+k)))/2;//Calculo energía cinética.
            }
    }
    //CALCULO R ENTRE PLANETAS.
    for (i = 0; i < n; i++) { // Ciclo para cada planeta
        for (j = i; j < n; j++) {    // Ciclo para interactuar con cada otro planeta
            // Calcular R entre los planetas i y j
            for (k = 0; k < d; k++) {//Iteración entre dimensiones x,y.
                R[i][j][k] = *(r + d*i + k) - *(r + d*j + k);//i*d es para que me salte al planeta i, saltandose las dimensiones, que al final son posiciones.
                R[j][i][k]=-R[i][j][k];//Para la parte inferior de la matriz de distancias R. (ANTISIMÉTRICA)
            }
            M[i][j]=sqrt(pow(R[i][j][0],2)+ pow(R[i][j][1],2)); //Matriz de módulos
            M[j][i]=M[i][j];//Hago la matriz simétrica.
        }
    }
    
    //POTENCIAL
    for(i=0;i<n;i++){
        *(U+i)=0;
    }
    for (i = 0; i < n; i++) {//Para un planeta
        for (j = 0; j < n; j++) {// Ciclo para interactuar con cada otro planeta
            if(i!=j){//La aceleración de un planeta consigo mismo es 0, no me interesa.
            // Calcular U entre los planetas i y j
                *(U+i)+= -10e7*(*(m+i))*(*(m+j))/M[i][j];//sumatoria de potenciales
            }
        }
    }
}
//Funcion que me convierte el modelo en geocentrico
void Geocentrico(double* r, double* rt, int n, int d){
    int i, k;
    for (i = 0; i < n; i++) {//Para un planeta
        for (k = 0; k < d; k++) {//Dimensiones
                *(rt+i*d+k)=*(r+i*d+k)-*(r+3*d+k); //Le voy restando la posicion de la tierra (r+3*d+k)
                // a cada planeta.
        }
    }
}