#include <stdio.h>
#include <stdlib.h>


struct table {
    float t;
    float k;
} table[] = {
{-40 ,208.15},   
{-39 ,196.57},   
{-38 ,185.71},   
{-37 ,175.52},   
{-36 ,165.96},   
{-35 ,156.97},   
{-34 ,148.53},   
{-33 ,140.60},   
{-32 ,133.14},   
{-31 ,126.12},   
{-30 ,119.52},   
{-29 ,113.30},   
{-28 ,107.45},   
{-27 ,101.93},   
{-26 , 96.73},   
{-25 , 91.83},   
{-24 , 87.21},   
{-23 , 82.85},   
{-22 , 78.73},   
{-21 , 74.85},   
{-20 , 71.18},   
{-19 , 67.71},   
{-18 , 64.43},   
{-17 , 61.33},   
{-16 , 58.40},  
{-15 , 55.62},   
{-14 , 53.00},   
{-13 , 50.51},   
{-12 , 48.16},   
{-11 , 45.93},   
{-10 , 43.81},   
{-9  , 41.81},   
{-8  , 39.91},  
{-7  , 38.11},   
{-6  , 36.40},   
{-5  , 34.77},   
{-4  , 33.23},   
{-3  , 31.77},   
{-2  , 30.38},   
{-1  , 29.05},  
{0   , 27.80},   
{1   , 26.60}, 
{2   ,25.46}, 
{3   ,24.38}, 
{4   ,23.35}, 
{5   ,22.37}, 
{6   ,21.44}, 
{7   ,20.55}, 
{8   ,19.70}, 
{9   ,18.90}, 
{10  ,18.13}, 
{11  ,17.39}, 
{12  ,16.69}, 
{13  ,16.02}, 
{14  ,15.39}, 
{15  ,14.78}, 
{16  ,14.20}, 
{17  ,13.64}, 
{18  ,13.11}, 
{19  ,12.61}, 
{20  ,12.12}, 
{21  ,11.66}, 
{22  ,11.22}, 
{23  ,10.79}, 
{24  ,10.39}, 
{25  ,10.00}, 
{26  ,9.63 }, 
{27  ,9.27 }, 
{28  ,8.93 }, 
{29  ,8.61 }, 
{30  ,8.30 }, 
{31  ,8.00 }, 
{32  ,7.71 }, 
{33  ,7.43 }, 
{34  ,7.17 }, 
{35  ,6.92 }, 
{36  ,6.67 }, 
{37  ,6.44 }, 
{38  ,6.22 }, 
{39  ,6.00 }, 
{40  ,5.80 }, 
{41  ,5.60 }, 
{42  ,5.41 }, 
{43,  5.23 },
{44,  5.05 },
{45,  4.88 },
{46,  4.72 },
{47,  4.57 },
{48,  4.42 },
{49,  4.27 },
{50,  4.13 },
{51,  4.00 },
{52,  3.87 },
{53,  3.75 },
{54,  3.63 },
{55,  3.51 },
{56,  3.40 },
{57,  3.30 },
{58,  3.19 },
{59,  3.09 },
{60,  3.00 },
{61,  2.91 },
{62,  2.82 },
{63,  2.73 },
{64,  2.65 },
{65,  2.57 },
{66,  2.49 },
{67,  2.42 },
{68,  2.35 },
{69,  2.28 },
{70,  2.21 },
{71,  2.15 },
{72,  2.09 },
{73,  2.03 },
{74,  1.97 },
{75,  1.91 },
{76,  1.86 },
{77,  1.80 },
{78,  1.75 },
{79,  1.70 },
{80,  1.66 },
{81,  1.61 },
{82,  1.57 },
{83,  1.52 },
{84,  1.48 },
{85  ,1.44},
{86  ,1.40},
{87  ,1.37},
{88  ,1.33},
{89  ,1.29},
{90  ,1.26},
{91  ,1.23},
{92  ,1.19},
{93  ,1.16},
{94  ,1.13},
{95  ,1.10},
{96  ,1.07},
{97  ,1.05},
{98  ,1.02},
{99  ,0.99},
{100 ,0.97},
{101 ,0.95},
{102 ,0.92},
{103 ,0.90},
{104 ,0.88},
{105 ,0.85},
{106 ,0.83},
{107 ,0.81},
{108 ,0.79},
{109 ,0.77},
{110 ,0.76},
{111 ,0.74},
{112 ,0.72},
{113 ,0.70},
{114 ,0.69},
{115 ,0.67},
{116 ,0.65},
{117 ,0.64},
{118 ,0.62},
{119 ,0.61},
{120 ,0.60},
{121 ,0.58},
{122 ,0.57},
{123 ,0.56},
{124 ,0.54},
{125 ,0.53},
};
























int main(int argc, char *argv[]) {
    
    int i;
    float percent = 0;
    
    if(argc != 2) {
        printf("Input param first please!\n");
        return 0;
    }
    
    percent = atoi(argv[1])*0.465/100+0.21;

    printf("percent\t:%f\n", percent);

    float r = 5.23*30.1*percent/(30.1-percent*30.1-percent*5.23);
    printf("R\t:%f\n", r);

    for(i=0; i<sizeof(table)/sizeof(float)/2; i++) {

        if(r >= table[i].k) {
            printf("temp\t:%f\n", table[i].t);
            break;
        }
    }


    return;

}
