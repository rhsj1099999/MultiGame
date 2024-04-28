#pragma once




typedef struct tagInfo
{
	D3DXVECTOR3		vScale;
	D3DXVECTOR3		vPos;
	D3DXVECTOR3		vDir;
	D3DXVECTOR3		vLook;

	D3DXMATRIX		matWorld;

}INFO;

// ���� ���� : ���ʹ� ������ �߿����� ũ��� ��������� �߿����� ���� ��찡 ����. �׷��� ������ ũ�⸦ 1�� ������ �Ͽ� ����ϴµ� �̷��� ũ�Ⱑ 1�� ���� ���͸� ������ ���� ���Ͷ� �ϸ�, ���� ���ͷ� ����� ������ '������ ����ȭ'��� �Ѵ�.
// ������ ��� : �� ���Ͱ� ũ��� ������ ���ٸ� �� ���ʹ� ���� ���Ͷ�� ����

static D3DXVECTOR3	Get_Mouse(void)
{
	POINT		pt{};

	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	return D3DXVECTOR3((float)pt.x, (float)pt.y, 0.f);
}