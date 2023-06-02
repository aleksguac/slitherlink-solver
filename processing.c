#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "structures.c"

// TODO: do more variety slitherlinks to figure out which rules can still work and how to modify them
// TODO: try move and see if immediately illegal?
// TODO: split this file into logically-grouped sections
// TODO: add more comments explaining what everything is doing in case i forget

/* HELPER FUNCTIONS*/
#define DEBUG(s) if(debug) printf("%s\n", s)

int flip(int i) { return (i+2) % 4; }

void tally_patch(Patch* patch, Tally* tally) {
  Edge* edge;
  tally->n_lines = 0;
  tally->n_crosses = 0;
  tally->n_empties = 0;
  for (int i = 0; i < MAXNEIGHBOURS; i++) {
    tally->lines[i] = -1;
    tally->crosses[i] = -1;
    tally->empties[i] = -1;
  }
  for (int e = 0; e < patch->n_edges; e++) {
    edge = patch->edges[e];
    if (edge != NULL) {
      if (*edge == Line) {
        tally->lines[tally->n_lines++] = e;
      } else if (*edge == Cross) {
        tally->crosses[tally->n_crosses++] = e;
      } else if (*edge == Empty) {
        tally->empties[tally->n_empties++] = e;
      } else {
        printf("Incorrect edge");
      }
    }
  }
}

void tally_node(Node* node, Tally* tally) {
  tally->n_lines = 0;
  tally->n_crosses = 0;
  tally->n_empties = 0;
  for (int i = 0; i < MAXNEIGHBOURS; i++) {
    tally->lines[i] = -1;
    tally->crosses[i] = -1;
    tally->empties[i] = -1;
  }
  for (int e = 0; e < node->n_edges; e++) {
    Edge* edge = node->edges[e];
    if (edge != NULL) {
      if (*edge == Line) {
        tally->lines[tally->n_lines++] = e;
      } else if (*edge == Cross) {
        tally->crosses[tally->n_crosses++] = e;
      } else if (*edge == Empty) {
        tally->empties[tally->n_empties++] = e;
      } else {
        printf("Incorrect edge");
      }
    }
  }
}

int get_diagonal_from_edges(int edge1, int edge2) {
  int dir = -1;
  if (edge1 == 0) {
    if (edge2 == 1) {
      dir = 1;
    } else if (edge2 == 3) {
      dir = 0;
    }
  } else if (edge1 == 1 && edge2 == 2) {
    dir = 2;
  } else if (edge1 == 2 && edge2 == 3) {
    dir = 3;
  }
  return dir;
}

void get_edges_from_diagonal(int diagonal, int* edge1, int* edge2) {
  switch (diagonal) {
    case 0:
      *edge1 = 3;
      *edge2 = 0;
      break;
    case 1:
      *edge1 = 0;
      *edge2 = 1;
      break;
    case 2:
      *edge1 = 1;
      *edge2 = 2;
      break;
    case 3:
      *edge1 = 2;
      *edge2 = 3;
      break;
    default:
      printf("Unsupported diagonal\n");
  }
}

bool list_contains_node(Node* node, Node** list, int list_len) {
  for (int i = 0; i < list_len; i++) {
    if (node == list[i]) return true;
  }
  return false;
}

/* PER-PATCH FUNCTIONS */
bool fill_zero(Patch* patch) {
  bool changed = false;
  if (!patch->filled && patch->value == 0) {
    for (int e = 0; e < patch->n_edges; e++) {
      *patch->edges[e] = Cross;
      changed = true;
    }
    patch->filled = true;
  }
  return changed;
}

bool adjacent_threes(Patch* patch) {
  Patch* neighbour_patch;
  Tally tally = {0};
  tally_patch(patch, &tally);
  bool changed = false;
  if (patch->value == 3 && tally.n_lines != 3) {
    for (int dir = 0; dir < patch->n_neighbour_patches; dir++) {
      neighbour_patch = patch->neighbour_patches[dir];
      if (neighbour_patch != NULL && neighbour_patch->value == 3) {
        // dir 0 - cross out node 0 direction 3, node 1 direction 1
        // dir 1 - cross out node 1 direction 0, node 2 direction 2
        // dir 2 - cross out node 2 direction 1, node 3 direction 3
        // dir 3 - cross out node 3 direction 2, node 0 direction 0
        Edge* edge1 = patch->neighbour_nodes[dir]->edges[(dir + 3) % 4];
        Edge* edge2 = patch->neighbour_nodes[(dir + 1) % 4]->edges[(dir + 1) % 4];
        if (edge1 != NULL && *edge1 != Cross ) { *edge1 = Cross; changed = true; }
        if (edge2 != NULL && *edge2 != Cross) { *edge2 = Cross; changed = true; }

        if (*patch->edges[dir] != Line) { *patch->edges[dir] = Line; changed = true; }
        if (*patch->edges[flip(dir)] != Line) { *patch->edges[flip(dir)] = Line; changed = true; }
        if (*neighbour_patch->edges[dir] != Line) { *neighbour_patch->edges[dir] = Line; changed = true; }
      }
    }
  }
  return changed;
}

bool corner_threes(Patch* patch) {
  Patch* corner;
  int e1, e2;
  bool changed = false;
  if (patch->value == 3) {
    for (int d = 0; d < patch->n_corner_patches; d++) {
      corner = patch->corner_patches[d];
      if (corner != NULL && corner->value == 3) {
        get_edges_from_diagonal(d, &e1, &e2);
        if (*corner->edges[e1] == Empty) { *corner->edges[e1] = Line; changed = true; }
        if (*corner->edges[e2] == Empty) { *corner->edges[e2] = Line; changed = true; }
        if (*patch->edges[flip(e1)] == Empty) { *patch->edges[flip(e1)] = Line; changed = true; }
        if (*patch->edges[flip(e2)] == Empty) { *patch->edges[flip(e2)] = Line; changed = true; }
      }
    }
  }
  return changed;
}

bool cross_innie_on_two(Patch* patch) {
  bool changed = false;
  Tally tally = {0}, node_tally = {0};
  int e1, e2;
  if (patch->value == 2) {
    tally_patch(patch, &tally);
    for (int e = 0; e < tally.n_crosses; e++) {
      for (int n = 0; n < patch->n_neighbour_nodes; n++) {
        tally_node(patch->neighbour_nodes[n], &node_tally);
        get_edges_from_diagonal(n, &e1, &e2);
        if (node_tally.n_lines == 1 && (node_tally.lines[0] == e1 || node_tally.lines[0] == e2)) {
          e1 = flip(e1);
          e2 = flip(e2);
          if (tally.crosses[e] == e1 && *patch->edges[e2] == Empty) {
            *patch->edges[e2] = Line;
            changed = true;
          } else if (tally.crosses[e] == e2 && *patch->edges[e1] == Empty) {
            *patch->edges[e1] = Line;
            changed = true;
          }
        }
      }
    }
  }
  return false;
}

bool cross_out_opposites_on_two(Patch* patch, int dir, int e1, int e2) {
  bool changed = false;
  if (patch->neighbour_nodes[dir]->edges[e1] != NULL && *patch->neighbour_nodes[dir]->edges[e1] == Empty) { *patch->neighbour_nodes[dir]->edges[e1] = Cross; changed = true; }
  if (patch->neighbour_nodes[dir]->edges[e2] != NULL && *patch->neighbour_nodes[dir]->edges[e2] == Empty) { *patch->neighbour_nodes[dir]->edges[e2] = Cross; changed = true; }
  if (patch->neighbour_nodes[flip(dir)]->edges[flip(e1)] != NULL && *patch->neighbour_nodes[flip(dir)]->edges[flip(e1)] == Empty) { *patch->neighbour_nodes[flip(dir)]->edges[flip(e1)] = Cross; changed = true; }
  if (patch->neighbour_nodes[flip(dir)]->edges[flip(e2)] != NULL && *patch->neighbour_nodes[flip(dir)]->edges[flip(e2)] == Empty) { *patch->neighbour_nodes[flip(dir)]->edges[flip(e2)] = Cross; changed = true; }
  return changed;
}

bool opposite_innies_on_two(Patch* patch) {
  bool changed = false;
  Tally tally = {0}, node1_tally = {0}, node2_tally = {0};
  tally_patch(patch, &tally);
  if (patch->value == 2 && tally.n_empties == 4) {
    int e1, e2;
    tally_node(patch->neighbour_nodes[0], &node1_tally);
    tally_node(patch->neighbour_nodes[2], &node2_tally);
    if (node1_tally.n_lines == 1 && node2_tally.n_lines == 1) {
      get_edges_from_diagonal(0, &e1, &e2);
      changed = cross_out_opposites_on_two(patch, 0, e1, e2);
    }
    tally_node(patch->neighbour_nodes[1], &node1_tally);
    tally_node(patch->neighbour_nodes[3], &node2_tally);
    if (node1_tally.n_lines == 1 && node2_tally.n_lines == 1) {
      get_edges_from_diagonal(1, &e1, &e2);
      changed = cross_out_opposites_on_two(patch, 1, e1, e2);
    }
  }
  return changed;
}

bool either_ors(Patch* patch, int dir) {
  bool changed = false;
  int e1, e2;
  get_edges_from_diagonal(dir, &e1, &e2);

  if (*patch->edges[flip(e1)] == Empty && *patch->edges[flip(e2)] != Empty) {
    *patch->edges[flip(e1)] = *patch->edges[flip(e2)] == Cross ? Line : Cross;
    changed = true;
  }
  if (*patch->edges[flip(e2)] == Empty && *patch->edges[flip(e1)] != Empty) {
    *patch->edges[flip(e2)] = *patch->edges[flip(e1)] == Cross ? Line : Cross;
    changed = true;
  }

  if (patch->value == 3 || patch->value == 1) {
    if (*patch->edges[e1] == Empty) { *patch->edges[e1] = patch->value == 3 ? Line : Cross; changed = true; }
    if (*patch->edges[e2] == Empty) { *patch->edges[e2] = patch->value == 3 ? Line : Cross; changed = true; }
  } else if (patch->value == 2) {
    if (*patch->edges[e1] == Empty && *patch->edges[e2] != Empty) { *patch->edges[e1] = *patch->edges[e2] == Line ? Cross: Line; changed = true; }
    if (*patch->edges[e2] == Empty && *patch->edges[e1] != Empty) { *patch->edges[e2] = *patch->edges[e1] == Line ? Cross: Line; changed = true; }

    if (patch->neighbour_nodes[dir]->edges[e1] != NULL && *patch->neighbour_nodes[dir]->edges[e1] == Empty) {
      if (patch->neighbour_nodes[dir]->edges[e2] == NULL || *patch->neighbour_nodes[dir]->edges[e2] == Cross) { *patch->neighbour_nodes[dir]->edges[e1] = Line; changed = true; }
      else if (patch->neighbour_nodes[dir]->edges[e2] != NULL && *patch->neighbour_nodes[dir]->edges[e2] == Line) { *patch->neighbour_nodes[dir]->edges[e1] = Cross; changed = true; }
    }
    if (patch->neighbour_nodes[dir]->edges[e2] != NULL && *patch->neighbour_nodes[dir]->edges[e2] == Empty) {
      if (patch->neighbour_nodes[dir]->edges[e1] == NULL || *patch->neighbour_nodes[dir]->edges[e1] == Cross) { *patch->neighbour_nodes[dir]->edges[e2] = Line; changed = true; }
      else if (patch->neighbour_nodes[dir]->edges[e1] != NULL && *patch->neighbour_nodes[dir]->edges[e1] == Line) { *patch->neighbour_nodes[dir]->edges[e2] = Cross; changed = true; }
    }

    if (patch->corner_patches[dir] != NULL) {
      changed |= either_ors(patch->corner_patches[dir], dir);
    }

  }

  return changed;
}

bool either_ors_from_patch(Patch* patch) {
  bool changed = false;
  Tally tally = {0};
  int dir, e1, e2;
  tally_patch(patch, &tally);
  if ((patch->value > 0 && patch->value - tally.n_lines == 1 && tally.n_empties == 2)) {
    e1 = tally.empties[0];
    e2 = tally.empties[1];
    dir = get_diagonal_from_edges(e1, e2);
    if (dir != -1) {
      if (patch->neighbour_nodes[dir]->edges[e1] != NULL && *patch->neighbour_nodes[dir]->edges[e1] == Empty) {
        if (patch->neighbour_nodes[dir]->edges[e2] == NULL || *patch->neighbour_nodes[dir]->edges[e2] == Cross) { *patch->neighbour_nodes[dir]->edges[e1] = Line; changed = true; }
        else if (patch->neighbour_nodes[dir]->edges[e2] != NULL && *patch->neighbour_nodes[dir]->edges[e2] == Line) { *patch->neighbour_nodes[dir]->edges[e1] = Cross; changed = true; }
      }
      if (patch->neighbour_nodes[dir]->edges[e2] != NULL && *patch->neighbour_nodes[dir]->edges[e2] == Empty) {
        if (patch->neighbour_nodes[dir]->edges[e1] == NULL || *patch->neighbour_nodes[dir]->edges[e1] == Cross) { *patch->neighbour_nodes[dir]->edges[e2] = Line; changed = true; }
        else if (patch->neighbour_nodes[dir]->edges[e1] != NULL && *patch->neighbour_nodes[dir]->edges[e1] == Line) { *patch->neighbour_nodes[dir]->edges[e2] = Cross; changed = true; }
      }

      if (patch->corner_patches[dir] != NULL) {
        changed |= either_ors(patch->corner_patches[dir], dir);
      }
    }
  }
  return changed;
}

bool corner_on_a_three(Patch* patch) {
  bool changed = false;
  Tally tally = {0};
  tally_patch(patch, &tally);
  int e1, e2;

  if (patch->value == 3 && tally.n_lines != 3) {
    for (int n = 0; n < patch->n_neighbour_nodes; n++) {
      Node* node = patch->neighbour_nodes[n];
      get_edges_from_diagonal(n, &e1, &e2);
      if ((node->edges[e1] != NULL && *node->edges[e1] == Line) ||
          (node->edges[e2] != NULL && *node->edges[e2] == Line)) {
        if (*patch->edges[flip(e1)] == Empty) { *patch->edges[flip(e1)] = Line; changed = true; }
        if (*patch->edges[flip(e2)] == Empty) { *patch->edges[flip(e2)] = Line; changed = true; }
        if (node->edges[e1] != NULL && *node->edges[e1] == Line && node->edges[e2] != NULL && *node->edges[e2] == Empty) { *node->edges[e2] = Cross; changed = true; }
        if (node->edges[e2] != NULL && *node->edges[e2] == Line && node->edges[e1] != NULL && *node->edges[e1] == Empty) { *node->edges[e1] = Cross; changed = true; }
      }
    }
  }
  
  return changed;
}

/* PER-NODE FUNCTIONS */
bool either_ors_from_node(Node* node) {
  bool changed = false;
  Tally tally = {0};
  tally_node(node, &tally);
  if (tally.n_lines == 1 && tally.n_empties == 2) {
    int dir = get_diagonal_from_edges(tally.empties[0], tally.empties[1]);
    if (dir != -1) {
      changed |= either_ors(node->neighbour_patches[dir], dir);
    }
  }
  return changed;
}

bool bound_at_corner(Node* node) {
  bool changed = false;
  Tally tally = {0};
  tally_node(node, &tally);
  if (tally.n_empties == 2 && tally.n_lines == 0) {
    int dir = get_diagonal_from_edges(tally.empties[0], tally.empties[1]);
    if (dir != -1 && node->neighbour_patches[dir]->value != None) {
      int value = node->neighbour_patches[dir]->value;
      if (value == 3 || value == 1) {
        *node->edges[tally.empties[0]] = value == 3 ? Line : Cross;
        *node->edges[tally.empties[1]] = value == 3 ? Line : Cross;
        changed = true;
      } else if (value == 2) {
        for (int i = 0; i < 2; i++) {
          Edge* edge1 = node->neighbour_nodes[tally.empties[i]]->edges[tally.empties[i]];
          Edge* edge2 = node->neighbour_nodes[tally.empties[i]]->edges[flip(tally.empties[i^1])];
          if (edge1 != NULL && *edge1 == Empty && (edge2 == NULL || *edge2 == Cross)) { *edge1 = Line; changed = true; }
          if (edge2 != NULL && *edge2 == Empty && (edge1 == NULL || *edge1 == Cross)) { *edge2 = Line; changed = true; }
        }
        if (node->neighbour_patches[dir] != NULL && node->neighbour_patches[dir]->corner_patches[dir] != NULL) {
          Patch* diag_patch = node->neighbour_patches[dir]->corner_patches[dir];
          while (diag_patch->value == 2 && diag_patch->corner_patches[dir] != NULL) diag_patch = diag_patch->corner_patches[dir];
          if (diag_patch->value == 1 || diag_patch->value == 3) {
            int e1 = flip(tally.empties[0]);
            int e2 = flip(tally.empties[1]);
            if (*node->edges[tally.empties[0]] == Empty && diag_patch->value == 3) { *node->edges[tally.empties[0]] = Line; changed = true; }
            if (*node->edges[tally.empties[1]] == Empty && diag_patch->value == 3) { *node->edges[tally.empties[1]] = Line; changed = true; }
            if (*diag_patch->edges[e1] == Empty) { *diag_patch->edges[e1] = diag_patch->value == 3 ? Line : Cross; changed = true; }
            if (*diag_patch->edges[e2] == Empty) { *diag_patch->edges[e2] = diag_patch->value == 3 ? Line : Cross; changed = true; }
          }
        }
      }
    }
  }
  return changed;
}

/* FINAL GRID-WIDE CLEANUP FUNCTIONS */
bool check_loops(Grid* grid, bool* finished) {
  bool changed = false;
  *finished = false;

  Edge* seen_edges[MAXEDGES] = {0};
  int n_seen_edges = 0;
  Node* seen_nodes[MAXNODES] = {0};
  int n_seen_nodes = 0;
  Tally tally = {0};

  Edge* loop_closers[MAXEDGES] = {0};
  int n_loop_closers = 0;
  int n_open_loops = 0;
  int n_closed_loops = 0; // TODO: use this to check for correctness

  Node* node;
  int n_empty_edges = 0;
  for (int e = 0; e < grid->n_edges; e++) { if (grid->edges[e] == Empty) n_empty_edges++; }

  // count open loops and store any loop closers
  for (int n = 0; n < grid->n_nodes; n++) {
    node = &grid->nodes[n];
    if (!list_contains_node(node, seen_nodes, n_seen_nodes)) {
      tally_node(node, &tally);
      if (tally.n_lines == 1) {
        seen_nodes[n_seen_nodes++] = node;
        Node* prev = node;
        Node* tmp = node;
        Node* next = node;
        do {
          tmp = next;
          next = next->neighbour_nodes[tally.lines[0]] == prev ? next->neighbour_nodes[tally.lines[1]] : next->neighbour_nodes[tally.lines[0]];
          prev = tmp;
          seen_nodes[n_seen_nodes++] = next;
          tally_node(next, &tally);
        } while (tally.n_lines != 1);
        n_open_loops++;
        if (list_contains_node(next, node->neighbour_nodes, node->n_neighbour_nodes)) {
          int dir = -1;
          for (int i = 0; i < node->n_neighbour_nodes; i++) {
            if (node->neighbour_nodes[i] == next) {
              dir = i;
              break;
            }
          }
          if (*node->edges[dir] == Empty) {
            loop_closers[n_loop_closers++] = node->edges[dir];
          }
        }
      }
    }
  }

  // cross out loop closers if not the final one, put line if final
  if (n_open_loops > 1 && n_loop_closers > 0) {
    for (int e = 0; e < n_loop_closers; e++) {
      *loop_closers[e] = Cross;
    }
    changed = true;
  } else if (n_open_loops == 1 && n_empty_edges == 1 && n_loop_closers == 1) {
    *loop_closers[0] = Line;
    changed = true;
  }

  // count closed loops
  for (int n = 0; n < grid->n_nodes; n++) {
    node = &grid->nodes[n];
    if (!list_contains_node(node, seen_nodes, n_seen_nodes)) {
      seen_nodes[n_seen_nodes++] = node;
      tally_node(node, &tally);
      if (tally.n_lines == 2) {
        Node* prev = node;
        Node* tmp = node;
        Node* next = node;
        do {
          tmp = next;
          next = next->neighbour_nodes[tally.lines[0]] == prev ? next->neighbour_nodes[tally.lines[1]] : next->neighbour_nodes[tally.lines[0]];
          prev = tmp;
          seen_nodes[n_seen_nodes++] = next;
          tally_node(next, &tally);
        } while (next != node);
        n_closed_loops++;
      }
    }
  }

  if (!changed && n_closed_loops == 1 && n_empty_edges == 0) *finished = true;

  return changed;
}

bool shade(Grid* grid) {
  bool changed = false;
  for (int p = 0; p < grid->n_patches; p++) {
    Patch* patch = &grid->patches[p];
    for (int i = 0; i < patch->n_neighbour_patches; i++) {
      if (*patch->edges[i] != Empty) {
        // shade if neighbour is shaded or at a border
        if (patch->neighbour_patches[i] == NULL || patch->neighbour_patches[i]->colour == Out) {
          patch->colour = *patch->edges[i] == Line ? In : Out;
        } else if (patch->neighbour_patches[i]->colour == In) {
          patch->colour = *patch->edges[i] == Line ? Out : In;
        }
      } else if (patch->colour != Unshaded && patch->neighbour_patches[i] != NULL && patch->neighbour_patches[i]->colour != Unshaded) {
        // fill edge if two neighbours are shaded
        *patch->edges[i] = patch->colour == patch->neighbour_patches[i]->colour ? Cross : Line;
        changed = true;
      } else if (patch->colour != Unshaded && patch->neighbour_patches[i] == NULL) {
        // fill edge if on boundary and current patch is shaded
        *patch->edges[i] = patch->colour == In ? Line : Cross;
        changed = true;
      }
    }
  }
  return changed;
}

bool clean_up(Grid* grid) {
  bool changed = false;
  Tally tally = {0};
  for (int p = 0; p < grid->n_patches; p++) {
    Patch* patch = &grid->patches[p];
    tally_patch(patch, &tally);
    // cross out remaining edges if value == line tally
    if (patch->value != None && patch->value == tally.n_lines && tally.n_empties != 0) {
      for (int i = 0; i < tally.n_empties; i++) {
        *patch->edges[tally.empties[i]] = Cross;
        changed = true;
      }
    }
    // fill remaining lines if value == line tally + empty tally
    else if (patch->value != None && patch->value == tally.n_empties + tally.n_lines) {
      for (int i = 0; i < tally.n_empties; i++) {
        *patch->edges[tally.empties[i]] = Line;
        changed = true;
      }
    }
  }

  for (int n = 0; n < grid->n_nodes; n++) {
    Node* node = &grid->nodes[n];
    tally_node(node, &tally);
    // cross out remaining edges when node has two filled edges
    if (tally.n_lines == 2 && tally.n_empties != 0) {
      for (int i = 0; i < tally.n_empties; i++) {
        *node->edges[tally.empties[i]] = Cross;
        changed = true;
      }
    }
    // cross or fill line when only one edge is unfilled
    else if (tally.n_empties == 1) { *node->edges[tally.empties[0]] = tally.n_lines == 1; changed = true; }
  }
  return changed;
}

/* ONE ITERATION OF ALL FUNCTIONS*/
bool fill_once(Grid* grid, bool debug, bool* finished) {
  bool changed = false;
  for (int p = 0; p < grid->n_patches; p++) {
    Patch* patch = &grid->patches[p];
    // TODO: have a tally of changes per rule
    DEBUG("fill_zero");
    changed |= fill_zero(patch);
    DEBUG("adjacent_threes");
    changed |= adjacent_threes(patch);
    DEBUG("corner_threes");
    changed |= corner_threes(patch);
    DEBUG("corner_on_a_three");
    changed |= corner_on_a_three(patch);
    DEBUG("either_ors_from_patch");
    changed |= either_ors_from_patch(patch);
    DEBUG("cross_innie_on_two");
    changed |= cross_innie_on_two(patch);
    DEBUG("opposite_innies_on_two");
    changed |= opposite_innies_on_two(patch);
  }
  
  for (int n = 0; n < grid->n_nodes; n++) {
    Node* node = &grid->nodes[n];
    DEBUG("bound_at_corner");
    changed |= bound_at_corner(node);
    DEBUG("either_ors_from_node");
    changed |= either_ors_from_node(node);
  }

  DEBUG("clean_up");
  changed |= clean_up(grid);
  DEBUG("check_loops");
  changed |= check_loops(grid, finished);
  DEBUG("shade");
  changed |= shade(grid);
  return changed;
}