#include "lab4_mpi.h"

#include <malloc.h>
#include <iostream>
#include <vector>
#include "mpi.h"
using namespace std;


void findWitnessArray (const char *pattern, int plen, vector<int> &WitnessArray, int wlen);
int DUEL (const char *Z, int n, const char *Y, int m, vector<int> &WitnessArray, int wlen, int i, int j);

void periodic_pattern_matching (
		int n, 
		char *text, 
		int num_patterns, 
		int *m_set, 
		int *p_set, 
		char **pattern_set, 
		int **match_counts, 
		int **matches)
{
	int codeToRun = 1;

	if (codeToRun == 0) {
		// Run Sequential Code for comparing output
		vector<vector<int>> string_matches (num_patterns);
		int total_matches = 0;

		for (int p = 0; p < num_patterns; p++) {
			char *pattern = pattern_set[p];
			int plen = m_set[p];
			int period = p_set[p];

			for (int i = 0; i + plen - 1 < n; i++) {
				int j;
				for (j = 0; j < plen; j++) {
					if (text[i+j] != pattern[j])
						break;
				}
				if (j == plen) {
					// Found pattern
					string_matches[p].push_back(i);
					total_matches++;
				}
			}
		}

		*matches = (int *) malloc(total_matches * sizeof(int));
		*match_counts = (int *) malloc(num_patterns * sizeof(int));
		int m = 0;
		for (int p = 0; p < num_patterns; p++) {
			(*match_counts)[p] = string_matches[p].size();
			for (int i = 0; i < string_matches[p].size(); i++) {
				(*matches)[m++] = string_matches[p][i];
			}
		}
	}

	if (codeToRun == 1) {
		// Sequential Joseph JaJa
		vector<vector<int>> WitnessArrays (num_patterns);
		vector<vector<int>> string_matches (num_patterns);
		int total_matches = 0;

		const char *Z = "abcaabcabaa";
		const char *Y = "abcabcab";
		vector<int> WitArr;
		findWitnessArray (Y, 8, WitArr, 3);
		WitArr[1] = 3;
		WitArr[2] = 2;
		for (int i = 0; i < WitArr.size(); i++) {
			cout << WitArr[i] << " ";
		}
		cout << endl;

		cout << DUEL (Z, 11, Y, 9, WitArr, 3, 4, 6) << endl;

		// for (int p = 0; p < num_patterns; p++) {
		// 	char *pattern = pattern_set[p];
		// 	int plen = m_set[p];
		// 	int period = p_set[p];
		// 	int mBY2 = (plen+1) / 2;

		// 	findWitnessArray (pattern, plen, WitnessArrays[p], min(period, mBY2));

		// 	for (int i = 0; i + plen - 1 < n; i++) {
		// 		int j;
		// 		for (j = 0; j < plen; j++) {
		// 			if (text[i+j] != pattern[j])
		// 				break;
		// 		}
		// 		if (j == plen) {
		// 			// Found pattern
		// 			string_matches[p].push_back(i);
		// 			total_matches++;
		// 		}
		// 	}
		// }
	}
}

void findWitnessArray (const char *pattern, int plen, vector<int> &WitnessArray, int wlen) {
	WitnessArray.resize(wlen, wlen);
	WitnessArray[0] = 0;
	for (int i = 1; i < wlen; i++) {
		for (int k = 0; i+k < plen; k++) {
			if (pattern[k] != pattern[i + k]) {
				WitnessArray[i] = k;
				break;
			}
		}
	}
}

int DUEL (const char *Z, int n, const char *Y, int m, vector<int> &WitnessArray, int wlen, int i, int j) {
	int k = WitnessArray[j - i];
	if (j + k >= n || Z[j+k] != Y[k]) return i;
	else return j;
}