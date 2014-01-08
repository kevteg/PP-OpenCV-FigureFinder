#include "engine.h"
//Mat engine::image = Mat::zeros(3,3, CV_8UC1);
//Inicializando variables estáticas
    int engine::brightness = 0;
    int engine::contrast = 1;
engine::engine(int argc, char **argv){
    this->argc = argc;
    this->argv = argv;

    if(argc == 2){
//cap.open(argv[1]);
        (isdigit(*argv[1]))?cap.open(atoi(argv[1])):cap.open(argv[1]);//Se abre la cámara o el video para capturar la imagen
 //       if(isalpha(*argv[1]))
     //ŕ     cout << "video";
    }else if(argc == 1)
        cap.open(-1);
    //engine::imag =  Mat::zeros(3,3, CV_8UC1);
    cout << "\nNota: El valor real de brillo es -100 unidades"<<endl;
    memo = cvCreateMemStorage(0);
    namedWindow("Opciones",WINDOW_AUTOSIZE);
    namedWindow("Imagen",WINDOW_NORMAL);
    namedWindow("Imagen con el filtro",WINDOW_NORMAL);
    namedWindow("Bordes :)",WINDOW_NORMAL);
}

/*void convertTo( OutputArray m, int rtype, double alpha=1, double beta=0 )
This OpenCV function converts  image into another format with scaling. Scaling is done according to the following formula.
                                              m[i,j] = alfa * img[i,j] + beta

Here is the parameters of this function
OutputArray m - Store the converted image
int rtype - Depth of the output image. If the value of rtype is negative, output type is same as the input image. I have used a negative value in the above program because I don't want to change the depth of the original image. Possible inputs to this parameter
CV_8U
CV_32S
CV_64F
-1
Complete list of depths can be found in Basics of OpenCV API
double alpha - Multiplication factor; Every pixel will be multiplied by this value
double beta - This value will be added to very pixels after multiplying with the above value.
Here is the formula again. Here m[i, j] means a pixel at ith row and jth column.
                              m[i,j] = alfa * img[i,j] + beta*/
void engine::trackbarBright(int brightness, void *){
    engine::brightness = brightness;
    //engine::contrast = *( static_cast<int*>(usrdata) );
    cout << "trackbarBright: Contraste=" << engine::contrast << ", Brillo=" << engine::brightness-100 << endl;
    //image = image + Scalar(brightness,brightness,brightness);
    //adjust the brightness and contrast
    //engine::image.convertTo(engine::image, -1, dContrast, iBrightness);
}
void engine::trackbarContrast(int contrast, void *){
     //engine::brightness = *( static_cast<int*>(usrdata) );
     engine::contrast = contrast;
     cout << "trackbarContrast: Contraste=" << engine::contrast << ", Brillo=" << engine::brightness-100 << endl;
}
int engine::workImage(){
    int kernel = 3;
    int filtro = 0;
    int thresmin = 0;
    int canny = 50;
    int maxbc = 200;
    char charcheckforkeypress;
    if(!cap.isOpened()){
        cout << (argc == 1?"\nNo hay ninguna cámara":"Esa cámara no existe") << ":(" << endl;
        return -1;
    }
    cout << "Tamaño de kernel del filtro: " <<kernel << ":)"<< endl;
    createTrackbar("Brillo imagen", "Opciones", &this->brightness, maxbc, trackbarBright);
    createTrackbar("Contraste imagen", "Opciones", &this->contrast, maxbc, trackbarContrast);
    createTrackbar("Umbral de algoritmo canny", "Opciones", &canny, maxbc);


     for(;;){
        if(!cap.read(image)){
            cout << "\nHubo un error con la imagen" << endl;
            return -1;
        }

        //Nota: El valor real de brillo es -100 unidades
        image.convertTo(image, -1, this->contrast, this->brightness-100);
        //image = image + Scalar(brightness,brightness,brightness); Para ajustar solo el brillo
        //Variables estáticas de métodos no estáticos se llaman usando this->

        mimage = image.clone();
        filterAplied(image, mimage,  kernel, filtro);
        /*1.Se toma la image de la camara
        * 2.Se copia a mimage
        * 3.Se convierte mimage a escala de grises
        * 4.Se aplica threshold a mimage para reducir aun mas cosas innecesarias
        * 5.Se aplica el algoritmo canny para detectar los bordes a mimagen
        * 6.Se encuentran los contornos de la imagen con cvFindContours y se buscan los que tienen 4 puntos
        */

        cvtColor(mimage, mimage, CV_BGR2GRAY);    // Escala de grises
        threshold(mimage, mimage,thresmin,255,3); // Otro filtro para la imagen
        Canny(mimage,edges,canny,thresmin,3);      // Bordes de la imagen
        /*Canny Parameters:
        image – single-channel 8-bit input image.
        edges – output edge map; it has the same size and type as image .
        threshold1 – first threshold for the hysteresis procedure.
        threshold2 – second threshold for the hysteresis procedure.
        apertureSize – aperture size for the Sobel() operator.
        L2gradient – a flag, indicating whether a more accurate  L_2 norm  =\sqrt{(dI/dx)^2 + (dI/dy)^2} should be used to calculate the image gradient magnitude ( L2gradient=true ), or whether the default  L_1 norm =|dI/dx|+|dI/dy| is enough ( L2gradient=false ).
        */



	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle


        quality(edges);

        countFig(edges); //Se cuentan los cuadrados y se ponen sobre image
        imshow("Imagen con el filtro",mimage);
        imshow("Imagen",this->image);
        imshow("Bordes :)", edges);

        charcheckforkeypress = waitKey(20);
        if(options( charcheckforkeypress,  kernel,  filtro, thresmin)<0)
             return 0;
    }//Infinite for
}
void engine::quality(Mat &edges){
	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 1px by 1px rectangle
    Mat erodeElement = getStructuringElement( MORPH_RECT,Size(1,1));
    //dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(5,5));
	erode(edges,edges,erodeElement);
	//erode(edges,edges,erodeElement);
	dilate(edges,edges,dilateElement);
	//dilate(edges,edges,dilateElement);
}
 void engine::filterAplied(Mat &image, Mat &mimage, int kernel, int filtro){
    switch(filtro){
        case 0:
            GaussianBlur(mimage, mimage, Size(kernel,kernel), 0, 0);
            break;
        case 1:
            blur(mimage, mimage, Size(kernel, kernel),Point(-1,-1));
            break;
        case 2:
            medianBlur(mimage, mimage, kernel);
            break;
    }
 }
int engine::options(char charcheckforkeypress, int &kernel, int &filtro, int &thresmin){
        if(charcheckforkeypress == 27)
            return -1;
        else if(charcheckforkeypress == 'R'){
           kernel<31?kernel += 2:kernel-=2;
           cout << "Tamaño de kernel del filtro: " <<kernel << ":)"<< endl;
        }else if(charcheckforkeypress == 'T'){
           kernel>1?kernel -= 2:kernel+=2;
           cout << "Tamaño de kernel del filtro: " <<kernel << ":)"<< endl;
        }else if(charcheckforkeypress == ' '){
            filtro = -1;
            cout << "Introdusca el filtro nuevo: ";
                while(filtro<0||filtro>2)
                    cin >> filtro;
            if(!filtro)
                cout << "Aplicando filtro gaussian blur" << endl;
            else if(filtro == 1)
                cout << "Aplicando filtro blur" << endl;
            else
                cout << "Aplicando filtro median blur" << endl;
        }else if(charcheckforkeypress == 'S'){
            thresmin<255?thresmin++:thresmin--;
            cout << thresmin << endl;
        }else if(charcheckforkeypress == 'Q'){
             thresmin>0?thresmin--:thresmin++;
             cout << thresmin << endl;
        }
        return 0;
}

void engine::countFig(Mat grayImage){
    int number = 0;

    //c++:
    /*vector<Vec4i> vect;
    vector<vector<Point> >cont;
    findContours(grayImage, cont, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));*/
    //c:

    IplImage* iplImage;
    iplImage = cvCreateImage(grayImage.size(), IPL_DEPTH_8U, 1);
    iplImage->imageData = (char *) grayImage.data;
    cvFindContours(iplImage, memo, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
/*
    *Se ha usado la forma cvFindContours porque así los contornos se guardan en contours y se puede mostrar
    *el poligono que se quiera la forma de ć++ findContours no es la mejor en este caso porque retorna todas las

    */
  /*for(int i = 0; i < cont.size(); i++)

        drawContours(image, cont, i, Scalar(255,255,255), 5, 8, vect, 0,Point());*/


    while(contours){
        //approxPolyDP(*contours, *results, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 1);
        results = cvApproxPoly(contours, sizeof(CvContour), memo, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.03, 0);
        if(results->total == 4  && fabs(cvContourArea(results, CV_WHOLE_SEQ))>100 )
         {
             number++;
             CvPoint *pt[4];
             for(int i=0;i<4;i++){
                 pt[i] = (CvPoint*)cvGetSeqElem(results, i);
             }

             //Dibujar lineas al rededor del rectangulo
             line(image, *pt[0], *pt[1], Scalar(255,255,255),4);
             line(image, *pt[1], *pt[2], Scalar(255,255,255),4);
             line(image, *pt[2], *pt[3], Scalar(255,255,255),4);
             line(image, *pt[3], *pt[0], Scalar(255,255,255),4);
         }
         contours = contours->h_next;
    }
   cout << "Hay aproximadamente " << number << " cuadrados" << endl;
}
engine::~engine(){
    cvDestroyAllWindows();
    cvReleaseMemStorage(&memo);
}
