#include "lab4_mpi.h"

#include <malloc.h>
#include <iostream>
#include <vector>
#include "mpi.h"
using namespace std;


void findWitnessArray (char *pattern, int plen, vector<int> &WitnessArray, int wlen);
int DUEL (char *Z, int n, const char *Y, int m, vector<int> &WitnessArray, int wlen, int i, int j);
void TextAnalysis (char *text, int n, char *pattern, int plen, int minPeriod, vector<int> &matches, vector<int> &WitArr);
int TextAnalysisTest (char *text, int n, char *P, int m, int period);

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
	int codeToRun = 2;

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

		for (int i = 0; i < num_patterns; i++) {
			cout << (*match_counts)[i] << " ";
		}
		cout << endl;
	}

	if (codeToRun == 2) {
		// Parallel jaja
		vector<vector<int>> WitnessArrays (num_patterns);
		vector<vector<int>> string_matches (num_patterns);
		// int total_matches = 0;

		int rank, size;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &size);

		int num_assigned;
		int *my_match_counts;
		int my_total;
		int *my_matches;

		// cout << num_patterns << " " << size << " " << rank << endl;
		int start = rank * (num_patterns / size);
		int end = min((rank + 1) * (num_patterns / size), num_patterns); // Actually end - 1
		num_assigned = end - start;

		for (int p = 0, gp = start; p < num_assigned; p++, gp++) {
			char *pattern = pattern_set[gp];
			int plen = m_set[gp];
			int period = p_set[gp];
			int mBY2 = (plen+1) / 2;
			int minPeriod = min(period, mBY2);

			findWitnessArray (pattern, plen, WitnessArrays[p], minPeriod);

			TextAnalysis (text, n, pattern, plen, minPeriod, string_matches[p], WitnessArrays[p]);
			my_total += string_matches[p].size();

			my_match_counts = (int *) malloc (num_assigned * sizeof(int));
			my_matches = (int *) malloc (my_total * sizeof(int));
		}

		int m = 0;
		for (int p = 0; p < num_assigned; p++) {
			my_match_counts[p] = string_matches[p].size();
			for (int i = 0; i < string_matches[p].size(); i++) {
				my_matches[m++] = string_matches[p][i];
			}
		}

		const int root = 0;
		/** Gathering the match_counts **/
		int *mc_recv_counts = NULL;

		if (rank == root)
			mc_recv_counts = (int *) malloc (size * sizeof(int));

		// cout << rank << " " << num_assigned << endl;
		MPI_Gather (&num_assigned, 1, MPI_INT,
					mc_recv_counts, 1, MPI_INT,
					root, MPI_COMM_WORLD);

		// Compute displs and gather match_counts
		int totalGather = 0;
		int *displs = NULL;

		if (rank == root) {
			displs = (int*) malloc (size*sizeof(int));

			displs[0] = 0;
			totalGather += mc_recv_counts[0];

			for (int i = 1; i < size; i++) {
				totalGather += mc_recv_counts[i];
				displs[i] = displs[i-1]+mc_recv_counts[i-1];
			}

			*match_counts = (int *) malloc (totalGather * sizeof(int));

			// cout << totalGather << endl;
			// for (int i = 0; i < size; i++) {
			// 	cout << displs[i] << " ";
			// }
			// cout << endl;
		}

		MPI_Gatherv (my_match_counts, num_assigned, MPI_INT,
					*match_counts, mc_recv_counts, displs, MPI_INT,
					root, MPI_COMM_WORLD);

		// if (rank == root) {
		// 	for (int i = 0; i < num_patterns; i++) {
		// 		cout << (*match_counts)[i] << " ";
		// 	}
		// 	cout << endl;
		// }
		
		MPI_Barrier(MPI_COMM_WORLD);

		/** Gathering the matches **/
		int *m_recv_counts = NULL;

		if (rank == root)
			m_recv_counts = (int *) malloc (size * sizeof(int));

		// cout << rank << " " << my_total << endl;
		num_assigned = my_total;
		MPI_Gather (&num_assigned, 1, MPI_INT,
					m_recv_counts, 1, MPI_INT,
					root, MPI_COMM_WORLD);

		// // Compute displs and gather match_counts
		int mTotalGather = 0;
		int *mDispls = NULL;

		if (rank == root) {
			mDispls = (int*) malloc (size*sizeof(int));

			mDispls[0] = 0;
			mTotalGather += m_recv_counts[0];

			for (int i = 1; i < size; i++) {
				mTotalGather += m_recv_counts[i];
				mDispls[i] = mDispls[i-1]+m_recv_counts[i-1];
			}

			*matches = (int *) malloc (mTotalGather * sizeof(int));

			// cout << mTotalGather << endl;
			// for (int i = 0; i < size; i++) {
			// 	cout << mDispls[i] << " ";
			// }
			// cout << endl;
		}



		MPI_Gatherv (my_matches, num_assigned, MPI_INT,
					*matches, m_recv_counts, mDispls, MPI_INT,
					root, MPI_COMM_WORLD);


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

int TextAnalysisTest (char *text, int n, char *P, int m, int period) {
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
