#ifndef __LABELING_H__
#define __LABELING_H__

class Labeling  {
public:
    	
    	
    Labeling(int nplan, int rowSz, int colSz);

    virtual ~Labeling();
	
	//3D ----------------
	
	int GetArray3D(int plan, int row, int col);
	void SetArray3D(int plan, int row, int col, int value);
	
	int* GetPlane(int plan);
	
	//RICERCA REGIONI CONNESSE
	
	int LabelingTwoSteps83D();
	
	int SecondStep3D(int last_i, int last_j, int last_k, unsigned int NewLabel, int* C );
	
	int rowSz;
	
	int colSz;
	
	int nplan;
	
	int* array_value;	
	
};

#endif // __LABELING_H__
