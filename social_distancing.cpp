#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <set>
#include <map>

using namespace std;


int NoBenefit = 0; // Valor para indicar que no hay beneficio. POR QUE SE USA ESTE NOMBRE?
															// Noto que devuelve el maximoBeneficio

//Shops[0] = Benefits
//Shops[1] = Risks.
int benefit_index = 0;
int risk_index = 1;
int M_INFTY = std::numeric_limits<int>::min();

// Información de la instancia a resolver.
int NumberOfShops, RiskLimit; // Cantidad de tienas y el RiskLimit maximo de contagios.
vector<vector<int>> Shops; // Asumo que Shops[0] y Shops[1] representan beneficios/riesgos en algun orden

// possibleShopToOpen: Indice del elemento que estoy analizando.
// accumulatedRisk: Riesgo acumulado hasta ahora.


int FB(int possibleShopToOpen, int accumulatedRisk)
{
	if(possibleShopToOpen>=NumberOfShops){
		if(accumulatedRisk > RiskLimit){
			return M_INFTY;
		}else{
			return NoBenefit;
		}
	}
	return max(FB(possibleShopToOpen + 2, accumulatedRisk + Shops[possibleShopToOpen][risk_index]) + Shops[possibleShopToOpen][benefit_index],FB(possibleShopToOpen + 1,accumulatedRisk));

}

bool poda_factibilidad = true; // define si la poda por factibilidad esta habilitada.
bool poda_optimalidad = true; // define si la poda por optimalidad esta habilitada.
int CurrentMaxBenefit = NoBenefit; // Mejor beneficio hasta el momento.
vector<int> MaxPossibleBenefits,MinPossibleRisk;

//MaxPossibleBenefits[i] = el maximo beneficio posible que podria obtener sumando todas las tiendas desde i.
void precalculateMaxBenefits(){
	MaxPossibleBenefits.resize(NumberOfShops);
	MaxPossibleBenefits[NumberOfShops - 1] = Shops[NumberOfShops - 1][benefit_index];
	for (int i = NumberOfShops - 2; i >= 0; --i)
	{
		MaxPossibleBenefits[i] = Shops[i][benefit_index] + MaxPossibleBenefits[i + 1]; 
	}
	
}

//MinPossibleRisk[i] = el minimo riesgo que existe al ingresar una nueva tienda desde la i.
void precalculateMinRisks(){
	MinPossibleRisk.resize(NumberOfShops);
	MinPossibleRisk[NumberOfShops - 1] = Shops[NumberOfShops - 1][risk_index];
	for (int i = NumberOfShops - 2; i >= 0; --i)
	{
		MinPossibleRisk[i] = min(Shops[i][risk_index],MinPossibleRisk[i + 1]); 
	}
	
}


int BT(int possibleShopToOpen, int accumulatedRisk,int partialBenefit)
{
	if (possibleShopToOpen >= NumberOfShops){
		if (accumulatedRisk > RiskLimit) return M_INFTY; 
		if (partialBenefit > CurrentMaxBenefit) CurrentMaxBenefit = partialBenefit;
		return NoBenefit;
	} 
	//poda de factibilidad
	if (poda_factibilidad)
	{
		//if(accumulatedRisk + MinPossibleRisk[possibleShopToOpen] > RiskLimit) return NoBenefit;
		if(accumulatedRisk > RiskLimit) return -1;
	}
	//poda de optimalidad
	if (poda_optimalidad)
	{
		if (partialBenefit + MaxPossibleBenefits[possibleShopToOpen] <= CurrentMaxBenefit) return NoBenefit;
	}	
	int openShop = BT(possibleShopToOpen + 2, accumulatedRisk + Shops[possibleShopToOpen][risk_index],partialBenefit + Shops[possibleShopToOpen][benefit_index]) + Shops[possibleShopToOpen][benefit_index];
	int closedShop = BT(possibleShopToOpen + 1,accumulatedRisk,partialBenefit);
	return max(openShop,closedShop);

	
}



vector<vector<int>> DynamicProgrammingMatrix; // Matriz de Memoria de PD.
const int UNDEFINED = -1;
// DP(unprocessedShops, accumulatedRisk): máximo beneficio de un conjunto de negocios {{benefit1,infection1}, ...} que tenga como máxima infección RiskLimit.
int DP(int unprocessedShops, int accumulatedRisk)
{
	if (unprocessedShops >= NumberOfShops) return NoBenefit;
	if (DynamicProgrammingMatrix[unprocessedShops][accumulatedRisk] == UNDEFINED) {
		if (accumulatedRisk + Shops[unprocessedShops][risk_index] <= RiskLimit){
			DynamicProgrammingMatrix[unprocessedShops][accumulatedRisk] = max(DP(unprocessedShops+1, accumulatedRisk), DP(unprocessedShops+2, accumulatedRisk+Shops[unprocessedShops][risk_index]) + Shops[unprocessedShops][benefit_index]);
		} else {
			DynamicProgrammingMatrix[unprocessedShops][accumulatedRisk] = DP(unprocessedShops + 1, accumulatedRisk);
		}
	} 
	return DynamicProgrammingMatrix[unprocessedShops][accumulatedRisk];
}

// Recibe por parámetro qué algoritmos utilizar para la ejecución separados por espacios.
// Imprime por clog la información de ejecución de los algoritmos.
// Imprime por cout el resultado de algun algoritmo ejecutado.
int main(int argc, char** argv)
{
	// Leemos el parametro que indica el algoritmo a ejecutar.
	map<string, string> algoritmos_implementados = {
		{"FB", "Fuerza Bruta"}, {"BT", "Backtracking con podas"}, {"BT-F", "Backtracking con poda por factibilidad"}, 
		{"BT-O", "Backtracking con poda por optimalidad"}, {"DP", "Programacion dinámica"}
	};

	// Verificar que el algoritmo pedido exista.
	if (argc < 2 || algoritmos_implementados.find(argv[1]) == algoritmos_implementados.end())
	{
		cerr << "Algoritmo no encontrado: " << argv[1] << endl;
		cerr << "Los algoritmos existentes son: " << endl;
		for (auto& alg_desc: algoritmos_implementados) cerr << "\t- " << alg_desc.first << ": " << alg_desc.second << endl;
		return 0;
	}
	string algoritmo = argv[1];

    // Leemos el input.
    cin >> NumberOfShops >> RiskLimit;
   	Shops.resize(NumberOfShops,std::vector<int>(2));
    for (int i = 0; i < NumberOfShops; ++i){
		//cin >> Shops[i][0] >> Shops[i][1];
		// Primero leeme todos los riesgos.
		cin >> Shops[i][1];
	}
	// Segundo leeme todos los beneficios.
	for(int i = 0; i < NumberOfShops; ++i){
		cin >> Shops[i][0];
	}

	// Lee: Number of Shops
	// Lee: RiskLimit
	// Lee: cantidad de veces number of shops repite:
	// beneficio y riesgo del shop i.

    // Ejecutamos el algoritmo y obtenemos su tiempo de ejecución.
	int optimum;
	optimum = NoBenefit;
	auto start = chrono::steady_clock::now();
	if (algoritmo == "FB")
	{
		optimum = FB(0, 0);
	}
	else if (algoritmo == "BT")
	{
		CurrentMaxBenefit = NoBenefit;
		poda_optimalidad = poda_factibilidad = true;
		precalculateMaxBenefits();
		optimum = BT(0,0,0);
	}
	else if (algoritmo == "BT-F")
	{
		CurrentMaxBenefit = NoBenefit;
		poda_optimalidad = false;
		poda_factibilidad = true;
		optimum = BT(0, 0, 0);
	}
	else if (algoritmo == "BT-O")
	{
		CurrentMaxBenefit = NoBenefit;
		poda_optimalidad = true;
		poda_factibilidad = false;
		precalculateMaxBenefits();
		optimum = BT(0, 0, 0);
	}
	else if (algoritmo == "DP")
	{
		
		DynamicProgrammingMatrix = vector<vector<int>>(NumberOfShops+1, vector<int>(RiskLimit+1, UNDEFINED));
		
		// Obtenemos la solucion optima.
		optimum = DP(0, 0);
	}
	auto end = chrono::steady_clock::now();
	double total_time = chrono::duration<double, milli>(end - start).count();

	// Imprimimos el tiempo de ejecución por stderr.
	cout << "Clog" << endl;
	clog << total_time << endl;

    // Imprimimos el resultado por stdout.
    cout << "Cout" << endl;
    cout << optimum << endl;
    return 0;
}