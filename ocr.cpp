/*A character recognizer that uses neural nets

TODO: YOUR NAME HERE, 10/2014

assignment and helper code by Michael Black, 10/2014

TODO:  
	YOUR CODE WILL GO IN FUNCTIONS test() AND train()
	HERE STATE WHAT STEPS YOU ACCOMPLItestSHED

usage:
ocr sample X
	pops up a window, user draws an example of an X, user doubleclicks and the X is saved for later
ocr train
	builds a neural net for each letter type, trains each of them on the samples until they predict perfectly
ocr test
	pops up a window, user draws a letter and doubleclicks, the program tries to guess which letter was drawn
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <QtGui>
#include <math.h>
//graphics defs here
#include "ocr.h"
//neural network is defined here.  assumes neural.cpp is in the same directory too.
#include "neural.h"

using namespace std;
int* getSquares();

//some global vars:
//squares wide
const int GRIDWIDTH=10;
//squares tall
const int GRIDHEIGHT=20;
//the window is this many pixels wide
const int SCREENWIDTH=400;
//and pixels tall
const int SCREENHEIGHT=400;
//an array of clickable grid squares
Square* square[GRIDWIDTH][GRIDHEIGHT];
//the window background
QGraphicsScene* thescene;
OCRView* view;

//keep track of the command line input
//operation is "sample" "train" or "test"
//symbol is the letter entered when the operation is "sample"
string operation,symbol;

//called immediately on "ocr train"
//reads the images in ocrdata.txt, builds a set of neural nets, trains them, and saves the weights to perceptron.txt
void train()
{
	//read the images from file ocrdata.txt
	ifstream datafile;	//file object
	string line;		//lines will go here
	datafile.open("ocrdata.txt");	//open the file
	//stop the program if the file isn't found
	if (!datafile.is_open())
	{
		cout<<"Couldn't open ocrdata.txt"<<endl;
		return;
	}
	int linecount=0;	//keep track of how many samples are in the file
	//go through the file and just count the number of samples
	while(getline(datafile,line))
	{
		linecount++;
	}
	//close the file.  we'll reopen it in a moment.
	datafile.close();

	//make an array to hold the samples
	int sample_input[linecount][GRIDWIDTH*GRIDHEIGHT];
	//make another array to hold the output letter for each sample
	char sample_output[linecount];
	//reopen the file
	datafile.open("ocrdata.txt");
	//for each sample,
	for(int i=0; i<linecount; i++)
	{
		//read it from the file
		getline(datafile,line);
		//the first character is the output letter
		sample_output[i]=line[0];
		//then a space, then a 1 or 0 for each square on the screen
		for (int j=0; j<GRIDWIDTH*GRIDHEIGHT; j++)
		{
			sample_input[i][j]=line[j+2]=='1'?1:0;
		}
	}
	//now we're done with ocrdata.txt
	datafile.close();

//TODO: MAKE SOME NEURAL NETS AND TRAIN THEM HERE, THEN SAVE THE WEIGHTS TO perceptron.txt

	//create array of possible letters
	char letterarray[26];
	//number of unique letters
	int uniqueletters = 0;
	for(int k = 0; k < linecount; k++)
	{
		for(int m = 0; m < 26; m++)
		{
			if(sample_output[k] == letterarray[m])
				break;
			if(sample_output[k] != letterarray[m])
			{
				if(letterarray[m] == NULL)
				{
					letterarray[uniqueletters] = sample_output[k];
					uniqueletters += 1;
					break;
				}
			}
		}
	}
	
	//Instantiate and fill perceptron array 
	Perceptron* perceptronarray[uniqueletters];
	for (int n = 0; n<uniqueletters; n++)
		perceptronarray[n] = new Perceptron(200);



	//Begin training of perceptrons
	cout<<"Begin training..."<<endl;
	for(int p = 0; p < uniqueletters; p++)
	{
		char trainedletter = letterarray[p];
		for (int q=0; q<100000; q++)
		{
			bool desired = false;
			bool trained = true;
			for(int r = 0; r < linecount; r++)
			{
				if(sample_output[r] == trainedletter)
				{
					desired = true;
				
				}
				else
				{
					desired = false;
				}
				if (perceptronarray[p]->train(sample_input[r], desired) == false)
				{
					perceptronarray[p]->train(sample_input[r], desired);
					trained = false;
				}
			}
			if (trained == true)
				break;
		}
	}
	cout<<"...training complete."<<endl;
	

	//saving perceptron weights
	ofstream myfile;
	myfile.open("perceptron.txt", ios::out | ios::trunc);
	
	cout<<"Begin writing weight file..."<<endl;
	for (int s = 0; s < uniqueletters; s++)
	{
		for (int t = 0; t < perceptronarray[s]->size+1; t++)
		{
			myfile<<perceptronarray[s]->outputweight[t]<<endl;	
		}

		for (int u = 0; u < perceptronarray[s]->size; u++)
		{
			for(int v=0; v < perceptronarray[s]->size; v++)
			{
				myfile<<perceptronarray[s]->hiddenweight[u][v]<<endl;
			}
		
		}
	}
	myfile.close();
	cout<<"...weight file complete."<<endl;

}


//called on "ocr test", after the user draws and double-clicks the mouse
void test()
{

	ifstream openfile;
	openfile.open("perceptron.txt", ios::in);
	
	Perceptron* testperceptronarray[10];
	for(int g =0; g<10; g++)
		testperceptronarray[g] = new Perceptron(200);
	
	for(int h = 0; h<10; h++)
	{
		for(int k = 0; k<201; k++)
			openfile>>testperceptronarray[h]->outputweight[k];

		for (int i = 0; i<200; i++)
		{
			for(int j=0; j<200; j++)
				openfile>>testperceptronarray[h]->hiddenweight[i][j];
		}
	}

	int* predictionSquares=getSquares();
	
	float prediction0 = testperceptronarray[0]->getRawPrediction(predictionSquares);
	float prediction1 = testperceptronarray[1]->getRawPrediction(predictionSquares);
	float prediction2 = testperceptronarray[2]->getRawPrediction(predictionSquares);
	float prediction3 = testperceptronarray[3]->getRawPrediction(predictionSquares);
	float prediction4 = testperceptronarray[4]->getRawPrediction(predictionSquares);
	float prediction5 = testperceptronarray[5]->getRawPrediction(predictionSquares);
	float prediction6 = testperceptronarray[6]->getRawPrediction(predictionSquares);
	float prediction7 = testperceptronarray[7]->getRawPrediction(predictionSquares);
	float prediction8 = testperceptronarray[8]->getRawPrediction(predictionSquares);
	float prediction9 = testperceptronarray[9]->getRawPrediction(predictionSquares);
	
	cout<<"the prediction for A is:..."<<prediction0<<endl;
	cout<<"the prediction for D is:..."<<prediction1<<endl;
	cout<<"the prediction for X is:..."<<prediction2<<endl;
	cout<<"the prediction for E is:..."<<prediction3<<endl;
	cout<<"the prediction for N is:..."<<prediction4<<endl;
	cout<<"the prediction for R is:..."<<prediction5<<endl;
	cout<<"the prediction for S is:..."<<prediction6<<endl;
	cout<<"the prediction for T is:..."<<prediction7<<endl;
	cout<<"the prediction for B is:..."<<prediction8<<endl;
	cout<<"the prediction for G is:..."<<prediction9<<endl;
	
	
	
	

//TODO: MAKE SOME NEURAL NETS, READ THE WEIGHTS FROM A FILE perceptron.txt, USE THE NEURAL NETS TO IDENTIFY THE LETTER


	openfile.close();
}

//read the contents of the grid and save them to the end of ocrdata.txt
void saveSample()
{
	ofstream datafile;
	datafile.open("ocrdata.txt",ios::out|ios::app);
	datafile << symbol << " ";
	int* s=getSquares();
	for(int i=0; i<GRIDWIDTH*GRIDHEIGHT; i++)
		datafile<<s[i];
	datafile << endl;
	datafile.close();
	cout << "Wrote sample to ocrdata.txt" << endl;
}

//determines what function is called when the user double clicks the window, based on the command line input
void doOperation()
{
	if(operation.compare("sample")==0)
		saveSample();
	else if (operation.compare("train")==0)
		train();
	else if (operation.compare("test")==0)
		test();
}

//returns an integer array of 0s and 1s that represents what the user drew on the window
int* getSquares()
{
	int* s=new int[GRIDWIDTH*GRIDHEIGHT];
	for(int i=0; i<GRIDWIDTH; i++)
		for(int j=0; j<GRIDHEIGHT; j++)
			s[i*GRIDHEIGHT+j]=square[i][j]->selected?1:0;
	return s;
}

//constructor for a grid square
Square::Square(int x, int y)
{
	xpos=x; ypos=y;
	xcoor1=x*SCREENWIDTH/GRIDWIDTH;
	ycoor1=y*SCREENHEIGHT/GRIDHEIGHT;
	xcoor2=(x+1)*SCREENWIDTH/GRIDWIDTH;
	ycoor2=(y+1)*SCREENHEIGHT/GRIDHEIGHT;
	selected=FALSE;
}

//location of the square on the screen, used for rendering
QRectF Square::boundingRect() const
{
	return QRectF(xcoor1,ycoor1,xcoor2-xcoor1,ycoor2-ycoor1);
}

//called when the user selects the square
void Square::click()
{
	selected=TRUE;
	update();
}

//render the square on the screen
void Square::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	QColor color;
	if(selected)
	{
		//if the user clicked it, draw it purple
		color.setBlue(100); color.setRed(100); color.setGreen(0);
	}
	else
	{
		//otherwise draw it white
		color.setBlue(255); color.setRed(255); color.setGreen(255);
	}
	painter->setBrush(color);
	painter->drawRect(xcoor1,ycoor1,xcoor2-xcoor1,ycoor2-ycoor1);
}

OCRView::OCRView(QGraphicsScene *scene, QWidget* parent):QGraphicsView(scene, parent)
{
	//the mouse is initially assumed to be unpressed
	isPressed=FALSE;
}

//the mouse has been pressed or dragged
//figure out which grid square was selected and call it
void squareClickEvent(QMouseEvent *event)
{
	int x=(event->x()*GRIDWIDTH/SCREENWIDTH);
	int y=(event->y()*GRIDHEIGHT/SCREENHEIGHT);
	if(x<0 || y<0 || x>=GRIDWIDTH || y>=GRIDHEIGHT) return;
	square[x][y]->click();
}

//if the mouse button is down and the user moves over a square, select it
void OCRView::mouseMoveEvent(QMouseEvent *event)
{
	if(!isPressed) return;
	squareClickEvent(event);
}

//if the user presses the mouse on a square, select it
void OCRView::mousePressEvent(QMouseEvent *event)
{
	isPressed=TRUE;
	squareClickEvent(event);
}

//the mouse is released, stop selecting squares
void OCRView::mouseReleaseEvent(QMouseEvent *event)
{
	isPressed=FALSE;
}

//double click means that we're done drawing
void OCRView::mouseDoubleClickEvent(QMouseEvent *event)
{
	if(event->button() == Qt::RightButton)
	{
		// a right-button double click just exits the program
		exit(0);
	}
	if(event->button() == Qt::LeftButton)
	{
		//a left-button double click saves the sample or tests, depending on the command line parameters
		doOperation();
		exit(0);
	}
}

//program starts here
int main(int argc, char **argv)
{
	//save the command line arguments
	if(argc>=2)
		operation=argv[1];
	if(argc==3)
		symbol=argv[2];
	else
		symbol=" ";
	//if the user didn't give any, print out some help and stop
	if(argc==1)
	{
		cout << "Usage: " << endl;
		cout << " ocr sample A" << endl;
		cout << " ocr train" << endl;
		cout << " ocr test" << endl;
		exit(0);
	}

	//seed the random number generator
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

	//we don't need to make a window if the user selects "train"
	if(operation.compare("train")==0)
	{
		train();
		exit(0);
	}

	//make a window
	QApplication app(argc,argv);

	thescene=new QGraphicsScene();
	thescene->setSceneRect(0,0,SCREENWIDTH+100,SCREENHEIGHT+100);

	//add in all the squares
	for(int i=0; i<GRIDWIDTH; i++)
	{
		for(int j=0; j<GRIDHEIGHT; j++)
		{
			square[i][j] = new Square(i,j);
			thescene->addItem(square[i][j]);
		}
	}

	view=new OCRView(thescene);
	view->setWindowTitle("Neural OCR");
	view->resize(SCREENWIDTH+100,SCREENHEIGHT+100);
	view->show();
	view->setMouseTracking(true);

	//make the window visible.  we're done with setup.
	return app.exec();
}
