
#include <stdio.h>

#include "Labeling.h"

#define MAXLABELCONNECTED 65536

Labeling::Labeling(int nplan, int rowSz, int colSz) : array_value(0) {
	
	this->rowSz = rowSz;
	this->colSz = colSz;
	this->nplan = nplan;
	
	array_value = (int*) new int[nplan * rowSz * colSz];
	
}

Labeling::~Labeling() {

	delete []array_value;
	
}

void Labeling::SetArray3D(int plan, int row, int col, int value) {
	if(plan < 0 || plan >= nplan)
		return;
	if(row < 0 || row >= rowSz)
		return;
	if(col < 0 || col >= colSz)
		return;
	array_value[rowSz * colSz * plan + colSz * row + col] = value;
}

int Labeling::GetArray3D(int plan, int row, int col) {
	if(plan < 0 || plan >= nplan)
		return 0;
	if(row < 0 || row >= rowSz)
		return 0;
	if(col < 0 || col >= colSz)
		return 0;
	return array_value[rowSz * colSz * plan + colSz * row + col];
}

int* Labeling::GetPlane(int plan) {
	if(plan < 0 || plan >= nplan)
		return 0;
	else
		return array_value + plan*rowSz*colSz;
}

int Labeling::SecondStep3D(int last_i, int last_j, int last_k, unsigned int NewLabel, int* C ) {
	
	int MAXLABEL = MAXLABELCONNECTED;
	unsigned int i, j, k, NumGroup;
	
    //Compattazione degli identificatori delle classi di equivalenza
	//Passaggio non necessario, ma consente una visualizzazione migliore
	
    NumGroup = 1;
	for(k=1; k<=NewLabel; k++)
	{
		int group = C[k];

		if(group >= NumGroup)
		{
			for(i=k; i<=NewLabel;i++)
				if(C[i] == group) C[i] = NumGroup;
			NumGroup++;
		}
	}

	//secondo passo
	for(i = 0; i < last_i; i++)
		for(j = 0; j < last_j ; j++)
			for(k = 0; k < last_k ; k++)
			{	
				//ad ogni pixel con una etichetta gli si assegna
				//una nuova etichetta che coincide con il
				//numero del gruppo (classe di equivalenza) alla
				//quale l'etichetta attuale appartiene
	//			if ((frame[index] != BACKGROUND) && (frame[index] != OBJECT))
				SetArray3D(i, j, k, C[(int)GetArray3D(i, j, k)]);
			}


 	for(k=0; k<MAXLABEL; k++) C[k] = k;

	return NumGroup;

}





int Labeling::LabelingTwoSteps83D()
{

	int MAXLABEL = MAXLABELCONNECTED;
	
    //Versione con soluzione del problema dell'
	//esaurimento delle label

	//(8-connettivita')

	int lx = 0;					//label del pixel x
	unsigned int NewLabel = 0;	//tiene traccia delle label non ancora utilizzate
	int C[MAXLABEL];			//Array di equivalenza
	int CL[13];					//Classi di equivalenza dei dintorni del punto x
	int L[13];					//le label del pixel
	unsigned int last_i;

	int i, j, k;
	// int NumObject = 0;	//utilizzato per contare il numero di oggetti individuati
	unsigned short BG = 0;
	long int count=0;

	int N = nplan;
	int Mrow = rowSz;
	int Mcol = colSz;
	
	int F[MAXLABEL];

	//init di R
	for(k=0; k<MAXLABEL; k++) C[k] = k, F[k] = 1;

	//prima fase
	for(i=0; i<N; i++)
		for(j=0; j<Mrow; j++)
			for(k=0; k<Mcol; k++)
			{
				lx = 0;
				if (GetArray3D(i, j, k) != BG)	//e' un punto da analizzare
				{
					CL[0]	= C[L[0]	=(int)GetArray3D(i-1, j-1, k-1)];
					CL[1]	= C[L[1]	=(int)GetArray3D(i-1, j-1, k)];
					CL[2]	= C[L[2]	=(int)GetArray3D(i-1, j-1, k+1)];
					CL[3]	= C[L[3]	=(int)GetArray3D(i-1, j, k-1)];
					CL[4]	= C[L[4]	=(int)GetArray3D(i-1, j, k)];
					CL[5]	= C[L[5]	=(int)GetArray3D(i-1, j, k+1)];
					CL[6]	= C[L[6]	=(int)GetArray3D(i-1, j+1, k-1)];
					CL[7]	= C[L[7]	=(int)GetArray3D(i-1, j+1, k)];
					CL[8]	= C[L[8]	=(int)GetArray3D(i-1, j+1, k+1)];
					CL[9]	= C[L[9]	=(int)GetArray3D(i, j-1, k-1)];
					CL[10]	= C[L[10]	=(int)GetArray3D(i, j-1, k)];
					CL[11]	= C[L[11]	=(int)GetArray3D(i, j-1, k+1)];
					CL[12]	= C[L[12]	=(int)GetArray3D(i, j, k-1)];
					
					//devo anche assegnare lx
					
					//CASO A - tutti le classi di equivalenza sono uguali
					bool bkg = true;
					for(int a=0; a<13; a++)
						if(CL[a] != BG) {
							bkg = false;
							break;
						}
					if(bkg) {
						//tutte le classi di equivalenza sono uguali, ma sono BG
						//creazione di una nuova label
						NewLabel++;
	
						if(NewLabel >= MAXLABEL)
						{
                            printf("TOO MUCH LABELS !!!!!\n");
							
                            NewLabel --;
							last_i = i;
							
                            NewLabel = SecondStep3D(last_i, j, k, NewLabel, C);
							for(k=0; k<MAXLABEL; k++) F[k] = 1;
	
							if(NewLabel >= MAXLABEL)
							{
								printf("LABELING ERROR\n");
								return -1;
							}
						}
	
						lx = NewLabel;
						
					} else {
						
						int CLTEMP[13];
						int LTEMP[13];
						int ncltemp = 0;
						bool eq = true;
						//rimuovo il BKG
						for(int a = 0; a < 13; a++)
							if(CL[a] != BG) {
								CLTEMP[ncltemp] = CL[a];
								LTEMP[ncltemp++] = L[a];
// 								lx = L[a];
							}
						lx = LTEMP[0];
						
						for(int a=0; a<ncltemp-1; a++)
							if(CLTEMP[a] != CLTEMP[a+1]) {
								eq = false;
								break;
							}
						//CASO B: non è tutto BG. Tutte le classi di equivalenza sono uguali
						if(eq) {
							;
							//lx è già assengato
						} else  {
							//CASO C: le classi di equivalenza non sono tutte uguali
							//almeno una delle classi di equivalenza è diversa.
							//Si fondono tutte le classi in CL, e poi ad lx si assegna 
							//l'etichetta del primo pixel non BG in CL
							//NB ancora una volta lx è già assegnato con l'ultimo pixel non BG
							for(int a=0; a<ncltemp-1; a++) {
								if(CLTEMP[a] != CLTEMP[a+1]) {
									int k, Clp = CLTEMP[a], Clr = CLTEMP[a+1];
									// int lp = a;
									// int lr = a+1;
									
									
									for(int ka=0; ka<= NewLabel; ka++)
										if(C[ka] == Clp)
											C[ka] = Clr; //elimina Clp

									CL[0] = C[L[0]=(int)GetArray3D(i-1, j-1, k-1)];
									CL[1] = C[L[1]=(int)GetArray3D(i-1, j-1, k)];
									CL[2] = C[L[2]=(int)GetArray3D(i-1, j-1, k+1)];
									CL[3] = C[L[3]=(int)GetArray3D(i-1, j, k-1)];
									CL[4] = C[L[4]=(int)GetArray3D(i-1, j, k)];
									CL[5] = C[L[5]=(int)GetArray3D(i-1, j, k+1)];
									CL[6] = C[L[6]=(int)GetArray3D(i-1, j+1, k-1)];
									CL[7] = C[L[7]=(int)GetArray3D(i-1, j+1, k)];
									CL[8] = C[L[8]=(int)GetArray3D(i-1, j+1, k+1)];
									CL[9] = C[L[9]=(int)GetArray3D(i, j-1, k-1)];
									CL[10] = C[L[10]=(int)GetArray3D(i, j-1, k)];
									CL[11] = C[L[11]=(int)GetArray3D(i, j-1, k+1)];
									CL[12] = C[L[12]=(int)GetArray3D(i, j, k-1)];
									ncltemp = 0;
									for(int aa = 0; aa < 13; aa++)
										if(CL[aa] != BG) {
											CLTEMP[ncltemp] = CL[aa];
											LTEMP[ncltemp++] = L[aa];
											
										}
									lx = LTEMP[0];
									a = 0; //non sarebbe necessario
								} 
							}
						}
					}
					/*
					if(lx == 0)
						cout << "LX == 0 ERROR " << endl;
					*/	
					SetArray3D(i, j, k, lx);
				}
			}

 	NewLabel = SecondStep3D(N, Mrow, Mcol, NewLabel, C);

	return count;

}

void Labeling::saveToFile(const char *fname) {

    FILE *pFile = fopen(fname, "w");
    
    fwrite(&colSz, sizeof(int), 1, pFile);
    fwrite(&rowSz, sizeof(int), 1, pFile);
    fwrite(&nplan, sizeof(int), 1, pFile);
    
    fwrite(array_value, sizeof(int), colSz*rowSz*nplan, pFile);
    
    fclose(pFile);
    
}