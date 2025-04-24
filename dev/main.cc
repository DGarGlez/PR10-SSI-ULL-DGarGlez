/*
/
/
/   REPOSITORIO: https://github.com/DGarGlez/PR10-SSI-ULL-DGarGlez.git
/
/
*/
#include <iostream>
#include <vector>

using namespace std;

// Estructura que representa un punto en la curva elíptica
struct Point {
    int x, y;           // Coordenadas del punto
    bool atInfinity;    // Indica si el punto es el "punto en el infinito"

    // Constructor por defecto con valores iniciales
    Point(int x = 0, int y = 0, bool atInfinity = false) : x(x), y(y), atInfinity(atInfinity) {}

    // Comparador para saber si dos puntos son iguales
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y && atInfinity == other.atInfinity;
    }
};

// Variables globales para la curva
int p, a, b;
Point G; // Punto base de la curva

// Función para operar en aritmética modular p (n mod p), siempre positivo
int mod(int n) {
    return (n % p + p) % p;
}

// Cálculo del inverso modular de a mod m usando fuerza bruta
int modInverse(int a, int m) {
    a = mod(a); // Asegura que a sea positivo
    for (int x = 1; x < m; ++x) {
        if ((a * x) % m == 1) return x; // Si a*x mod m = 1, entonces x es el inverso
    }
    cout << "Inverso no encontrado para " << a << " mod " << m << endl;        
    return -1; // Si no existe inverso (no debería ocurrir si p es primo)
}

// Suma de dos puntos P y Q sobre la curva elíptica
Point pointAdd(const Point& P, const Point& Q) {
    if (P.atInfinity) return Q; // Si P es el punto en el infinito, devuelve Q
    if (Q.atInfinity) return P; // Si Q es el punto en el infinito, devuelve P

    // Caso de punto inverso o suma con opuesto => El resultado es el punto en el infinito (0, 0)
    if (P.x == Q.x && (P.y != Q.y || P.y == 0))
        return Point(0, 0, true);

    int lambda;

    if (P == Q) {   // Si P y Q son iguales, usamos la fórmula de duplicación
        // Fórmula para duplicar un punto: λ = (3x² + a) / 2y
        int num = mod(3 * P.x * P.x + a);   // Numerador
        int den = modInverse(2 * P.y, p);   // Denominador
        lambda = mod(num * den);            // λ = (3x² + a) / 2y
    } else {    // Si P y Q son distintos, usamos la fórmula de suma
        // Fórmula para suma de dos puntos distintos: λ = (y2 - y1) / (x2 - x1)
        int num = mod(Q.y - P.y);        // Diferencia de las coordenadas y
        int den = modInverse(mod(Q.x - P.x), p);    // Diferencia de las coordenadas x
        lambda = mod(num * den);        // λ = (y2 - y1) / (x2 - x1)
    }

    // Cálculo de las nuevas coordenadas (x3, y3)
    int x3 = mod(lambda * lambda - P.x - Q.x);  // x3 = λ² - x1 - x2
    int y3 = mod(lambda * (P.x - x3) - P.y);    // y3 = λ(x1 - x3) - y1

    return Point(x3, y3);   // Devuelve el nuevo punto (x3, y3)
}

// Multiplicación escalar de un punto P * n (repetida suma de puntos)
Point scalarMult(Point P, int n) {  
    Point R(0, 0, true); // Inicializa en el punto en el infinito (identidad aditiva)
    while (n > 0) {
        if (n % 2 == 1) // Si el bit menos significativo de n es 1
            R = pointAdd(R, P); // Suma P a R
        P = pointAdd(P, P);     // Doble de P
        n /= 2;               // Desplaza n a la derecha (divide por 2)
    }
    return R;
}

// Genera todos los puntos (x, y) que pertenecen a la curva y² = x³ + ax + b (mod p)
vector<Point> getCurvePoints() {
    vector<Point> points;   // Vector para almacenar los puntos de la curva
    for (int x = 0; x < p; ++x) {   // Recorre todos los valores de x
        int rhs = mod(x * x * x + a * x + b); // RHS de la ecuación de la curva
        for (int y = 0; y < p; ++y) {   // Recorre todos los valores de y
            if (mod(y * y) == rhs)  // Si y² ≡ RHS (mod p), entonces (x, y) es un punto de la curva
                points.push_back(Point(x, y));  // Agrega el punto a la lista
        }
    }
    return points;
}

// Codifica el mensaje m como un punto Qm en la curva
Point encodeMessage(int m, const vector<Point>& curve, int M) {
    int h = p / M;  // Constante para codificar mensajes como puntos
    for (int j = 0; j < h; ++j) {   // Recorre los posibles valores de j
        int x = mod(m * h + j); // Calcula el valor de x
        for (const auto& pt : curve) {  // Recorre los puntos de la curva
            if (pt.x == x)  // Si el punto tiene la coordenada x deseada
                return pt;  // Devuelve el punto correspondiente
        }
    }
    return Point(0, 0, true); // No se encontró un punto válido
}

// Función principal
int main() {
    cout << "-------------------------------------------------------------------------" << endl;
    cout << "|\tPRÁCTICA 10: ALGORITMOS DIFFIE-HELLMAN y ELGAMAL ELÍPTICOS\t|" << endl;
    cout << "-------------------------------------------------------------------------" << endl;

    // Entrada de parámetros de la curva
    cout << "Ingrese el valor de p (primo): ";
    cin >> p;
    cout << "Ingrese los coeficientes de la curva a: ";
    cin >> a;
    cout << "Ingrese los coeficientes de la curva b: ";
    cin >> b;
    cout << "Ingrese el punto generador G (x): ";
    cin >> G.x;
    cout << "Ingrese el punto generador G (y): ";
    cin >> G.y;

    // Entrada de claves privadas y mensaje original
    int dA, dB;
    cout << "Ingrese la clave privada de A (dA): ";
    cin >> dA;
    cout << "Ingrese la clave privada de B (dB): ";
    cin >> dB;
    int m;
    cout << "Ingrese el mensaje original (entero): ";
    cin >> m;

    // Parámetros de codificación
    int M = 1; // Inicializa M
    while(m > M) {
        M *= 2; // Aumenta M a la siguiente potencia de 2
    } 
    int h = p / M;      // Constante para codificar mensajes como puntos

    cout << "\n----------------------------------------- RESULTADOS -----------------------------------------\n";

    // Generación y visualización de todos los puntos de la curva
    vector<Point> curve = getCurvePoints(); 
    cout << "\nPuntos de la curva: ";
    for (const auto& pt : curve)
        cout << "(" << pt.x << "," << pt.y << "),";
    cout << "\b \n";

    // Generación de claves públicas
    Point dAG = scalarMult(G, dA); // dA * G
    Point dBG = scalarMult(G, dB); // dB * G

    // Muestra las claves públicas
    cout << "Clave pública de B: punto dBG=" << dB << "(" << G.x << "," << G.y << ")=(" << dBG.x << "," << dBG.y << ")\n";
    cout << "Clave pública de A: punto dAG=" << dA << "(" << G.x << "," << G.y << ")=(" << dAG.x << "," << dAG.y << ")\n";

    // Cálculo de claves compartidas (A y B deberían obtener el mismo punto)
    Point sharedA = scalarMult(dBG, dA); // dA * (dB * G)
    Point sharedB = scalarMult(dAG, dB); // dB * (dA * G)

    cout << "Clave secreta compartida calculada por A: " << dA << "*(" << dBG.x << "," << dBG.y << ")=(" << sharedA.x << "," << sharedA.y << ")\n";
    cout << "Clave secreta compartida calculada por B: " << dB << "*(" << dAG.x << "," << dAG.y << ")=(" << sharedB.x << "," << sharedB.y << ")\n";

    // Codificación del mensaje m como punto Qm
    Point Qm = encodeMessage(m, curve, M);
    cout << "M=" << M << "\n";
    cout << "h=" << h << "<" << p << "/" << M << "\n";
    cout << "Mensaje original codificado como punto Qm =(" << Qm.x << "," << Qm.y << ")\n";

    // Cifrado: Qm + dA * (dBG)
    Point cipher = pointAdd(Qm, sharedA);
    cout << "Mensaje cifrado y clave pública enviados de A a B: {(" << cipher.x << "," << cipher.y << "),"
         << "(" << dAG.x << "," << dAG.y << ")}\n";

    return 0;
}
