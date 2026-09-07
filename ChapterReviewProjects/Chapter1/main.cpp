
#include <windows.h>
#include<iostream>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

ostream& XM_CALLCONV operator<<(ostream& os, FXMVECTOR v){
	XMFLOAT4 dest;
	XMStoreFloat4(&dest, v);

	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ", " << dest.w << ")";
	return os;
}

int main() {
	cout.setf(ios_base::boolalpha);

	if (!XMVerifyCPUSupport()) {
		cout << "directx math not supported" << endl;
		return 0;
	}

	XMVECTOR p = XMVectorSet(2,2,1,0);
	XMVECTOR q = XMVectorSet(2, -0.5, 0.5, 0.1);
	XMVECTOR u = XMVectorSet(1,2,4,8);
	XMVECTOR v = XMVectorSet(-2,1,-3,2.5);
	XMVECTOR w = XMVectorSet(0, XM_PIDIV4, XM_PIDIV2, XM_PI);


	cout << XMVectorAbs(v) << endl;
	cout << XMVectorCos(w) << endl;
	cout << XMVectorLog(u) << endl;
	cout << XMVectorExp(p) << endl;
	cout << XMVectorPow(u, p) << endl;
	cout << XMVectorSqrt(u) << endl;

	cout << XMVectorSwizzle(u,2,2,1,3) << endl;
	cout << XMVectorSwizzle(u,2,1,0,3) << endl;

	cout << XMVectorMultiply(u, v) << endl;
	cout << XMVectorSaturate(q) << endl;
	cout << XMVectorMin(p, v) << endl;
	cout << XMVectorMax(p, v) << endl;

}