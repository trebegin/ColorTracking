// Trent Begin
// Computer Vision
// color_tracking.cpp

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int THRESHOLD = 0;
int TRACK_THRESHOLD = 120;

Mat median(Mat frame);

int main(int argc, char** argv)
{
	// Opens video
    VideoCapture video("Light-Room-Laser-Spot-with-Clutter.mpeg");
    VideoWriter output;
    
    // Creates the output video file
	Size S = Size((int) video.get(CV_CAP_PROP_FRAME_WIDTH), 
				  (int) video.get(CV_CAP_PROP_FRAME_HEIGHT));

    output.open("color_laser_tracking.mpeg", CV_FOURCC('M','P','E','G'), 
					video.get(CV_CAP_PROP_FPS), S, true);
    
    // Checks that the videos are acceptable
    if(!video.isOpened())
	{
		cout << "Could not open video" << endl;
		return -1;
	}
	
	namedWindow("test", WINDOW_AUTOSIZE);

	// Gets frames from both videos
	Mat frame, prevFrame;
	video >> prevFrame;
	video >> frame;
	
	//prevFrame = median(prevFrame);
	//frame = median(frame);
	
	Point maxPt, prevPt;
	
	while(!frame.empty())
	{
		Mat RGB[3], prevRGB[3], outputRGB[3];
	
		// Splits frame into RGB channels
		split(prevFrame, prevRGB);
		split(frame, RGB);
		
		// Initializes outputRGB
		split(frame, outputRGB);
		
		int maxValue = 0, value = 0;
		maxPt = Point(0,0);
		
		for(int i = 1; i < frame.rows - 1; i++)
		{
			// Gets rows for each channel on both frames
			unsigned char * blueP = prevRGB[0].ptr<unsigned char>(i);
			unsigned char * blue = RGB[1].ptr<unsigned char>(i);
			
			unsigned char * greenP = prevRGB[1].ptr<unsigned char>(i);
			unsigned char * green = RGB[1].ptr<unsigned char>(i);
			
			unsigned char * redP = prevRGB[2].ptr<unsigned char>(i);
			unsigned char * red = RGB[2].ptr<unsigned char>(i);
		    
		    
			unsigned char * blueOut = outputRGB[0].ptr<unsigned char>(i);
			unsigned char * greenOut = outputRGB[1].ptr<unsigned char>(i);
			unsigned char * redOut = outputRGB[2].ptr<unsigned char>(i);
			
			for(int j = 1; j < frame.cols - 1; j++)
			{
				value = 0;
				
				// Calculates the difference in values above the threshold
				if(abs(blue[j] - blueP[j]) > THRESHOLD)
				{
					blueOut[j] = abs(blue[j] - blueP[j]);
					//value += blueOut[j];
				}
				else
					blueOut[j] = 0;
				
				
				if(abs(green[j] - greenP[j]) > THRESHOLD)
				{
					greenOut[j] = abs(green[j] - greenP[j]);
					value += greenOut[j];
				}
				else
					greenOut[j] = 0;
				
				if(abs(red[j] - redP[j]))
				{
					redOut[j] = abs(red[j] - redP[j]);
					//value += redOut[j];
				}
				else
					redOut[j] = 0;
				
				if(value > maxValue and value > TRACK_THRESHOLD)
				{
					maxValue = value;
					maxPt.x = j;
					maxPt.y = i;
				}
			}
		}
		
		// Creates a vector<Mat> of the channles
		
		/*
		vector<Mat> vectorRGB;
		vectorRGB.push_back(outputRGB[0]);
		vectorRGB.push_back(outputRGB[1]);
		vectorRGB.push_back(outputRGB[2]);
		
		*/
		// Merges and writes frame to output 
		Mat outFrame, RGBFrame;
		frame.copyTo(outFrame);
		//merge(vectorRGB, RGBFrame);
		
		if(maxPt.x == 0 and maxPt.y == 0)
		{
			maxPt = Point(prevPt.x, prevPt.y);
		}
		
		prevPt = Point(maxPt.x, maxPt.y);
		
		// Draws two white lines at the max value on the full video
		Scalar white = Scalar(255, 255, 255);
		
		line(outFrame, Point(0, maxPt.y), Point(frame.cols, maxPt.y), 
				white, 2, 8, 0);
		
		line(outFrame, Point(maxPt.x, 0), Point(maxPt.x, frame.rows), 
				white, 2, 8, 0);
		
		/*
		imshow("test", outFrame);
		waitKey(0);
		*/
		output.write(outFrame);
		
		
		// stores the previous frame to the next frame
		frame.copyTo(prevFrame);
		video >> frame;
	}
	
    return 0;
}

Mat  median(Mat frame)
{   
    // Split image by channel
    Mat output;
	Mat rgb[3];
	split(frame, rgb);
	Mat green = rgb[1];
	
	green.copyTo(output);
	
	int hist [255] = {0};
	
    for(int i = 1; i < (green.rows) - 1; i++)
    {
		// Grab rows from frame
		unsigned char * row1 = green.ptr<unsigned char>(i-1);
		unsigned char * row2 = green.ptr<unsigned char>(i);
		unsigned char * row3 = green.ptr<unsigned char>(i+1);
		
		unsigned char * outRow = output.ptr<unsigned char>(i);
		
		for(int j = 1; j < (green.cols) - 1; j++)
		{
			// Sample neighboring pixel values
			hist[row1[j-1]]++;
			hist[row1[j]]++;
			hist[row1[j+1]]++;
			hist[row2[j-1]]++;
			hist[row2[j]]++;
			hist[row2[j+1]]++;
			hist[row3[j-1]]++;
			hist[row3[j]]++;
			hist[row3[j+1]]++;
			
			int sum = 0;
			int k = 0;
			
			// Determine Median value
			while(sum < 5)
			{
				sum += hist[k];
				k++;
			}
			
			// Set output to Median value
			outRow[j] = k-1;
			
			// Reset Historgram
			for(int i = 0; i < 256; i++)
			{
				hist[i] = 0;
			}
		}	
	}
	
	return output;
  
} 


