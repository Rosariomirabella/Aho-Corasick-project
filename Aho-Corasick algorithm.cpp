using namespace std;
#include <bits/stdc++.h>

const int MAXS = 500; // massimo numero di stati (nodi) della matching machine.
const int MAXC = 26; // massimo numero di caratteri nell'alfabeto di input.

int f[MAXS]; // funzione fail implementata usando un array
int g[MAXS][MAXC]; // funzione goto implementata usando una matrice.

// funzione out implementata con un vettore. 
//Indica quali stringhe del patter vengono 
//riconosciute nel momento in cui si entra in uno stato q
int out[MAXS]; 

// Funzione attraverso cui si costruisce l'automa di Aho-Corasick
// ritorna il numero di nodi (stati) dell'automa.
// P - array of words. The index of each keyword is important:
//		 "out[state] & (1 << i)" is > 0 if we just found word[i]
//		 in the text.
int buildMatchingMachine(string *P, int k) 
{
	memset(out, 0, sizeof out); // si inizializza ogni posizione dell'array di output con il valore 0
	memset(g, -1, sizeof g); //si inizializza ogni posizione della matrice g con il valore 0
	int states = 1; // inizialmente si ha un solo nodo (stato), lo start state.

	// Si costruiscono il keyword tree 
	// e parzialamente i valori di out[]
	for (int i = 0; i < k; ++i)
	{
	 	string word = P[i];
		int currentState = 0;

		// Si inseriscono tutti i caratteri della parola corrente nell'array P[] 
		for (int j = 0; j < word.size(); ++j)
		{
			int ch = word[j] - 'a';
			
			//Si alloca un nuovo nodo (stato) se questo 
			// ancora non esiste per il carattere "ch".
			if (g[currentState][ch] == -1) {
				g[currentState][ch] = states;
				states++;
			}
			currentState = g[currentState][ch];
		}

		//Aggiungiamo word nella funzione di output alla posizione currentState  
		out[currentState] |= (1 << i);
	}

	// Per ogni carattere per cui non esiste un arco
	// dalla root (stato 0) nel keyword tree, si aggiunge
	// un arco che va dallo stato 0 allo stato 0.
	for (int ch = 0; ch < MAXC; ++ch) {
		if (g[0][ch] == -1) {
			g[0][ch] = 0;
		}
	}
	
	// Costruiamo la funzione fail. Inizzialzziamo, per adesso, ogn posizione del vettore con il valore -1 
	memset(f, -1, sizeof f);

	// I valori di f vengono calcolati attraverso una BFS usando una coda.
	queue<int> q;
	
	// Si itera per ogni possibile carattere
	for (int ch = 0; ch < MAXC; ++ch)
	{
		// Tutti i nodi a profondità 1 hanno un valore di fail
		// uguale a 0. Per esempio, in questo caso f(1) = f(3) = 0.
		if (g[0][ch] != 0)
		{
			f[g[0][ch]] = 0;
			q.push(g[0][ch]);
		}
	}

	// in coda c'è lo stato 1 e lo stato 3
	while (q.size())
	{
		// si rimuove il primo elemento della coda
		int state = q.front();
		q.pop();
		
		// Per lo stato rimosso, si determina il valore di fail
		// per tutti quei caratteri per cui la funzione g non è definita
		for (int ch = 0; ch <= MAXC; ++ch)
		{
			//Se la funzione g è definita per il carattaere 'ch' e 'state'
			if (g[state][ch] != -1)
			{
				// Si inscerisce il prossimo stato in coda
				q.push(g[state][ch]);
				
				// Si determina il valore di fail dello stato rimosso dalla coda
				int failure = f[state];

				// Trovare il nodo più profondo etichettato dal suffiso proprio della stringa, 
				//dalla radice (stato 0) allo stato corrente.
				while (g[failure][ch] == -1) {
					failure = f[failure];
				}
				
				failure = g[failure][ch];
				f[g[state][ch]] = failure;

				// calcola l'unione dei valori di output.
				out[g[state][ch]] |= out[failure];
			}
		}
	}

	return states;
}


// Funzione che ritorna il prossimo stato dell'automa. Esso è 
// calcolato usando la funzione g e la funzione f.
// currentState -> è lo stato corrente dell'automa.   
// nextInput    -> Il prossimo carattere ad essere esaminato dalla macchina.
int findNextState(int currentState, char nextInput) {
	int answer = currentState;
	int ch = nextInput - 'a';

	// Se la funzione g non è definitia
	// ricorri allora alla funzione f.
	while (g[answer][ch] == -1) {
		answer = f[answer];
	}
 	return g[answer][ch];
}

// funzione che trova tutte le occorrenze di P all'interno di T.
void searchWords(string *P, int k, string T) {

	buildMatchingMachine(P, k); // costruiamo l'automa di Aho-Corasik
	int currentState = 0; // si iniziliazzia lo stato corrente

	// si scorre ongi carattere del testo T
	for (int i = 0; i < T.size(); ++i)
	{
		currentState = findNextState(currentState, T[i]);

		// Se nessun match è stato trovato
		// si passa al prossimo stato 
		if (out[currentState] == 0)
			continue; 

		// Se è stata trovata una stringa di P,
		// allora si stampano gli indici di inizio e fine 
		// della stringa all'interno di T, usando l'array out[]
		for (int j = 0; j < k; ++j)
		{
			if (out[currentState] & (1 << j))
			{
				cout << "Word " << P[j] << " appears from "
					<< i - P[j].size() + 1 << " to " << i << endl;
			}
		}
	}
}

int main()
{
	string P[] = {"a", "ab", "bc", "aac", "aab", "bd"};
	string T = "bcaab";
	int k = sizeof(P)/sizeof(P[0]);
	
	searchWords(P, k, T);

	return 0;
}

