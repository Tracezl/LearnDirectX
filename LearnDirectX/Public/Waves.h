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
};

#endif // WAVES_H