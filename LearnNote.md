# ѧϰ�ʼ�
2019-05-08 ѧϰ�ʼ�,��ѧϰ�Ĺ�����,���ֻ������ι�.��Direct3D�ĳ�ʼ����ʼ��ϰ  
## ��4�� Direct3D�ĳ�ʼ��
### 4.1 Ԥ��֪ʶ
#### 4.1.2 �������ģ��
COM: Component Object Model �������ģ��,��һ����DirectX���ܱ����������,��ʹ֮�����ݵļ���  
Ϊ�˹���COM�������������,Windows����ʱ��(WRL: Windows Runtime Library)�ṩ��Microsoft::WRL::ComPtr��  
COMPtr 3������ Get: ����ָ��˵ײ�COM�ӿڵ�ָ��  
GetAddressOf: ����ָ��˵ײ�COM�ӿ�ָ��ĵ�ַ  
Reset  
#### 4.1.6 ��Դ������
������������֮ǰ,��Ҫ���뱾�λ��Ƶ�����ص���Դ�󶨵���Ⱦ��ˮ����  
GPU��Դ����ֱ������Ⱦ��ˮ�����,����Ҫͨ�������������������  
���õ�������: CBV(constant buffer view), SRV(shader resource view), UAV(unordered access view)  
RTV(render target view), DSV(depth/stencil view)  
��������(���Կ�������������)  
#### 4.1.10 DirectXͼ�λ����ṹ
DXGI: DirectX Graphics Infrastructure DirectXͼ�λ����ṹ: �ṩͨ��API  
��ʾ������(�Կ�)IDXGI, ��ʾ���(��ʾ��)IDXGIOutput
### 4.2 CPU��GPU�Ľ���
#### 4.2.1 ������к������б�
GPUά���������(command queue,�������ǻ��λ�����)**ID3D12CommandQueue**  
CPU�������������б�(command list,��װ��һϵ��ͼ����Ⱦ����)**ID3D12GraphicsCommandList**�������ύ�����������ȥ  
��д�ṹ����������: **D3D12_COMMAND_QUEUE_DESC**  
��������: **ID3D12Device::CreateCommandQueue**  
�������б����������ӵ��������: **ID3D12CommandQueue::ExecuteCommandLists**  
��������������б��,���� **ID3D12GraphicsCommandList::Close**  
�ڵ��� ID3D12CommandQueue::ExecuteCommandLists �ύ�����б�ǰ,һ��Ҫ����ر�  
���������(command allocator)**ID3D12CommandAllocator**,�ڴ������ӿ�  
��¼�������б������,ʵ�����Ǵ�������֮�����������������
�������������: **ID3D12Device::CreateCommandAllocator**  
���������б�: **ID3D12Device::CreateCommandList**  
�ڵ���ID3D12CommandQueue::ExecuteCommandLists֮��,����ͨ��**ID3D12GraphicsComandList::Reset**����,���������б�ռ�õ���صײ��ڴ�(��ص��������������ά���ڴ��б�����������õ�ϵ������)  
����������������ڴ�(GPUִ�������������������)**ID3D12CommandAllocator::Reset**  
#### 4.2.2 CPU��GPU���ͬ��
����Χ��: **ID3D12Device::CreateFence**  
#### 4.2.3 ��Դת��
��Դת�����Ͻṹ��**D3D12_RESOURCE_BARRIER**  
### 4.3 ��ʼ��Direct3D
#### 4.3.1 �����豸
��ʾ������(�Կ�)**ID3D12Device**  
**D3D12CreateDevice**
#### 4.3.2 ����Χ������ȡ�������Ĵ�С
#### 4.3.3 ����4X MSAA���������֧��
#### 4.3.4 ����������к������б�
#### 4.3.5 ����������������
����д�ṹ��**DXGI_SWAP_CHAIN_DESC**,�ٵ���**IDXGIFactory::CreateSwapChain**  
#### 4.3.6 ������������
**ID3D12Device::CreateDescriptorHeap**  
#### 4.3.7 ������ȾĿ����ͼ(������)
��ȡ�������еĻ�������Դ**IDXGISwapChain::GetBuffer**  
Ϊ��ȡ�ĺ�̨������������ȾĿ����ͼ**ID3D12Device::CreateRenderTargetView
#### 4.3.8 �������/ģ�建����������ͼ
��д**D3D12_RESOURCE_DESC**�ṹ��,**ID3D12Device::CreateCommittedResource**  
#### 4.3.9 �����ӿ�(��̨�������Ļ��Ʒ�Χ)
��д**D3D12_VIEWPORT**�ṹ��,**ID3D12GraphicsCommandList::RSSetViewports**  
#### 4.3.10 ���òü�����
**D3D12_RECT**, **ID3D12GraphicsCommandList::RSSetScissorRects**