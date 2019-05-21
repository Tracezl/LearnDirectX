// ģ�Ⲩ�˵ļ���,�ͻ���Ҫ������������Ƶ����㻺����
// �����ֻ����,������






#ifndef WAVES_H
#define WAVES_H

#include <vector>
#include <DirectXMath.h>

class Waves
{
public:
	Waves(int m, int n, float dx, float dt, float speed, float damping);
	Waves(const Waves& rhs) = delete;
	Waves& operator=(const Waves& rhs) = delete;
	~Waves();

	int RowCount() const;
	int ColumnCount() const;
	int VertexCount() const;
	int TriangleCount() const;
	float Width() const;
	float Depth() const;

	// ���ص�i��grid point��λ��
	const DirectX::XMFLOAT3& Position(int i) const { return mCurrSolution[i]; }

	// ���ص�i��grid point�ķ���
	const DirectX::XMFLOAT3& Normal(int i) const { return mNormals[i]; }

	// ���ص�i��grid point�ľֲ�x���unit tangent vector
	const DirectX::XMFLOAT3& TangetX(int i) const { return mTangentX[i]; }

	void Update(float dt);
	void Disturb(int i, int j, float magnitude);

private:
	int mNumRows = 0; // ����
	int mNumCols = 0; // ����

	int mVertexCount = 0; // ������
	int mTriangleCount = 0; // grid��������

	// Ԥ�ȼ���ģ�ⳣ��
	float mK1 = 0.0f;
	float mK2 = 0.0f;
	float mK3 = 0.0f;

	float mTimeStep = 0.0f; // ����ʱ��Ƶ��
	float mSpatialStep = 0.0f; // grid�ı߳�,Spatial:�ռ�

	std::vector<DirectX::XMFLOAT3> mPrevSolution; // ��һ֡�Ķ���λ������
	std::vector<DirectX::XMFLOAT3> mCurrSolution; // ��ǰ֡�Ķ���λ������
	std::vector<DirectX::XMFLOAT3> mNormals; // ��������
	std::vector<DirectX::XMFLOAT3> mTangentX; // TabgetX����
};

#endif // WAVES_H