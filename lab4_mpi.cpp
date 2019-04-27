#include "lab4_mpi.h"

#include <malloc.h>
#include <iostream>
#include <vector>
#include "mpi.h"
using namespace std;


void findWitnessArray (char *pattern, int plen, vector<int> &WitnessArray, int wlen);
int DUEL (char *Z, int n, const char *Y, int m, vector<int> &WitnessArray, int wlen, int i, int j);
void TextAnalysis (char *text, int n, char *pattern, int plen, int minPeriod, vector<int> &matches, vector<int> &WitArr);

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

		for (int p = 0; p < num_patterns; p++) {
			char *pattern = pattern_set[p];
			int plen = m_set[p];
			int period = p_set[p];
			int mBY2 = (plen+1) / 2;
			int minPeriod = min(period, mBY2);

			findWitnessArray (pattern, plen, WitnessArrays[p], minPeriod);

			TextAnalysis (text, n, pattern, plen, minPeriod, string_matches[p], WitnessArrays[p]);
			total_matches += string_matches[p].size();

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
}

void findWitnessArray (char *pattern, int plen, vector<int> &WitnessArray, int wlen) {
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

int DUEL (char *Z, int n, char *Y, int m, vector<int> &WitnessArray, int wlen, int i, int j) {
	int k = WitnessArray[j - i];
	if (j + k >= n || Z[j+k] != Y[k]) return i;
	else return j;
}

void TextAnalysis (char *text, int n, char *pattern, int plen, int minPeriod, vector<int> &matches, vector<int> &WitArr) {
	// Partition into blocks
	vector<int> pmatches;
	for (int b = 0; b < n / minPeriod; b++) {
		int i = b * minPeriod;

		for (int ip = i; ip < (b+1)*minPeriod; ip++) {
			int j;
			for (j = 0; ip+j < n && j < plen; j++) {
				if (text[ip+j] != pattern[j]) break; }
			if (j == plen) {
				matches.push_back(ip); break; }}
		
		if (i + plen - 1 >= n) {
			for (int j = i+1; j < (b+1)*minPeriod; j++) {
				i = DUEL (text, n, pattern, plen, WitArr, WitArr.size(), i, j);
			}

			// i is a potential position
			int j;
			for (j = 0; i+j < n && j < plen; j++) {
				if (text[i+j] != pattern[j])
					break;
			}
			if (j == plen) {
				// Found pattern
				pmatches.push_back(i);
			}
		}
	}
}

int P_TextAnalysis (char *text, int n, char *P, int m, int period) {
	char *P_dash = P;
	int m_dash = 2*period - 1;

	vector<int> WitArray;
	findWitnessArray (P_dash, m_dash, WitArray, period);

	vector<int> matches;
	TextAnalysis (text, n, P_dash, m_dash, period, matches, WitArray);

	char *P_u = P;
	int m_u = period;

	int k = m / period;
	char *P_v = P + k*period;
	int m_v = m - k*period;

	for (int i = 0; i < n; i++) {

	}

}
