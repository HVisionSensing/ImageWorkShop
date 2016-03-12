ushort computeDistance( __global const uchar* target, int target_row, int target_col, __global const uchar* source, 
                      int source_row, int source_col, const uint4 imageSize, __global const uchar* mask, __global const uchar* currentRestrict){
	long distance = 0;
	long weight_sum = 0;
	int penalty = 655350;
	int radius = 2;
	for(int dy =- radius; dy <= radius; dy++) {
		for(int dx =- radius; dx <= radius; dx++) {
			weight_sum += penalty;
			
			int s_row = source_row + dy;
			int s_col = source_col + dx;
			if (s_row < 0 || s_row >= imageSize.x){
				distance += penalty;
				continue;
			}

			if (s_col < 0 || s_col >= imageSize.y){
				distance += penalty;
				continue;
			}
			
			//penalty for hole in source
			if (mask[s_row * imageSize.y + s_col] != 0) {
				distance += penalty;
				continue;
			}
			
			int t_row= target_row + dy;
			int t_col = target_col + dx;
			if (t_row < 0 || t_row >= imageSize.x){
				distance += penalty;
				continue;
			}
			if (t_col < 0 || t_col >= imageSize.y){
				distance += penalty;
				continue;
			}
			
			if (mask[t_row * imageSize.y + t_col] != 0) {
				//判断source是否在restrict内部，若不在，则惩罚
				if(currentRestrict[s_row * imageSize.y + s_col] == 0){
					distance += penalty;
					continue;
				}
			}
				
			
			//compute ssd
			long ssd = 0;
			int weight[3] = {1, 6, 3};
			for(int channel = 0; channel < 3; channel++){
				int s = source[s_row * imageSize.y * 3 + s_col * 3 + channel];
				int t = target[t_row * imageSize.y * 3 + t_col * 3 + channel];
				float dis = (s - t) * (s - t); // Value 
				ssd += weight[channel] * dis;
			}
			distance += ssd;
		}
	}
	//distance = 0;
	float result = (distance * 1.0f / weight_sum);
	return (ushort)(65535 * result);
}
 
 
 __kernel void jumpFloodingRestrict(__global const char* source, __global const char* target, __global ushort* oldNNF,  __global ushort* oldDistance, 
                          const uint4 imageSize, __global const uchar* mask, __global const uchar* currentRestrict)
{
	
	int row = get_global_id(0);
	int col = get_global_id(1);
	
	int ROW[8];
	int COL[8];
	
	int window_radius = imageSize.z * 2;
	
	int knn = imageSize.w;
	if(knn > 16)
		knn = 16;

	int NNFOFFSET = imageSize.x * imageSize.y * 3;
	int DISOFFSET = imageSize.x * imageSize.y;
	
	int rows[9 * 17];
	int cols[9 * 17];
	int dis[9 * 17];

	int iternum = 1;
	for(int iter = 0; iter < iternum; iter++){
		if(iter == 1){
			window_radius = 4;
		}
		while(window_radius > 0){
			window_radius /= 2;
			
			ROW[0] = row - window_radius;
			COL[0] = col - window_radius;
			
			ROW[1] = row - window_radius;
			COL[1] = col;
			
			ROW[2] = row - window_radius;
			COL[2] = col + window_radius;
			
			ROW[3] = row;
			COL[3] = col - window_radius;
			
			ROW[4] = row;
			COL[4] = col + window_radius;
			
			ROW[5] = row + window_radius;
			COL[5] = col - window_radius;
			
			ROW[6] = row + window_radius;
			COL[6] = col;
			
			ROW[7] = row + window_radius;
			COL[7] = col + window_radius;
			
			//ROW[8] = row;
			//COL[8] = col;
			
			//int target_row = row;
			//int target_col = col;
			
			
			int k = 0;
			
			dis[k] = computeDistance(target, row, col, source, row, col, imageSize, mask, currentRestrict);
			rows[k] = row;
			cols[k] = col;
			k++;
			
			
			for(int i = 0; i < knn; i++){
				dis[k] = oldDistance[i * DISOFFSET + row * imageSize.y + col];
				rows[k] = oldNNF[i * NNFOFFSET + row * imageSize.y * 3 + col * 3 + 1];
				cols[k] = oldNNF[i * NNFOFFSET + row * imageSize.y * 3 + col * 3];
				k++;
			}
			for(int i = 0; i < 8; i++){
				if(ROW[i] < 0 || COL[i] < 0 || ROW[i] >= imageSize.x || COL[i] >= imageSize.y)
					continue;
				if(ROW[i] < 0) ROW[i] = 0;
				if(ROW[i] >= imageSize.x) ROW[i] = imageSize.x - 1;
				if(COL[i] < 0) COL[i] = 0;
				if(COL[i] >= imageSize.y) COL[i] = imageSize.y - 1;
				
				dis[k] = computeDistance(target, row, col, source, ROW[i], COL[i], imageSize, mask, currentRestrict);
				rows[k] = ROW[i];
				cols[k] = COL[i];
				k++;
				
				for(int j = 0; j < knn; j++){	
					int currentRow = oldNNF[j * NNFOFFSET + ROW[i] * imageSize.y * 3 + COL[i] * 3 + 1] + row - ROW[i];
					int currentCol = oldNNF[j * NNFOFFSET + ROW[i] * imageSize.y * 3 + COL[i] * 3] + col - COL[i];
					
					dis[k] = computeDistance(target, row, col, source, currentRow, currentCol, imageSize, mask, currentRestrict);
					rows[k] = currentRow;
					cols[k] = currentCol;
					k++;
				}
			}
			for(int i = 0; i < knn; i++){
				for(int j = i + 1; j < k; j++){
					if(rows[i] == rows[j] && cols[i] == cols[j]){
						ushort c = dis[j];
						dis[j] = dis[k-1];
						dis[k-1] = c;
						
						c = rows[k-1];
						rows[k-1] = rows[j];
						rows[j] = c;
						
						c = cols[k-1];
						cols[k-1] = cols[j];
						cols[j] = c;
						k--;
						j--;
						continue;
					}
					if(dis[i] > dis[j]){
						ushort c = dis[j];
						dis[j] = dis[i];
						dis[i] = c;
						
						c = rows[i];
						rows[i] = rows[j];
						rows[j] = c;
						
						c = cols[i];
						cols[i] = cols[j];
						cols[j] = c;
					}
					
				}
			}
			
			barrier(CLK_LOCAL_MEM_FENCE); 
			for(int i = 0; i < knn; i++){
				oldDistance[i * DISOFFSET + row * imageSize.y + col] = dis[i];
				oldNNF[i * NNFOFFSET + row * imageSize.y * 3 + col * 3] = cols[i];
				oldNNF[i * NNFOFFSET + row * imageSize.y * 3 + col * 3 + 1] = rows[i];
			}
			barrier(CLK_LOCAL_MEM_FENCE); 
			
		}
	}
	//if(mask[row * imageSize.y + col] != 0){
	//	oldNNF[row * imageSize.y * 3 + col * 3] = col - 50;
	//	oldNNF[row * imageSize.y * 3 + col * 3 + 1] = row;
	//}
		

}