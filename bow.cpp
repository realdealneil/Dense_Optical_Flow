#include "bow.h"

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

BoW::BoW()
{

}

BoW::~BoW()
{

}

//////////////// CrearDiccionario ///////////////////
void BoW::CrearDiccionario(std::string file, std::string dir)
//void BoW::CrearDiccionario(string descriptor)
{
    std::string name=dir+"/"+file;
    const char *fileName = name.c_str();

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    //fp = fopen("/home/ercmos/Videos/test_sequences/prueba2.txt", "r");
    fp = fopen(fileName, "r");
    if (fp == NULL)
       exit(EXIT_FAILURE);

/*/////////////////////////////////////////////////////*/
    int ch, number_of_lines = 0, number_of_colums = 0;
    read = getline(&line, &len, fp);
    char * pch;
    pch = strtok (line,"\t");
    while (pch != NULL)
    {
      number_of_colums++;
      pch = strtok (NULL, "\t");
    }

    free(pch);
    rewind(fp); //rewind the file to count the lines
    do
    {
        ch = fgetc(fp);
        if(ch == '\n')
            number_of_lines++;
    } while (ch != EOF);

/*////////////////////////////////////////////////////*/
    float Data[number_of_lines][number_of_colums];

    int rows=0;
    int cols=0;
    rewind(fp);

    while ((read = getline(&line, &len, fp)) != -1)
    {
        //Extract parts separated by "\t"
        char * pch;
        pch = strtok (line,"\t");
        while (pch != NULL)
        {
          //printf ("%s\n",pch);
          float value=atof(pch);
          Data[rows][cols]=value;
          printf("%f",value);
          pch = strtok (NULL, "\t");
          ++cols;
        }
        ++rows;
        cols=1;
        free(pch);
    }

    fclose(fp);
    if (line)
        free(line);

    //Step 1 - Obtain the set of bags of features.

    //to store the input file names
    //char * filename = new char[100];
    //to store the current input image
//    Mat input;

    //To store the keypoints that will be extracted by SIFT
//    vector<KeyPoint> keypoints;
    //To store the SIFT descriptor of current image
    //Mat descriptor;
    //To store all the descriptors that are extracted from all the images.
    Mat featuresUnclustered;
    Mat descriptor(number_of_lines,number_of_colums,CV_32FC1,Data);

    //The SIFT feature extractor and descriptor
    //SiftDescriptorExtractor detector;
//    Ptr<SIFT> detector = SIFT::create();

    //QString directorio = QString::fromStdString(dirName);
    //QDir dir(directorio);
//    QDir dir(QString::fromStdString(dirName));
    //QMessageBox msgBox;
//    foreach(QFileInfo item, dir.entryInfoList() )
//        {
//            if(item.isFile())
//           {
               // msgBox.setText(item.absoluteFilePath());
               // msgBox.exec();
               //create the file name of an image
               //sprintf(filename,"G:\\testimages\\image\\%i.jpg",f);
               //open the file
//               input = imread(item.absoluteFilePath().toStdString(), CV_LOAD_IMAGE_GRAYSCALE); //Load as grayscale
               //detect feature points
               //detector.detect(input, keypoints);
//               detector->detect( input, keypoints );
               //compute the descriptors for each keypoint
               //detector.compute(input, keypoints,descriptor);
//               detector->compute(input, keypoints,descriptor);
               //put the all feature descriptors in a single Mat object
               featuresUnclustered.push_back(descriptor);
               //print the percentage
               //printf("%i percent done\n",f/10);
//            }
//        }
    Mat descr;
    //featuresUnclustered.convertTo(descr, CV_32F);
    featuresUnclustered.convertTo(descr, CV_32FC1);
    //Construct BOWKMeansTrainer
    //the number of bags
    //int dictionarySize=200;
    int dictionarySize=4;   //máximo el número de descriptores (filas)
    //define Term Criteria
    TermCriteria tc(CV_TERMCRIT_ITER,100,0.001);
    //retries number
    int retries=1;
    //necessary flags
    int flags=KMEANS_PP_CENTERS;
    //Create the BoW (or BoF) trainer
    BOWKMeansTrainer bowTrainer(dictionarySize,tc,retries,flags);
    //BOWKMeansTrainer bowTrainer(dictionarySize);
    //bowTrainer.add(featuresUnclustered);
    //cluster the feature vectors
    Mat dictionary=bowTrainer.cluster(featuresUnclustered);
    //Mat dictionary=bowTrainer.cluster();
    //store the vocabulary
    //FileStorage fs("diccionario.yml", FileStorage::WRITE);
    FileStorage fs(dir+"/diccionario.yml", FileStorage::WRITE);
    fs << "vocabulary" << dictionary;
    fs.release();
}
////////////////////////////////////////////////


//////////////// TageadorDiccionario ///////////////////
void BoW::TageadorDiccionario(std::string dirName, std::string fileName)
{
    //Step 2 - Obtain the BoF descriptor for given image/video frame.

    //prepare BOW descriptor extractor from the dictionary
/*
    /////defining terms for bowkmeans trainer/////////////////////////////////////////
    int MAX_ITER = 15;
    int EPS = 2;
    TermCriteria tc(MAX_ITER + EPS, 10, 0.001);
    int dictionarySize = 1000;
    int retries = 1;
    int flags = KMEANS_PP_CENTERS;
    BOWKMeansTrainer bowTrainer(dictionarySize, tc, retries, flags);
    Mat dictionary = bowTrainer.cluster();
//    bowDE.setVocabulary(dictionary);
///////////////////////////////////////////////////////////////////////////////////
*/
    Mat dictionary;
    FileStorage fs(dirName+"/diccionario.yml", FileStorage::READ);
    fs["vocabulary"] >> dictionary;
    fs.release();

    //create a nearest neighbor matcher Fast Library for Approximate Nearest Neighbors (FLANN)
    //Ptr<DescriptorMatcher> matcher(new FlannBasedMatcher);

    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");

    //create Sift feature point extracter
    //Ptr<FeatureDetector> detector(new SiftFeatureDetector());
    Ptr<FeatureDetector> detector = SiftFeatureDetector::create();
    //Ptr<SIFT> detector = SIFT::create();
    //create Sift descriptor extractor
    //Ptr<DescriptorExtractor> extractor(new SiftDescriptorExtractor);
    Ptr<DescriptorExtractor> extractor = SiftDescriptorExtractor::create();

    //create BoF (or BoW) descriptor extractor
    BOWImgDescriptorExtractor bowDE(extractor,matcher); //extractor es la imagen matcher es el vocabulario

    //Set the dictionary with the vocabulary we created in the first step
    bowDE.setVocabulary(dictionary);

    //To store the image file name
    //char * filename = new char[100];
    //To store the image tag name - only for save the descriptor in a file
    char * imageTag = new char[10];

    //open the file to write the resultant descriptor
    FileStorage fs1(dirName+"/diccionario_2.yml", FileStorage::WRITE);

    //the image file with the location. change it according to your image file location
    //sprintf(filename,"G:\\testimages\\image\\1.jpg");
    //read the image
    Mat img=imread(dirName+"/"+fileName,CV_LOAD_IMAGE_GRAYSCALE);
    //To store the keypoints that will be extracted by SIFT
    vector<KeyPoint> keypoints;
    //Detect SIFT keypoints (or feature points)
    detector->detect(img,keypoints);
    //To store the BoW (or BoF) representation of the image
    Mat bowDescriptor;
    //extract BoW (or BoF) descriptor from given image
    bowDE.compute(img,keypoints,bowDescriptor);

    //prepare the yml (some what similar to xml) file
    sprintf(imageTag,"img1");
    //write the new BoF descriptor to the file
    fs1 << imageTag << bowDescriptor;

    //You may use this descriptor for classifying the image.

    //release the file storage
    fs1.release();
}
/////////////////////////////////////////////////////

//////////////// CrearDiccionariodeDirectorio ///////////////////
void BoW::CrearDiccionarioDirectorio(std::string dirName)
{
/*
    //float trainingData[4][2]= { {501, 10}, {255, 10}, {501, 255}, {10, 501} };
    float trainingData[4][2], x, y;
    x=501;y=10;
    trainingData[0][0] = x;
    trainingData[0][1] = y;
    x=255;y=10;
    trainingData[1][0] = x;
    trainingData[1][1] = y;
    x=501;y=255;
    trainingData[2][0] = x;
    trainingData[2][1] = y;
    x=10;y=501;
    trainingData[3][0] = x;
    trainingData[3][1] = y;

    Mat trainingDataMat(4, 2, CV_32FC1, trainingData);

    // Set up training data
    float etiquetas[4] = {1.0, -1.0, -1.0, -1.0};
    Mat etiquetasMat(4, 1, CV_32FC1, etiquetas);

*/
    Mat MatDescriptorSVM,MatLabelsSVM,featuresUnclustered,LabelsUnclustered;

    std::string file;
    string action;

    if(dirName.find_last_of("/") != std::string::npos)
            action = dirName.substr(dirName.find_last_of("/")+1);

    DIR *pDIR;
    struct dirent *entry;
    if(pDIR=opendir(dirName.c_str()))
    {
        while(entry=readdir((pDIR)))
        {
            if(strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name,".."))
            {
                string Name=entry->d_name;
                if(Name.substr(Name.find_first_of(".")+1) == "txt")
                {
                    file=dirName+"/"+Name;
                    const char *fileName = file.c_str();

                    FILE * fp;
                    char * line = NULL;
                    size_t len = 0;
                    ssize_t read;

                    fp = fopen(fileName, "r");
                    if (fp == NULL)
                       exit(EXIT_FAILURE);

                /*/////////////////////////////////////////////////////*/
                    int ch, number_of_lines = 0, number_of_colums = 0;
                    read = getline(&line, &len, fp);
                    char * pch;
                    pch = strtok (line,"\t");
                    while (pch != NULL)
                    {
                      number_of_colums++;
                      pch = strtok (NULL, "\t");
                    }
                    free(pch);
                    rewind(fp); //rewind the file to count the lines
                    do
                    {
                        ch = fgetc(fp);
                        if(ch == '\n')
                            number_of_lines++;
                    } while (ch != EOF);

                /*////////////////////////////////////////////////////*/
                    Mat MatLabels_TMP_SVM(number_of_lines,1,CV_32FC1);
                    Mat MatDescriptor_TMP_SVM(number_of_lines,number_of_colums,CV_32FC1);

                    std::map<float, std::string> actions;

                    // Initialize the map
                    actions.insert(std::make_pair(1, "boxing"));
                    actions.insert(std::make_pair(2, "handclapping"));
                    actions.insert(std::make_pair(3, "handwaving"));
                    actions.insert(std::make_pair(4, "jogging"));
                    actions.insert(std::make_pair(5, "running"));
                    actions.insert(std::make_pair(6, "walking"));
                    actions.insert(std::make_pair(7, "test_sequences"));

                    int rows=0;
                    int cols=0;
                    rewind(fp);

                    while ((read = getline(&line, &len, fp)) != -1)
                    {
                        //Extract parts separated by "\t"
                        char * pch;
                        pch = strtok (line,"\t");
                        while (pch != NULL)
                        {
                          //printf ("%s\n",pch);
                          float value=atof(pch);
                          MatDescriptor_TMP_SVM.at<float>(rows,cols)=value;
                          pch = strtok (NULL, "\t");
                          ++cols;
                        }

                        free(pch);
                        // Use the map
                        std::map<float, std::string>::iterator it = actions.begin();
                           while(it != actions.end())
                           {
                               if (it->second == action)
                               {
                                  MatLabels_TMP_SVM.at<float>(rows,0)=it->first;
                               }
                               it++;
                           }
                        ++rows;
                        cols=0;
                    }

                    fclose(fp);
                    if (line)
                        free(line);

                    featuresUnclustered.push_back(MatDescriptor_TMP_SVM);
                    LabelsUnclustered.push_back(MatLabels_TMP_SVM);

//                    MatDescriptorSVM=MatDescriptor_TMP_SVM.clone();
//                    MatLabelsSVM=MatLabels_TMP_SVM.clone();
                }
            }
        }
    }

/*//////////////////////////////////////////////
    Classifier SVM;
    //SVM.svm(featuresUnclustered,LabelsUnclustered,featuresUnclustered,LabelsUnclustered);
    //SVM.svm(descriptorSVM,MatLabelsSVM,trainingDataMat,etiquetasMat);
    //SVM.svm(trainingDataMat,etiquetasMat,descriptorSVM,MatLabelsSVM);
    SVM.svm(trainingDataMat,etiquetasMat,MatDescriptorSVM,MatLabelsSVM);
***********/

//Just in case featuresUnclustered not defined as <float>
    Mat descriptor;
    Mat labels;

    featuresUnclustered.convertTo(descriptor, CV_32FC1);
    LabelsUnclustered.convertTo(labels, CV_32FC1);
/*///////////////////////////////////////////////////////////*/

    //Construct BOWKMeansTrainer
    //the number of bags
    int dictionarySize=10;
    //define Term Criteria
    TermCriteria tc(CV_TERMCRIT_ITER,100,0.001);
    //retries number
    int retries=1;
    //necessary flags
    int flags=KMEANS_PP_CENTERS;
    //Create the BoW (or BoF) trainer
    BOWKMeansTrainer bowTrainer(dictionarySize,tc,retries,flags);
    //BOWKMeansTrainer bowTrainer(dictionarySize);
    bowTrainer.add(featuresUnclustered);
    //cluster the feature vectors
    Mat dictionary=bowTrainer.cluster(featuresUnclustered);
    Mat dictionary_labels=bowTrainer.cluster(LabelsUnclustered);
    //Mat dictionary=bowTrainer.cluster();
    //store the vocabulary
    //FileStorage fs("diccionario.yml", FileStorage::WRITE);
    FileStorage fs(dirName+"/"+action+".yml", FileStorage::WRITE);
    //fs << action.c_str() << featuresUnclustered;
    fs << action.c_str() << dictionary;
    fs.release();
    FileStorage fs2(dirName+"/"+action+"_Labels.yml", FileStorage::WRITE);
    //fs2 << action.c_str() << LabelsUnclustered;
    fs2 << action.c_str() << dictionary_labels;
    fs2.release();
}
////////////////////////////////////////////////

//////////////// CrearDiccionariodeDirectorio ///////////////////
void BoW::CrearDiccionarioAcciones(std::string dirName)
{
    std::string file;
    string action;
    Mat TrainingData,TrainingLabels;

    std::map<float, std::string> actions;

    // Initialize the map
    actions.insert(std::make_pair(1, "boxing"));
    actions.insert(std::make_pair(2, "handclapping"));
    actions.insert(std::make_pair(3, "handwaving"));
    actions.insert(std::make_pair(4, "jogging"));
    actions.insert(std::make_pair(5, "running"));
    actions.insert(std::make_pair(6, "walking"));
    actions.insert(std::make_pair(7, "test_sequences"));

    DIR *pDIR;
    struct dirent *entry;
    if(pDIR=opendir(dirName.c_str()))
    {
        while(entry=readdir((pDIR)))
        {
            if(strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name,".."))
            {
                string Name=entry->d_name;
                if(Name.substr(Name.find_first_of(".")+1) == "yml")
                {
                    file=dirName+"/"+Name;
                    action=Name.substr(0,Name.find_first_of("."));
                    Mat vocabulary, vocabulary_f;
                    FileStorage fs(file.c_str(), FileStorage::READ);
                        fs[action.c_str()] >> vocabulary;
                        vocabulary.convertTo(vocabulary_f, CV_32FC1);
                        fs.release();

                    Mat labels(vocabulary.rows,1,CV_32FC1);
                    int i=0;
                    while (i<=vocabulary.rows)
                    {
                        std::map<float, std::string>::iterator it = actions.begin();
                           while(it != actions.end())
                           {
                               if (it->second == action)
                               {
                                  labels.at<float>(i,0)=it->first;
                               }
                               it++;
                           }
                        i++;
                    }
                    TrainingData.push_back(vocabulary_f);
                    TrainingLabels.push_back(labels);
                }
            }
        }
    }
    Classifier SVM;
    SVM.svm(TrainingData,TrainingLabels,TrainingData,TrainingLabels);
}
////////////////////////////////////////////////
