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
CPU�������������б�(command list)�������ύ�������������**ID3D12GraphicsCommandList**(��װ��һϵ��ͼ����Ⱦ����)  
��д�ṹ����������: D3D12_COMMAND_QUEUE_DESC  
��������: ID3D12Device::CreateCommandQueue  
�������б����������ӵ��������: ID3D12CommandQueue::ExecuteCommandLists  
��������������б��,���� ID3D12GraphicsCommandList::Close  
�ڵ��� ID3D12CommandQueue::ExecuteCommandLists �ύ�����б�ǰ,һ��Ҫ����ر�  
