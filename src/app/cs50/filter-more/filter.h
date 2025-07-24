#ifndef FILTER
#define FILTER
// Entry point
int filter(int argc, char *argv[]);

// Filters
int grayscale(FILE *infile, FILE *outfile);
int reflect(FILE *infile, FILE *outfile);
int blur(FILE *infile, FILE *outfile, int radius);
int edges(FILE *infile, FILE *outfile);
#endif