using CUDA

export compute_laplacian!

# Define the CUDA kernel
function laplacian_kernel!(∇²ϕ, ϕ, dx, N)
    i = threadIdx().x
    if i >= 2 && i <= (N-1)
        ∇²ϕ[i] = (ϕ[i-1] - 2*ϕ[i] + ϕ[i+1]) / dx^2
    end
    return
end


function compute_laplacian!(∇²ϕ::CuArray{Float64}, ϕ::CuArray{Float64}, dx::Float64, N::Integer)
    @cuda threads=length(ϕ) laplacian_kernel!(∇²ϕ, ϕ, dx, N)
end
