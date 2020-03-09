#include <mpi.h>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <vector>

using namespace std;
class WorldManager {
private:
  int worldSize;
  int worldRank;
  char processorName[MPI_MAX_PROCESSOR_NAME];
  int nameLen;
  int* rgb;
  const int MAX = 127;
  const int IMAGE_WIDTH = 10000;
  const int IMAGE_HEIGHT = 8000;
  const double xMin = -2.5;
  const double xMax = 1.5;
  const double yMin = -2.0;
  const double yMax = 2.0;
  const double pWidth = (xMax - xMin) / IMAGE_WIDTH;
  const double pHeight = (yMax - yMin) / IMAGE_HEIGHT;
  int mandelbrot(double startReal, double startImag) {
    double zReal = startReal;
    double zImag = startImag;
    
    for (int counter = 0; counter < MAX; ++counter) {
      double r2 = zReal * zReal;
      double i2 = zImag * zImag;
      if (r2 + i2 > 4.0) {
	return counter;
      }
      zImag = 2.0 * zReal * zImag + startImag;
      zReal = r2 - i2 + startReal;
    }
    return MAX;
  }
  void setColor() {
    rgb = new int[128];
    for(int i = 0; i < 128; i++) {
      int r = 0, g = 0, b =0;
      getColor(r, g, b, i - 1);
      rgb[i] = (r << 16) + (g << 8) + b;
    }
  }
  void getColor(int &r, int &g, int &b, int iterations) {
    if (iterations == -1) {
      r = 0;
      g = 0;
      b = 0;
    } else if (iterations == 0) {
      b = 255;
      g = 0;
      r = 0;
    } else {
      // colour gradient:      Red -> Blue -> Green -> Red -> Black
      // corresponding values:  0  ->  16  ->  32   -> 64  ->  127 (or -1)
      if (iterations < 16) {
	b = 16 * (16 - iterations);
	g = 0;
	r = 16 * iterations - 1;
      } else if (iterations < 32) {
	b = 0;
	r = 16 * (iterations - 16);
	g = 16 * (32 - iterations) - 1;
      } else if (iterations < 64) {
	b = 8 * (iterations - 32);
	r = 8 * (64 - iterations) - 1;
	g = 0;
      } else { // range is 64 - 127
	b = 255 - (iterations - 64) * 4;
	g = 0;
	r = 0;
      }
    }
  }
public:
  WorldManager() {
    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Get_processor_name(processorName, &nameLen);
    setColor();
  }
  
  void run() {
    
    double totaltime = 0;
    if(worldRank == 0);
     totaltime -= MPI_Wtime();
    int totalNum = IMAGE_WIDTH * IMAGE_HEIGHT;
    int* task = nullptr;
    int rangeNum = totalNum / worldSize;
    int startEnd[2];
    if(worldRank == 0) {
      task = new int[worldSize * 2];
      for(int i = 0; i < worldSize; i++) {
	task[i * 2] =  rangeNum * i;
	task[i * 2 + 1] = rangeNum * (i + 1);
      }
    }
    MPI_Scatter(task, 2, MPI_INT, startEnd, 2, MPI_INT, 0, MPI_COMM_WORLD);
    const int start = startEnd[0];
    const int end = startEnd[1];
    
    
    int* res = new int[rangeNum];

    // x x x x x x y x x x x x x x x x
    // x x x x x x x x x x x x x x x x

    //    double x = i % ... ;  dx = ____
    // for (int i = 0; i < cols; i++, x += dx)
    
    for(int i = start, j = 0; i < end; i++, j++) {
      int value = mandelbrot((i % IMAGE_WIDTH - IMAGE_HEIGHT / 2.0) * pHeight, (i / IMAGE_WIDTH - IMAGE_WIDTH / 2.0) *  pWidth);
      res[j] = rgb[value]; 
    }
    
     
    if(worldRank != 0) task = nullptr;
    if(worldRank == 0) task = new int[totalNum];
    
    MPI_Gather(res, rangeNum, MPI_INT, task, rangeNum, MPI_INT, 0, MPI_COMM_WORLD);
    
    if(worldRank == 0) {
      totaltime += MPI_Wtime();
      cout << totaltime << endl;
    }
    
    if(worldRank == 0) {
      ofstream of("mandelbrot.ppm");
      of << "P3 " << IMAGE_WIDTH << " " << IMAGE_HEIGHT << " " << 255 << " " ;
      for (int i = 0; i < totalNum; i++) {
	of << ((task[i] >> 16) & 0x0000FF)  << " "
	   << ((task[i] >> 8) & 0x0000FF)  << " "
	   << ((task[i]) & 0x0000FF)  << " ";
      }
    }
    delete[] task;
    delete[] res;
  }

  ~WorldManager() {
    delete[] rgb;
    MPI_Finalize();
  }
};

int main(int argc, char** argv) {
  WorldManager wm;
  
  wm.run();
  
}
