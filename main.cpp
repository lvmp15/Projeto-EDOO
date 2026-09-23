#include "Carro.h"

using namespace std;

int main()
{
    Carro carro("ABC1D23", "Fiat Uno", 1, 1);

    cout << carro << endl;
    cout << "Tarifa de 3 horas: " << carro.calcularTarifa(3.0) << endl;

    return 0;
}
