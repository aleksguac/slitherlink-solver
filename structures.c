#include <stdio.h>
#include <stdbool.h>

#define MAXPATCHES 405
#define MAXNODES 448
#define MAXEDGES 852
#define MAXNEIGHBOURS 6
#define MAXSOLUTION 213

typedef enum Edge {
  Line = 1,
  Cross = 0,
  Empty = -1,
} Edge;

typedef enum Colour {
  In = 1,
  Out = 0,
  Unshaded = -1,
} Colour;

typedef enum Value {
  None = -1,
} Value;

typedef struct Node {
  unsigned int index;
  Edge* edges[MAXNEIGHBOURS];
  unsigned int n_edges;
  struct Node* neighbour_nodes[MAXNEIGHBOURS];
  unsigned int n_neighbour_nodes;
  struct Patch* neighbour_patches[MAXNEIGHBOURS];
  unsigned int n_neighbour_patches;
} Node;

typedef struct Patch {
  unsigned int index;
  Value value;
  Edge* edges[MAXNEIGHBOURS];
  unsigned int n_edges;
  struct Patch* neighbour_patches[MAXNEIGHBOURS];
  unsigned int n_neighbour_patches;
  struct Patch* corner_patches[MAXNEIGHBOURS];
  unsigned int n_corner_patches;
  struct Node* neighbour_nodes[MAXNEIGHBOURS];
  unsigned int n_neighbour_nodes;
  Colour colour;
} Patch;

typedef struct Grid {
  Patch patches[MAXPATCHES];
  unsigned int n_patches;
  Node nodes[MAXNODES];
  unsigned int n_nodes;
  Edge edges[MAXEDGES];
  unsigned int n_edges;
} Grid;

typedef struct Tally {
  int n_lines;
  int n_crosses;
  int n_empties;
  int lines[MAXNEIGHBOURS];
  int crosses[MAXNEIGHBOURS];
  int empties[MAXNEIGHBOURS];
} Tally;

void make_grid(Grid* grid, char* input, int size[2], int max_value) {
  for (int e = 0; e < MAXEDGES; e++) { grid->edges[e] = Empty; }
  int edge_counter = 0;
  for (int p = 0; p < size[0] * size[1]; p++) {
    Patch* patch = &grid->patches[p];
    for (int i = 0; i < MAXNEIGHBOURS; i++) {
      patch->neighbour_nodes[i] = NULL;
      patch->neighbour_patches[i] == NULL;
      patch->edges[i] = NULL;
    }
    Value value = input[p] - '0';
    if (value < 0 || value > max_value) {
      value = None;
    }
    patch->value = value;
    patch->index = p;
    patch->colour = Unshaded;

    patch->n_corner_patches = 4;
    patch->n_neighbour_nodes = 4;
    patch->n_neighbour_patches = 4;
    patch->n_edges = 4;

    int neighbour_patches_inds[] = {p - size[0], p + 1, p + size[0], p - 1};
    int corner_patches_inds[] = {p - size[0] - 1, p - size[0] + 1, p + size[0] + 1, p + size[0] - 1};
    int node_0 = (p / size[0]) * (size[0] + 1) + (p % size[0]);
    int neighbour_nodes_inds[] = {node_0, node_0 + 1, node_0 + size[0] + 2, node_0 + size[0] + 1};

    for (int i = 0; i < 4; i++) {
      int ip = neighbour_patches_inds[i];
      int in = neighbour_nodes_inds[i];
      int ic = corner_patches_inds[i];
      patch->neighbour_patches[i] = ip >= 0 && ip < size[0] * size[1] && !(i == 1 && ip % size[0] == 0) && !(i == 3 && ip % size[0] == size[0] - 1) ? &grid->patches[ip] : NULL;
      patch->neighbour_nodes[i] = in >= 0 && in < (size[0] + 1) * (size[1] + 1) ? &grid->nodes[in] : NULL;
      patch->corner_patches[i] = ic >= 0 && ic < size[0] * size[1] && !((i == 1 || i == 2) && ic % size[0] == 0) && !((i == 0 || i == 3) && ic % size[0] == size[0] - 1) ? &grid->patches[ic] : NULL;
    }
    
    patch->edges[0] = patch->neighbour_patches[0] == NULL ? &grid->edges[edge_counter++] : patch->neighbour_patches[0]->edges[2];
    patch->edges[1] = &grid->edges[edge_counter++];
    patch->edges[2] = &grid->edges[edge_counter++];    
    patch->edges[3] = patch->neighbour_patches[3] == NULL ? &grid->edges[edge_counter++] : patch->neighbour_patches[3]->edges[1];

    patch->neighbour_nodes[0]->edges[1] = patch->edges[0];
    patch->neighbour_nodes[0]->edges[2] = patch->edges[3];
    patch->neighbour_nodes[1]->edges[2] = patch->edges[1];
    patch->neighbour_nodes[1]->edges[3] = patch->edges[0];
    patch->neighbour_nodes[2]->edges[3] = patch->edges[2];
    patch->neighbour_nodes[2]->edges[0] = patch->edges[1];
    patch->neighbour_nodes[3]->edges[0] = patch->edges[3];
    patch->neighbour_nodes[3]->edges[1] = patch->edges[2];

    patch->neighbour_nodes[0]->neighbour_patches[2] = patch;
    patch->neighbour_nodes[1]->neighbour_patches[3] = patch;
    patch->neighbour_nodes[2]->neighbour_patches[0] = patch;
    patch->neighbour_nodes[3]->neighbour_patches[1] = patch;
  }
  for (int n = 0; n < (size[0] + 1) * (size[1] + 1); n++) {
    Node* node = &grid->nodes[n];
    node->index = n;

    node->n_edges = 4;
    node->n_neighbour_nodes = 4;
    node->n_neighbour_patches = 4;

    int neighbour_nodes_inds[] = {n - (size[0] + 1), n + 1, n + (size[0] + 1), n - 1};

    for (int i = 0; i < 4; i++) {
      int in = neighbour_nodes_inds[i];
      node->neighbour_nodes[i] = in >= 0 && in < (size[0] + 1) * (size[1] + 1) && !(i == 1 && in % (size[0] + 1) == 0) && !(i == 3 && in % (size[0] + 1) == (size[0] + 1) - 1) ? &grid->nodes[in] : NULL;
    }
  }
  grid->n_patches = size[0] * size[1];
  grid->n_nodes = (size[0] + 1) * (size[1] + 1);
  grid->n_edges = edge_counter;
}

void print_grid(Grid* grid, int size[2], bool shading, bool number_nodes) {
  int n;
  int p;
  for (int i = 0; i < size[0]*4 + 1; i++) { printf("-"); }
  if (shading && !number_nodes) {
    printf("     ");
    for (int i = 0; i < size[0]*4 + 1; i++) { printf("-"); }
  }
  printf("\n");
  for (int row = 0; row <= size[1]; row++) {
    for (int col = 0; col <= size[0]; col++) {
      n = row * (size[0] + 1) + col;
      Node* node = &grid->nodes[n];
      if (number_nodes) { printf("%.3d", node->index); } else { printf("·"); }
      if (node->edges[1] != NULL) { printf("%s", *node->edges[1] == Line ? "---" : (*node->edges[1] == Empty ? "   " : " x ")); }
    }
    if (shading && !number_nodes) {
      printf("     ");
      for (int col = 0; col <= size[0]; col++) {
        n = row * (size[0] + 1) + col;
        Node* node = &grid->nodes[n];
        printf("·");
        if (node->edges[1] != NULL) { printf("%s", *node->edges[1] == Line ? "---" : (*node->edges[1] == Empty ? "   " : " x ")); }
      }
    }
    printf("\n");
    if (row < size[1]) {
      for (int col = 0; col < size[0]; col++) {
        p = row * size[0] + col;
        Patch* patch = &grid->patches[p];
        if (p % size[0] == 0) { printf("%c", *patch->edges[3] == Line ? '|' : (*patch->edges[3] == Empty ? ' ' : 'x')); }
        if (number_nodes) {
          printf("   %c %c", patch->value >= 0 ? patch->value + '0' : ' ', *patch->edges[1] == Line ? '|' : (*patch->edges[1] == Empty ? ' ' : 'x'));
        } else {
          printf(" %c %c", patch->value >= 0 ? patch->value + '0' : ' ', *patch->edges[1] == Line ? '|' : (*patch->edges[1] == Empty ? ' ' : 'x'));
        }
      }
      if (shading && !number_nodes) {
        printf("     ");
        for (int col = 0; col < size[0]; col++) {
          p = row * size[0] + col;
          Patch* patch = &grid->patches[p];
          if (p % size[0] == 0) { printf("%c", *patch->edges[3] == Line ? '|' : (*patch->edges[3] == Empty ? ' ' : 'x')); }
          printf(" %s %c", patch->colour == Empty ? " " : (patch->colour == Line ? "■" : "□"), *patch->edges[1] == Line ? '|' : (*patch->edges[1] == Empty ? ' ' : 'x'));
        }
      }
      printf("\n");
    }
  }
  for (int i = 0; i < size[0]*4 + 1; i++) { printf("-"); }
  if (shading && !number_nodes) {
    printf("     ");
    for (int i = 0; i < size[0]*4 + 1; i++) { printf("-"); }
  }
  printf("\n\n");
}

bool check_solution(Grid* grid, char* known_solution) {
  if (known_solution == NULL || grid == NULL) { printf("Missing grid or solution\n"); return false; }
  int n_nibbles = grid->n_edges / 4 + (grid->n_edges % 4 > 0);
  int solution_bytes[MAXSOLUTION] = {0};
  char solution_string[MAXSOLUTION+1] = {0};
  for (int e = 0; e < grid->n_edges; e++) {
    if (grid->edges[e] == Line) solution_bytes[e/4] += 1 << (e % 4);
  }

  for (int i = 0; i < n_nibbles; i++) sprintf(&solution_string[i], "%X", solution_bytes[i]);

  return strncmp(solution_string, known_solution, n_nibbles) == 0;
}

bool check_partial_solution(Grid* grid, char* known_solution) {
  int n_errors = 0;
  int errors[MAXEDGES] = {0};
  if (known_solution == NULL || grid == NULL) { printf("Missing grid or solution\n"); return false; }
  int n_nibbles = grid->n_edges / 4 + (grid->n_edges % 4 > 0);
  int nibble;
  for (int i = 0; i < n_nibbles; i++) {
    if (known_solution[i] == 0) { printf("Solution string is too short\n"); return false; }
    if ('0' <= known_solution[i] && known_solution[i] <= '9') {
      nibble = known_solution[i] - '0';
    } else if ('A' <= known_solution[i] && known_solution[i] <= 'F') {
      nibble = known_solution[i] - 'A' + 10;
    } else { printf("Incorrect character in solution string\n"); return false; }

    for (int j = 0; j < 4; j++) {
      if (i*4 + j >= grid->n_edges) break;
      if (grid->edges[i*4 + j] != Empty && grid->edges[i*4 + j] != nibble % 2) errors[n_errors++] = i*4 + j;
      nibble >>= 1;
    }
  }

  if (n_errors) printf("Errors at edge indices: ");
  for (int i = 0; i < n_errors; i++) { printf("%d ", errors[i]); }
  if (n_errors) printf("\n\n");
  return n_errors == 0;
}