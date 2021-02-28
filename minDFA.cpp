// Programul foloseste metoda partitionarii multimii Q a starilor, 
//prezentata la seminar


#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <set>
#define Partitie vector<Nod>
using namespace std;

struct Nod
{
	int nume;
	map<string, int> tranzitii;
};
class DFA   // implementarea clasica DFA OOP
{
	set<int> Q;
	set<char> Sigma;
	int q0;
	map <pair <int, string>, int> delta;
	set<int> F;
	vector<Nod> Noduri;
public:
	vector<Partitie> dfaPartitii;
	DFA(set<int> Q, set<char> Sigma, map<pair<int, string>, int> delta, int q0, set<int> F)
	{
		this -> Q = Q;
		this -> Sigma = Sigma;
		this -> delta = delta;
		this -> q0 = q0;
		this -> F = F;
		for (auto stare : Q)
		{
			Nod temp;
			temp.nume = stare;
			Noduri.push_back(temp);
		}
		for (auto d : delta)
		{
			for (auto itNod = Noduri.begin(); itNod != Noduri.end(); itNod++)
			{
				if ((*itNod).nume == d.first.first)
				{
					(*itNod).tranzitii[d.first.second] = d.second;
				}
			}
		}
	}
	bool eFinal(int stare)  // verifica daca o stare este finala
	{
		for (auto f : F)
		{
			if (f == stare) return true;
		}
		return false;
	}
	bool esteInPartitie(int myNod, Partitie partitie)		//verifica daca o stare se afla in partitia data
	{
		for (auto nod : partitie)
		{
			if (myNod == nod.nume) return true;
		}
		return false;
	}
	bool sePotrivesteLaPartitie(Nod nod, Partitie partitie) // verifica daca o stare se potriveste la o 
															// partitie la care vrem sa o mutam
															// i.e. se uita daca q0 tranzitioneaza cu toate
															// literele inspre partitia respectiva
	{
		for (auto tranzNod : nod.tranzitii)
			if (tranzNod.second != partitie[0].tranzitii[tranzNod.first]) 
				return false;
		return true;
	}
	bool trebuieMutat(Nod nod, Partitie partitie)			// asemenea functiei de mai sus, doar ca folosita in
															// contextul partitionarii multimii actuale
	{
		for (auto tranzitie : nod.tranzitii)
			if (!esteInPartitie(tranzitie.second, partitie))
				return true;
		return false;
	}
	bool verif()											// verifica daca partitionarea actuala (dupa pasul k)
															// este corespunzatoare, i.e. nu mai putem sparge nicio 
															// submultime a lui Q in mai multe partitii
	{
		
		for (auto partitie : dfaPartitii)
			for (auto nod : partitie)
				for (auto tranzitie : nod.tranzitii)
					if (!esteInPartitie(tranzitie.second, partitie))
						return false;
		
		return true;
	}
	void minimizare()
	{
		// pasul 1: despartim starile finale de starile non-finale
		vector<Nod> QF, F;
		for (auto nod : Noduri)
			if (eFinal(nod.nume)) F.push_back(nod);		//daca o stare e finala
			else QF.push_back(nod);						//daca o stare nu e finala
		
		dfaPartitii.push_back(QF);
		dfaPartitii.push_back(F);					//populam vectorul de partitii cu QF si F
		while (!verif())
		{
			vector<Partitie> partitiiTemporare;		//vector de partitii temporare, adica viitoare partitii
													//ce vor fi adaugagte la dfaPartitii obtinute
													//prin spargerea unei partitii
			for (auto itPartitie = dfaPartitii.begin(); itPartitie != dfaPartitii.end(); itPartitie++) 
			{
				for (auto itNod = (*itPartitie).begin(); itNod != (*itPartitie).end(); itNod++ )
				{
					//daca un nod trebuie mutat (are cel putin o tranzitie care iese din el 
					//care nu duce in partitia actuala in care se afla
					if (trebuieMutat(*itNod, *itPartitie))
					{
						bool aFostPotrivita = false;
						//incerc sa o pun intr-o partitie "temporara" adica una din
						//cele create pana acum
						for (auto itTemporare = partitiiTemporare.begin(); itTemporare != partitiiTemporare.end(); itTemporare++)
						{
							if (sePotrivesteLaPartitie(*itNod, *itTemporare))
							{
								(*itTemporare).push_back(*itNod);
								(*itPartitie).erase(itNod);
								aFostPotrivita = true;
							}
						}
						//daca nu am gasit o partitie temporara unde sa o pun, 
						//creez una noua si o pun acolo
						if (!aFostPotrivita)
						{
							Partitie temp;
							temp.push_back(*itNod);
							partitiiTemporare.push_back(temp);
							(*itPartitie).erase(itNod);
						}

					}
				}
				
			}
			//cand am terminat verificarea de la pasul "k" curent,
			//adaug partitiile nou create, updatez vectorul
			for (auto partitieTemporara : partitiiTemporare)
				dfaPartitii.push_back(partitieTemporara);
		}
		
		// am ramas cu o partitionare finala a multimii
		// acum grupez starile care fac parte din aceeasi partitie
		// (daca sunt mai mult de una)
		for (auto itPartitie = dfaPartitii.begin(); itPartitie != dfaPartitii.end(); itPartitie++)
		{
			if ((*itPartitie).size() > 1)
			{
				
				int numeStare = 0;
				//aflu denumirea nouii stari compuse
				for (auto itNod = (*itPartitie).begin(); itNod != (*itPartitie).end(); itNod++)
				{
					numeStare *= 10;
					numeStare += (*itNod).nume;  // ex: starile 1,2,4 vor deveni starea 124
				}
				//in fiecare partitie cu mai mult de 1 element, parcurg toate starile,
				//le sterg din Q si le inlocuiesc cu starea compusa (si updatez functia delta)
				for (auto itNod = (*itPartitie).begin(); itNod != (*itPartitie).end(); itNod++)
				{
					for (auto it = delta.begin(); it != delta.end(); it++)
					{
						if ((it->first.first == (*itNod).nume))
						{
							it->second = numeStare;
						}
					}
					for (auto it = delta.begin(); it != delta.end(); it++)
					{
						if ((it->second == (*itNod).nume))
						{
							string litera = it->first.second;
							int stare2 = it->second;
							delta.erase(it);
							delta[make_pair(numeStare, litera)] = stare2;
						}
					}
					Q.erase(itNod->nume);
				}
				Q.insert(numeStare);
			}
		}
	}
	
};

int main()
{
	set<int> Q = {1, 2, 3, 4, 5, 6};
	set<char> Sigma = { 'a', 'b' };
	int q0 = 1;
	map <pair <int, string>, int> delta;
	delta[{1, "a"}] = 3;
	delta[{1, "b"}] = 2;
	delta[{2, "a"}] = 3;
	delta[{2, "b"}] = 2;
	delta[{3, "a"}] = 6;
	delta[{3, "b"}] = 5;
	delta[{4, "a"}] = 6;
	delta[{4, "b"}] = 5;
	delta[{5, "a"}] = 6;
	delta[{5, "b"}] = 2;
	delta[{6, "a"}] = 4;
	delta[{6, "b"}] = 5;
	set<int> F = { 6 };
	DFA dfa(Q, Sigma, delta, q0, F);
	dfa.minimizare();
	
}