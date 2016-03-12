#include "PatchMatch.h"
#include <iostream>
#include <fstream>
/* -------------------------------------------------------------------------
   PatchMatch, using L2 distance between upright patches that translate only
   ------------------------------------------------------------------------- */

//int patch_w  = 7;
int pm_iters = 5;
int rs_max   = INT_MAX;



/* Measure distance between 2 patches with upper left corners (ax, ay) and (bx, by), terminating early if we exceed a cutoff distance.
   You could implement your own descriptor here. */
int PatchMatch::dist(cv::Mat &a, cv::Mat &b, int ax, int ay, int bx, int by, int cutoff=INT_MAX) {
  int ans = 0;
  int targetCount = 0;
  for (int dy = 0; dy < patch_w; dy++) {
    //int *arow = a.ro//&(*a)[ay+dy][ax];
    //int *brow = &(*b)[by+dy][bx];
    for (int dx = 0; dx < patch_w; dx++) {
		
		if(targetMask[(by + dy) * b.cols + bx + dx] == 1){
			ans += INT_MAX / 2;
			return INT_MAX;
		}
		if(targetMask[(ay + dy) * a.cols + ax + dx] == 1){
			targetCount++;
			continue;
			
		}
		else
		{
			int db = a.at<cv::Vec3b>(ay + dy, ax + dx).val[0] - b.at<cv::Vec3b>(by + dy, bx + dx).val[0];
			int dg = a.at<cv::Vec3b>(ay + dy, ax + dx).val[1] - b.at<cv::Vec3b>(by + dy, bx + dx).val[1];
			int dr = a.at<cv::Vec3b>(ay + dy, ax + dx).val[2] - b.at<cv::Vec3b>(by + dy, bx + dx).val[2];
			ans += dr*dr + dg*dg + db*db;

		}
    }
    if (ans >= cutoff) { return cutoff; }
  }

  if(targetCount > patch_w )
	  return INT_MAX;

  ans = ans * patch_w * patch_w / (patch_w * patch_w - targetCount);
  return ans;
}



void PatchMatch::improve_guess(cv::Mat &a, cv::Mat &b, int ax, int ay, int &xbest, int &ybest, int &dbest, int bx, int by) {
  int d = dist(a, b, ax, ay, bx, by, dbest);
  if (d < dbest) {
    dbest = d;
    xbest = bx;
    ybest = by;
  }
}

/* Match image a to image b, returning the nearest neighbor field mapping a => b coords, stored in an RGB 24-bit image as (by<<12)|bx. */
void PatchMatch::patchmatch(cv::Mat &image, cv::Mat &b, BITMAP *ann, BITMAP &annd) {
  /* Initialize with random nearest neighbor field (NNF). */
		cv::Mat a(image.size(), CV_64F);
		image.copyTo(a);
  int aew = a.cols - patch_w+1, aeh = a.rows - patch_w + 1;       /* Effective width and height (possible upper left corners of patches). */
  int bew = b.cols - patch_w+1, beh = b.rows - patch_w + 1;

  for (int ay = 0; ay < aeh; ay++) {
    for (int ax = 0; ax < aew; ax++) {
		int bx, by;
		//if((*ann)[ay][ax] == 0){
		//	  bx = rand() % bew;
		//	  by = rand() % beh;
		//	  (*ann)[ay][ax] = XY_TO_INT(bx, by);
		//}
		//else{
		//	bx = INT_TO_X((*ann)[ay][ax]);
		//	by = INT_TO_Y((*ann)[ay][ax]);
		//}
		bx = rand() % bew;
		by = rand() % beh;
		(*ann)[ay][ax] = XY_TO_INT(bx, by);
		
      annd[ay][ax] = dist(a, b, ax, ay, bx, by);
    }
  }
    int as;
    std::ofstream o_file;
    o_file.open("1.txt");
  for (int iter = 0; iter < pm_iters; iter++) {
    /* In each iteration, improve the NNF, by looping in scanline or reverse-scanline order. */
    int ystart = 0, yend = aeh, ychange = 1;
    int xstart = 0, xend = aew, xchange = 1;
    if (iter % 2 == 1) {
      xstart = xend-1; xend = -1; xchange = -1;
      ystart = yend-1; yend = -1; ychange = -1;
    }

	
//	if(iter == 2)
		as = 0;
    for (int ay = ystart; ay != yend; ay += ychange) {
      for (int ax = xstart; ax != xend; ax += xchange) { 
        /* Current (best) guess. */
		  //if(ax == 366 && ay == 367)
			 // as++;
		//  int sdsd = targetMask[ay * a.cols + ax];
		//if(ax == 352 && ay == 73){
		//	as++;
		//}
        int v = (*ann)[ay][ax];
        int xbest = INT_TO_X(v), ybest = INT_TO_Y(v);
        int dbest = annd[ay][ax];

        /* Propagation: Improve current guess by trying instead correspondences from left and above (below and right on odd iterations). */
		int targetCount = 0;
		for(int i = 0; i < patch_w; i++){
			for(int j = 0; j < patch_w; j++){
				if(targetMask[(ay + i) * a.cols + ax + j] > 0)
					targetCount++;
			}
		}
		if(targetCount > patch_w){
			int vp = (*ann)[ay][ax - xchange];
			int xp = INT_TO_X(vp) + xchange, yp = INT_TO_Y(vp);
			if ((unsigned) xp < (unsigned) bew) {
				xbest = xp;
				ybest = yp;
				(*ann)[ay][ax] = XY_TO_INT(xbest, ybest);
				annd[ay][ax] = annd[ay][ax - xchange];
				continue;
			}
			//else{
			//	vp = (*ann)[ay - ychange][ax];
			//	xp = INT_TO_X(vp), yp = INT_TO_Y(vp) + ychange;
			//	if ((unsigned) yp < (unsigned) beh) {
			//		xbest = xp;
			//		ybest = yp;
			//		(*ann)[ay][ax] = XY_TO_INT(xbest, ybest);
			//		annd[ay][ax] = annd[ay - ychange][ax];
			//		continue;
			//	}
			//}
			if((unsigned) (ay - ychange) < (unsigned) aeh){
				int vp = (*ann)[ay - ychange][ax];
			    int xp = INT_TO_X(vp), yp = INT_TO_Y(vp) + ychange;
			    if ((unsigned) yp < (unsigned) beh) {
			 		improve_guess(a, b, ax, ay, xbest, ybest, dbest, xp, yp);
			    }
			}
		}
		else{
			if ((unsigned) (ax - xchange) < (unsigned) aew) {
			  int vp = (*ann)[ay][ax - xchange];
			  int xp = INT_TO_X(vp) + xchange, yp = INT_TO_Y(vp);
			  if ((unsigned) xp < (unsigned) bew) {
				improve_guess(a, b, ax, ay, xbest, ybest, dbest, xp, yp);
			  }
			}

			if ((unsigned) (ay - ychange) < (unsigned) aeh) {
			  int vp = (*ann)[ay - ychange][ax];
			  int xp = INT_TO_X(vp), yp = INT_TO_Y(vp) + ychange;
			  if ((unsigned) yp < (unsigned) beh) {
				improve_guess(a, b, ax, ay, xbest, ybest, dbest, xp, yp);
			  }
			}
		}

        /* Random search: Improve current guess by searching in boxes of exponentially decreasing size around the current best guess. */
        int rs_start = rs_max;
        if (rs_start > MAX(b.cols, b.rows)) { rs_start = MAX(b.cols, b.rows); }
        for (int mag = rs_start; mag >= 1; mag /= 2) {
            /* Sampling window */
            int xmin = MAX(xbest-mag, 0), xmax = MIN(xbest+mag+1,bew);
            int ymin = MAX(ybest-mag, 0), ymax = MIN(ybest+mag+1,beh);
            int xp = xmin+rand()%(xmax-xmin);
            int yp = ymin+rand()%(ymax-ymin);

		    int targetCount = 0;
			for(int i = 0; i < patch_w; i++){
				for(int j = 0; j < patch_w; j++){
					if(targetMask[(yp + i) * a.cols + xp + j] > 0){
						targetCount++;
						break;
					}
				}
				if(targetCount > 0)
					break;
			}
			if(targetCount > 0){
				mag *= 2;
				continue;
			}
            improve_guess(a, b, ax, ay, xbest, ybest, dbest, xp, yp);
        }

        (*ann)[ay][ax] = XY_TO_INT(xbest, ybest);
        annd[ay][ax] = dbest;
		//a.at<cv::Vec3b>(ay, ax) = b.at<cv::Vec3b>(ybest, xbest);
		
		//for (int dy = 0; dy < patch_w; dy++) {
		//	for (int dx = 0; dx < patch_w; dx++) {
		//		a.at<cv::Vec3b>(ay + dy, ax + dx) = b.at<cv::Vec3b>(ybest + dy, xbest + dx);
		//	}
	 //   }
		if(ay != ybest || ax != xbest)
			as++;

		if(ax == 352 && ay == 73){
			as++;
		}
	//	for(int i = 0; i < cvImage.rows - 7 + 1; i++){
	//	for(int j = 0; j < cvImage.cols - 7 + 1; j++){
	//		int v = (*ann)[i][j];
	//		int x = INT_TO_X(v);
	//		int y = INT_TO_Y(v);
	//		anni.at<cv::Vec3b>(i, j).val[0] = (0+ x) % 256;
	//		anni.at<cv::Vec3b>(i, j).val[1] = (0 + y) % 256;
	//		anni.at<cv::Vec3b>(i, j).val[2] = 0;

	//		if(annd[i][j] > 0){
	//		anndi.at<cv::Vec3b>(i, j).val[0] = annd[i][j] % 256;
	//		}
	//		else 
	//			anndi.at<cv::Vec3b>(i, j).val[0] = 0;
	//		anndi.at<cv::Vec3b>(i, j).val[1] = 0;
	//		anndi.at<cv::Vec3b>(i, j).val[2] = 0;
	//	}
	//}
	//cv::imshow("r", anni);
	//cv::imshow("rd", anndi);
      }
    }
	//std::string s[] = {"0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19"};
	//cv::imshow(s[iter], a);

	o_file<<as<<std::endl;
	std::cout<<as<<std::endl;
  }
  o_file.close();

  a.copyTo(image);
}


void PatchMatch::vote(cv::Mat &result, cv::Mat &source, BITMAP *ann){
	int length = result.rows * result.cols;
	int *recordL = new int[length];
	int *recordA = new int[length];
	int *recordB = new int[length];
	int *recordT = new int[length];

	for(int row = 0; row < result.rows; row++){
		for(int col = 0; col < result.cols; col++){
			recordL[row * result.cols + col] = 0;
			recordA[row * result.cols + col] = 0;
			recordB[row * result.cols + col] = 0;
			recordT[row * result.cols + col] = 0;
		}
	}
	int count = 0;
	for(int row = 0; row < result.rows - patch_w + 1; row++){
		for(int col = 0; col < result.cols - patch_w + 1; col++){
			// 判断是否在target内部，内部的不算加权
			int tagertCount = 0;
			for (int dy = 0; dy < patch_w; dy++) {
				for (int dx = 0; dx < patch_w; dx++) {
					int ry = row + dy;
					int rx = col + dx;
					if(targetMask[ry * result.cols + rx] == 1)
						tagertCount++;
				}
		    }
			if(tagertCount > patch_w)
				continue;

			//加权
			int v = (*ann)[row][col];
			int x = INT_TO_X(v);
			int y = INT_TO_Y(v);
			for (int dy = 0; dy < patch_w; dy++) {
				for (int dx = 0; dx < patch_w; dx++) {
					int sy = y + dy;
					int sx = x + dx;
					int ry = row + dy;
					int rx = col + dx;

					int b = recordT[0];
					if(source.at<cv::Vec3b>(sy, sx).val[0] == 0 && source.at<cv::Vec3b>(sy, sx).val[1] == 0 && source.at<cv::Vec3b>(sy, sx).val[2] == 0)
						continue;
					recordL[ry * result.cols + rx] += source.at<cv::Vec3b>(sy, sx).val[0];
					recordA[ry * result.cols + rx] += source.at<cv::Vec3b>(sy, sx).val[1];
					recordB[ry * result.cols + rx] += source.at<cv::Vec3b>(sy, sx).val[2];
					recordT[ry * result.cols + rx] += 1;

					if(b < recordT[0]){
						count++;
					}
				}

		    }
		}
	}

	targetCount = 0;

	for(int row = 0; row < result.rows; row++){
		for(int col = 0; col < result.cols; col++){
			if(targetMask[row * result.cols + col] == 1){
				targetCount++;
				if(recordT[row * result.cols + col] == 0)
					continue;
				result.at<cv::Vec3b>(row, col).val[0] = recordL[row * result.cols + col] / recordT[row * result.cols + col];
				result.at<cv::Vec3b>(row, col).val[1] = recordA[row * result.cols + col] / recordT[row * result.cols + col];
				result.at<cv::Vec3b>(row, col).val[2] = recordB[row * result.cols + col] / recordT[row * result.cols + col];
				targetMask[row * result.cols + col] = 0;
			}
			//if(result.at<cv::Vec3b>(row, col).val[0] != 0 || result.at<cv::Vec3b>(row, col).val[1] != 0 || result.at<cv::Vec3b>(row, col).val[2] != 0){
			//	targetMask[row * result.cols + col] = 0;
			//}
		}
	}
	//int c = 0;
	//for(int i = 0; i < result.cols * result.rows; i++){
	//	if(targetMask[i] > 0)
	//		c++;
	//}

	delete recordL;
	delete recordA;
	delete recordB;
	delete recordT;
}

void PatchMatch::zeroMask(int length, bool newImage){
	if(newImage){
		if(sourceMask != NULL)
			delete sourceMask;
		if(targetMask != NULL)
			delete targetMask;
		sourceMask = new char[length];
		targetMask = new char[length];
	}

	for(int i = 0; i < length; i++){
		sourceMask[i] = 0;
		targetMask[i] = 0;
	}
}

void PatchMatch::addToTargetMask(int ax, int ay, int rows, int cols){
	int radius = targetRadius;
	int centerX = ax;
	int centerY = ay;
	for(int row = -radius; row < radius + 1; row++){
		for(int col = -radius; col < radius + 1; col++){
			if(centerX + col < 0 || centerY + row < 0 || centerX + col >= cols || centerY + row >= rows)
				continue;
			if(row * row + col * col < radius * radius){	
				targetMask[(centerY + row) * cols + centerX + col] = 1;
			}
		}
	}
	//cv::imshow("SkinMask", skinMask);
}

void PatchMatch::addToSourceMask(int ax, int ay, int rows, int cols){
	int radius = sourceRadius;
	int centerX = ax;
	int centerY = ay;
	for(int row = -radius; row < radius + 1; row++){
		for(int col = -radius; col < radius + 1; col++){
			if(centerX + col < 0 || centerY + row < 0 || centerX + col >= cols || centerY + row >= rows)
				continue;
			if(row * row + col * col < radius * radius){	
				sourceMask[(centerY + row) * cols + centerX + col] = 1;
			}
		}
	}
	//cv::imshow("SkinMask", skinMask);
}

void PatchMatch::updateSourceMask(int rows, int cols){
	for(int row = 0; row < rows; row++){
		for(int col = 0; col < cols; col++){
			if(targetMask[row * cols + col] == 1)
				sourceMask[row * cols + col] = 0;
		}
	}
}

void PatchMatch::getSourceImageFromSourceMask(cv::Mat &image, cv::Mat &img){
	int minRow = image.rows, maxRow = 0, minCol = image.cols, maxCol = 0;
	int count = 0;
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			if(sourceMask[row * image.cols + col] == 1){
				count++;
				if(row > maxRow)
					maxRow = row;
				if(row < minRow)
					minRow = row;
				if(col > maxCol)
					maxCol = col;
				if(col < minCol)
					minCol = col;
			}
		}
	}
	//image.copyTo(sourceImage);
	//sourceImage.rows = maxRow - minRow + 1;
	//sourceImage.cols = maxCol - minCol + 1;cv::Size(maxRow - minRow + 1, maxCol - minCol + 1)
	cv::Mat sourceImage;//(image.size(), CV_8UC3) ;
	sourceImage.create(maxRow - minRow + 1, maxCol - minCol + 1, CV_8UC3);
	for(int row = 0; row < sourceImage.rows; row ++){
		for(int col = 0; col < sourceImage.cols; col++){
			sourceImage.at<cv::Vec3b>(row, col).val[0] = image.at<cv::Vec3b>(row + minRow, col + minCol).val[0];
			sourceImage.at<cv::Vec3b>(row, col).val[1] = image.at<cv::Vec3b>(row + minRow, col + minCol).val[1];
			sourceImage.at<cv::Vec3b>(row, col).val[2] = image.at<cv::Vec3b>(row + minRow, col + minCol).val[2];
		}
	}
	cv::imshow("sourceImage", sourceImage);	
	sourceImage.copyTo(img);
}

void PatchMatch::addMaskToImage(cv::Mat &image){
	//target :red
	//source :green
	for(int row = 0; row < image.rows; row++){
		for(int col = 0; col < image.cols; col++){
			if(sourceMask[row * image.cols + col] == 1){
				targetMask[row * image.cols + col] = 0;
			}

			if(sourceMask[row * image.cols + col] == 1){
				image.at<cv::Vec3b>(row, col).val[1] = (image.at<cv::Vec3b>(row, col).val[1] + 255) / 2;
			}
			else if(targetMask[row * image.cols + col] == 1){
				image.at<cv::Vec3b>(row, col).val[0] = 0;
				image.at<cv::Vec3b>(row, col).val[1] = 0;
				image.at<cv::Vec3b>(row, col).val[2] = 150;
			}
		}
	}
}

bool PatchMatch::checkTargetMask(int length){
	for(int i = 0; i < length; i++){
		if(targetMask[i] == 1)
			return true;
	}
	return false;
}

bool PatchMatch::checkSourceMask(int length){
	for(int i = 0; i < length; i++){
		if(sourceMask[i] == 1)
			break;
	}
	return false;
}


void PatchMatch::patchmatch(cv::Mat &a, cv::Mat &b, BITMAP *&ann, BITMAP *&annd) {
  /* Initialize with random nearest neighbor field (NNF). */
  ann = new BITMAP(a.cols, a.rows);
  annd = new BITMAP(a.cols, a.rows);
  int aew = a.cols - patch_w+1, aeh = a.rows - patch_w + 1;       /* Effective width and height (possible upper left corners of patches). */
  int bew = a.cols - patch_w+1, beh = a.rows - patch_w + 1;
  memset(ann->data, 0, sizeof(int) * a.cols * a.rows);
  memset(annd->data, 0, sizeof(int) * a.cols * a.rows);
  for (int ay = 0; ay < aeh; ay++) {
    for (int ax = 0; ax < aew; ax++) {
      int bx = rand()%bew;
      int by = rand()%beh;
      (*ann)[ay][ax] = XY_TO_INT(bx, by);
      (*annd)[ay][ax] = dist(a, b, ax, ay, bx, by);
    }
  }
  for (int iter = 0; iter < pm_iters; iter++) {
    /* In each iteration, improve the NNF, by looping in scanline or reverse-scanline order. */
    int ystart = 0, yend = aeh, ychange = 1;
    int xstart = 0, xend = aew, xchange = 1;
    if (iter % 2 == 1) {
      xstart = xend-1; xend = -1; xchange = -1;
      ystart = yend-1; yend = -1; ychange = -1;
    }
    for (int ay = ystart; ay != yend; ay += ychange) {
      for (int ax = xstart; ax != xend; ax += xchange) { 
        /* Current (best) guess. */
        int v = (*ann)[ay][ax];
        int xbest = INT_TO_X(v), ybest = INT_TO_Y(v);
        int dbest = (*annd)[ay][ax];

        /* Propagation: Improve current guess by trying instead correspondences from left and above (below and right on odd iterations). */
        if ((unsigned) (ax - xchange) < (unsigned) aew) {
          int vp = (*ann)[ay][ax-xchange];
          int xp = INT_TO_X(vp) + xchange, yp = INT_TO_Y(vp);
          if ((unsigned) xp < (unsigned) bew) {
            improve_guess(a, b, ax, ay, xbest, ybest, dbest, xp, yp);
          }
        }

        if ((unsigned) (ay - ychange) < (unsigned) aeh) {
          int vp = (*ann)[ay-ychange][ax];
          int xp = INT_TO_X(vp), yp = INT_TO_Y(vp) + ychange;
          if ((unsigned) yp < (unsigned) beh) {
            improve_guess(a, b, ax, ay, xbest, ybest, dbest, xp, yp);
          }
        }

        /* Random search: Improve current guess by searching in boxes of exponentially decreasing size around the current best guess. */
        int rs_start = rs_max;
        if (rs_start > MAX(b.cols, b.rows)) { rs_start = MAX(b.cols, b.rows); }
        for (int mag = rs_start; mag >= 1; mag /= 2) {
          /* Sampling window */
          int xmin = MAX(xbest-mag, 0), xmax = MIN(xbest+mag+1,bew);
          int ymin = MAX(ybest-mag, 0), ymax = MIN(ybest+mag+1,beh);
          int xp = xmin+rand()%(xmax-xmin);
          int yp = ymin+rand()%(ymax-ymin);
          improve_guess(a, b, ax, ay, xbest, ybest, dbest, xp, yp);
        }

        (*ann)[ay][ax] = XY_TO_INT(xbest, ybest);
        (*annd)[ay][ax] = dbest;
      }
    }
  }
}