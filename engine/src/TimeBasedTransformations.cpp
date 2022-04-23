void buildRotMatrix(float *x, float *y, float *z, float *m) {

	m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
	m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
	m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}


void cross(float *a, float *b, float *res) {

	res[0] = a[1]*b[2] - a[2]*b[1];
	res[1] = a[2]*b[0] - a[0]*b[2];
	res[2] = a[0]*b[1] - a[1]*b[0];
}


void normalize(float *a) {

	float l = sqrt(a[0]*a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0]/l;
	a[1] = a[1]/l;
	a[2] = a[2]/l;
}


float length(float *v) {

	float res = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	return res;

}

void multMatrixVector(float *m, float *v, float *res) {

	for (int j = 0; j < 4; ++j) {
		res[j] = 0;
		for (int k = 0; k < 4; ++k) {
			res[j] += v[k] * m[j * 4 + k];
		}
	}

}

void getCatmullRomPoint(float t, float *p0, float *p1, float *p2, float *p3, float *pos, float *deriv) {
	float t_matrix[4];
	t_matrix[0] = t * t * t;
	t_matrix[1] = t * t;
	t_matrix[2] = t;
	t_matrix[3] = 1;

	float t_matrix_derivative[4];
	t_matrix_derivative[0] = 3 * (t * t);
	t_matrix_derivative[1] = 2 * t;
	t_matrix_derivative[2] = 1;
	t_matrix_derivative[3] = 0;
	// catmull-rom matrix
	float m[4][4] = {	{-0.5f,  1.5f, -1.5f,  0.5f},
						{ 1.0f, -2.5f,  2.0f, -0.5f},
						{-0.5f,  0.0f,  0.5f,  0.0f},
						{ 0.0f,  1.0f,  0.0f,  0.0f}};
			
	// Compute A = M * P
	float ax[4], ay[4], az[4];
	float px[4], py[4], pz[4];

	px[0] = p0[0]; px[1] = p1[0]; px[2] = p2[0]; px[3] = p3[0];
	py[0] = p0[1]; py[1] = p1[1]; py[2] = p2[1]; py[3] = p3[1];
	pz[0] = p0[2]; pz[1] = p1[2]; pz[2] = p2[2]; pz[3] = p3[2];

	multMatrixVector((float *)m,(float *)px,(float *)ax);
	multMatrixVector((float *)m,(float *)py,(float *)ay);
	multMatrixVector((float *)m,(float *)pz,(float *)az);
	// Compute pos = T * A
	float sum[3] = {0.0,0.0,0.0};

	for(int i = 0 ; i < 4 ; i++){
		sum[0] += t_matrix[i] * ax[i];
		sum[1] += t_matrix[i] * ay[i];
		sum[2] += t_matrix[i] * az[i];
	}

	pos[0] = sum[0]; pos[1] = sum[1]; pos[2] = sum[2];
	// compute deriv = T' * A
	for(int i = 0 ; i < 3 ; i++) sum[i] = 0.0f;

	for(int i = 0 ; i < 4 ; i++){
		sum[0] += t_matrix_derivative[i] * ax[i];
		sum[1] += t_matrix_derivative[i] * ay[i];
		sum[2] += t_matrix_derivative[i] * az[i];
	}

	deriv[0] = sum[0]; deriv[1] = sum[1]; deriv[2] = sum[2];
}

// given  global t, returns the point in the curve
void getGlobalCatmullRomPoint(float gt, float *pos, float *deriv) {

	float t = gt * POINT_COUNT; // this is the real global t
	int index = floor(t);  // which segment
	t = t - index; // where within  the segment

	// indices store the points
	int indices[4]; 
	indices[0] = (index + POINT_COUNT-1)%POINT_COUNT;	
	indices[1] = (indices[0]+1)%POINT_COUNT;
	indices[2] = (indices[1]+1)%POINT_COUNT; 
	indices[3] = (indices[2]+1)%POINT_COUNT;

	getCatmullRomPoint(t, p[indices[0]], p[indices[1]], p[indices[2]], p[indices[3]], pos, deriv);
}
