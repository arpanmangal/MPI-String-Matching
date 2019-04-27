#include "lab4_mpi.h"

#include <malloc.h>
#include <iostream>
#include <vector>
#include "mpi.h"
using namespace std;


// /*
// 	*****************************************************
// 		TODO -- You must implement this function
// 	*****************************************************
// */
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
	int codeToRun = 0;

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
}
