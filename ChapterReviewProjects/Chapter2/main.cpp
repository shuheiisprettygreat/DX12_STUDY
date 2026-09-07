#include <windows.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <iostream>

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

ostream& XM_CALLCONV operator << (ostream& os, FXMVECTOR v) {
	XMFLOAT4 dest;
	XMStoreFloat4(&dest, v);

	os << dest.x << ", " << dest.y << ", " << dest.z << ", " << dest.w << ")";
	return os;
}

ostream& XM_CALLCONV operator << (ostream& os, FXMMATRIX m) {
	for (int i = 0; i < 4; ++i) {
		os << XMVectorGetX(m.r[i]) << "\t";
		os << XMVectorGetY(m.r[i]) << "\t";
		os << XMVectorGetZ(m.r[i]) << "\t";
		os << XMVectorGetW(m.r[i]) << "\t";
		os << endl;
	}
	return os;
}

float determinant2(float m11, float m12, float m21, float m22) {
	return m11 * m22 - m12 * m21;
}

float determinant3(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33) {
	return m11 * determinant2(m22, m23, m32, m33) - m12 * determinant2(m21, m23, m31, m33) + m12 * determinant2(m22, m23, m32, m33);
}

float determinant(XMMATRIX m) {
	XMFLOAT4 r1, r2, r3, r4;
	XMStoreFloat4(&r1, m.r[0]);
	XMStoreFloat4(&r2, m.r[1]);
	XMStoreFloat4(&r3, m.r[2]);
	XMStoreFloat4(&r4, m.r[3]);

	return r1.x * determinant3(r2.y, r2.z, r2.w, r3.y, r3.z, r3.w, r4.y, r4.z, r4.w)
		- r1.y * determinant3(r2.x, r2.z, r2.w, r3.x, r3.z, r3.w, r4.x, r4.z, r4.w)
		+ r1.z * determinant3(r2.x, r2.y, r2.w, r3.x, r3.y, r3.w, r4.x, r4.y, r4.w)
		- r1.w * determinant3(r2.x, r2.y, r2.z, r3.x, r3.y, r3.z, r4.x, r4.y, r4.z);
}


XMMATRIX inverse(XMMATRIX m) {
	float matrixSet[4][4];

	for (int i = 0; i < 4; i++) {
		int majorCol1, majorCol2, majorCol3;
		if (i == 0) { majorCol1 = 1, majorCol2 = 2, majorCol3 = 3; }
		if (i == 1) { majorCol1 = 0, majorCol2 = 2, majorCol3 = 3; }
		if (i == 2) { majorCol1 = 0, majorCol2 = 1, majorCol3 = 3; }
		if (i == 3) { majorCol1 = 0, majorCol2 = 1, majorCol3 = 2; }
		for (int j = 0; j < 4; j++) {
			int sign = ((i + j) % 2) == 0 ? 1 : -1;
			int minorCol1, minorCol2, minorCol3;
			if (j == 0) { minorCol1 = 1, minorCol2 = 2, minorCol3 = 3; }
			if (j == 1) { minorCol1 = 0, minorCol2 = 2, minorCol3 = 3; }
			if (j == 2) { minorCol1 = 0, minorCol2 = 1, minorCol3 = 3; }
			if (j == 3) { minorCol1 = 0, minorCol2 = 1, minorCol3 = 2; }
			XMVECTOR r1 = XMVectorSwizzle(m.r[majorCol1], minorCol1, minorCol2, minorCol3, 0);
			XMVECTOR r2 = XMVectorSwizzle(m.r[majorCol2], minorCol1, minorCol2, minorCol3, 0);
			XMVECTOR r3 = XMVectorSwizzle(m.r[majorCol3], minorCol1, minorCol2, minorCol3, 0);
			XMFLOAT3 s1, s2, s3;
			XMStoreFloat3(&s1, r1);
			XMStoreFloat3(&s2, r2);
			XMStoreFloat3(&s3, r3);
			matrixSet[j][i] = sign * determinant3(s1.x, s1.y, s1.z, s2.x, s2.y, s2.z, s3.x, s3.y, s3.z);
		}
	}

	float det = determinant(m);

	XMMATRIX result((float*)matrixSet);
	
	return result / det;
}


int main() {
	if (!XMVerifyCPUSupport()) {
		cout << "directx math not supported" << endl;
		return 0;
	}

	//// Exercise 1 - Transpose 2d array
	//const int r = 2, c = 3;
	//double A[r][c] = { {1,2,3}, {2,4,6} };
	//
	//double At[c][r];
	//for (int i = 0; i < r; i++) {
	//	for (int j = 0; j < c; j++) {
	//		At[j][i] = A[i][j];
	//	}
	//}

	//for (int i = 0; i < c; i++) {
	//	for (int j = 0; j < r; j++) {
	//		cout << At[i][j] << "\t";
	//	}
	//	cout << endl;
	//}

	// Exercise 2 - get derteminant and inverse matrix 
	XMMATRIX A = XMMATRIX(1, 0, 0, 0, 2, 2, 0, 0, 3, 3, 3, 0, 4, 4, 4, 4);
	cout << "det(custom): " << determinant(A) << endl;

	XMVECTOR det = XMMatrixDeterminant(A);
	cout << "det(correct): " << XMVectorGetX(det) << endl;

	XMMATRIX invA = inverse(A);
	cout << "inverse(custom):" << endl << invA << endl;
	cout << "inverse(correct):" << endl << XMMatrixInverse(nullptr, A) << endl;

	cout << "A * invA:" << endl << A * invA << endl;

}