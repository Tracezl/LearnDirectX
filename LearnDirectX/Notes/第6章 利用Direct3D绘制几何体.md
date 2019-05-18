## ��6�� ����Direct3D���Ƽ�����
### 6.1 ���������벼��
Ϊ���Զ��嶥���ʽ,���ȴ����ṹ��������ѡ���Ķ������� struct Vertex  
���벼������: ���嶥��ṹ���,��DX�ṩ����ṹ�������,ʹ���˽���������ṹ���е�ÿ����Ա  
**D3D12_INPUT_LAYOUT_DESC**  
���벼�����������������:**D3D12_INPUT_ELEMENT_DESC**���ɵ�����,�����鳤��  
**D3D12_INPUT_ELEMENT_DESC**�����е�Ԫ�����������˶���ṹ��������Ӧ�ĳ�Ա  
�ṹ���**SemanticName**(����) == **������ɫ������ǩ��(vertex shader input signature)**  
### 6.2 ���㻺����
Ϊ��ʹGPU���Է��ʶ�������,��Ҫ�����Ƿ����ڳ�Ϊ������(buffer)��**GPU��Դ**(**ID3D12Resource**)��,�Ѵ��涥��Ļ�������Ϊ���㻺����(vertex buffer)  
����ID3D12Resource����: ��дD3D12_RESOURCE_DESC(**CD3DX12_RESOURCE_DESC**, C++��װ��)�ṹ��,����ID3D12Device::CreateCommittedResource  
���ھ�̬������(ÿһ֡�����仯),���䶥�㻺��������Ĭ�϶�(**D3D12_HEAP_TYPE_DEFAULT**)���Ż�����  
��**D3D12_HEAP_TYPE_UPLOAD**�������������ϴ�������(upload buffer)��Դ.  
���������ݴ�ϵͳ�ڴ渴�Ƶ��ϴ�������,�ٴ��ϴ����������Ƶ������Ķ��㻺����  
��Ҫ�����ϴ�����������ʼ��Ĭ�ϻ������ο�d3dUtil :: CreateDefaultBuffer()  
Ϊ�˽����㻺�����󶨵���Ⱦ��ˮ����,�贴�����㻺������ͼ(vertex buffer view,����Ҫ��������)  
**D3D12_VERTEX_BUFFER_VIEW**,����BufferLocation: �������Ķ��㻺������Դ�����ַ,ͨ��**ID3D12Resource :: GetGPUVirtualAddress**��ȡ  
�ڶ��㻺���������Ӧ��ͼ������ɺ�,��������Ⱦ��ˮ���ϵ�һ�������(input slot)���.  
����һ��,��������ˮ�ߵ�����װ��׶δ��ݶ���������  
**ID3D12GraphicsCommandList :: IASetVertexBuffers**  
���ƶ���: **ID3D12GraphicsCommandList :: DrawInstanced**  
### 6.3 ����������������
Ϊ��ʹGPU���Է�����������,��Ҫ�����Ƿ�����GPU�Ļ�������Դ(ID3D12Resource)��,��Ϊ����������(index buffer)  
Ϊ��ʹ��������������Ⱦ��ˮ�߰�,��Ҫ��������������Դ����������������ͼ(index buffer view,����Ҫ��������)  
**D3D12_INDEX_BUFFER_VIEW**,����BufferLocation:��������ͼ��������������Դ�����ַ,ͨ��ID3D12Resource::GetGPUVirtualAddress��ȡ  
�������������󶨵�����װ�����׶�: **ID3D12GraphicsCommandList :: IASetIndexBuffer**  
����: **ID3D12GraphicsCommandList :: DrawIndexedInstand**  
### 6.6 ����������
#### 6.6.1 ��������������
����������(constant buffer)Ҳ��һ��GPU��Դ(ID3D12Resource),���������ݿɹ���ɫ������������  
������������CPUÿ֡����һ��,������һ���ϴ�����,��СΪ256B��������  
#### 6.6.2 ���³���������
**ComPtr\<ID3D12Resource\>::Map**��ȡָ����������Դ���ݵ�ָ��  
**memcpy**()�����ݴ�ϵͳ�ڴ渴�Ƶ�����������  
�ο�UploadBuffer.h  
#### 6.6.4 ����������������
����������������������������Ⱦ��ˮ��  
��д**D3D12_DESCRIPTOR_HEAP_DESC**�ṹ��  
Type=**D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV**  
Flags=**D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE**  
������������: **CreateDescriptorHeap()**  
��д**D3D12_CONSTANT_BUFFER_VIEW_DESC**�ṹ��  
��������������**ID3D12Device::CreateConstantBufferView()**  
#### 6.6.5 ��ǩ������������
��ͬ���͵���Դ�ᱻ�󶨵��ض��ļĴ�����(register slot)��,����ɫ���������  
��ǩ��(root signature): �󶨵���Ⱦ��ˮ���ϵ���Դ,ӳ�䵽��ɫ���Ķ�Ӧ����Ĵ���  
��ǩ����**ID3D12RootSignature**�ӿڱ�ʾ,��һ��������ɫ������Ҫ��Դ�ĸ�����(root parameter)����  
������(**CD3DX12_ROOT_PARAMETER**)�����Ǹ�����(root constant),��������(root descriptor),��������(descriptor table,**CD3DX12_DESCRIPTOR_RANGE**)  
**CD3DX12_DESCRIPTOR_RANGE.Init()**, **ID3D12RootSignature.InitAsDescriptorTable()**  
��ǩ��ֻ������Ӧ�ó���Ҫ�󶨵���Ⱦ��ˮ�ߵ���Դ,û��ִ�а󶨲���  
��������������Ⱦ��ˮ�߰�: **ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable()**  
### 6.7 ������ɫ��
**D3DCompileFromFile()**,�ο�d3dUtil::CompileShader()  
**ID3DBlob**��������һ���ڴ�,��������  
**GetBufferPointer**(��������void*ָ��,ʹ��ǰ��ת������),**GetBufferSize**()  
### 6.8 ��դ��״̬
**D3D12_RASTERIZER_DESC**  
### 6.9 ��ˮ��״̬����
PSO: Pipeline State Object, **ID3D12PipelineState**  
��д**D3D12_GRAPHICS_PIPELINE_STATE_DESC**�ṹ��  
����PSO: **ID3D12Device::CreateGraphicsPipelineState()**  
## ʾ������
#### 1, ʵ����D3DApp����BoxApp theApp(hInstance)
#### 2, theApp.Initialize()
�����ʼ��**D3DApp::Initialize()**  
���������б�**mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr)**  
##### BuildDescriptorHeaps() // ������������ mCbvHeap  
&emsp;D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc  
&emsp;md3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCbvHeap))  
##### BuildConstantBuffers() // �������������� mCbvHeap(���)  
&emsp;D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc  
&emsp;cbvDesc.BufferLocation=GetGPUVirtualAddress()+i*size(��i������)  
&emsp;md3dDevice->CreateConstantBufferView(
		&cbvDesc,
		mCbvHeap->GetCPUDescriptorHandleForHeapStart())  
##### BuildRootSignature() // ������ǩ�� mRootSignature
  
&emsp;CD3DX12_ROOT_PARAMETER[]  
&emsp;CD3DX12_ROOT_SIGNATURE_DESC  
&emsp;D3D12SerializeRootSignature()  
&emsp;md3dDevice->CreateRootSignature()  
##### BuildShadersAndInputLayout() // ������ɫ�������벼�� mInputLayout, mvsByteCode, mpsByteCode  
&emsp;������ɫ���ֽ��� mvsByteCode=CompileShader()  
&emsp;������ɫ���ֽ��� mpsByteCode=CompileShader()  
&emsp;���벼������,��Ӧ����ṹ�������(˳����ͬ),Ҳ����ɫ������ǩ��(�ַ�����ͬ) mInputLayout  
##### BuildBoxGeometry() // ����Box������ mBoxGeo->DrawArgs["box"] 
&emsp;�������� vertices  
&emsp;�������� indices  
&emsp;����ͼ�θ����ṹ�� mBoxGeo = std::make_unique\<MeshGeometry\>()  
&emsp;����Blob���͵Ķ������� D3DCreateBlob(vbByteSize, &mBoxGeo->VertexBufferCPU)  
&emsp;���������鸴�Ƶ��ڴ� CopyMemory(mBoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize)  
&emsp;����Blob���͵��������� D3DCreateBlob(ibByteSize, &mBoxGeo->IndexBufferCPU)  
&emsp;���������鸴�Ƶ��ڴ� CopyMemory(mBoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data&emsp;(), ibByteSize)  
&emsp;������������ڴ渴�Ƶ��ϴ���,�ٴ��ϴ��Ѹ��Ƶ�GPU����������  
&emsp;mBoxGeo->VertexBufferGPU = d3dUtil :: CreateDefaultBuffer()  
&emsp;mBoxGeo->IndexBufferGPU = d3dUtil :: CreateDefaultBuffer()  
&emsp;SubmeshGeometry submesh  
&emsp;mBoxGeo->DrawArgs["box"] = submesh  
##### BuildPSO() // ������ˮ��״̬���� mPSO
&emsp;D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc  
&emsp;md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO))  
  
ִ�г�ʼ������**mCommandList->Close()**  
ID3D12CommandList* cmdsLists[] = { mCommandList.Get() }  
**mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists)**  
�ȴ���ʼ�����**FlushCommandQueue()**
#### 3, theApp.Run()
������Ϣѭ��֮ǰ,������Ϸʱ�� mTimer.Reset()  
while (msg.message != WM_QUIT)  
�����Window��Ϣ,���� if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))  
����,������Ϸ�߼� else  
mTimer.Tick()  
##### Update(mTimer)
&emsp;ÿ֡���³�����������ת������  
&emsp;XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj))  
&emsp;mObjectCB->CopyData(0, objConstants)  
##### Draw(mTimer)
&emsp;mDirectCmdListAlloc->Reset()  
&emsp;mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO.Get())  
&emsp;mCommandList->RSSetViewports(1, &mScreenViewport)  
&emsp;mCommandList->RSSetScissorRects(1, &mScissorRect)  
&emsp;mCommandList->ResourceBarrier()  
&emsp;mCommandList->ClearRenderTargetView()  
&emsp;mCommandList->ClearDepthStencilView()  
&emsp;ָ����Ҫ��Ⱦ��Ŀ�껺���� **mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView())**  
&emsp;ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap.Get() }  
&emsp;mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps)  
&emsp;���ø�ǩ�� **mCommandList->SetGraphicsRootSignature(mRootSignature.Get())**  
&emsp;����װ�䶥�㻺�� **mCommandList->IASetVertexBuffers(0, 1, &mBoxGeo->VertexBufferView())**  
&emsp;mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)  
&emsp;��������������Ⱦ��ˮ����� **mCommandList->SetGraphicsRootDescriptorTable(0, mCbvHeap->GetGPUDescriptorHandleForHeapStart())**  
&emsp;ʹ���������л��� **mCommandList->DrawIndexedInstanced(
		mBoxGeo->DrawArgs["box"].IndexCount,
		1, 0, 0, 0)**  
&emsp;mCommandList->ResourceBarrier()  
&emsp;�������ļ�¼ mCommandList->Close()  
&emsp;�����������������б� ID3D12CommandList* cmdsLists[] = { mCommandList.Get() }  
&emsp;**mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists)**  
&emsp;������̨��������ǰ̨������ mSwapChain->Present(0, 0)  
&emsp;�ȴ����ƴ�֡������ִ����� **FlushCommandQueue()**  
