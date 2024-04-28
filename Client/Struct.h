#pragma once




typedef struct tagInfo
{
	D3DXVECTOR3		vScale;
	D3DXVECTOR3		vPos;
	D3DXVECTOR3		vDir;
	D3DXVECTOR3		vLook;

	D3DXMATRIX		matWorld;

}INFO;

// 단위 벡터 : 벡터는 방향이 중요하지 크기는 상대적으로 중요하지 않은 경우가 많다. 그래서 벡터의 크기를 1로 보정을 하여 사용하는데 이렇게 크기가 1인 방향 벡터를 가리켜 단위 벡터라 하며, 단위 벡터로 만드는 과정을 '벡터의 정규화'라고 한다.
// 벡터의 상등 : 두 벡터가 크기와 방향이 같다면 두 벡터는 같은 벡터라는 개념

static D3DXVECTOR3	Get_Mouse(void)
{
	POINT		pt{};

	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	return D3DXVECTOR3((float)pt.x, (float)pt.y, 0.f);
}