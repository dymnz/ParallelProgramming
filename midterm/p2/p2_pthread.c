#include <math.h>		// sqrt
#include <stdio.h>		// printf, fprintf
#include <stdlib.h>		// malloc
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <pthread.h>

#define THREAD_NUM 32

typedef struct {
	int R, G, B;
	float H, S, I;
} Pixel;

typedef struct {
	int index_start;
	int index_end;
} Thread_StartEnd_Param;

void RGB2HSI(int R, int G, int B, float *Hff , float *Sff, float *Iff);
void HSI2RGB(float H, float S, float I, int *R, int *G, int *B);
void ScaleUpI(float *I);
void ScaleDownI(float *I);
void Equalization();


/** Define Variable **/
Pixel *pixels;
long long pixel_count = 0;
long long img_rows, img_cols;

void *thread_RGB2HSI_ScaleUpI_func(void *args) {
	Thread_StartEnd_Param *param = (Thread_StartEnd_Param *) args;

	int i;
	for (i = param->index_start; i < param->index_end; ++i) {
		RGB2HSI(pixels[i].R, pixels[i].G, pixels[i].B,
		        &pixels[i].H, &pixels[i].S, &pixels[i].I);
		ScaleUpI(&pixels[i].I);
	}

	return 0;
}


void *thread_HSI2RGB_func(void *args) {
	Thread_StartEnd_Param *param = (Thread_StartEnd_Param *) args;

	int i;
	for (i = param->index_start; i < param->index_end; ++i) {
		HSI2RGB (pixels[i].H, pixels[i].S, pixels[i].I, &pixels[i].R, &pixels[i].G, &pixels[i].B);
	}
	return 0;
}

int main(int argc, char* argv[]) {

	/***************************************************************/
	/***********************  Pre-Processing  **********************/
	/** Read argv **/
	if (argc < 3) {
		printf("Usage: ./%s <input> <output>\n", argv[0]);
		return 0;
	}

	/** Read data **/
	FILE * fd_in = fopen(argv[1], "r");
	if (fd_in == NULL) {
		fprintf(stderr, " File I/O: %s open failed.\n", argv[1]);
		return 0;
	}

	fscanf(fd_in, "%d %d %d", &pixel_count, &img_rows, &img_cols);	// read number of pixels
	pixels = malloc(sizeof(Pixel) * pixel_count);
	long long idx;
	for (idx = 0; idx < pixel_count; idx++) {
		fscanf(fd_in, "%d %d %d", &pixels[idx].R, &pixels[idx].G, &pixels[idx].B);
	}
	fclose(fd_in);

	/** Setup timer **/
	struct timeval start, end;
	gettimeofday(&start, NULL);
	/*****************    End of Pre-Processing ********************/

	/******************  Midterm Exam Starts Here  *****************/

	int i;
	pthread_t *thread_list = (pthread_t *) malloc(THREAD_NUM * sizeof(pthread_t));
	int segment_size_per_thread = pixel_count / THREAD_NUM;

	Thread_StartEnd_Param *StartEnd_param_list = (Thread_StartEnd_Param *)
	        malloc(THREAD_NUM * sizeof(Thread_StartEnd_Param));

	/**  Step1. RGB to HSI  **/
	// Spawn threads
	for (i = 0; i < THREAD_NUM; ++i) {
		// Thread runs [index_start index_end)
		StartEnd_param_list[i].index_start = i * segment_size_per_thread;

		// Last thread process the remainder of the particles
		if (i < THREAD_NUM - 1)
			StartEnd_param_list[i].index_end = (i + 1) * segment_size_per_thread;
		else
			StartEnd_param_list[i].index_end = pixel_count + 1;

		pthread_create(&thread_list[i], NULL, thread_RGB2HSI_ScaleUpI_func, &StartEnd_param_list[i]);
	}
	// Join the threads
	for (i = 0; i < THREAD_NUM; ++i)
		pthread_join(thread_list[i], NULL);

	// Combined with above
	/**  Step2. Pick I and scale up to (0, 1000)  **/
	// for (idx = 0; idx < pixel_count; idx++) {
	// 	ScaleUpI(&pixels[idx].I);
	// }

	/**  Step3. Perform Equalization on I  **/
	Equalization(pixels, pixel_count);

	/**  Step4. RGB to HSI  **/
	// for (idx = 0; idx < pixel_count; idx++) {
	// 	ScaleDownI(&pixels[idx].I);
	// }

	/**  Step5. HSI to RGB  **/
	// for (idx = 0; idx < pixel_count; idx++) {
	// 	HSI2RGB (pixels[idx].H, pixels[idx].S, pixels[idx].I, &pixels[idx].R, &pixels[idx].G, &pixels[idx].B);
	// }
	// Spawn threads
	for (i = 0; i < THREAD_NUM; ++i) {
		// Thread runs [index_start index_end)
		StartEnd_param_list[i].index_start = i * segment_size_per_thread;

		// Last thread process the remainder of the particles
		if (i < THREAD_NUM - 1)
			StartEnd_param_list[i].index_end = (i + 1) * segment_size_per_thread;
		else
			StartEnd_param_list[i].index_end = pixel_count + 1;

		pthread_create(&thread_list[i], NULL, thread_HSI2RGB_func, &StartEnd_param_list[i]);
	}
	// Join the threads
	for (i = 0; i < THREAD_NUM; ++i)
		pthread_join(thread_list[i], NULL);

	/******************  Midterm Exam Ends Here  *****************/
	/***************************************************************/


	free(thread_list);
	free(StartEnd_param_list);

	/***************************************************************/
	/********************  Calculate Performance  ******************/


	/** Calculate Time **/
	gettimeofday(&end, NULL);
	double timeInterval = ((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

	/** Get memory usage **/
	struct rusage r_usage;
	getrusage(RUSAGE_SELF, &r_usage);

	/** Output performence **/
	printf("Memory usage: %ld bytes\n", r_usage.ru_maxrss);
	printf("Time: %lf sec\n", timeInterval);


	/** Write result data **/
	FILE * fd_out = fopen(argv[2], "w");
	if (fd_out == NULL) {
		fprintf(stderr, " File I/O: %s open failed.\n", argv[2]);
		return 0;
	}

	fprintf(fd_out, "%d %d %d\n", pixel_count, img_rows, img_cols);// read number of pixels

	for (idx = 0; idx < pixel_count; idx++) {
		fprintf(fd_out, "%d %d %d\n", pixels[idx].R, pixels[idx].G, pixels[idx].B);
	}
	fclose(fd_out);

	/******************  End of Calculate Performance  *************/
	/***************************************************************/

	free(pixels);

	return 0;
}



void RGB2HSI(int R, int G, int B, float *Hff , float *Sff, float *Iff) {
	float Hf, Sf, If;

	if (R == 0 && G == 0 && B == 0) {
		Hf = 0;
		Sf = 0;
		If = 0;
	}
	else if (R == 255 && G == 255 && B == 255) {
		Hf = 0;
		Sf = 0;
		If = 1;
	}
	else {
		float Rf, Gf, Bf, minf;

		// Get minimum of (R,G,B)
		int min = R;
		if (min > G)
			min = G;
		if (min > B)
			min = B;

		// Normalize the value from (0, 255) to (0, 1)
		minf = (float)min / 255;
		Rf = (float)R / 255;
		Gf = (float)G / 255;
		Bf = (float)B / 255;

		// Calculate I
		If = 1.00f / 3.00f * (Rf + Gf + Bf);

		// Calculate S
		Sf = 1.00f - (3.00f / (Rf + Gf + Bf)) * minf;

		// Calculate H
		if (Sf == 0 || (Rf == Gf && Gf == Bf)) {
			Hf = 0;
		}
		else {
			float angle = (0.5f * ((Rf - Gf) + (Rf - Bf))) / sqrt((Rf - Gf) * (Rf - Gf) + (Rf - Bf) * (Gf - Bf));
			angle = (angle > 1) ? 1 : angle;
			angle = (angle < -1) ? -1 : angle;
			Hf = (float)acos(angle);
		}

		if ((Bf / If) > (Gf / If))
			Hf = (float)(2 * M_PI - Hf);
	}

	*Hff = Hf;
	*Sff = Sf;
	*Iff = If;
}

void HSI2RGB(float H, float S, float I, int *R, int *G, int *B) {
	float x = I * (1 - S);
	if (H < 2 * M_PI / 3)
	{
		float y = I * (1 + (S * cos(H)) / (cos(M_PI / 3 - H)));
		float z = 3 * I - (x + y);
		*B = round(x * 255);
		*R = round(y * 255);
		*G = round(z * 255);
	}
	else if (H < 4 * M_PI / 3)
	{
		float y = I * (1 + (S * cos(H - 2 * M_PI / 3)) / (cos(M_PI / 3 - (H - 2 * M_PI / 3))));
		float z = 3 * I - (x + y);
		*R = round(x * 255);
		*G = round(y * 255);
		*B = round(z * 255);
	}
	else
	{
		float y = I * (1 + (S * cos(H - 4 * M_PI / 3)) / (cos(M_PI / 3 - (H - 4 * M_PI / 3))));
		float z = 3 * I - (x + y);
		*R = round(z * 255);
		*G = round(x * 255);
		*B = round(y * 255);
	}
}

void ScaleUpI(float *I) {
	*I *= 10000;
}
void ScaleDownI(float *I) {
	*I /= 10000;
}

void Equalization() {
	long long statistic_arry[10001] = {0};		// statistic_arry array
	float f_statistic_arry[10001];
	long long idx = 0;					// Index of for loop


	// WHY
	/** Initial the arry **/
	/*
	for (idx = 0; idx < 10001; idx++) {
		statistic_arry[idx] = 0;
	}*/

	/** Calculate PDF **/
	for (idx = 0; idx < pixel_count; idx++) {
		statistic_arry[(int)(pixels[idx].I)]++;
	}

	/** Calculate CDF **/
	for (idx = 1; idx < 10001; idx++) {
		statistic_arry[idx] += statistic_arry[idx - 1];
	}

	/** Find CDF min (Find the first non-zero) **/
	long long cdf_min = 0;
	for (idx = 0; idx < 10001; idx++) {
		if (statistic_arry[idx] != 0) {
			cdf_min = statistic_arry[idx];
			break;
		}
		else {
			continue;
		}
	}

	/** Calculate new value in statistic_arry **/
	// for (idx = 0; idx < 10001; idx++) {
	// 	statistic_arry[idx] = round((((double)statistic_arry[idx] - cdf_min) / (pixel_count - cdf_min)) * (10000 - 10000 / 255));
	// }
	for (idx = 0; idx < 10001; idx++) {
		// statistic_arry[idx] = round(
		//                           (((double)statistic_arry[idx] - cdf_min)
		//                            / (pixel_count - cdf_min)) *
		//                           (10000 - 10000 / 255));
		f_statistic_arry[idx] = round(
		                            (((double)statistic_arry[idx] - cdf_min)
		                             / (pixel_count - cdf_min)) *
		                            (10000 - 10000 / 255)) / 10000;
	}

	/** Update I value **/
	for (idx = 0; idx < pixel_count; idx++) {
		pixels[idx].I = f_statistic_arry[(int)(pixels[idx].I)];
	}
}




/********************************************************
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;#;'#;
;;;;;;;;;;+';;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;'+;'#;
;;++;;;##+###;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;'#'#;;
;;;#'+;;;;#+##;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;++;;
;+##+';;###';;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;++;;
;;;;++;;;+###';;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;##+';++++'+#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;'+;++#+#+#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;##++;'+++#+;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;';;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;###';;#+###;;;;;;;;;;;;;;;;++####+';;;;;;;;;;;;;;;;;;;;
;;#'++;;##';#;;;;;;;;;;;;;+##';,,,,;'###';;;;;;;;;;;;;;;;
;;+++;;;#;#+#;;;;;;;;;;;'#+............:++';;;;;;;;;;;;;;
;;;';;;;#+++;;:::::::;'#+````....,'+###+;:+#';;;;;;;;;;;;
;;;;;;;;'#'#:::::::::##````.;+###++;::::::;;#+;;;;;;;;;;;
;;;;;;;::::'::::::::#',+####+;::::::::::::'###++#####';;;
;;;;;::::::::::::::###+'::::::::::::'+#+##',..'+#:::;#;;;
;;;;::+#+::::::::,#::::::::::::'+####',........++#+:#';;;
;;;:::#,#:::::::,#;::;'++######':...............#+###;;;;
;;::::##+::::,,,'+###++;,.`````````..##..........#;;;;;;;
;:::::,,:,,,,,,,#`````````````````:#++;,.........++;;;;;;
:::::::::,,,,,,'+```````````````###+::'##........;#;;;;;;
:::::::,,,,,,,,######':``````,#+#` ` `` +#......###;;;;;;
::::::,,,,,,,,.#`;###+++````;#:# `'',    ;#...+#:.++;;;;;
:::::,,,,,,,,,.###    ,##`````#. @`    #  #.:#'...++;;;;;
::::,,,,,,,#,,.+,` .',  ##````# +``` @  + +##.....++;;;;;
::::,,,,,,.#.,'#`;:  .@  #,```# @`#.  #`: :+......++;;;;;
:::,,,,,,..',.# .`,```#` .#```# ``# + # : :#......+';;;;;
:::,,,,,,;+;#.# ,' :: `   #,''# , `' `:`; #:......#;;;;;;
';:,,,,,,..#.`#``# ,`, .  +#;;#: `  ;,  + #.......+;;;;;;
;''''';::,.+.`# `+  @ '   #.```#  #```.@ '#......:#;;;;;;
:,,,,:;';'';'.+' `#++:,' ,+````,#  +##. `#.......#';;;;;;
:,,,,,,,,,,,,;'#;@`   @` #.`````:#`    :#,.......#;;;;;;;
,,,,,,,,,,,,,:+;## ..   +#```````.######........++#;;;;;;
+'',,,,,,,,,,+:',###++###```````````...........:#++#;;;;;
;'+''';:,,,,+;,.;.#,::.`````````````````.......#'+#'#;;;;
'';;;;''+';+'.,,,'++`````````,,:```````.......#';+';'#;;;
;'++';;;::::':,,.;.#'``````+###:``````......'#'';;;;;;;;;
,,,,;''+'':;:'.,,:;`#+```````````````.....'#+''###;;;;;;;
:,,,,,,,,;+;:;',,.;..+#.````````````....'#+;,,,,.:#';;+';
:,,,,,,,,+++::':,,;;..,+#+'```.+##@`@####..#+,,,,,,+;;'';
:,,,,,,,,+;+;::',,,',.#'.:```##'..#..,......++,,,,`#;;;;;
::,,,,,,,+:;+::;',,,#++````'#:+####+.`.......#',,,.#;;;+;
:,,:,,,,''::+'::':,,#####,'++#;...,#.``.......#,,,,#;#+;;
::::,,,,':;;:''::',,:+###++#:.@.:#,#.`....#...+;,,,+'';;+
:::::,,;';;;;:'';'',,+',,:+.,,+##:,'+`...'#...+;,,,#,'##+
::::,:,';;;;;;:';;'',,#:,:#:,,.##.,,#...#+`...#;,,'+;+;;+
:::::,;';;;;;;;'';;';,:#,,;#,.@##:,.#.+#.`...,#:,,+:,#+';
::::,,';;;;;;;;;';;;',:+#:,#..##@..,###`,...;#:,,,#,.#;+;
::::,+';;;;;;;;;'+;;'':,#':#;.+'+#.,++...,#++;,,,,#+,,+:+
::::'+;;;;;;;;;;;'';;+;::#;'+:######+++###+++++++++++++++
:::;+';;;;;;;;;;;;+;;'+,,++:::;';;;;':';::::,;;:;::......
:::+';;;;;;;;;;;;;++;;++,:#:,::::;;;#+';::::::::::::::...
::'+;;;;;;;;;;;;;;;+';;+:'+..:::::''....;'''#;::::,,,....
::+';;;;;;;;;;;;;;;'+;;'++,...:,:#........+'#;;;;;;;;::..
:++;;;;;;;;;;;;;;;;;+';;',....,;'.......,+++,::,,,,,,,,,.
********************************************************/