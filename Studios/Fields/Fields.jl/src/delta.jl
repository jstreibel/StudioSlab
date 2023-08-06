using CUDA

function δ!(x::CuDeviceVector{Float64}, ε::Float64, out::CuDeviceVector{Float64})
    index = threadIdx().x
    
    absx = abs(x[index])
    
    @inbounds begin
        out[index] = (absx < ε) * (1 - absx/ε)
    end

    return
end

function δ_call(x, eps)
    y = similar(x)
    
    N = length(x)
    
    @cuda threads=N δ!(x, eps, y)
    
    return y
end

if false
    using Plots, Interact

    x = CuArray(-2:0.01:2)

    @manipulate for ε ∈ 0.1:0.02:1.2, t ∈ 0.0:0.02:1
        y = δ_call(x.-t, ε)
        plot(Array(x),Array(y))
    end
end