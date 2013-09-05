
// 2D�|���S���`��T���v���v���O����

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")

#include <windows.h>
#include <tchar.h>
#include <d3d9.h>
#include <math.h>


struct CUSTOMVERTEX{  
   float x, y, z; // ���_���W 
   float rhw;     // ���Z�� 
   DWORD dwColor; // ���_�̐F 
   float u, v;    //�@�e�N�X�`�����W 
}; 

#define FVF_CUSTOM ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

TCHAR gName[100] = _T("2D�|���S���`��T���v���v���O����");



// ���F�֐�
DWORD RainbowRGB(DWORD num){
        #define RAINBOWVAL(C) ((DWORD)((C>=1024) ?0 : (C<256) ? C : (C>768) ? (1023-C) : 255))
        DWORD R = num%1536;
        DWORD G = ((num%1536)+1024)%1536;
        DWORD B = ((num%1536)+ 512)%1536;
        return 0xff000000 + (RAINBOWVAL(R)<<16) + (RAINBOWVAL(G)<<8) + RAINBOWVAL(B); 
}


// ���_��]�֐�
void SetPos(float Cx, float Cy, float r, float ratio, float Ix, float Iy, float *Ox, float *Oy){
        *Ox = Cx + ratio*(Ix*cos(r) - Iy*sin(r));
        *Oy = Cy + ratio*(Ix*sin(r) + Iy*cos(r));
}


// �E�B���h�E�v���V�[�W��
LRESULT CALLBACK WndProc(HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam){
        if(mes == WM_DESTROY || mes == WM_CLOSE ) {PostQuitMessage(0); return 0;}
   return DefWindowProc(hWnd, mes, wParam, lParam);
}


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
   // �A�v���P�[�V�����̏�����
   MSG msg; HWND hWnd;
   WNDCLASSEX wcex ={sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance, NULL, NULL,
                                    (HBRUSH)(COLOR_WINDOW+1), NULL, (TCHAR*)gName, NULL};
   if(!RegisterClassEx(&wcex))
      return 0;

   if(!(hWnd = CreateWindow(gName, gName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
                                    NULL, NULL, hInstance, NULL)))
      return 0;

   // Direct3D�̏�����
   LPDIRECT3D9 g_pD3D;
   LPDIRECT3DDEVICE9 g_pD3DDev;
   if( !(g_pD3D = Direct3DCreate9( D3D_SDK_VERSION )) ) return 0;

   D3DPRESENT_PARAMETERS d3dpp = {0,0,D3DFMT_UNKNOWN,0,D3DMULTISAMPLE_NONE,0,
                                                      D3DSWAPEFFECT_DISCARD,NULL,TRUE,0,D3DFMT_UNKNOWN,0,0}; 

   if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDev ) ) )
   if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDev ) ) )
   if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDev ) ) )
   if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDev ) ) )
   {
      g_pD3D->Release();
      return 0;
   }

   ShowWindow(hWnd, nCmdShow);


   // ���_�i�����ʒu�j�̐ݒ�
   float Ratio = 1.0f;
   float width = 240.0f * Ratio;
   float height = 180.0f * Ratio;
   CUSTOMVERTEX v[4]= 
   {  
           { width/2, -height/2, 0.0f, 1.0f, 0x00000000, 0.0f, 0.0f},
           { width/2,  height/2, 0.0f, 1.0f, 0x000000ff, 0.0f, 1.0f}, 
           {-width/2, -height/2, 0.0f, 1.0f, 0x00000300, 1.0f, 0.0f}, 
           {-width/2,  height/2, 0.0f, 1.0f, 0x00000400, 1.0f, 1.0f} 
   };

   // ���_�o�b�t�@�̍쐬
   IDirect3DVertexBuffer9* pVertex;
   if(FAILED(g_pD3DDev->CreateVertexBuffer( sizeof(CUSTOMVERTEX)*4, D3DUSAGE_WRITEONLY, FVF_CUSTOM, D3DPOOL_MANAGED, &pVertex, NULL))){
           g_pD3DDev->Release(); g_pD3D->Release();
           return 0;
   }

   float Cx, Cy, t=0;
   DWORD Rainbow = 0;
   CUSTOMVERTEX CurV[4];
   memcpy(CurV, v, sizeof(CUSTOMVERTEX)*4);

   // ���b�Z�[�W ���[�v
   int i;
   do{
      Sleep(1);
      if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ){ DispatchMessage(&msg);}
         // ���_�ʒu�ƐF�̌v�Z�i�K���Ȍv�Z�ł��j
         // ���_���CurV���m�肷��̂��ړI
         t += 0.03146f/2;
         Ratio = 1.5f+cos(t);
         SetPos(320, 240, t*1.23, Ratio, 64, 64, &Cx, &Cy);
         for(i=0; i<4; i++){
            SetPos(Cx, Cy, t, Ratio, v[i].x, v[i].y, &CurV[i].x, &CurV[i].y);
            CurV[i].dwColor = RainbowRGB(v[i].dwColor+(Rainbow++)*(i+1));
         }

         // ���_�o�b�t�@�ɒ��_���������݂ł������ɂ����`��
         void *pData;
         if(SUCCEEDED(pVertex->Lock(0, sizeof(CUSTOMVERTEX)*4, (void**)&pData, 0))){
            memcpy(pData, CurV, sizeof(CUSTOMVERTEX)*4);
            pVertex->Unlock();

         // Direct3D�̏���
         g_pD3DDev->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
         g_pD3DDev->BeginScene();
                         
         // �`��
         g_pD3DDev->SetStreamSource(0, pVertex, 0, sizeof(CUSTOMVERTEX));
         g_pD3DDev->SetFVF(FVF_CUSTOM);
         g_pD3DDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

         g_pD3DDev->EndScene();
         g_pD3DDev->Present( NULL, NULL, NULL, NULL );
      }
   }while(msg.message != WM_QUIT);

   pVertex->Release();
   g_pD3DDev->Release();
   g_pD3D->Release();

   return 0;
}