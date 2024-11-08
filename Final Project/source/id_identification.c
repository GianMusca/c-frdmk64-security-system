#include "id_identification.h"
#include "array_compare.h"

//#define AMOUNT_OF_ID	4
//#define ID_SIZE			8
//
//static const char id_list[AMOUNT_OF_ID][ID_SIZE]={
//		"60612683",
//		"46605701",
//		"60105667",
//		"45176601"
//};

char Cards[N_CARDS][ID_SIZE+1]={
	"60612683","46605701","60105667","45176601","","","","","",""
};

bool search_id_in_list(const char* id){
	bool found = false;
	int i;
	for(i=0; i<N_CARDS && !found; i++){
//		int j;
//		bool difference = false;
//		for(j=0; j<ID_SIZE && !difference; j++){
//			if(id[j] != id_list[i][j])	difference = true;
//		}
//		if(!difference)	found = true;
		if (compareArrays(id,Cards[i],ID_SIZE)) found = true;
	}
	return found;
}
