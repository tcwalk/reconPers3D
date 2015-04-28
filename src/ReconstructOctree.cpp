#include "include/ReconstructOctree.h"
#include <ctime>
#include <math.h>
#include <string>
#include "include/para.h"

using namespace std;

ReconstructOctree::ReconstructOctree(int maxNode, Point fullSize, Point endSize, 
									 string sil_prefix, int nfile, 
//									 string file_ext, string file_conf,
                                     string file_ext,
                                     int distortion_radius,
                                   int rotation_digits, float fxOverDelta)
	:Octree(maxNode, fullSize, endSize)
{
        cout << "Loading " << nfile << " rotation images with prefix: " <<  sil_prefix << endl;
	readSilhouettes(sil_prefix, nfile, file_ext, rotation_digits);
    // tw 2015feb5
//    readConfiguration(file_conf);
    readConfiguration();
    this->distortion_radius = distortion_radius;
	cout << "Loaded all the images." << endl;
}

void ReconstructOctree::readSilhouettes(string file_prefix, int nfile, 
                                        string file_ext,
                                        int rotation_digits)
{
    silhouette_stat = new int** [nfile];

    char buffer[10];
    char buffer2[10];
	int i, j, count;
	int *iter;
	int **counts;
	for (int file = 0; file < nfile; file++)
	{
            // read bmp image
            int file_num = file+1;
            cout << "load #" << file_num << " image" << endl;
            string filename = file_prefix;
            sprintf(buffer, "%d", file+1);
        
            switch (rotation_digits)
            {
              case 2:
                if (file < 9) filename.append("0");
                break;
              case 3:
                if (file < 9) filename.append("00");
                if (file >= 9 && file < 99) filename.append("0");
                break;
              default:
                //msg= 'Only two or three digits allowed for rotation number';
                cout << "Only two or three digits allowed for rotation number" << endl;
                sprintf(buffer2, "%d", rotation_digits);
            }
        
            filename.append(buffer);
            filename.append(file_ext);

            bool** myimg = readBlackWhiteImage(filename, image_height, image_width);

            //    count #. 1's
            counts = silhouette_stat[file] = new int* [image_height];

            i = 0;
            iter = counts[i] = new int [image_width];
            count = 0;
            for (j = 0; j < image_width; j++)
            {
                if (myimg[i][j] == 1) count++;
                *iter = count;
                iter++;
            }

            for (i = 1; i < image_height; i++)
            {
               iter = counts[i] = new int [image_width];
               count = 0;
               for (j = 0; j < image_width; j++)
               {
                    if (myimg[i][j] == 1) count++;
                    *iter = counts[i-1][j] + count;
                    iter++;
                }
            }
    }

	nimages = nfile;
	image_center_x = image_height / 2.0f;
	image_center_y = image_width / 2.0f;
}

bool** ReconstructOctree::readBlackWhiteImage(string filename, int& n, int& m)
{
	CImg<bool> image(filename.c_str());
	n = image.height();
	m = image.width();
	bool **myimg = new bool*[n];
	CImg<bool>::iterator iter = image.begin();
	bool *myiter;
	for (int i = 0; i < n; i++) 
	{
		myiter = myimg[i] = new bool[m];
		for (int j = 0; j < m; j++)
		{
			*myiter = *iter;
			myiter++;
			iter++;
		}
	}
/*
///////////////////////////////////////////////////////
// DEBUG
//////////////////////////////////////////////////////
string filename2="/data/rsa/processed_images/rice/RIL/p00001/d12/sandbox/rootwork_3d/ct103_2013-06-20_15-10-53/silhouette3/OsRILp00001d12_07.bmp";


	CImg<bool> image2(filename2.c_str());
	int n2 = image2.height();
	int m2 = image2.width();
	bool **myimg2 = new bool*[n2];
	CImg<bool>::iterator iter2 = image2.begin();
	bool *myiter2;
	for (int i = 0; i < n2; i++) 
	{
		myiter2 = myimg2[i] = new bool[m2];
		for (int j = 0; j < m2; j++)
		{
			*myiter2 = *iter2;
			myiter2++;
			iter2++;
		}
	}
        //compare
        int count = 0;
	for (int i = 0; i < n2; i++) 
	{
		for (int j = 0; j < m2; j++)
		{
                   if(myimg[i][j] != myimg2[i][j])
                   {
                     count++;
                   }       
		}
	}
        cout << "count = " << count << endl;

///////////////////////////////////////////////////////
*/

	return myimg;
}

int ReconstructOctree::getSilStat(int image, int lr, int lc, int hr, int hc)
{
	// check image index
	if (image<0 || image>=nimages) return 0;

	// check boundary
	if (lr>hr || lc>hc) return 0;
	if (lr < 0) lr = 0;
	if (lc < 0) lc = 0;
	if (hr < 0) hr = 0;
	if (hc < 0) hc = 0; 
	if (lr>=image_height) lr = image_height-1;
	if (hr>=image_height) hr = image_height-1;
	if (hc>=image_width) hc = image_width-1;
	if (lc>=image_width) lc = image_width-1;

	int** iter = silhouette_stat[image];
	int ret = iter[hr][hc];
	if (lr>0 && lc>0)
		ret += iter[lr-1][lc-1];
	if (lr>0)
		ret -= iter[lr-1][hc];
	if (lc>0)
		ret -= iter[hr][lc-1];
	return ret;
}

// tw 2015feb5
//void ReconstructOctree::readConfiguration(string file_conf)
void ReconstructOctree::readConfiguration()
{

    cout << "enter readConfiguratin" << endl;
    Rs = new float* [nimages];
    int i;
    float *iter_rs;

    std::vector <std::vector <double> > img_para;


    switch ( nimages*2 )
    {
       case 12:
          img_para = para12;
          break;
       case 14:
          img_para = para15;
          break;
       case 15:
          img_para = para15;
          break;
       case 16:
          img_para = para15;
          break;
       case 18:
          img_para = para18;
          break;
       case 20:
          img_para = para20;
          break;
       case 24:
          img_para = para24;
          break;
       case 30:
          img_para = para30;
          break;
       case 36:
          img_para = para36;
          break;
       case 40:
          img_para = para40;
          break;
       case 45:
          img_para = para45;
          break;
       case 60:
          img_para = para60;
          break;
       case 72:
          img_para = para72;
          break;
       case 90:
          img_para = para90;
          break;
       case 120:
          img_para = para120;
          break;
       case 180:
          img_para = para180;
          break;
       case 360:
          img_para = para360;
          break;
       default:
          cout << "no parameter set found for number of images input\n";
          cout << "total number of images needs to be a divisor of 360\n";
          cout << "Using default parameters for 60 image set\n";
          img_para = para60;
          break;
    }


    for (int image = 0; image < nimages; image++)
    {
        iter_rs = Rs[image] = new float [2];
        for (i = 0; i < 2; i++)
        {
           *iter_rs = img_para[image][i];
           iter_rs++;
        }
    }



/*
    ifstream myfile(file_conf.c_str());

    //	get translation and rotation matrix
    Rs = new float* [nimages];
    int i;
    float *iter_rs;
    float tmp;
    for (int image = 0; image < nimages; image++)
    {
        iter_rs = Rs[image] = new float [2];
        for (i = 0; i < 2; i++)
        {
                       myfile >> *iter_rs;
                       iter_rs++;
                }
    }
    myfile.close();
*/


/*
    ifstream myfile(file_conf.c_str());

	//	get translation and rotation matrix
	Rs = new float* [nimages];
	int i;
	float *iter_rs;
	float tmp;
	for (int image = 0; image < nimages; image++)
	{
		iter_rs = Rs[image] = new float [2];
		for (i = 0; i < 2; i++)
		{


                       myfile >> *iter_rs;

//			myfile >> tmp;
//                          cout << "tmp = " << tmp << endl;
//                       *iter_rs = tmp;
//                          cout << "*iter_rs = " << *iter_rs << endl;
                       iter_rs++;

                }
	}
	myfile.close();
*/

}

unsigned int ReconstructOctree::check(Point lp, Point hp, int &radius, int &nImagesConsistent)
{
	// check whether lp is a lower point than hp
	if (lp.x >= hp.x || lp.y >= hp.y || lp.z >= hp.z) {
		nImagesConsistent = 0; 
		return 0;
	}
	
	int x[2] = {lp.x, hp.x};
	int y[2] = {lp.y, hp.y};
	int z[2] = {hp.z, lp.z};
	
    int lr=0, rr=0, lc=0, rc=0, count;
    int tr=0, br=0, tc=0, bc=0;
    int nfail = 0;
    bool isFull = true;
    int r;

    radius = 0;
    nImagesConsistent = nimages;
    for (int image = 0; image < nimages; image++)
    {
        PerspectiveProject(image, Point(x[chooselx[image]], y[choosely[image]], z[chooselz[image]]), lr, lc);
        PerspectiveProject(image, Point(x[chooserx[image]], y[choosery[image]], z[chooserz[image]]), rr, rc);
        PerspectiveProject(image, Point(x[choosetx[image]], y[choosety[image]], z[choosetz[image]]), tr, tc);
        PerspectiveProject(image, Point(x[choosebx[image]], y[chooseby[image]], z[choosebz[image]]), br, bc);
        if (getSilStat(image, tr, lc, br, rc)==0) nImagesConsistent--;
        for  (r = 0; r <= distortion_radius; ++r) {
            count = getSilStat(image, tr-r, lc-r, br+r, rc+r);
            if (count > 0) break;
        }
        if (count == 0) return 0;
        if (isFull && count < (br-tr+1)*(rc-lc+1))
            isFull = false;
        radius += r;

    }

	if (isFull) return 1;
	return 2;
}

void ReconstructOctree::PerspectiveProject(int image, Point p, int& row, int& col)
{
    float *iter_rs;
    iter_rs = Rs[image];
    float tempX = p.x - halfSize.x;
    float tempY = p.y - halfSize.y;
    float tempZ = -(p.z - halfSize.z);
    float tempFxOverDeltaOverXPlus = fxOverDelta / (iter_rs[1] * tempX - iter_rs[0] * tempY + fxOverDelta);

    row = (int)floor(scales[0]*tempZ*tempFxOverDeltaOverXPlus + image_center_x);
    col = (int)floor(scales[1]*tempFxOverDeltaOverXPlus*(iter_rs[0] * tempX
                        + iter_rs[1] * tempY) + image_center_y);
}

void ReconstructOctree::setResolution(Point cubeSize)
{
	this->setCubesize(cubeSize);
	halfSize = cubeSize/2;
	scales[0] = 1.0f * fullSize.z / cubeSize.z;
	scales[1] = 1.0f * fullSize.x / cubeSize.x;
	//cout << cubeSize.x << " " << cubeSize.y << " " << cubeSize.z << " " 
	//	<< scales[0] << " " << scales[1] << endl;

	//	compute which to choose to form a rectangle backprojected on the image
    choosetx = new bool[nimages];
    choosety = new bool[nimages];
    choosetz = new bool[nimages];
    choosebx = new bool[nimages];
    chooseby = new bool[nimages];
    choosebz = new bool[nimages];
    chooselx = new bool[nimages];
    choosely = new bool[nimages];
    chooselz = new bool[nimages];
    chooserx = new bool[nimages];
    choosery = new bool[nimages];
    chooserz = new bool[nimages];

    int x[2] = {0, fullSize.x};
    int y[2] = {0, fullSize.y};
    int z[2] = {0, fullSize.z};
    int minc = 0, maxc = 0, minr = 0, maxr = 0;
    int row=0, col=0;
    for (int image = 0; image < nimages; image++)
    {
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++)
                {
                    PerspectiveProject(image, Point(x[i], y[j], z[k]), row, col);
                    if (i == 0 && j == 0 && k == 0 || col < minc)
                    {
                        minc = col;
                        chooselx[image] = (bool)i;
                        choosely[image] = (bool)j;
                        chooselz[image] = (bool)k;
                    }
                    if (i == 0 && j == 0 && k == 0 || col > maxc)
                    {
                        maxc = col;
                        chooserx[image] = (bool)i;
                        choosery[image] = (bool)j;
                        chooserz[image] = (bool)k;
                    }
                    if (i == 0 && j == 0 && k == 0 || row < minr)
                    {
                        minr = row;
                        choosetx[image] = (bool)i;
                        choosety[image] = (bool)j;
                        choosetz[image] = (bool)k;
                    }
                    if (i == 0 && j == 0 && k == 0 || row > maxr)
                    {
                        maxr = row;
                        choosebx[image] = (bool)i;
                        chooseby[image] = (bool)j;
                        choosebz[image] = (bool)k;
                    }
                }
    }
    //cout << "setResolution end" << endl;
}

void ReconstructOctree::dropImage()
{
	for (int i = 0; i < nimages; ++i) {
		for (int j = 0; j < image_height; ++j)
			delete[] silhouette_stat[i][j];
		delete[] silhouette_stat[i];
	}
	delete[] silhouette_stat;
}

bool ReconstructOctree::addConsistency(int refImageID, int* consistency, int* reliability, Point* points, int npts, double ratio)
{
	int i, j;
	int row, col;
	int id;
	int count1, count2;
	Point* p_point = points;
	int x[2], y[2], z[2];
	int lr, hr, lc, hc;
	int tmp;

	int** bestMatch = new int*[image_height];
	for (i = 0; i < image_height; i++) {
		bestMatch[i] = new int[image_width];
		for (j = 0; j < image_width; j++)
			bestMatch[i][j] = -1;
	}

	count1 = 0;
	for (i = 0; i < image_height; i++)
		for (j = 0; j < image_width; j++)
			if (getSilStat(refImageID, i, j, i, j)>0) count1++;
	count1 = (int)floor(count1*ratio);
//	cout << count1 << " ";

	int d = 1;
	count2 = 0;
	for (i = 0; i < npts; i++, p_point++) {
		x[0] = p_point->x; x[1] = p_point->x+1;
		y[0] = p_point->y; y[1] = p_point->y+1;
		z[0] = p_point->z; z[1] = p_point->z+1;

        PerspectiveProject(refImageID, Point(x[chooselx[refImageID]], y[choosely[refImageID]], z[0]), lr, lc);
        PerspectiveProject(refImageID, Point(x[chooserx[refImageID]], y[choosery[refImageID]], z[1]), hr, hc);

		if (lr>hr) {tmp = lr; lr = hr; hr = tmp;}
		if (lc>hc) {tmp = lc; lc = hc; hc = tmp;}
		lr -= d; hr += d;
		lc -= d; hc += d;
		if (lr<0) lr = 0;
		if (lc<0) lc = 0;
		if (hr>=image_height) hr = image_height-1;
		if (hc>=image_width) hc = image_width-1;

		for (row = lr; row <= hr; row++)
			for (col = lc; col <= hc; col++)
				if (getSilStat(refImageID, row, col, row, col)>0) {
					id = bestMatch[row][col];
					if (id == -1) count2++;
					if (id == -1 || reliability[i] < reliability[id] ||
						reliability[i] == reliability[id] && consistency[i] > consistency[id])
					{
						bestMatch[row][col] = i;
					}
				}
	}

	int count3 = 0;
	if (count2 >= count1) {
		for (i = 0; i < image_height; i++)
			for (j = 0; j < image_width; j++)
			{
				id = bestMatch[i][j];
				if (id == -1) continue;
				reliability[id] = 0; 
				consistency[id] = 20;
				count3++;
			}
	}
//	cout << count3 << " " ;

//	cout << count2 << endl;
	for (i = 0; i < image_height; i++) delete[] bestMatch[i];
	delete[] bestMatch;

	return count2 >= count1;
}

void testOctree(ReconstructOctree *mytree, Point cubeSize) 
{
	mytree->setResolution(cubeSize);
	clock_t starttime = clock();
	int feedback = mytree->construct();
	if (feedback == -1)
	{
		cout << "please increase #. available octree nodes." << endl;
		exit(-1);
	}
	clock_t endtime = clock();
	cout << "Total time: " << (endtime-starttime)*1.0/CLOCKS_PER_SEC << " seconds." << endl;
}
