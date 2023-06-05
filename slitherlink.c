#include <stdio.h>
#include <stdbool.h>

#include "processing.c"

typedef struct Puzzle {
  int size[2];
  int difficulty;
  char* input;
  char* solution;
  bool solved;
} Puzzle;

int n_puzzles = 24;
Puzzle puzzles[] = {
  {{7, 7}, 0, "..33..32.02..3232...22.132223..3.2.....1..1...33.", "4E9393543B564D83F79A264C2EDD", false},
  {{7, 7}, 0, ".223.33...221.2.3..23323...23.223..231.1..3...3.1", "9357E1339DCC9BC42B93964CDED0", false},
  {{7, 7}, 1, "1.3..33.3....11.021.....33.332...1..21..131...3..", "4A96E5F6C8265DAEE79A26093A73", false},
  {{7, 7}, 1, "..2..3.3..3......1.2..13321..0.2..3...1..21..2...", "4E47E99397169CE263E562992E9E", false},
  {{7, 7}, 2, "2233..2..2..3...1..3.....1...2.....20..2.2223.3.1", "93F4D157EA266EA8B5733113C773", false},
  {{7, 7}, 2, "1.322.3.1..2..312..2.3..33.3.1...1......2.233..31", "237ADF074D16CBAE463AB13527A3", false},
  {{10, 10}, 0, ".3.......122.2.21...2..303.21321....1.2.1..2....13213332.323....2..2.331321..1232...32...2..32.1.2..", "BEC49960BC9D9A391D9876A23464677359345F8D92A560B92AC9A7E", false},
  {{10, 10}, 0, "...33323.3.32.0.....31..3..2..2232...2..32..3332..2...211.1.212.....232.332....3..22..22.3.12.2..313", "9D97EC5B6A8957CECA4726595BBB393A8C4C8B2D57C1F2D2E4BED9E", false},
  {{10, 10}, 1, ".1.2.322.13..2......2.2.33.3.3223......3..21...3.33..03.2...2.3.....33.3..121......3...223.23......1", "D96B75685A8994DEC9BC826632B3F723A82EC9EA32AA9DA4D647AE0", false},
  {{10, 10}, 1, "3..33233.3..1.......2222.0.2.3.22232.2.3.2.22.........2....3...23..1...202..2.2.2323231..3..2.3.2221", "BA97A93F687A9D24C8B55556BAE2E65279D3FC8D2267AEA207B77E0", false},
  {{10, 10}, 2, ".3.2.3.222..21.2.11..3.........3..33...3.3......3..3.....2.3..3.21.22.2..12...23232..2.3.23....32...", "2DEC356491E1ECB0B4B6DD83F013DA5D9ECE445D263ABE2C45B77AE", false},
  {{10, 10}, 2, "..3.2..3.332....2.132.2........3...3..3...3..3.3...2.....11..3.33.....2...2.2.1..31.1...223.3...23.1", "D9B2D93F567A9431DCE956C9C3F62B880EABEC496A627342E877E93", false},
  {{20, 20}, 0, "12333.21..2...2.21.3....2..11.21.......2.......2.23222..21.2..20..2..1...122.2.23.3223..0.3.23212..33..323.33.1...1...03.....2.0.13.3.232.2......2.333..2.......22....2....23....2233..3.2.2..2.3....2..2..2112.211132...31312..3..231..31..2..333.23..2.2.1...2..23................221...333..2211..22..22310212...3.32..21...3.1..2222311...1.2..332222..03.3.2...13.331.2....21.2.....32.2..32.232..2222223.1", "4E97A46AC435B9EDAC96959E9AC35BCB455232AD062A6CBA8193B1A3D77ED89822B4151BC3EA4B5ADD8615CA2C453E2E2B57AA795137741198BAC4A17517656CFECAA0F2B252655721713BBB213F8A42AE179A37CE05FC019937588CD5562BBAA24F22E8CDDD9A7E93", false},
  {{20, 20}, 0, ".222222.22.1...........32...3.33..333.33.222......12120..2...22313..22..2.2..22.3212..23.2.23.223...2233...1...12...22..32.12...33.22.222..1..2....2.2....22.222.222..13.2.1..1131..3122...2...2...2.2.23.3.1...222...22...2.12.2.2313.3.32....2.1312.1..3.2.3233.22.1.1..22131.3.1..332333....3.2..2..........3.32112..3..22.....23..32....122201.3321........32.223..32..23...23..0.22..1....13..213.22.2..23.", "DD67AC496A96A9E4A79D57777B5DC43454559EC851E1AE613FEE4F415D9222AAAD555FAB01D42BAA9CDD46E2599344574D8DCC59A23DA426E44B1766E99C916A674D93CB3C489A669A475F447A0D1D6BC92E5AE4AA35F431519DA8A237AD5AC31FC0C436BA7AE97A73", false},
  {{20, 20}, 1, ".3..3..223..2...33....2130....0...0.12.22.312.32..22..2....22...2.2.2.22.20..2..3..1..2..230..2..12.....3222.2..2...32231.2.3......21..2...3..3.....31..3.321...2203213..22...11.3....2...3..30232..2022.1..22..2..21...32.....3.2........33...12.2....1.2....1.1.1.2..22333313.3..2..3.2.....1.1....2.3.2.3....2..3..2.2...1....3..2.221.1.222..1.2322.......3.1.122.3....233.32.1233.3..1.3...1...23.1...1..31", "637EA67EC4997EC88989272AABF8A4F0F455137A27A8AEDC44B4C354BEC15638E1313323534F4F9523A7384293E8140FC9569527E88A3EC3F01B2A59AAADDDD4C454C4455CDA677C9A69A435D245F47B9AED9AAAAA15554555CCE88C92267B4FE64AB351BAAA7AAAA3", false},
  {{20, 20}, 1, ".3.32222.3.3.33.3..22312......1..1.2...1.32.31322..3....221...2.2.2..........1...1....1212.2333332....2......21.12...1.231...23.3.3.211..32....12...1...3...2212..2.3.2..3.3..2.22..2.2322..2.....2.1.1212.2.........2.2.3..21....2..321.222211..3....1.1.312.0.2..32..2.3.3..2...3222..23322..11..1.2.2....2..23.3......2..2...12.1.11.3233.1.132.3..3...223.22.33.....222.2..121...2....1...21.....2...22.3.2.", "97D496B7DC37EACA659862A331FA936E4D1FA3EC93226A6C134DBB3D716512AAA8ABABBB11C6A532A8C97AE4DA8B25555B675C98A2213DA2C34B37BCD59639A8A35B0741DB82A77A9D8E5B111D9464FAED522633A4542EA3CC9BC5E9967A66325D02553A7AAE9AAA7E", false},
  {{20, 20}, 2, "2....2.2.1.2.22.33....2.3...3.32....1.1212..223..2...22.32..1.2..2..22.......22.2..1...33....213.1..1..22...13....31.3...32.....2..32..10..33.3..21..3...3.23.2.2..2.2.1..1....2.1....2..22331..33..2...213..2...2.....3..32....3...11.2..0.1..3..3.232..32...2.2.2..3.2.2.232.3.....2.3.3...12..2.....123..2.2..21....33.2.3.02..3...3.2.2..2.2..2..1.....2..0.21.2......1.3121..232.3..2..3.21....22...22..32.", "9D6A37A9EC997ECA65857AA883BAA254DE25ED5163E89E55EA391B449611A7CD437B2E4D645CC771AE8967E29B44A031595467CD178BB13C31FC3D165185A88ECBA3B2CA25B69AD575923F0907CA1FC2795A536A4FCC6C3D3DC4EC832A8EC15238AC256E7A7A7E9E93", false},
  {{20, 20}, 2, "223333.3.22.3232..2..1...........23...222.1...3...121..22.23..322.2222...2..23..2.22.31..322211.....2.2.....2.22....2.12...3.221...13.3.33.3.2..0...3.32..22.21231....1..3.2..........321.33.2.2.22..2..231....0...........2.2..1.223..22.33.2..........12..211.12.33.13.30..3..2.2..303...3.....1.2.23......2.31.2211....11...2.3..3..232.3..2.323...2.1.1.1......23.2..12.23.2...........33.3.22..32.23.2.321.", "93FC3F4996AEC32DAA8567E8A94CB27417C66369A17A7A6DB1DB01594A86AE5581BD6211BBBE45A8D59AE865CE0B1553EA52D9E8A7957641574C83B995AA777CA993155193D6625AA552B66660DB3F4A9136A8826D8EC3DB2EAA922A2E59551FA0F0F8BE7A77A7A7E0", false},
  {{27, 15}, 0, "...2..3...2........2.2..33.132..32.2.22221..2.0.12..022....213.23...2..2.3.1.2....3.22..2..23220...1..133.12.21..22312...33.....32.0....3..1212...2.0.13.3.12.3....2..323..2.1......1.1.1...1..203..2.232.02..33.2.32..2.2......2...33.3..203....22.02...1222..0..2..2..2......22.........33232..2..132.2.......12.3.2222.22232320.2211.33.3323...223222..2....3.2...12122....12.....12.223..3222..2..2...322.2..3223", "2D96A9AD67E92DBA3FC3AEEE013DA0915982A6758775316ED6C4BB11B1CE4CAC9E82772E9271F05C831D35D6EAC9D51E92221E4D06CB64CAEC64F7EC03EDD9236AA0D476444C2E4CB19AECD555587AD144C4CBAAE4133EDD9727798E5F24555D353A0C4D7AAAAAAE97E9E", false},
  {{27, 15}, 0, ".......2......2.22.1223.2.1....22.3..13..1...1.2.322..212.3222.2.3.223.3232.3.22...2.32.332..222.......321..2.2...2....3..3.....1...2.22.213...3..222.1.2.3.3.3..3121.....32.3..3232.1.2.3...33222..2...1..2..1233.2.......121..3.33323....2..0...32.23.31221..2...2...2...2.333.2.2123.....22......3231....223..21..3.2..121.2..2..2.3.....222.1....1...2.31.31021...022222..321.2.202.23.233...2...222.3.3..2212223", "9D9AD67A63756A46AE43CB05E4ECC2C9B2559585A977A956D2EAB035457E8B656275278ACC17F892BC2B0FCC29636C97ED9817CF4FA44444BB3B492697D55688433DE643CBCAA213F9A6C589DD5C5C21DB8A235DE8B46327E5C829C9131B21DB8C0B0A2D77E9A7A777A7E", false},
  {{27, 15}, 1, "....12...2...3.12..2.2..3..3..1..21...20.0....13231222.1.2.2.....33...3321.1..2...1.2.322321..........2.212.3..23...211....2.........33.1...13.....2232.2.....2....122312122.2.....223.232.1..2..22.....2.12.....3231.122..1.0..122.3..22.2..1..222.3..3.3223.222..21.2.....13.12.2....1..22.2.2...3.23.3.23..1.0.2..3..2.223...2.....323132....2222.1.0..2..33.2.1.2..3....2..2.3.3.312..332....3.12..22.2..3..3222.", "D352356A97E46EC49B6ADA0F834444A3BA851617DDD9AE57EAAA6255555D1B5571AAA8AAAA53B1B355B1544B4DA78AAE8355C1425F455D0DBEC47A02A8A71A3A3A63F2B45F607439B8EC988AA366E9F4C4EA455FC113971769AA2227535A2C355DAA2B4F977AAAA777E9E", false},
  {{27, 15}, 1, "....33.2122..22...3..33.221...1..31...2.32.2.312..1..33..2.221.2..2.....2.22..222.0.1..322.22......3...22.23.2.....13.3..21..211.2..1...3222......2.3.3...3....3..221...1...23130...1.3.201.2.22.2222......212.122111..233.2.12..33....1...3...2..3.2....321...2..3....12.2.3..22..23...2..2....3.3.3..22.333....2...........3.........2.232...22.2.3.23123..2.....3...21.3.311.......11..1.3.22.332.12...3.3..2..1.1", "9DEC35235BD96AEC3764691B174EAC155DF97A165C9A1AAA244FAAA41F645F6506779C18E886F2F056ECC49B9C686376293E10EAA843CC453C07CE5A6D1326DD161D193C2EE21DAEA85955D8EC9D1EDB3BA2D4C07C25AA67789DD9CD8F1D11B3701B19409A77AAA77A7A3", false},
  {{27, 15}, 2, "1.21.12121223...22.32..22.112..3..2...2..1...21.32...2.........2.12..21...23123.2.23.2......3220.....2........221..2.1.2231...31.122.3.21.2.3...2.2....2.....3.......2..313..2.2...3.3..11.33.3.212...2..0.1..1.....2.1.1...2...2.1..3..3.232..0.2.22.22121.32.2....13.21...2.2...222...2..22.1.....20...2..1.2..22...21.2....2...231.32....32..23..1.3....2..3.3.223...3..0.0.2...3222.23...2121..2.3.3223.3..3...2.", "2D6A96A9EC9923F4D9238CBAAE4D253E2398553A872E56E4EBA17A196CC4179783A870E116921F43B137CCE6C83AEC87AE93ACD53098111153E45697E95773E52AB7451F0152D93556A69E511A238CC4C24DA83D6CA4A755ADD9A365175FE474401BAA2DAAAAED977777E", false},
  {{27, 15}, 2, ".2231...2..3...2..2233..23.1..3...33213.22222.1......2.122.22.2...122...1..2333.3.2.2.2..3..22.1..22...2....3..2...2..3.....2......3..3.2.22.....3.3.2.....323.212222.22..3....0......2.2.......23...02.231..1..1..22.3..1...2.332.2..23.32.3.221...2..1.....1.1...........2..31.12.3...23...3.31.2.322..23.1...122...2..1...21..3..1.0.2.22.2.....20.23.2...2.23221.23.32222.3.312111.1.2...23.2.22...23...2.2..22.1", "935375A357A9ADEC376E13FCE2EA4D1119DAA9C569A27579253BCC21D2E436B6E93769D52E5AC4513D33D939989867A84AA361E4AC01F2015EA25B9773C979565892144C056E8AAAACCDD53766CD55D905D13C9A0DE2AC9A8558CD19E5B05FAAAA4D01CFD9AEDDD9A77E0", false},
};

// TODO: add segfault protections everywhere

void summarise() {
  int count = 0;
  printf("Solved:\n");
  for (int i = 0; i < n_puzzles; i++) {
    if (puzzles[i].solved) {
      printf("%dx%d diff %d, ", puzzles[i].size[0], puzzles[i].size[1], puzzles[i].difficulty);
      count++;
      if (count % 12 == 0) printf("\n");
    }
  }
  printf("\b\b | (total = %d)\n\n", count);

  count = 0;
  printf("To do:\n");
  for (int i = 0; i < n_puzzles; i++) {
    if (!puzzles[i].solved) { printf("%dx%d diff %d, ", puzzles[i].size[0], puzzles[i].size[1], puzzles[i].difficulty); count++; }
  }
  printf("\b\b | (total = %d)\n", count);
}

int main() {
  bool finished, valid;
  int edges;
  for (int i = 0; i < n_puzzles; i++) {
    finished = false;
    valid = false;
    Grid grid = {0};
    make_grid(&grid, puzzles[i].input, puzzles[i].size, 3);
    edges = grid.n_edges;
    while (fill_once(&grid, false, &finished, &valid, &edges));
    if (!finished || !(puzzles[i].solved = check_solution(&grid, puzzles[i].solution))) {
      print_grid(&grid, puzzles[i].size, true, false);
      if (finished && valid) printf("Solution doesn't match but only one loop exists and all edges are filled\n");
      check_partial_solution(&grid, puzzles[i].solution);
    }
  }

  summarise();
  return 0;
}