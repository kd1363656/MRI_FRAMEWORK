// AmplificationShader
// MeshShader を1回起動するだけの最小構成

[numthreads(1, 1, 1)]
void main()
{
    DispatchMesh(1, 1, 1);
}