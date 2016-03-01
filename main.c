#include <stdio.h>
#include <string.h>
#include "edu/humboldt/wbi/graph.h"
#include "edu/humboldt/wbi/index.h"

int compute_path_ngrams_node(TGraph *g, TArray *array, int array_index, int vertex_index, int n_counter, int n) {
    TVertex *v = graph_get_vertex(g, vertex_index);

    ((int *) array_get(array, array_index))[n_counter - 1] = v->id;

    if (v->outdegree == 0) { // we reached a sink
        return n_counter == 1;  // return, whether we completed an n-gram here or prune
    }
    else if (n_counter > 1) {   // we have not completed the current n-gram yet
        int ngram_count = 0;
        TIterator *it = vertex_get_successors(v);

        while (iterator_has_next(it)) {
            int suc_index = *(int *) iterator_next(it);

            for (int i = n_counter - 1; i < n; ++i) {
                ((int *) array_get(array, array_index + ngram_count))[i] = ((int *) array_get(array, array_index))[i];
            }

            ngram_count += compute_path_ngrams_node(g, array, array_index + ngram_count, suc_index, n_counter - 1, n);
        }

        return ngram_count;
    }
    else {  // we completed an n-gram somewhere within the graph
        return 1;
        // take 2nd component and compute overlapping n-gram(s)
        //return compute_path_ngrams_node(g, array, array_index + 1, current_ngram.values[n - 2], n, n) + 1;
    }
}

TArray *compute_path_ngrams(TGraph *g, int n) {
    //unsigned int edge_count = graph_edge_count(g);
    unsigned int ngram_count = 0;
    TArray *ngrams = malloc(sizeof(TArray));

    array_init(ngrams, g->node_count * g->node_count * g->node_count / 4,
               sizeof(int) * n); // max. possible amount of n-grams = (n^3 - n)/6

    for (int i = 0; i < g->node_count; ++i) {
        TVertex *v = graph_get_vertex(g, i);

        if (v->outdegree > 0) {
            ngram_count += compute_path_ngrams_node(g, ngrams, ngram_count, i, n, n);
        }
    }

    array_subarray(ngrams, 0, ngram_count);

    return ngrams;
}

unsigned int compute_neighbourhood_ngram_count(TGraph *g, int n) {
    unsigned int ngram_count = 0;

    for (int i = 0; i < g->node_count; ++i) {
        TVertex *v = graph_get_vertex(g, i);

        if (v->indegree > 0 && v->outdegree >= n - 2) {
            ngram_count += v->indegree * (v->outdegree - n + 3);
        }
    }

    return ngram_count;
}

static inline void swap(int *a, int *b) {
	int t = *a;
	*a = *b;
	*b = t;
}

static inline void sort(int* array, int length) {
	// TODO: quicksort please!!
	for (size_t i = 0; i < length; i++) {
		for (size_t j = 0; j < length - i - 1; j++) {
			if(array[j] > array[j + 1]) {
				swap(array + j, array + j + 1);
			}
		}
	}
}

int compute_node_neighbourhood_ngrams(TGraph *g, TVertex *v, TNGram *ngrams, int n) {
    int pred_ngrams = v->outdegree - n + 3; // how many n-grams per predecessor
	int count = 0;

	// sort successors first
	sort(v->neighbours, v->outdegree);

    for (int j = 1; j <= v->indegree; ++j) {
        for (int i = 0; i < pred_ngrams; ++i) {
            int *ngram = (int *) ngrams + (count++) * n;
            int pred_index = v->neighbours[g->node_count - j];

            *ngram = graph_get_vertex(g, pred_index)->id;  // predecessor id
            *(ngram + 1) = v->id; // vertex id

            for (int k = 2; k < n; ++k) {
                int succ_index = v->neighbours[i + k - 2];  // i+k-2 ??

                *(ngram + k) = graph_get_vertex(g, succ_index)->id; // successor id
            }
        }
    }

    return v->indegree * pred_ngrams;
}

TArray *compute_neighbourhood_ngrams(TGraph *g, int n) {
    unsigned int ngram_count = compute_neighbourhood_ngram_count(g, n);

    TArray *ngrams = malloc(sizeof(TArray));
    array_init(ngrams, ngram_count, sizeof(int) * n);

    int offset = 0;

    for (int i = 0; i < g->node_count; ++i) {
        TVertex *v = graph_get_vertex(g, i);

        if (v->indegree > 0 && v->outdegree >= n - 2) {
            offset += compute_node_neighbourhood_ngrams(g, v, array_get(ngrams, offset), n);
        }
    }

    return ngrams;
}

/*int main(int argc, char* argv[]) {
    TIndex index;
    index_init(&index, argv[1]);

    int ids[] = {
            10, 100, 1000, 1001, 1002, 1003, 1004, 1005, 1009, 101, 1012, 1013, 1014, 1015, 1016, 1017, 1018, 1019, 1020, 1028, 103, 1031, 1032, 1033, 1036, 1037, 1049, 1051, 1052, 1054, 1055, 1056, 1057, 1058, 1059, 106, 1060, 1061, 1065, 1076, 1077, 1086, 1088, 109, 1097, 1099, 1103, 1104, 1105, 1107, 111, 1110, 1118, 1120, 1123, 1129, 113, 1132, 1133, 114, 1146, 115, 1151, 1152, 1157, 116, 1165, 1166, 1167, 1169, 117, 1170, 1172, 1173, 1177, 1178, 1179, 118, 1180, 1181, 1182, 1183, 1184, 1185, 1186, 1188, 1189, 119, 1193, 1197, 1198, 12, 120, 1200, 1201, 1202, 1203, 1206, 1209, 1210, 1213, 1214, 1215, 1216, 1217, 1218, 1222, 1223, 1224, 1225, 1226, 1237, 1238, 1239, 124, 1246, 1254, 1255, 1256, 1258, 126, 1260, 1262, 1263, 1264, 1283, 1285, 1286, 1287, 1288, 1289, 1294, 13, 1318, 1320, 133, 1346, 1347, 1353, 1360, 1363, 1364, 1365, 1366, 1367, 1368, 1369, 1370, 1371, 1372, 1373, 1374, 1375, 1376, 1377, 1378, 138, 1382, 1383, 1384, 1387, 1388, 1389, 1390, 1391, 1392, 1393, 1394, 1395, 1396, 1397, 1398, 1399, 14, 140, 1400, 1401, 1404, 1405, 1406, 1407, 1408, 1409, 1410, 1411, 1412, 1413, 1414, 1415, 1417, 1418, 1419, 1420, 1421, 1422, 1423, 1424, 1426, 1427, 1428, 1430, 1431, 1432, 1433, 1434, 1435, 144, 1450, 1451, 1452, 1454, 1455, 1456, 1457, 146, 1466, 1467, 147, 1470, 1471, 1474, 1477, 1478, 148, 1480, 1483, 1484, 1485, 1486, 149, 1491, 1492, 1493, 1494, 1498, 15, 150, 1508, 151, 1510, 1512, 1513, 1516, 1517, 152, 1525, 1526, 1527, 1528, 1530, 154, 1556, 157, 1571, 1578, 1579, 158, 1580, 1581, 1582, 1583, 1584, 1585, 1586, 1587, 1588, 1589, 159, 1590, 1591, 1593, 1594, 1595, 1596, 1597, 1598, 1599, 16, 1600, 1601, 1602, 1603, 1604, 1605, 1606, 1607, 1608, 161, 1610, 1611, 1612, 1613, 1614, 1615, 1616, 1617, 1618, 1619, 162, 1620, 1621, 1622, 1623, 1624, 1625, 1626, 1627, 1628, 1629, 1630, 1631, 1632, 1633, 1634, 1635, 1636, 1637, 1638, 1639, 164, 1640, 1641, 1642, 1643, 1644, 1645, 1648, 1649, 165, 1651, 1652, 1653, 1661, 1662, 1663, 167, 1670, 1671, 1672, 1675, 1676, 1678, 1679, 168, 1680, 1682, 1683, 1684, 1685, 1686, 1687, 1688, 1689, 1694, 1696, 1697, 1698, 1699, 17, 170, 1700, 1701, 1702, 1703, 1705, 1709, 1710, 1711, 1712, 1719, 1720, 1721, 1725, 1726, 1727, 1728, 173, 1730, 1733, 1735, 1738, 174, 1743, 175, 1750, 1753, 1754, 1756, 1757, 1764, 1765, 1767, 1768, 1772, 1773, 1776, 1778, 178, 1781, 1782, 1783, 1784, 1786, 1788, 1792, 1794, 1799, 18, 1803, 1806, 181, 182, 1826, 1833, 1834, 1835, 1842, 1846, 185, 1856, 1857, 187, 19, 1903, 1905, 1906, 1911, 1913, 1914, 192, 1924, 1925, 1928, 1932, 1953, 1957, 197, 1975, 1976, 1977, 198, 1981, 1984, 1988, 1989, 199, 1990, 1992, 1994, 1996, 20, 200, 2000, 2004, 2007, 201, 2012, 2013, 202, 2020, 2021, 2024, 2025, 2026, 2027, 2029, 203, 2030, 204, 205, 2058, 206, 2066, 2067, 207, 2079, 208, 2083, 2088, 2089, 209, 2090, 2091, 2092, 2093, 21, 210, 2105, 2106, 211, 2118, 212, 2120, 2121, 2122, 2124, 2125, 2126, 2127, 213, 2131, 2139, 214, 215, 2152, 2153, 2154, 2155, 2156, 2157, 2158, 2159, 216, 2160, 2167, 2169, 217, 2176, 2177, 2178, 218, 219, 22, 220, 2203, 2204, 221, 2212, 2213, 2214, 2215, 2217, 2218, 2219, 222, 2220, 2221, 2222, 2223, 2224, 2225, 2226, 2228, 2229, 223, 2230, 2231, 2236, 2237, 2238, 224, 225, 2253, 2254, 2255, 2256, 2258, 2259, 2262, 2264, 2265, 2266, 2267, 2268, 227, 2270, 2271, 2272, 2274, 2275, 2276, 2279, 228, 2280, 2284, 2285, 2287, 2288, 2289, 229, 2290, 2293, 2294, 2299, 23, 230, 2302, 2303, 231, 2313, 2314, 2315, 2316, 2317, 232, 233, 2330, 2331, 2334, 2336, 2338, 2339, 2340, 2341, 2343, 2344, 2345, 2346, 2347, 2348, 2349, 235, 2354, 2359, 236, 2360, 237, 2370, 2371, 2372, 2373, 2375, 2378, 2379, 238, 2380, 2381, 2382, 2383, 2384, 2385, 2386, 2387, 2389, 239, 2390, 2391, 2393, 2394, 2395, 2396, 2397, 2399, 2400, 2401, 2409, 242, 244, 2442, 2445, 2446, 2447, 245, 2455, 2458, 246, 2460, 2461, 2462, 2463, 2464, 2466, 2468, 2469, 2470, 2471, 2472, 2473, 2474, 2475, 2479, 248, 2480, 2481, 2482, 2486, 2488, 249, 2490, 2491, 2492, 2493, 2494, 2498, 25, 250, 2500, 251, 2515, 2516, 2517, 252, 2524, 253, 254, 2545, 2548, 255, 2559, 256, 2560, 2562, 2563, 257, 2577, 2578, 2579, 258, 2581, 259, 2592, 2593, 2594, 26, 260, 2601, 2603, 2613, 2614, 2615, 262, 2620, 2625, 2626, 2627, 263, 2637, 2639, 264, 2644, 2646, 2648, 2649, 265, 2653, 2658, 2659, 266, 2663, 2665, 267, 2673, 2676, 2677, 2679, 268, 2680, 2681, 2686, 269, 270, 271, 2713, 2714, 2717, 272, 2721, 2724, 2725, 2726, 2727, 2728, 273, 2734, 274, 2742, 2743, 2744, 2746, 2749, 275, 2750, 2753, 2754, 2755, 2756, 2757, 2758, 2759, 276, 2763, 2768, 277, 2770, 2779, 278, 2781, 2783, 2784, 2786, 2787, 2788, 2789, 279, 2790, 2791, 2792, 2793, 2794, 2795, 2798, 2799, 280, 2800, 2801, 2803, 2805, 2806, 281, 2819, 282, 2820, 2821, 2822, 2824, 2825, 2828, 2829, 283, 2834, 2836, 2837, 284, 2843, 2846, 2847, 2848, 2849, 285, 2850, 2851, 2853, 2854, 2855, 2856, 2857, 2858, 2859, 286, 2860, 2862, 2863, 2864, 2865, 2866, 2867, 2868, 2869, 287, 2870, 2873, 2874, 2876, 2877, 288, 2880, 2881, 2882, 2883, 2884, 2885, 2886, 2888, 2889, 289, 2890, 2891, 2899, 290, 2900, 2902, 291, 2914, 2917, 2918, 2919, 292, 2920, 2921, 2922, 2925, 293, 2931, 294, 2940, 2941, 2942, 295, 296, 2969, 297, 2971, 2972, 2973, 298, 2980, 2981, 2982, 2983, 2986, 299, 2992, 2997, 2998, 2999, 300, 3001, 3006, 301, 3012, 302, 303, 3032, 3039, 304, 3040, 3044, 3046, 3047, 3048, 3049, 305, 3050, 3053, 3054, 3055, 3056, 3057, 3058, 3059, 306, 3060, 3061, 3066, 3067, 3068, 3069, 307, 3076, 3078, 308, 3083, 3084, 3085, 3086, 309, 310, 3105, 3106, 3107, 3108, 3109, 311, 3110, 3112, 3113, 3114, 3115, 3116, 3119, 312, 3120, 3121, 3124, 3126, 3127, 3129, 313, 3130, 3133, 3134, 3136, 3137, 314, 3144, 3146, 3147, 3148, 3149, 315, 3150, 3152, 3158, 316, 3165, 3167, 3168, 317, 3171, 3178, 318, 319, 32, 320, 3201, 3204, 3205, 321, 3212, 323, 3246, 3249, 325, 3250, 3251, 3252, 3253, 3254, 3255, 3258, 3259, 326, 3262, 3268, 3269, 327, 3277, 3278, 328, 3282, 329, 3293, 3294, 330, 3300, 3301, 3306, 3308, 3309, 331, 3310, 3311, 333, 3336, 335, 3351, 3352, 3355, 336, 3360, 3362, 3364, 3369, 3370, 3373, 3374, 3376, 3384, 3396, 3397, 3398, 3399, 34, 3400, 3401, 3404, 3417, 3418, 3423, 3424, 3425, 3426, 3427, 3428, 3432, 3439, 3443, 345, 3466, 3467, 3468, 3469, 3476, 3477, 3478, 3479, 3480, 3481, 3482, 3483, 3484, 3486, 3487, 349, 3490, 3491, 3492, 3493, 3494, 3497, 3498, 35, 350, 3510, 3511, 3512, 3513, 3514, 3515, 3516, 3517, 3518, 3519, 3520, 3521, 3522, 354, 3547, 3551, 3552, 3553, 3554, 3555, 3556, 3557, 3558, 3559, 356, 3560, 3562, 3563, 3564, 3566, 3567, 3569, 3570, 3571, 3578, 359, 36, 3600, 3602, 3606, 3607, 3608, 3609, 3610, 3611, 3625, 3626, 3628, 364, 365, 366, 368, 376, 379, 38, 380, 381, 384, 385, 386, 388, 389, 39, 391, 392, 394, 395, 397, 398, 399, 4, 40, 400, 401, 402, 403, 404, 405, 407, 408, 409, 41, 410, 411, 412, 413, 414, 415, 416, 417, 418, 42, 425, 426, 427, 428, 429, 43, 430, 431, 432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447, 448, 449, 45, 450, 451, 452, 453, 454, 455, 456, 457, 458, 459, 46, 460, 461, 462, 463, 464, 465, 466, 467, 468, 469, 47, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479, 48, 480, 481, 482, 483, 484, 485, 486, 487, 488, 489, 490, 491, 492, 493, 494, 495, 496, 497, 498, 499, 5, 50, 501, 502, 503, 506, 51, 510, 511, 514, 518, 519, 52, 520, 522, 523, 524, 525, 528, 530, 531, 532, 533, 545, 551, 552, 553, 554, 555, 556, 557, 558, 559, 562, 563, 564, 565, 566, 567, 581, 59, 592, 598, 599, 6, 60, 600, 603, 606, 610, 611, 612, 62, 630, 631, 634, 635, 639, 640, 642, 643, 644, 647, 648, 651, 652, 653, 654, 655, 656, 659, 66, 660, 661, 664, 67, 688, 695, 7, 70, 701, 703, 705, 706, 710, 72, 727, 728, 730, 737, 738, 739, 74, 740, 746, 747, 748, 749, 750, 751, 752, 754, 755, 757, 76, 761, 763, 765, 767, 77, 772, 773, 774, 778, 779, 780, 781, 782, 783, 784, 786, 788, 789, 79, 790, 799, 80, 800, 801, 802, 803, 804, 805, 806, 807, 808, 809, 811, 812, 813, 814, 820, 821, 822, 823, 824, 825, 826, 827, 828, 829, 830, 831, 832, 833, 834, 840, 841, 843, 845, 85, 856, 857, 884, 892, 90, 900, 902, 906, 907, 908, 909, 911, 912, 913, 914, 916, 931, 932, 933, 936, 939, 940, 941, 942, 943, 945, 95, 950, 951, 954, 956, 957, 958, 960, 963, 964, 965, 966, 967, 97, 977, 98, 980, 982, 984, 985, 986, 988, 989, 990, 991, 992, 994, 996, 997, 998, 999
    };

    for (int i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i) {
        TGraph* g = index_get_entry(&index, ids[i]);
        printf("%d\t%d\t%d\r\n", ids[i], graph_vertex_count(g), graph_edge_count(g));
    }

    return 0;
}*/

/**
 * Removes a trailing path delimiter from the string.
 * @param path The string to remove the trailing path delimiter from.
 */
char *strip_path_delimiter(char *path) {
    size_t len = strlen(path);

    if (path[len - 1] == '/') {
        path[len - 1] = '\0';
    }

    return path;
}

char *build_index_path(char *base, char *index, char *ext) {
    size_t res_strlen = strlen(base) + strlen(index) + strlen(ext) + 3; // '+3' because of path separator, dot and '\0'
    char *buf = malloc(sizeof(char) * res_strlen);

    snprintf(buf, res_strlen, "%s/%s.%s", base, index, ext);

    return buf;
}

void map_ids(TGraph *graph, int *map) {
    for (int i = 0; i < graph->node_count; ++i) {
        TVertex *v = graph_get_vertex(graph, i);

        v->id = map[v->id];
    }
}

void load_index(TIndex *index, char* base_path, char* name) {
	strip_path_delimiter(base_path);

	char *header_path = build_index_path(base_path, name, "hdr");
	char *body_path = build_index_path(base_path, name, "idx");

	index_init(index, header_path, body_path);
}

int main(int argc, char *argv[]) {
    TIndex graphIndex;
    TIndex mappingIndex;
	ngram_fn fns[2] = { &compute_path_ngrams, &compute_neighbourhood_ngrams };

	load_index(&graphIndex, argv[1], "graphs");
	load_index(&mappingIndex, argv[1], "mappings");

	int ids[2], fn, n;

    while (scanf("%i %i %i %i", &fn, &n, &ids[0], &ids[1]) != EOF) {
		TGraph *g1 = index_get(&graphIndex, &ids[0]);
        TGraph *g2 = index_get(&graphIndex, &ids[1]);
        int *map = index_get(&mappingIndex, ids);

		int err = ((g1 == NULL) << 2) | ((g2 == NULL) << 1) | (map == NULL);

		if (err == 0) {
			map_ids(g1, map);

			/*double result = 0;

			for (size_t i = 2; i <= n; i++) {
				result += i * graph_compare(g1, g2, fns[fn], i)
			}

			result = result / (((n * (n+1))/2)-1);

			printf("%i\t%i\t%f\n", ids[0], ids[1], result);*/
	        printf("%i\t%i\t%f\n", ids[0], ids[1], graph_compare(g1, g2, fns[fn], n));
		}
		else {
			fprintf(stderr, "%d\t%d\t%d\n", err, ids[0], ids[1]);
		}
    }

    return 0;
}
