## ��7�� ����Direct3D���Ƽ�����(��)
### Initialize()
D3DApp::Initialize()  
mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr)  
#### BuildRootSignature() // �޸���mRootSignature
#### BuildShadersAndInputLayout() // �޸���mShaders, mInputLayout
#### BuildShapeGeometry() // �޸���mGeometries
�����еļ������ݺϲ���һ����Ķ���/������������  
�Ժϲ����㻺������ÿ������Ķ���ƫ�������л���  
�Ժϲ�������������ÿ���������ʼ�������л���  
����Ķ��SubmeshGeometry�ṹ���а����˶���/�����������ڲ�ͬ�����������������  
��ȡ����Ԫ��,����������Ķ���װ��һ�����㻺���� vertices, indices  
...  
#### BuildRenderItems() // �޸���mAllRitems, mOpaqueRitems
�����˶��RenderItem,�������ǵ�λ��,����,����ƫ��  
#### BuildFrameResources() // ����֡��Դ,�޸���mFrameResources
#### BuildDescriptorHeaps() // �޸���mCbvHeap
Ϊÿ��֡��Դ�е�ÿһ�����崴��һ��CBV������  
ÿ��֡��Դ����Ⱦ����һ��CBV������  
...  
#### BuildConstantBufferViews()
...  
#### BuildPSOs() // �޸���mPSOs
&emsp;���ｨ����solid��wireframe����pso  
mCommandList->Close()  
ID3D12CommandList* cmdsLists[] = { mCommandList.Get() }
mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists)  
FlushCommandQueue()  
### Run()
#### Update(mTimer)
&emsp;OnKeyboardInput() ��ⰴ��: if (GetAsyncKeyState('1'))  
&emsp;UpdateCamera()  
&emsp;ѭ�������Ļ�ȡ֡��Դ�����е�Ԫ��  
&emsp;���GPU���Ƿ���ִ���괦��ǰ֡��Դ����������  
&emsp;�����û�о���CPU�ȴ�,ֱ��GPU��������ִ��,���ִ����Χ����  
&emsp;**if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)**  
##### &emsp;UpdateObjectCBs()
##### &emsp;UpdateMainPassCB()
#### Draw(mTimer)
&emsp;...  
##### &emsp;DrawRenderItems()  

&emsp;...  
&emsp;����Χ��ֵ,�������ǵ���Χ����  
&emsp;mCurrFrameResource->Fence = ++mCurrentFence  
&emsp;������������һ��ָ��������һ���µ�Χ����  
&emsp;**mCommandQueue->Signal(mFence.Get(), mCurrentFence)**  